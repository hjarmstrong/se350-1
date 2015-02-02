#include "../mem/mem.h"
#include "scheduler.h"
#include "../uart_polling.h"
#include "user_process.h"

PROC_INIT g_test_procs[NUM_TEST_PROCS];

int test_status[NUM_TESTS] = {0}; //0 means not yet run, 1 means success, -1 means failure. 
//Other values are possible in some tests. They are used for communication between processes, and are defined when used.

/* GUIDE TO PRIORITIES:
 * 0: running test
 * 1/2: used for preemption/blocking tests
 * 3: test is finished running
 */

/* GUIDE TO TESTS: (comments also found at line where tests pass)
 * 1: a process is started and runs
 * 2: Allocated memory is not corrupted
 * 3: proc_3 was preempted when changing it's priority down
 * 4: proc_4 was preempted by changing proc_3 priority up
 */

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
		int pid = 1;
		uart0_put_string("G007_test: START\n\r");

		test_status[0] = 1;//TEST 1: a process is started and runs

    if (RTX_OK == set_process_priority(pid, 3)) {
        release_processor();
    }
		//set_process_priority(pid, 3);//Done test. When everything else is done too, this will run again, printing the results

		for(int i = 0; i < NUM_TESTS; i++){
				uart0_put_string("G007_test: test ");
				uart0_put_char(i + 1 + 48);
				if(test_status[i] == 1){
						uart0_put_string(" OK\n\r");
						passes++;
				}else{
						uart0_put_string(" FAIL\n\r");
						failures++;
				}
		}
		
		uart0_put_string("G007_test: ");
		uart0_put_char(passes + 48);
		uart0_put_string("/");
		uart0_put_char(NUM_TESTS + 48);
		uart0_put_string(" tests OK\n\r");
		
		uart0_put_string("G007_test: ");
		uart0_put_char(failures + 48);
		uart0_put_string("/");
		uart0_put_char(NUM_TESTS + 48);
		uart0_put_string(" tests FAILS\n\r");
		
	  uart0_put_string("G007_test: END\n\r");
		
    while (1) {
        release_processor();
    }
}

//Test that allocated memory is not overwritten
void proc_2(void) {
		int pid = 2;
	
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
		

    if (RTX_OK == set_process_priority(pid, 3)) {
        release_processor();
    }
		//set_process_priority(pid, 3);//Done testing, get out of the way
    while (1) {
        release_processor();
    }
}

void proc_3(void) {//Part 1 of preemption tests for priority change
		int pid = 3;
	
		test_status[2] = 2;//2 tells proc_4 that this process has run
    if (RTX_OK == set_process_priority(pid, 2)) {
        release_processor();
    }
		//set_process_priority(pid, 2);
		if(test_status[2] != 1){//proc_4 will set this to 1 if it runs before we get to this line
				test_status[2] = -1;
		}
		
		while(test_status[3] == 0){
				release_processor();
		}
		if(test_status[3] == 2){//TEST 4: proc_4 was preempted by changing proc_3 priority up
				test_status[3] = 1;
		}

    if (RTX_OK == set_process_priority(pid, 3)) {
        release_processor();
    }
		//set_process_priority(pid, 3);//Done testing, get out of the way
    while (1) {
        release_processor();
    }
}

void proc_4(void) {//Part 2 of preemption tests for priority change
		int pid = 4;
		int part_1_pid = 3;
		
		while(test_status[2] == 0){
				release_processor();
		}
		if(test_status[2] == 2){//TEST 3: proc_3 was preempted when changing it's priority down
				test_status[2] = 1;//
		}

    if (RTX_OK == set_process_priority(pid, 1)) {
        release_processor();
    }
		//set_process_priority(pid, 1);//set priority down so that proc_3 can be set to higher priority
		test_status[3] = 2;//indicates that this has run
    if (RTX_OK == set_process_priority(part_1_pid, 0)) {
        release_processor();
    }
		//set_process_priority(part_1_pid, 0);
		if(test_status[3] != 1){//proc_3 will set test_status if this process is preempted
				test_status[3] = -1;
		}

    if (RTX_OK == set_process_priority(pid, 3)) {
        release_processor();
    }
		//set_process_priority(pid, 3);//Done testing, get out of the way
    while (1) {
        release_processor();
    }
}

void proc_5(void) {
		int pid = 5;

    if (RTX_OK == set_process_priority(pid, 3)) {
        release_processor();
    }
		//set_process_priority(pid, 3);//Done testing, get out of the way
    while (1) {
        release_processor();
    }
}

void proc_6(void) {
		int pid = 6;

    if (RTX_OK == set_process_priority(pid, 3)) {
        release_processor();
    }
		//set_process_priority(pid, 3);//Done testing, get out of the way
    while (1) {
        release_processor();
    }
}
