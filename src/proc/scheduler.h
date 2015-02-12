/**
 * proc/scheduler.h -- Internal methods used by the process scheduler
 */

#ifndef SCHEDULER_H
#define SCHEDULER_H

#include "../list/list.h"
#include "process.h"
#include "../rtx.h"

/*---- Constants ------------------------------------------------------------*/

#define PRIORITY_BLOCKED_ON_MEMORY 5
#define NUM_BLOCKED_QUEUES 1
#define NUM_READY_QUEUES 5

#define NUM_QUEUES (NUM_READY_QUEUES + NUM_BLOCKED_QUEUES)

/*---- Global variables -----------------------------------------------------*/

extern PCB *g_queues[NUM_QUEUES][NUM_PROCS];

/*---- Kernel Scheduler Methods ---------------------------------------------*/

extern void k_scheduler_init(void);
#define scheduler_init() _scheduler_init((U32)k_scheduler_init)
extern int _scheduler_init(U32 p_func) __SVC_0;

/**
 * Returns the PCB of the next process to be run.
 */
PCB *k_scheduler(void);

int k_enqueue_process(int process_id);
int k_dequeue_process(int process_id);
int k_unblock_queue(int blocked_queue);

#endif // SCHEDULER_H
