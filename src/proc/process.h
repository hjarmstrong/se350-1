#ifndef PROCESS_H
#define PROCESS_H

#include "../stdefs.h"

#define NUM_PROCS 1

#define PROC_1 &null_proc

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

typedef struct PROC_CTX
{	
	int m_pid;
	int m_priority;
	int m_stack_size;       /* size of stack in words */
	void (*m_pc) ();  
} PROC_CTX;

//PCB **pcb_ptrs;
extern PCB **gp_pcbs;

extern U32 *gp_stack;

extern PROC_CTX g_proc_table[NUM_PROCS];

/* User Processes */

void null_proc(void);

#endif
