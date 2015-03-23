/**
 * process.c -- Process implementation. See also proc/scheduler.c
 */

#include <LPC17xx.h>
#include "../rtx.h"

#include "mem.h"
#include "process.h"
#include "scheduler.h"
#include "../sys/hal.h"
#include "../sys/uart.h"
#include "../sys/timer.h"
#include "../sysproc/crt.h"
#include "../sysproc/kcd.h"
#include "../sysproc/null.h"
#include "../user/clock.h"
#include "../user/setpri.h"
#include "../user/stress_test_procs.h"

#define INITIAL_xPSR 0x01000000

PCB **gp_pcbs;
U32 *gp_stack;
PROC_INIT g_proc_table[NUM_PROCS];
PCB *gp_current_process = NULL;

/**
 * Requires k_memory_init to be called.
 * This initializes processes and finalizes memory initialization.
 * Must be called before k_scheduler_init.
 */
void k_process_init() {
    U32 *sp;
    int i;
    int j;

    int procIdx = -1;
  
    // Initialize system processes
	
    g_proc_table[++procIdx].m_pid = PID_NULL; 
    g_proc_table[procIdx].m_stack_size = STACK_SIZE;
    g_proc_table[procIdx].mpf_start_pc = &null_proc;
    g_proc_table[procIdx].m_priority = PNULL;

    g_proc_table[++procIdx].m_pid = PID_KCD; 
    g_proc_table[procIdx].m_stack_size = STACK_SIZE;
    g_proc_table[procIdx].mpf_start_pc = &kcd_proc;
    g_proc_table[procIdx].m_priority = HIGH;

    g_proc_table[++procIdx].m_pid = PID_CRT; 
    g_proc_table[procIdx].m_stack_size = STACK_SIZE;
    g_proc_table[procIdx].mpf_start_pc = &crt_proc;
    g_proc_table[procIdx].m_priority = HIGH;

    ASSERT(procIdx + 1 == NUM_SYS_PROCS) // Check NUM_SYS_PROCS

    // Initialize user processes
    g_proc_table[++procIdx].m_pid = PID_CLOCK;
    g_proc_table[procIdx].m_stack_size = STACK_SIZE;
    g_proc_table[procIdx].mpf_start_pc = &clock_proc;
    g_proc_table[procIdx].m_priority = MEDIUM;
    
    g_proc_table[++procIdx].m_pid = PID_SET_PRIO;
    g_proc_table[procIdx].m_stack_size = STACK_SIZE;
    g_proc_table[procIdx].mpf_start_pc = &setpri_proc;
    g_proc_table[procIdx].m_priority = HIGH;
		
    g_proc_table[++procIdx].m_pid = PID_A;
    g_proc_table[procIdx].m_stack_size = STACK_SIZE;
    g_proc_table[procIdx].mpf_start_pc = &proc_a;
    g_proc_table[procIdx].m_priority = HIGH;
		
    g_proc_table[++procIdx].m_pid = PID_B;
    g_proc_table[procIdx].m_stack_size = STACK_SIZE;
    g_proc_table[procIdx].mpf_start_pc = &proc_b;
    g_proc_table[procIdx].m_priority = HIGH;
		
    g_proc_table[++procIdx].m_pid = PID_C;
    g_proc_table[procIdx].m_stack_size = STACK_SIZE;
    g_proc_table[procIdx].mpf_start_pc = &proc_c;
    g_proc_table[procIdx].m_priority = HIGH;

    ASSERT(procIdx + 1 == NUM_SYS_PROCS + NUM_USR_PROCS) // Check NUM_USR_PROCS

    // Initialize i-processes: they are always ready to run, but never in a queue, so no priority is set
    g_proc_table[++procIdx].m_pid = PID_TIMER_IPROC;
    g_proc_table[procIdx].m_stack_size = STACK_SIZE;
    g_proc_table[procIdx].mpf_start_pc = &c_TIMER0_IRQ_Handler;
  
    g_proc_table[++procIdx].m_pid = PID_UART_IPROC;
    g_proc_table[procIdx].m_stack_size = STACK_SIZE;
    g_proc_table[procIdx].mpf_start_pc = &c_UART0_IRQ_Handler;

    ASSERT(procIdx + 1 == NUM_SYS_PROCS + NUM_IPROCS + NUM_USR_PROCS) // Check NUM_IPROCS

    // Initialize test processes
    set_test_procs();
    for (i = 0; i < NUM_TEST_PROCS; ++i) {
        g_proc_table[++procIdx].m_pid = g_test_procs[i].m_pid;
        ASSERT(g_test_procs[i].m_pid >= PID_P1) // Check PID
        ASSERT(g_test_procs[i].m_pid <= PID_P6) // Check PID
        g_proc_table[procIdx].m_stack_size = g_test_procs[i].m_stack_size;
        g_proc_table[procIdx].mpf_start_pc = g_test_procs[i].mpf_start_pc;

        // This line just copies an unused val
        g_proc_table[procIdx].m_priority = g_test_procs[i].m_priority;
    }

    ASSERT(procIdx + 1 == NUM_SYS_PROCS + NUM_IPROCS + NUM_USR_PROCS + NUM_TEST_PROCS) // Check NUM_TEST_PROCS
    ASSERT(procIdx + 1 == NUM_PROCS) // Check NUM_PROCS

    // Initialize all processes
    for (i = 0; i < NUM_PROCS; i++) {
        gp_pcbs[i]->pid = g_proc_table[i].m_pid;
        gp_pcbs[i]->state = NEW;

        sp = k_alloc_stack((g_proc_table[i]).m_stack_size);
        *(--sp) = INITIAL_xPSR;      // user process initial xPSR
        *(--sp) = (U32)((g_proc_table[i]).mpf_start_pc); // PC contains the entry point of the process
        for (j = 0; j < 6; ++j) { // R0-R3, R12, LR are cleared with 0
            *(--sp) = 0x0;
        }
        gp_pcbs[i]->sp = sp;
    }

    // This variable must be set before we can use memory management functionality
    heap_high_address = gp_stack;

    // Initialize all process message queues (requires memory management)
    for (i = 0; i < NUM_PROCS; ++i) {
        gp_pcbs[i]->msg_queue = list_new(IS_KERNEL);
    }
}

