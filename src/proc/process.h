#ifndef PROCESS_H
#define PROCESS_H

#include "../list/list.h"
#include "../stdefs.h"
#include "sys_process.h"
#include "user_process.h"

#define NUM_PROCS (1 + NUM_TEST_PROCS)

/* 5 priorities plus blocking methods
   blocked methods: out of memory */
#define PRIORITY_BLOCKED_ON_MEMORY 5
#define NUM_BLOCKED_QUEUES 1

#define NUM_READY_QUEUES 5
#define NUM_QUEUES (NUM_READY_QUEUES + NUM_BLOCKED_QUEUES)

/* 512 / 4 == 128. => 512 Bytes per stack = ((U32)128) */
#define STACK_SIZE 0x100

#define INITIAL_xPSR 0x01000000

typedef enum PROC_STATE {
		NEW,
    READY,
    RUNNING,
    BLOCKED
} PROC_STATE;

typedef struct PCB {
    U32 pid;
    void *sp;
    PROC_STATE state;
} PCB;

typedef struct PROC_INIT {
    int m_pid;
    int m_priority;
    int m_stack_size;
    void (*mpf_start_pc) ();
} PROC_INIT;

// GLOBALS
extern PCB **gp_pcbs;
extern U32 *gp_stack;
extern PROC_INIT g_proc_table[NUM_PROCS];
extern PCB *gp_current_process;
extern List g_queues[NUM_QUEUES];

int set_process_priority(int process_id, int priority);
int get_process_priority(int process_id);

int process_switch(PCB *p_pcb_old);

void process_init(void);

extern void k_queue_init(void);
#define queue_init() _queue_init((U32)k_queue_init)
extern void *_queue_init(U32 p_func) __SVC_0;

extern int k_release_processor(void);
#define release_processor() _release_processor((U32)k_release_processor)
extern int __SVC_0 _release_processor(U32 p_func);

PCB *scheduler(void);

#endif // PROCESS_H
