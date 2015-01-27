#include "process.h"

/* globals */
PCB **gp_pcbs;
U32 *gp_stack;
PROC_CTX g_proc_table[NUM_PROCS];

void process_init() {
    PCB *new_proc;
    U32 pid = 0;
    U32 *stacks = gp_stack;
    void (*fcn_ptrs[NUM_PROCS]) ();
    
    fcn_ptrs[0] = &null_proc;
    
    // Possible optimization
    // Init NULL proc seperatly to free up memory
    for (int i = 0; i < NUM_PROCS; i++, pid++) {
        
        new_proc = gp_pcbs[i];
        new_proc->pid = pid;
        

        stacks += STACK_SIZE;
        
        new_proc->state = READY;

        /* Fill out the initial ready queue */        

		g_proc_table[i].m_pid = pid;
		g_proc_table[i].m_stack_size = STACK_SIZE;
		g_proc_table[i].m_pc = fcn_ptrs[i];
        
        sp = alloc_stack((g_proc_table[i]).m_stack_size);
		*(--sp) = INITIAL_xPSR;      // user process initial xPSR  
		*(--sp) = (U32)((g_proc_table[i]).m_pc); // PC contains the entry point of the process
		for (int j = 0; j < 6; j++ ) { // R0-R3, R12(interprocess scratch register), R14(Link Register) are cleared with 0
			*(--sp) = 0x0;
		}
         new_proc->sp = sp;
	}
    
    heap_high_address = gp_stack;
}

void null_proc(void) {
    while(1);
}