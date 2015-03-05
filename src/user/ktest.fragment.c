/**
 * ktest.fragment.c -- implementation of set_test_procs that
 *   results in run_ktests being called in kernel mode
 * 
 * usage: #include "ktest.fragment.c"
 */

#include "../sysproc/null.h"

PROC_INIT g_test_procs[NUM_TEST_PROCS];

#define run_ktests(pid) _run_ktests((U32)k_run_ktests)
extern int _run_ktests(U32 p_func) __SVC_0;

void run_tests() {
    run_ktests();
    while(1) {
        release_processor();
    }
}

void set_test_procs() {
    int i;
    for( i = 0; i < NUM_TEST_PROCS; i++ ) {
        g_test_procs[i].m_pid=(U32)(i+1);
        g_test_procs[i].m_stack_size=0x100;
    }
  
    g_test_procs[0].mpf_start_pc = &run_tests;
    g_test_procs[0].m_priority   = HIGH;
    
    g_test_procs[1].mpf_start_pc = &null_proc;
    g_test_procs[1].m_priority   = PNULL;
    
    g_test_procs[2].mpf_start_pc = &null_proc;
    g_test_procs[2].m_priority   = PNULL;
    
    g_test_procs[3].mpf_start_pc = &null_proc;
    g_test_procs[3].m_priority   = PNULL;
    
    g_test_procs[4].mpf_start_pc = &null_proc;
    g_test_procs[4].m_priority   = PNULL;
    
    g_test_procs[5].mpf_start_pc = &null_proc;
    g_test_procs[5].m_priority   = PNULL;
}
