#ifndef SCHEDULER_H
#define SCHEDULER_H

#include "../list/list.h"
#include "process.h"
#include "../stdefs.h"

/* Priority Levels and Queues */

#define PRIORITY_HIGHEST 0
#define PRIORITY_HIGH 1
#define PRIORITY_MEDIUM 2
#define PRIORITY_LOW 3
#define PRIORITY_NULL 4
#define NUM_READY_QUEUES 5

#define PRIORITY_BLOCKED_ON_MEMORY 5
#define NUM_BLOCKED_QUEUES 1

#define NUM_QUEUES (NUM_READY_QUEUES + NUM_BLOCKED_QUEUES)

/* Global Variables */

extern List g_queues[NUM_QUEUES];

/* Kernel Scheduler Methods */

extern void k_scheduler_init(void);
#define scheduler_init() _scheduler_init((U32)k_scheduler_init)
extern int _scheduler_init(U32 p_func) __SVC_0;

/* Scheduler Methods */

PCB *scheduler(void);

int k_enqueue_process(int process_id);
int k_dequeue_process(int process_id);
int k_unblock_queue(int blocked_queue);

/* Priority Methods */

int get_process_priority(int process_id);
int set_process_priority(int process_id, int priority);

#endif // SCHEDULER_H
