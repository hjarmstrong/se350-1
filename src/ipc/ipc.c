#include "ipc.h"
#include "../list/list.h"
#include "../proc/process.h"
#include "../proc/scheduler.h"
#include "../timer/timer.h"
#include "../rtx.h"

msg_metadata *get_message_metadata(void * message_envelope) {
		return (msg_metadata *) (((char *)message_envelope) + BLOCK_SIZE - sizeof(msg_metadata));
}

int k_send_message(int destination_proc_id, void *message_envelope) {
		return k_delayed_send(destination_proc_id, message_envelope, 0);
}

int k_delayed_send(int destination_proc_id, void *message_envelope, int delay) {
		PCB *receiving_proc;
		msg_metadata *metadata = get_message_metadata(message_envelope);

		__disable_irq();

		metadata->sender_pid = gp_current_process->pid;
	  metadata->destination_pid = destination_proc_id;
	  metadata->min_delivery_time = g_timer_count + delay;
	
		receiving_proc = k_get_pcb_from_pid(destination_proc_id);
		list_push(&receiving_proc->msg_queue, message_envelope);

		if (receiving_proc->state == BLOCKED_ON_RECEIVE) {
				receiving_proc->state = READY;
				k_dequeue_process(destination_proc_id);
				k_enqueue_process(destination_proc_id);

				if (k_get_proc_table_from_pid(destination_proc_id)->m_priority >
								k_get_proc_table_from_pid(metadata->sender_pid)->m_priority) {
						__enable_irq();
						k_release_processor();
						__disable_irq();
				}
		}

		__enable_irq();

		return RTX_OK;
}

void *k_receive_message(int *sender_id) {//blocks
		void *env;
		msg_metadata *metadata;

		while (list_empty(&gp_current_process->msg_queue)) {
				gp_current_process->state = BLOCKED_ON_RECEIVE;
				k_release_processor();
		}

		env = list_front(&gp_current_process->msg_queue);
		
		metadata = get_message_metadata(env);
		while (metadata->min_delivery_time > g_timer_count) {
				gp_current_process->state = BLOCKED_ON_RECEIVE;
				k_release_processor();
		}
		
		__disable_irq();
		list_shift(&gp_current_process->msg_queue);
		__enable_irq();

		return env;
}
