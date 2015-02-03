#include "uart_polling.h"
#include "user_process.h"
#include "rtx.h"

#define MANY_MEMORY_BLOCKS 150 //should be more than 1/2 the available memory, but not all of it
#define NUM_TESTS 5

PROC_INIT g_test_procs[NUM_TEST_PROCS];

int test_status[NUM_TESTS] = {0}; //0 means not yet run, 1 means success, -1 means failure. 
//Other values are possible in some tests. They are used for communication between processes, and are defined when used.

/* GUIDE TO PRIORITIES:
 * HIGH: running test
 * MEDIUM/LOW: used for preemption/blocking tests
 * PRIORITY LOW: test is finished running
 */

/* GUIDE TO TESTS: (comments also found at line where tests pass)
 * 1: a process is started and runs
 * 2: Allocated memory is not corrupted
 * 3: proc3 was preempted when changing it's priority down
 * 4: proc4 was preempted by changing proc3 priority up
 * 5: proc6 gets blocked when it runs out of memory. Note: a failure here could indicate that MANY_MEMORY_BLOCKS is too low
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
    
    // These tests use a greater amount of memory than the standard STACK_SIZE
    g_test_procs[4].m_stack_size = 0x400;
    g_test_procs[5].m_stack_size = 0x400;
}

//Prints test results, tests that a process runs
void proc1(void) {
    int failures = 0;
    int passes = 0;
    int pid = 1;
    int i;

    int char_offset = 48;

    uart0_put_string("G007_test: START\n\r");

    test_status[0] = 1;//TEST 1: a process is started and runs

    if (RTX_OK == set_process_priority(pid, LOWEST)) {
        release_processor();
    }

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

//Test that allocated memory is not overwritten
void proc2(void) {
    int pid = 2;
    int i;
  
    int *ptr = request_memory_block();
    *ptr = 42;
    
    for (i = 0; i < 10; i++){//give other processes time to adjust memory
        release_processor();
    }
    
    if (*ptr == 42) { //TEST 2: Allocated memory is not corrupted
        test_status[1] = 1;
    } else {
        test_status[1] = -1;
    }
    
    release_memory_block(ptr);//return to initial state
    

    if (RTX_OK == set_process_priority(pid, LOWEST)) {
        release_processor();
    }
    
    while (1) {
        release_processor();
    }
}

void proc3(void) {//Part 1 of preemption tests for priority change
    int pid = 3;
  
    test_status[2] = 2;//2 tells proc4 that this process has run
    if (RTX_OK == set_process_priority(pid, LOW)) {
        release_processor();
    }
    //set_process_priority(pid, 2);
    if(test_status[2] != 1){//proc4 will set test_status[2] to 1 if it runs before we get to this line 
        test_status[2] = -1;
    }
    
    //Begin test 4
    
    while(test_status[3] == 0){//Wait for proc4 to run
        release_processor();
    }
    if(test_status[3] == 2){//TEST 4: proc4 was preempted by changing proc3 priority up
        test_status[3] = 1;
    }

    if (RTX_OK == set_process_priority(pid, LOWEST)) {
        release_processor();
    }
    while (1) {
        release_processor();
    }
}

void proc4(void) {//Part 2 of preemption tests for priority change
    int pid = 4;
    int part_1_pid = 3;
    
    while(test_status[2] == 0){//Wait for proc3 to run
        release_processor();
    }
    
    if(test_status[2] == 2){//TEST 3: proc3 was preempted when changing it's priority down, or else it would have already set test_status[2] to -1
        test_status[2] = 1;
    }//Otherwise, proc3 will indicate that the test failed
    
    //Begin test 4
    
    //set priority down so that proc3 can be set to higher priority
    if (RTX_OK == set_process_priority(pid, MEDIUM)) {
        release_processor();
    }
    test_status[3] = 2;//indicates that this has run
    //Should be preempted by proc3
    if (RTX_OK == set_process_priority(part_1_pid, HIGH)) {
        release_processor();
    }
    //set_process_priority(part_1_pid, 0);
    if(test_status[3] != 1){//proc3 will set test_status if this process is preempted...
        test_status[3] = -1;//...so if it doesn't, fail the test
    }

    if (RTX_OK == set_process_priority(pid, LOWEST)) {
        release_processor();
    }
    while (1) {
        release_processor();
    }
}

void proc5(void) {//Part 1 of out of memory blocking queue tests
    void *allocated_memory[MANY_MEMORY_BLOCKS];
    int pid = 5;
    int i;

    //grab more than 1/2 the memory
    for(i = 0; i < MANY_MEMORY_BLOCKS; i++){
        allocated_memory[i] = request_memory_block();
    }

    test_status[4] = 2; //Indicates to proc6 that this has run

    //change priority down so that this process won't run until proc6 is blocked
    if (RTX_OK == set_process_priority(pid, MEDIUM)) {
        release_processor();
    }
    if (test_status[4] != -1){//if proc6 is not blocked, it will fail the test
        test_status[4] = 1;//otherwise, we know it was blocked, and should pass the test
    }

    //release memory to unblock proc6
    for (i = 0; i < MANY_MEMORY_BLOCKS; i++){
        release_memory_block(allocated_memory[i]);
    }
    
    if (RTX_OK == set_process_priority(pid, LOWEST)) {
        release_processor();
    }
    
    //Done testing, get out of the way
    while (1) {
        release_processor();
    }
}

void proc6(void) {//Part 2 of out of memory blocking queue tests
    void *allocated_memory[MANY_MEMORY_BLOCKS];
    int pid = 6;
    int i;
  
    while(test_status[4] != 2){//wait for proc5 to run
        release_processor();
    }
    
    //Try to get more memory than is left after proc5. This process should get blocked at some point in this loop
    for (i = 0; i < MANY_MEMORY_BLOCKS; i++){
        allocated_memory[i] = request_memory_block();
    }
    //proc5 runs when this is blocked, marks test passed, then releases memory so this process will finish running
    
    if (test_status[4] != 1){//This would indicate that this process never got blocked
        test_status[4] = -1;//if this process did not get blocked, the test should fail
    }
    
    //release memory to clean up
    for (i = 0; i < MANY_MEMORY_BLOCKS; i++){
        release_memory_block(allocated_memory[i]);
    }
    
    //Done testing, get out of the way
    if (RTX_OK == set_process_priority(pid, LOWEST)) {
        release_processor();
    }

    while (1) {
        release_processor();
    }
}
