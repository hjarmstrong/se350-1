#include <LPC17xx.h>
#include "../mem/mem.h"
#include "process.h"
#include "../stdefs.h"
#include "../svc/hal.h"

#define INITIAL_xPSR 0x01000000

PCB **gp_pcbs;
U32 *gp_stack;
PROC_INIT g_proc_table[NUM_PROCS];
PCB *gp_current_process = NULL;
List g_queues[NUM_QUEUES];

/**
 * Requires k_memory_init to be called.
 * This initializes processes and finalizes memory initalization.
 */
void k_process_init() {
    U32 *sp;

    // Initialize null process table
		// Memory is not setup yet.
    g_proc_table[0].m_pid = 0;
    g_proc_table[0].m_stack_size = STACK_SIZE;
    g_proc_table[0].mpf_start_pc = &null_proc;
	  g_proc_table[0].m_priority = PRIORITY_NULL;

    // Initialize test process tables
		// Memory is not setup yet.
    set_test_procs();
    for (int i = 0; i < NUM_TEST_PROCS; ++i) {
        g_proc_table[i + 1].m_pid = g_test_procs[i].m_pid;
        g_proc_table[i + 1].m_stack_size = g_test_procs[i].m_stack_size;
        g_proc_table[i + 1].mpf_start_pc = g_test_procs[i].mpf_start_pc;
        g_proc_table[i + 1].m_priority = g_test_procs[i].m_priority;
    }

    // Initilize all processes
		// Memory is not setup yet.
    for (int i = 0; i < NUM_PROCS; i++) {
        gp_pcbs[i]->pid = g_proc_table[i].m_pid;
        gp_pcbs[i]->state = NEW;

        set_process_priority(g_proc_table[i].m_pid, g_proc_table[i].m_priority);

        sp = k_alloc_stack((g_proc_table[i]).m_stack_size);
        *(--sp) = INITIAL_xPSR;      // user process initial xPSR
        *(--sp) = (U32)((g_proc_table[i]).mpf_start_pc); // PC contains the entry point of the process
        for (int j = 0; j < 6; j++ ) { // R0-R3, R12(interprocess scratch register), R14(Link Register) are cleared with 0
            *(--sp) = 0x0;
        }
        gp_pcbs[i]->sp = sp;
    }

		// This variable must be set before we can use memory management functionality
    heap_high_address = gp_stack;

		// Add all processes to ready queues
		// Now memory is setup. Not before.
    for (int i = 0; i < NUM_PROCS; ++i) {
				list_push(&g_queues[g_proc_table[i].m_priority], gp_pcbs[i]);
		} 
}

int process_switch(PCB *p_pcb_old) {
    PROC_STATE state = gp_current_process->state;

    // Initalize new processes by popping execution stack
		if (state == NEW) {
				if (gp_current_process != p_pcb_old && p_pcb_old->state != NEW) {
						p_pcb_old->state = READY;
						p_pcb_old->sp = ((U32 *)__get_MSP());
				}
				gp_current_process->state = RUNNING;
				__set_MSP((U32) gp_current_process->sp);
				__rte();  // pop exception stack frame from the stack for a new processes
		}

    // Switch processes if new process is READY
    if (gp_current_process != p_pcb_old) {
        if (state == READY) {
            p_pcb_old->state = READY;
            p_pcb_old->sp = (U32 *) __get_MSP(); // save the old process's sp
            gp_current_process->state = RUNNING;
            __set_MSP((U32) gp_current_process->sp); //switch to the new proc's stack
        } else {
            gp_current_process = p_pcb_old; // revert back to the old proc on error
					  return RTX_ERROR;
        }
    }

    return RTX_OK;
}

int k_release_processor(void) {
    PCB *p_pcb_old = NULL;

    p_pcb_old = gp_current_process;
    gp_current_process = scheduler();

    // revert back to the old process if the scheduler cannot
    // find a new process to execute
    if (gp_current_process == NULL) {
        gp_current_process = p_pcb_old;
        return RTX_ERROR;
    }

    // if this is our first release, p_pcb_old is NULL and we
    // should emulate switching from the current process
    if (p_pcb_old == NULL) {
        p_pcb_old = gp_current_process;
    }

    // if the previous process is RUNNING (ie. it was not blocked),
    // we should re-add it to the relevant ready queue
    // NOTE: if the process was blocked, the blocking code should
    // have added it to the relevant blocked queue
		if (p_pcb_old->state == RUNNING) {
				list_push(&g_queues[get_process_priority(p_pcb_old->pid)], p_pcb_old);
		}

    return process_switch(p_pcb_old);
}

int set_process_priority(int process_id, int priority) {
    if (process_id == 0 && priority != 4) {
        return RTX_ERROR_PROCESS_CHANGING_NULL_PROCESS_PRIORITY;
    }
    if (process_id != 0 && priority == 4) {
        return RTX_ERROR_PROCESS_SETTING_NULL_PRIORITY_TO_NON_NULL_PROCESS;
    }
    if (priority < 0 || priority > 4) {
        // priority out of bounds
        return RTX_ERROR_PROCESS_PRIORITY_DOESNT_EXIST;
    }

    for (int i = 0; i < (sizeof(g_proc_table) / sizeof(g_proc_table[0])); ++i) {
        if (g_proc_table[i].m_pid == process_id) {
            g_proc_table[i].m_priority = priority;
					
						k_release_processor();
            return RTX_OK;
        }
    }

    return RTX_ERROR_PROCESS_PID_NOT_FOUND;
}

int get_process_priority(int process_id) {
    for (int i = 0; i < (sizeof(g_proc_table) / sizeof(g_proc_table[0])); ++i) {
        if (g_proc_table[i].m_pid == process_id) {
            return g_proc_table[i].m_priority;
        }
    }

    return RTX_ERROR_PROCESS_PID_NOT_FOUND;
}

PCB *scheduler(void) {
    PCB *process;

    for (int i = 0; i < NUM_READY_QUEUES; ++i) {
        if (!list_empty(&g_queues[i])) {
            process = list_front(&g_queues[i]);
            list_shift(&g_queues[i]);
            return process;
        }
    }

    return RTX_ERROR_PROCESS_SCHEDULER_EMPTY;
}
