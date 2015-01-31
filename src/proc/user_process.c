#include "../mem/mem.h"
#include "../uart_polling.h"
#include "user_process.h"

PROC_INIT g_test_procs[NUM_TEST_PROCS];

void set_test_procs() {
    g_test_procs[0].mpf_start_pc = &proc_1;
    g_test_procs[1].mpf_start_pc = &proc_2;
    g_test_procs[2].mpf_start_pc = &proc_3;
    g_test_procs[3].mpf_start_pc = &proc_4;
    g_test_procs[4].mpf_start_pc = &proc_5;
    g_test_procs[5].mpf_start_pc = &proc_6;

    for (int i = 0; i < NUM_TEST_PROCS; ++i) {
        g_test_procs[i].m_pid = i + 1;
        g_test_procs[i].m_stack_size = STACK_SIZE;
        g_test_procs[i].m_priority = PRIORITY_HIGHEST;
    }
}

void proc_1(void) {
		uart0_put_string("G007_test: test 1 OK\n\r");
    while (1) {
        release_processor();
    }
}

void proc_2(void) {
		uart0_put_string("G007_test: test 2 OK\n\r");
    while (1) {
        release_processor();
    }
}

void proc_3(void) {
    uart0_put_string("G007_test: test 3 OK\n\r");
    while (1) {
        release_processor();
    }
}

void proc_4(void) {
    uart0_put_string("G007_test: test 4 OK\n\r");
    while (1) {
        release_processor();
    }
}

void proc_5(void) {
    uart0_put_string("G007_test: test 5 OK\n\r");
    while (1) {
        release_processor();
    }
}

void proc_6(void) {
    uart0_put_string("G007_test: test 6 OK\n\r");
    while (1) {
        release_processor();
    }
}
