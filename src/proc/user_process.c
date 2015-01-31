#include "../mem/mem.h"
#include "scheduler.h"
#include "../uart_polling.h"
#include "user_process.h"

PROC_INIT g_test_procs[NUM_TEST_PROCS];

int test_status[NUM_TESTS] = {0}; //0 means not yet run, 1 means success, -1 means failure 

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

//Prints test results, tests that a process runs
void proc_1(void) {

	  int failures = 0;
		int passes = 0;
		int finished = 0;//boolean
		uart0_put_string("G007_test: START\n\r");
		//If we have a spare test, test one passes here
		test_status[0] = 1;//TEST 1: a process is started and runs
	
		while(!finished){
				for(int i = 0; i < NUM_TESTS; i++){
						if(test_status[i] == 0){
								finished = 0;
								break;
						} else {
								finished = 1;//Will be reset to false if any tests are not finished
						}
				}
				//add request memory block later to get process blocked
				release_processor();
		}
		
		for(int i = 0; i < NUM_TESTS; i++){
				uart0_put_string("G007_test: test %d ", i + 1);
				if(test_status[i] == 1){
						uart0_put_string("OK\n\r");
						passes++;
				}else{
						uart0_put_string("FAIL\n\r");
						failures++;
				}
		}
		
		uart0_put_string("G007_test: %d/%d tests OK\n\r", passes, NUM_TESTS);
		uart0_put_string("G007_test: %d/%d tests FAIL\n\r", failures, NUM_TESTS);
	  uart0_put_string("G007_test: END\n\r");
		
    while (1) {
        release_processor();
    }
}

//Test that allocated memory is not overwritten
void proc_2(void) {
		//int pid = 2;//LAURA  remove if not using
	
		int *ptr = request_memory_block();
		*ptr = 42;
		
		for(int i = 0; i < 10; i++){//give other processes time to adjust memory
				release_processor();
		}
		
		if(*ptr == 42){ //TEST 2: Allocated memory is not corrupted
				test_status[1] = 1;
		}else{
				test_status[1] = -1;
		}
		
		//set_process_priority(pid, 3);//Done testing, get out of the way

    while (1) {
        release_processor();
    }
}

void proc_3(void) {
		//int pid = 3;//LAURA  remove if not using
		
		//set_process_priority(pid, 3);
    while (1) {
        release_processor();
    }
}

void proc_4(void) {
		//int pid = 4;//LAURA  remove if not using
		
		//set_process_priority(pid, 3);
    while (1) {
        release_processor();
    }
}

void proc_5(void) {
		//int pid = 5;//LAURA  remove if not using
		
		//set_process_priority(pid, 3);
    while (1) {
        release_processor();
    }
}

void proc_6(void) {

		//int pid = 6;//LAURA  remove if not using
		
		//set_process_priority(pid, 3);

    while (1) {
        release_processor();
    }
}