/**
 * Requires irq is disabled when this function is called. This function MUST only
 * be called from k_release_processor()
 */
int k_process_switch(PCB *p_pcb_old) {
    PROC_STATE state = gp_current_process->state;

    // Initalize new processes by popping execution stack
    if (state == NEW) {
        if (gp_current_process != p_pcb_old && p_pcb_old->state != NEW) {
            if (p_pcb_old->state == RUNNING) {
                p_pcb_old->state = READY;
            }

            p_pcb_old->sp = ((U32 *)__get_MSP());
        }
        gp_current_process->state = RUNNING;
        __set_MSP((U32) gp_current_process->sp);
        __enable_irq();
        __rte();  // pop exception stack frame from the stack for a new processes
        while (1) {
            // _rte() should return, this should never be executed
        }
    }

    // Switch processes if new process is READY
    if (gp_current_process != p_pcb_old) {
        if (state == READY) {
            if (p_pcb_old->state == RUNNING) {
                p_pcb_old->state = READY;
            }

            p_pcb_old->sp = (U32 *) __get_MSP(); // save the old process's sp
            gp_current_process->state = RUNNING;
            __set_MSP((U32) gp_current_process->sp); //switch to the new proc's stack
        } else {
            gp_current_process = p_pcb_old; // revert back to the old proc on error
            __enable_irq();
            return RTX_ERR;
        }
    }

    __enable_irq();
    return RTX_OK;
}

/**
 * If this function calls k_process_switch, irq is disabled first
 */
int k_release_processor(void) {    
    PCB *p_pcb_old = gp_current_process;

    __disable_irq();
    k_enqueue_process(p_pcb_old->pid);
    gp_current_process = k_scheduler();

    // revert back to the old process if the scheduler cannot
    // find a new process to execute
    if (gp_current_process == NULL) {
        gp_current_process = p_pcb_old;
        __enable_irq();
        return RTX_ERR;
    }

    // if this is our first release, p_pcb_old is NULL and we
    // should emulate switching from the current process
    if (p_pcb_old == NULL) {
        p_pcb_old = gp_current_process;
    }

    return k_process_switch(p_pcb_old);
}

PCB *k_get_pcb_from_pid(int pid){
    int i;

    for (i = 0; i < NUM_PROCS; ++i) {
        if (gp_pcbs[i]->pid == pid) {
            return gp_pcbs[i];
        }
    }

    return NULL;
}

PROC_INIT *k_get_proc_table_from_pid(int pid){
    int i;

    for (i = 0; i < NUM_PROCS; ++i) {
        if (g_proc_table[i].m_pid == pid) {
            return &g_proc_table[i];
        }
    }

    return NULL;
}
