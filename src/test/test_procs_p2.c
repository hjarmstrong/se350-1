#include "uart_polling.h"
#include "usr_proc.h"
#include "rtx.h"

#include "../proc/process.h"

#define NUM_TESTS 6

#define MSG_TEXT_1 'a'
#define MSG_TEXT_2 'b'
#define DELAY 1000 //milliseconds

PROC_INIT g_test_procs[NUM_TEST_PROCS];

int test_status[NUM_TESTS] = {0}; //0 means not yet run, 1 means success, -1 means failure. 
//Other values are possible in some tests. They are used for communication between processes, and are defined when used.

typedef struct msgbuf {
    int mtype; 
    char mtext[1];
} msgbuf;

/* GUIDE TO PRIORITIES:
 * HIGH: running test
 * MEDIUM/LOW: used for preemption/blocking tests
 * LOWEST: test is finished running
 */

/* GUIDE TO TESTS: (comments also found at line where tests pass)
 * 1: Message can be accurately sent/recieved
 * 2: Delayed send does not block
 * 3: Delayed recieve occurs after correct amount of time
 * 4: Normal message is recieved before a delayed message
 * 5: Receiving process blocks
 * 6: Higher priority recipient preempts upon recieving message
 */

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

//Prints test results, 
//Test 1 sender (message can be accurately sent/recieved)
void proc1(void) {
    int failures = 0;
    int passes = 0;
    int pid = 1;
    int destination = 2;
    int i;

    int char_offset = 48;
    
    msgbuf *msg;

    uart0_put_string("G007_test: START\n\r");
    
    //create a message to send to process 2
    msg = request_memory_block();
    msg->mtype = DEFAULT;
    msg->mtext[0] = MSG_TEXT_1;
    
    //send the message to process 2
    send_message(destination, msg);
    
    //mark tests 1 and 2 failed (process 2 will mark the tests passed later if sucessful)
    test_status[0] = -1;
    test_status[1] = -1;

    set_process_priority(pid, LOWEST);//Wait for other processes to finish before printing results

    for (i = 0; i < NUM_TESTS; i++) {
        uart0_put_string("G007_test: test ");
        uart0_put_char(i + 1 + 48);
        if (test_status[i] == 1) {
            uart0_put_string(" OK\n\r");
            passes++;
        } else {
            uart0_put_string(" FAIL\n\r");
            failures++;
        }
    }
    
    uart0_put_string("G007_test: ");
    uart0_put_char(passes + char_offset);
    uart0_put_string("/");
    uart0_put_char(NUM_TESTS + char_offset);
    uart0_put_string(" tests OK\n\r");
    
    uart0_put_string("G007_test: ");
    uart0_put_char(failures + char_offset);
    uart0_put_string("/");
    uart0_put_char(NUM_TESTS + char_offset);
    uart0_put_string(" tests FAIL\n\r");
    
    uart0_put_string("G007_test: END\n\r");
    
    while (1) {
        release_processor();
    }
}

//Test 1 reciever (message can be accurately sent/recieved)
void proc2(void) {
    int pid = 2;
    int sender = 1;
    msgbuf *msg;
    
    msg = receive_message(&sender);
    
    //check message contents
    if(msg->mtype == DEFAULT && msg->mtext[0] == MSG_TEXT_1){
        test_status[0] = 1;//TEST 1: Message contents same as when sent
    } else {
        test_status[0] = -1;//message contents incorrect
    }
    
    release_memory_block(msg);
    
    //Done testing
    set_process_priority(pid, LOWEST);
    while (1) {
        release_processor();
    }
}

