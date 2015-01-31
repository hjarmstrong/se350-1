#include "process.h"
#include "../mem/mem.h"
#include "../svc/hal.h"
#include <LPC17xx.h>

#include "../printf.h"

/* globals */
PCB **gp_pcbs;
U32 *gp_stack;
PROC_INIT g_proc_table[NUM_PROCS];
PCB *gp_current_process = NULL;
List g_queues[NUM_QUEUES];

/**
 * Requires k_memory_init to be called.
 * This initializes processes and finalizes memory initalization.
 */
void process_init() {
    PCB *new_proc;
    U32 *sp;

    // Initialize null process table
		// Memory is not setup yet.
    g_proc_table[0].m_pid = 0;
    g_proc_table[0].m_stack_size = STACK_SIZE;
    g_proc_table[0].mpf_start_pc = &null_proc;
	  g_proc_table[0].m_priority = 4;

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
        new_proc = gp_pcbs[i];
        new_proc->pid = g_proc_table[i].m_pid;
        new_proc->state = NEW;

        set_process_priority(g_proc_table[i].m_pid, g_proc_table[i].m_priority);

        sp = alloc_stack((g_proc_table[i]).m_stack_size);
        *(--sp) = INITIAL_xPSR;      // user process initial xPSR
        *(--sp) = (U32)((g_proc_table[i]).mpf_start_pc); // PC contains the entry point of the process
        for (int j = 0; j < 6; j++ ) { // R0-R3, R12(interprocess scratch register), R14(Link Register) are cleared with 0
            *(--sp) = 0x0;
        }
        new_proc->sp = sp;
    }

    heap_high_address = gp_stack;

		// Add all processes to ready queues
		// Now memory is setup. Not before.
    for (int i = 0; i < NUM_PROCS; ++i) {
				list_push(&g_queues[g_proc_table[i].m_priority], gp_pcbs[i]);
		} 
}

int process_switch(PCB *p_pcb_old) {
    PROC_STATE state = gp_current_process->state;

		if (state == NEW) {
				if (gp_current_process != p_pcb_old && p_pcb_old->state != NEW) {
						p_pcb_old->state = READY;
						p_pcb_old->sp = ((U32 *)__get_MSP());
				}
				gp_current_process->state = RUNNING;
				__set_MSP((U32) gp_current_process->sp);
				__rte();  // pop exception stack frame from the stack for a new processes
		}
	
    if (gp_current_process != p_pcb_old) {
        if (state == READY) {
            p_pcb_old->state = READY;
            p_pcb_old->sp = (U32 *) __get_MSP(); // save the old process's sp
            gp_current_process->state = RUNNING;
            __set_MSP((U32) gp_current_process->sp); //switch to the new proc's stack
        } else {
            gp_current_process = p_pcb_old; // revert back to the old proc on error
					  return -1;
        }
    }

    return 0;
}

int k_release_processor(void) {
    PCB *p_pcb_old = NULL;

    p_pcb_old = gp_current_process;
    gp_current_process = scheduler();

    if (gp_current_process == NULL) {
        gp_current_process = p_pcb_old; // revert back to the old process
        return -1;
    }

    if (p_pcb_old == NULL) {
        p_pcb_old = gp_current_process;
    }

		if (p_pcb_old->state == RUNNING) {
				list_push(&g_queues[get_process_priority(p_pcb_old->pid)], p_pcb_old);
		}

    return process_switch(p_pcb_old);
}

int set_process_priority(int process_id, int priority) {
    if (process_id == 0 && priority != 4) {
        // tried to change priority of null process
        return -4;
    }
    if (priority == 4 && process_id != 0) {
        // tried to set priority = 4 for for non-null process
        return -3;
    }
    if (priority < 0 || priority > 4) {
        // priority out of bounds
        return -2;
    }

    for (int i = 0; i < (sizeof(g_proc_table) / sizeof(g_proc_table[0])); ++i) {
        if (g_proc_table[i].m_pid == process_id) {
            g_proc_table[i].m_priority = priority;
            return 0;
        }
    }

    // not found
    return -1;
}

int get_process_priority(int process_id) {
    for (int i = 0; i < (sizeof(g_proc_table) / sizeof(g_proc_table[0])); ++i) {
        if (g_proc_table[i].m_pid == process_id) {
            return g_proc_table[i].m_priority;
        }
    }

    // not found
    return -1;
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

    // since the null process always exists, this should never happen
    return NULL;
}
