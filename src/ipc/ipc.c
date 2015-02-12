#include "ipc.h"
#include "../list/list.h"
#include "../proc/process.h"
#include "../proc/scheduler.h"
#include "../rtx.h"

int k_send_message(int destination_proc_id, void *message_envelope) {
		//int sender_proc_id; TODO: add back when we know why we need it.
		PCB *receiving_proc;
		__disable_irq();
	
		//sender_proc_id = gp_current_process->pid;
		receiving_proc = k_get_pcb_from_pid(destination_proc_id);
		list_push(&receiving_proc->msg_queue, message_envelope);
		if ( receiving_proc->state == BLOCKED_ON_RECEIVE ) {
				receiving_proc->state = READY;
				k_dequeue_process(destination_proc_id);
				k_enqueue_process(destination_proc_id);
		}
		__enable_irq();

		return RTX_OK;
}

void *k_receive_message(int *sender_id) {//blocks
		void *env;
	
		__disable_irq();
	
		while ( list_empty(&gp_current_process->msg_queue) ) {
				gp_current_process->state = BLOCKED_ON_RECEIVE ;
				k_release_processor ( ) ;
		}
		env = list_front(&gp_current_process->msg_queue);
		list_shift(&gp_current_process->msg_queue);
		
		__enable_irq();
		
		return env ;
}
