#include "uart_polling.h"
#include "usr_proc.h"
#include "rtx.h"

#include "../core/process.h"

#define NUM_TESTS 1
#define NUM_MSG 0x4E
#define NUM_BLOCKS 300

PROC_INIT g_test_procs[NUM_TEST_PROCS];
int test_runner_pid = 1;


void set_test_procs() {
    int i;

    g_test_procs[0].mpf_start_pc = &proc1;

    for (i = 0; i < NUM_TEST_PROCS; ++i) {
        g_test_procs[i].m_pid = i + 1;
        g_test_procs[i].m_stack_size = STACK_SIZE;
        g_test_procs[i].m_priority = HIGH;
    }
}

void proc1(void) {
    int time1;
    int time2;
    int i;
    
    time1 = get_time();
    for (i = 0; i < NUM_MSG; ++i) {
        send_message(1, (void *) (i + 1));
    }
    time2 = get_time();
    
    uart1_put_string("send_message x ");
    uart1_put_number(NUM_MSG);
    uart1_put_string(" took ");
    uart1_put_number(time2 - time1);
    uart1_put_string("msec = ");
    uart1_put_number((time2 - time1)/NUM_MSG);
    uart1_put_string(" msec per call\r\n");
    
    time1 = get_time();
    for (i = 0; i < NUM_MSG; ++i) {
        receive_message(NULL);
    }
    time2 = get_time();
    
    uart1_put_string("receive_message x ");
    uart1_put_number(NUM_MSG);
    uart1_put_string(" took ");
    uart1_put_number(time2 - time1);
    uart1_put_string("msec = ");
    uart1_put_number((time2 - time1)/NUM_MSG);
    uart1_put_string(" msec per call\r\n");
    
    time1 = get_time();
    for (i = 0; i < NUM_MSG; ++i) {
        request_memory_block();
    }
    time2 = get_time();
    uart1_put_string("request_memory_block x ");
    uart1_put_number(NUM_BLOCKS);
    uart1_put_string(" took ");
    uart1_put_number(time2 - time1);
    uart1_put_string("msec = ");
    uart1_put_number((time2 - time1)/NUM_BLOCKS);
    uart1_put_string(" msec per call\r\n");
    
    while (1) {
        release_processor();
    }
}
