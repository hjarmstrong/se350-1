#include "scheduler.h"

#ifdef DEBUG
   #include "../printf.h"
#endif // DEBUG

List g_queues[NUM_QUEUES];

void k_scheduler_init() {
    // Add all processes to ready queues
    for (int i = 0; i < NUM_PROCS; ++i) {
        k_enqueue_process(g_proc_table[i].m_pid);
#ifdef DEBUG
        //printf("Front of queue %d:", 0);
        //print_list(&g_queues[0]);
        //printf("Front of queue %d:", 4);
        //print_list(&g_queues[4]);
#endif // DEBUG
    } 
}

int k_enqueue_process(int process_id) {
    for (int i = 0; i < (sizeof(g_proc_table) / sizeof(g_proc_table[0])); ++i) {
        if (g_proc_table[i].m_pid == process_id) {
            switch (gp_pcbs[i]->state) {
                case NEW:
                case READY:
                case RUNNING:
                    list_push(&g_queues[g_proc_table[i].m_priority], gp_pcbs[i]);
                    return RTX_OK;
                case BLOCKED:
                    // TODO: change PRIORITY_BLOCKED_ON_MEMORY to generic case
                    list_push(&g_queues[PRIORITY_BLOCKED_ON_MEMORY], gp_pcbs[i]);
                    return RTX_OK;
                default:
                    return RTX_ERROR_SCHEDULER_UNDEFINED_STATE;
            }
        }
    }

    return RTX_ERROR_SCHEDULER_PID_NOT_FOUND;
}

int k_unblock_queue(int blocked_queue) {
    PCB *process;

    if (blocked_queue < PRIORITY_BLOCKED_ON_MEMORY || blocked_queue > PRIORITY_BLOCKED_ON_MEMORY) {
        return RTX_ERROR_SCHEDULER_UNBLOCKING_NON_BLOCK_QUEUE;
    }

    while (!list_empty(&g_queues[blocked_queue])) {
        process = list_front(&g_queues[blocked_queue]);
        list_shift(&g_queues[blocked_queue]);

        process->state = READY;
        k_enqueue_process(process->pid);
    }

    return RTX_OK;
}

PCB *scheduler(void) {
    PCB *process;

#ifdef DEBUG
    //for (int i = 0; i < NUM_QUEUES; ++i) {
        //printf("Front of queue %d:", i);
        //print_list(&g_queues[i]);
    //}
#endif // DEBUG

    for (int i = 0; i < NUM_READY_QUEUES; ++i) {
        if (!list_empty(&g_queues[i])) {
            process = list_front(&g_queues[i]);
            list_shift(&g_queues[i]);
            return process;
        }
    }

    return RTX_ERROR_SCHEDULER_EMPTY;
}

int get_process_priority(int process_id) {
    for (int i = 0; i < (sizeof(g_proc_table) / sizeof(g_proc_table[0])); ++i) {
        if (g_proc_table[i].m_pid == process_id) {
            return g_proc_table[i].m_priority;
        }
    }

    return RTX_ERROR_SCHEDULER_PID_NOT_FOUND;
}

int set_process_priority(int process_id, int priority) {
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

    for (int i = 0; i < (sizeof(g_proc_table) / sizeof(g_proc_table[0])); ++i) {
        if (g_proc_table[i].m_pid == process_id) {
            g_proc_table[i].m_priority = priority;

            k_enqueue_process(g_proc_table[i].m_pid);
            k_release_processor();
            return RTX_OK;
        }
    }

    return RTX_ERROR_SCHEDULER_PID_NOT_FOUND;
}
