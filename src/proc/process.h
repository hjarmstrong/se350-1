#ifndef PROCESS_H
#define PROCESS_H

#include "../list/list.h"
#include "../rtx.h"
#include "sys_process.h"
#include "../test/usr_proc.h"

/* Process Structs */

typedef enum PROC_STATE {
    NEW,
    READY,
    RUNNING,
    BLOCKED_ON_MEMORY,
    BLOCKED_ON_RECEIVE,
    BLOCKED_ON_UART_IO
} PROC_STATE;

typedef struct PCB {
    int pid;
    void *sp;
    PROC_STATE state;
    List msg_queue;
} PCB;

/* Global Variables */

#define NUM_IPROCS 2
#define NUM_SYS_PROCS (NUM_IPROCS + 1)
#define NUM_PROCS (NUM_SYS_PROCS + NUM_TEST_PROCS)
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
PROC_INIT *k_get_proc_table_from_pid(int pid);

#endif // PROCESS_H
