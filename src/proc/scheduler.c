/**
 * proc/scheduler.c -- Process scheduler implementation
 */

#include "scheduler.h"

#include "../printf.h"

#include "../uart_polling.h"

PCB *g_queues[NUM_QUEUES][NUM_PROCS];

void k_scheduler_init() {
    int i;
    int j;

    // Clear queues
    for (i = 0; i < NUM_QUEUES; ++i) {
        for (j = 0; j < NUM_PROCS; ++j) {
            g_queues[i][j] = NULL;
        }
    }

    // Add all processes to ready queues
    for (i = 0; i < NUM_PROCS; ++i) {
        k_enqueue_process(g_proc_table[i].m_pid);
    } 
}

int k_dequeue_process(int process_id) {
    int found_process = 0;
    int i;
    int j;

    for (i = 0; i < NUM_QUEUES && !found_process; ++i) {
        for (j = 0; j < NUM_PROCS; ++j) {
            if (found_process) {
                g_queues[i][j - 1] = g_queues[i][j];
            } else if (g_queues[i][j]->pid == process_id) {
                found_process = 1;
            }
        }
        if (found_process) {
            g_queues[i][NUM_PROCS - 1] = NULL;
        }
    }

    return found_process ? RTX_OK : RTX_ERR;
}

#define NOT_FOUND_YET -1
#define UNDEFINED_STATE -2

int k_enqueue_process(int process_id) {
    int i;
    int priority = NOT_FOUND_YET;
    PCB* pcb = NULL;

    if (process_id < 0 && process_id >= -NUM_IPROCS) {
        return RTX_OK; // Don't put i-processes in the queue
    }

    for (i = 0; i < NUM_PROCS && priority == NOT_FOUND_YET; ++i) {
        if (g_proc_table[i].m_pid == process_id) {
            switch (gp_pcbs[i]->state) {
                case NEW:
                case READY:
                case RUNNING:
                    pcb = gp_pcbs[i];
                    priority = g_proc_table[i].m_priority;
                    break;
                case BLOCKED_ON_MEMORY:
                    pcb = gp_pcbs[i];
                    priority = PRIORITY_BLOCKED_ON_MEMORY;
                    break;
                case BLOCKED_ON_RECEIVE:
                    pcb = gp_pcbs[i];
                    priority = PRIORITY_BLOCKED_ON_RECEIVE;
                    break;
                default:
                    priority = UNDEFINED_STATE;
                    break;
            }
        }
    }

    if (priority == NOT_FOUND_YET) {
        return RTX_ERROR_SCHEDULER_PID_NOT_FOUND;
    } else if (priority == UNDEFINED_STATE) {
        return RTX_ERROR_SCHEDULER_UNDEFINED_STATE;
    }

    for (i = 0; i < NUM_PROCS; ++i) {
        if (!g_queues[priority][i]) {
            g_queues[priority][i] = pcb;
            break;
        }
        if (i == NUM_PROCS - 1) {
            return RTX_ERR; // More than NUM_PROC procs or broken enqueue/dequeue code?
        }
    }

    return RTX_OK;
}

int k_unblock_queue(int blocked_queue) {
    PCB *process;
    int i;

    if (blocked_queue < PRIORITY_BLOCKED_ON_MEMORY || blocked_queue > PRIORITY_BLOCKED_ON_RECEIVE) {
        return RTX_ERROR_SCHEDULER_UNBLOCKING_NON_BLOCK_QUEUE;
    }

    for (i = 0; g_queues[blocked_queue][i]; ++i) {
        process = g_queues[blocked_queue][i];
        g_queues[blocked_queue][i] = NULL;

        process->state = READY;
        k_enqueue_process(process->pid);
    }

    return RTX_OK;
}

PCB *k_scheduler(void) {
    PCB *process;
    int i;
    int j;

    for (i = 0; i < NUM_READY_QUEUES; ++i) {
        if (g_queues[i][0]) {
            process = g_queues[i][0];
            for (j = 1; j < NUM_PROCS; ++j) {
                g_queues[i][j - 1] = g_queues[i][j];
            }
            g_queues[i][NUM_PROCS - 1] = NULL;
            return process;
        }
    }

    return RTX_ERROR_SCHEDULER_EMPTY;
}

int k_get_process_priority(int process_id) {
    int i;

    for (i = 0; i < NUM_PROCS; ++i) {
        if (g_proc_table[i].m_pid == process_id) {
            return g_proc_table[i].m_priority;
        }
    }

    return RTX_ERROR_SCHEDULER_PID_NOT_FOUND;
}

int k_set_process_priority(int process_id, int priority) {
    int i;

    if (process_id == 0 && priority != 4) {
        return RTX_ERROR_SCHEDULER_CHANGING_NULL_PROCESS_PRIORITY;
    }
    if (process_id != 0 && priority == 4) {
        return RTX_ERROR_SCHEDULER_SETTING_NULL_PRIORITY_TO_NON_NULL_PROCESS;
    }
    if (priority < 0 || priority > 4) {
        // priority out of bounds
        return RTX_ERROR_SCHEDULER_PRIORITY_DOESNT_EXIST;
    }

    for (i = 0; i < NUM_PROCS; ++i) {
        if (g_proc_table[i].m_pid == process_id) {
            g_proc_table[i].m_priority = priority;

            if (g_proc_table[i].m_pid != gp_current_process->pid) {
                k_dequeue_process(g_proc_table[i].m_pid);
                k_enqueue_process(g_proc_table[i].m_pid);
            }
            k_release_processor();
            return RTX_OK;
        }
    }

    return RTX_ERROR_SCHEDULER_PID_NOT_FOUND;
}
