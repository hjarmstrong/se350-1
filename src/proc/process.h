#ifndef PROCESS_H
#define PROCESS_H

#include "../list/list.h"
#include "../rtx.h"
#include "sys_process.h"
#include "../../auto_tests/usr_proc.h"

/* Process Structs */

typedef enum PROC_STATE {
    NEW,
    READY,
    RUNNING,
    BLOCKED_ON_MEMORY,
		BLOCKED_ON_RECEIVE
} PROC_STATE;

typedef struct PCB {
    U32 pid;
    void *sp;
    PROC_STATE state;
		List msg_queue;
} PCB;

/* Global Variables */

#define NUM_PROCS (1 + NUM_TEST_PROCS)
#define STACK_SIZE 0x100

extern PCB **gp_pcbs;
extern U32 *gp_stack;
extern PROC_INIT g_proc_table[NUM_PROCS];
extern PCB *gp_current_process;

/* Kernel Process Methods */

extern void k_process_init(void);
#define process_init() _process_init((U32)k_process_init)
extern int _process_init(U32 p_func) __SVC_0;

/* Process Methods */

int k_process_switch(PCB *p_pcb_old);
PCB *k_get_pcb_from_pid(int pid);

#endif // PROCESS_H
