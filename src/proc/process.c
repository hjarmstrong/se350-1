/**
 * process.c -- Process implementation. See also proc/scheduler.c
 */

#include <LPC17xx.h>
#include "../mem/mem.h"
#include "../printf.h"
#include "process.h"
#include "scheduler.h"
#include "../rtx.h"
#include "../svc/hal.h"

#define INITIAL_xPSR 0x01000000

PCB **gp_pcbs;
U32 *gp_stack;
PROC_INIT g_proc_table[NUM_PROCS];
PCB *gp_current_process = NULL;

/**
 * Requires k_memory_init to be called.
 * This initializes processes and finalizes memory initalization.
 * Must be called before k_scheduler_init.
 */
void k_process_init() {
    U32 *sp;
    int i;
    int j;

    // Initialize null process table
    g_proc_table[0].m_pid = 0;
    g_proc_table[0].m_stack_size = STACK_SIZE;
    g_proc_table[0].mpf_start_pc = &null_proc;
    g_proc_table[0].m_priority = PNULL;

    // Initialize test process tables
    set_test_procs();
    for (i = 0; i < NUM_TEST_PROCS; ++i) {
        g_proc_table[i + 1].m_pid = g_test_procs[i].m_pid;
        g_proc_table[i + 1].m_stack_size = g_test_procs[i].m_stack_size;
        g_proc_table[i + 1].mpf_start_pc = g_test_procs[i].mpf_start_pc;
        g_proc_table[i + 1].m_priority = g_test_procs[i].m_priority;
    }

    // Initilize all processes
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
}

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
        __rte();  // pop exception stack frame from the stack for a new processes
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
            return RTX_ERR;
        }
    }

    return RTX_OK;
}

int k_release_processor(void) {
    PCB *p_pcb_old = gp_current_process;
    k_enqueue_process(p_pcb_old->pid);
    gp_current_process = k_scheduler();

    // revert back to the old process if the scheduler cannot
    // find a new process to execute
    if (gp_current_process == NULL) {
        gp_current_process = p_pcb_old;
        return RTX_ERR;
    }

    // if this is our first release, p_pcb_old is NULL and we
    // should emulate switching from the current process
    if (p_pcb_old == NULL) {
        p_pcb_old = gp_current_process;
    }

    return k_process_switch(p_pcb_old);
}
