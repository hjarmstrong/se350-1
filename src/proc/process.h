#ifndef PROCESS_H
#define PROCESS_H

#include "../list/list.h"
#include "../stdefs.h"
#include "sys_process.h"
#include "user_process.h"

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

/* Process Structs */

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

/* Global Variables */

#define NUM_PROCS (1 + NUM_TEST_PROCS)
#define STACK_SIZE 0x100

extern PCB **gp_pcbs;
extern U32 *gp_stack;
extern PROC_INIT g_proc_table[NUM_PROCS];
extern PCB *gp_current_process;
extern List g_queues[NUM_QUEUES];

/* Kernel Process Methods */

extern void k_process_init(void);
#define process_init() _process_init((U32)k_process_init)
extern int _process_init(U32 p_func) __SVC_0;

extern int k_release_processor(void);
#define release_processor() _release_processor((U32)k_release_processor)
extern int __SVC_0 _release_processor(U32 p_func);

/* Process Methods */

int set_process_priority(int process_id, int priority);
int get_process_priority(int process_id);

int process_switch(PCB *p_pcb_old);

PCB *scheduler(void);

#endif // PROCESS_H