//Test 2 (delayed send does not block), and
//Test 3 (delayed recieve occurs after correct amount of time)
void proc3(void) {
    int pid = 3;
    msgbuf *msg;
    int start_time;
    int end_time;
    
    //create message
    msg = request_memory_block();
    msg->mtype = DEFAULT;
    msg->mtext[0] = MSG_TEXT_1;
    
    //check time
    start_time = get_time();
    
    //send with delay
    delayed_send(pid, msg, DELAY);
    
    test_status[1] = 1;//TEST 2: Send does not block 
    //(if it did, we wouldn't reach this line because this process is sending the message to itself)
    
    receive_message(&pid);//don't need to store, because we have the original. Just checking the timing.
    
    //check time
    end_time = get_time();
    //mark test 3 passed if it is late enough, otherwise make it failed
    if(end_time >= start_time + DELAY){
        test_status[2] = 1;//TEST 3: delayed message recieved after the appropriate delay
    } else {
        test_status[2] = -1;//message recieved too soon
    }
    
    //release message memory
    release_memory_block(msg);

    //Done testing
    set_process_priority(pid, LOWEST);
    while (1) {
        release_processor();
    }
}

//Test 4 (normal message is recieved before a delayed message)
void proc4(void) {
    int pid = 4;
    msgbuf *delayed_msg;
    msgbuf *normal_msg;
    msgbuf *received_msg;
    
    //create delayed message
    delayed_msg = request_memory_block();
    delayed_msg->mtype = DEFAULT;
    delayed_msg->mtext[0] = MSG_TEXT_1;
    
    //create normal message
    normal_msg = request_memory_block();
    normal_msg->mtype = DEFAULT;
    normal_msg->mtext[0] = MSG_TEXT_2;//different text from the delayed message
    
    //send delayed message
    delayed_send(pid, delayed_msg, DELAY);
    //send normal message
    send_message(pid, normal_msg);
    
    //recieve message
    received_msg = receive_message(&pid);
    
    //if normal message, mark test passed
    if(received_msg->mtext[0] == MSG_TEXT_2){//should get the non delayed message before the delayed message even though it was sent later.
        test_status[3] = 1;//TEST 4: normal message is recieved before delayed message
    } else {
        test_status[3] = -1;
    }
    //release message memory
    release_memory_block(received_msg);
    //recieve massage
    received_msg = receive_message(&pid);//reusing variable now that the test is finished.
    //release message memory
    release_memory_block(received_msg);

    //Done testing
    set_process_priority(pid, LOWEST);
    while (1) {
        release_processor();
    }
}

//Test 5 sender (receiving process blocks) and
//Test 6 sender (higher priority recipient preempts upon recieving message)
void proc5(void) {
    int pid = 5;
    int destination = 6;
    msgbuf *msg;
    
    //create message
    msg = request_memory_block();
    msg->mtype = DEFAULT;
    msg->mtext[0] = MSG_TEXT_1;
    
    //set priority to medium so that proc6 will run until it blocks
    set_process_priority(pid, MEDIUM);
    //mark test 5 passed if it is already failed by proc6 not blocking
    if(test_status[4] != -1){//This lower priority process should run before proc6 finishes receiving a message, because this process has not sent it yet.
        test_status[4] = 1;//TEST 5: receive blocks. 
    }
    
    //send message
    send_message(destination, msg);
    //should now be preempted by proc6, which is higher priority and now has a message to receive.
    
    
    //mark test 6 failed if it is not yet passed(should have been preempted on send by proc6
    if(test_status[5] != 1){//If proc5 is preempted, proc6 will mark this test passed before this line...
        test_status[5] = -1;//... so if this line runs, preemption did not work as expected.
    }

    //Done testing
    set_process_priority(pid, LOWEST);
    while (1) {
        release_processor();
    }
}

//Test 5 sender (receiving process blocks) and
//Test 6 sender (higher priority recipient preempts upon recieving message)
void proc6(void) {
    int pid = 6;
    int sender =5;

    //recieve message
    receive_message(&sender);
    //mark test 5 failed if not passed by proc5 while this process was blocked.
    if(test_status[4] != 1){//If this line runs before proc5 sends a message...
        test_status[4] = -1;//receive did not block.
    }
    
    //mark test 6 passed if it is not failed
    if(test_status[5] != -1){//if proc5 stops running after sending a message until after this process has run
        test_status[5] = 1;//TEST 6: higher priority recipient preempts upon recieving message
    }
    
    //Done testing
    set_process_priority(pid, LOWEST);
    while (1) {
        release_processor();
    }
}
