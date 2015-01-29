#ifndef PROCESS_H
#define PROCESS_H

#include "../stdefs.h"
#include "sys_process.h"
#include "user_process.h"

#define NUM_PROCS (1 + NUM_TEST_PROCS)

/* 5 priorities plus blocking methods
   blocked methods: out of memory */
#define NUM_QUEUES (5 + 1)

/* 512 / 4 == 128. => 512 Bytes per stack */
#define STACK_SIZE ((U32)128)

#define INITIAL_xPSR 0x01000000

typedef enum PROC_STATE {
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

extern PCB **gp_pcbs;

extern U32 *gp_stack;

extern PROC_INIT g_proc_table[NUM_PROCS];

int set_process_priority(int process_id, int priority);
int get_process_priority(int process_id);

int process_switch(PCB *p_pcb_old);

void process_init(void);
void queue_init(void);

extern int k_release_processor(void);
#define release_processor() _release_processor((U32)k_release_processor)
extern int __SVC_0 _release_processor(U32 p_func);

PCB *scheduler(void);

#endif // PROCESS_H
