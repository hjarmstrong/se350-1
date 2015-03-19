#include "uart.h"
#include "usr_proc.h"
#include "rtx.h"

#include "../core/process.h"


#define NUM_TESTS 6

#define TEST_SUCCESS 's'
#define TEST_FAILURE 'f'

#define DELAY 100


PROC_INIT g_test_procs[NUM_TEST_PROCS];


void set_test_procs() {
    int i;

    g_test_procs[0].mpf_start_pc = &proc1;
    g_test_procs[1].mpf_start_pc = &proc2;
    g_test_procs[2].mpf_start_pc = &proc3;
    g_test_procs[3].mpf_start_pc = &proc4;
    g_test_procs[4].mpf_start_pc = &proc5;
    g_test_procs[5].mpf_start_pc = &proc6;

    for (i = 0; i < NUM_TEST_PROCS; ++i) {
        g_test_procs[i].m_pid = i + 1;
        g_test_procs[i].m_stack_size = STACK_SIZE;
        g_test_procs[i].m_priority = HIGH;
    }
}

void proc1(void) {
    int failures = 0;
    int passes = 0;
    int pid = 1;
    int sender;
    int i;

    int char_offset = 48;

    msgbuf *msg;

    crt_send_string("G007_test: START\n\r");

    // ----------------------------------------
    // Test 1: send and receive message to self
    msg = request_memory_block();
    msg->mtype = DEFAULT;
    msg->mtext[0] = TEST_SUCCESS;

    send_message(pid, msg);
    // ----------------------------------------

    // ----------------------------------------
    // receives messages from each test, marks
    // them as pass / fail depending on message
    // contents
    for (i = 1; i <= NUM_TESTS; ++i) {
        msg = receive_message(&sender);
        while (sender != i) {
            send_message(pid, msg);
            release_processor();
            msg = receive_message(&sender);
        }

        crt_send_string("G007_test: test ");
        crt_send_char(i + char_offset);
        if (msg->mtext[0] == TEST_SUCCESS) {
            crt_send_string(" OK\n\r");
            ++passes;
        } else {
            crt_send_string(" FAIL\n\r");
            ++failures;
        }
        release_memory_block(msg);
    }
    // ----------------------------------------

    crt_send_string("G007_test: ");
    crt_send_char(passes + char_offset);
    crt_send_string("/");
    crt_send_char(NUM_TESTS + char_offset);
    crt_send_string(" tests OK\n\r");

    crt_send_string("G007_test: ");
    crt_send_char(failures + char_offset);
    crt_send_string("/");
    crt_send_char(NUM_TESTS + char_offset);
    crt_send_string(" tests FAIL\n\r");

    crt_send_string("G007_test: END\n\r");

    // Done
    while (1) {
        release_processor();
    }
}

void proc2(void) {
    int pid = 1;

    msgbuf *msg;

    // ----------------------------------------
    // Test 2: send and receive delayed message
    msg = request_memory_block();
    msg->mtype = DEFAULT;
    msg->mtext[0] = TEST_SUCCESS;

    delayed_send(pid, msg, DELAY);
    // ----------------------------------------

    // Done
    set_process_priority(pid, LOWEST);

    while (1) {
        release_processor();
    }
}

void proc3(void) {
    int pid = 3;
    int priority;

    msgbuf *msg;

    // ------------------------------------
    // Test 3: get and set process priority
    msg = request_memory_block();
    msg->mtype = DEFAULT;

    priority = get_process_priority(pid);
    if (priority != MEDIUM) {
        set_process_priority(pid, MEDIUM);
    }

    priority = get_process_priority(pid);
    if (priority == MEDIUM) {
        msg->mtext[0] = TEST_SUCCESS;
    } else {
        msg->mtext[0] = TEST_FAILURE;
    }

    pid = 1;
    send_message(pid, msg);
    // ------------------------------------

    // Done
    set_process_priority(pid, LOWEST);

    while (1) {
        release_processor();
    }
}

void proc4(void) {
    int pid = 4;

    msgbuf *msg;

    // -----------------------------------------
    // Test 4: non-delayed message arrives first
    msg = request_memory_block();
    msg->mtype = DEFAULT;
    msg->mtext[0] = TEST_SUCCESS;
    delayed_send(pid, msg, DELAY);

    msg = request_memory_block();
    msg->mtype = DEFAULT;
    msg->mtext[0] = TEST_FAILURE;
    send_message(pid, normal_msg);

    msg = receive_message(&pid);
    release_memory_block(msg);

    msg = receive_message(&pid);
    pid = 1;
    send_message(pid, msg);
    // -----------------------------------------

    // Done
    set_process_priority(pid, LOWEST);

    while (1) {
        release_processor();
    }
}

void proc5(void) {
    int pid = 6;
    int sender;

    msgbuf *msg;

    // ----------------------------------------
    // Test 5: verify bouncing messages between
    // processes does not corrupt them
    // see proc6
    msg = request_memory_block();
    msg->mtype = DEFAULT;
    msg->mtext[0] = TEST_SUCCESS;

    send_message(pid, msg);

    msg = receive_message(&sender);
    send_message(pid, msg);

    msg = receive_message(&sender);
    pid = 1;
    send_message(pid, msg);
    // ----------------------------------------

    // Done
    set_process_priority(pid, LOWEST);

    while (1) {
        release_processor();
    }
}

void proc6(void) {
    int pid = 5;
    int sender;

    // ----------------------------------------
    // Test 5: verify bouncing messages between
    // processes does not corrupt them
    // see proc5
    msg = request_memory_block();
    msg->mtype = DEFAULT;
    msg->mtext[0] = TEST_FAILURE;

    send_message(pid, msg);

    msg = receive_message(&sender);
    send_message(pid, msg);

    msg = receive_message(&sender);
    msg->mtext[0] = (msg->mtext[0] == TEST_SUCCESS ? TEST_FAILURE : TEST_SUCCESS);
    pid = 1;
    send_message(pid, msg);
    // ----------------------------------------

    // Done
    set_process_priority(pid, LOWEST);

    while (1) {
        release_processor();
    }
}
