#include "../core/scheduler.h"
#include "../rtx.h"
#include "../sysproc/crt.h"
#include "../util/string.h"
#include "debug.h"

#ifdef _DEBUG_HOTKEYS
#define DEBUG_BUFFER_SIZE 40

static char display_buffer[DEBUG_BUFFER_SIZE];

static void cls() {
    strncpy(display_buffer, "\r\n", DEBUG_BUFFER_SIZE);
    crt_send_string(display_buffer);
}

// Assumes pid is less than three digits
static void copy_integer(int *idx, int pid) {
	  if (pid >= 10) {
    		display_buffer[(*idx)++] = (pid / 10) + '0';
		}
    display_buffer[(*idx)++] = (pid % 10) + '0';
}
#endif // _DEBUG_HOTKEYS

void debug_proc() {
#ifdef _DEBUG_HOTKEYS
    int i, j, idx;
    int recipient = -1;
    msgbuf *kcd_reg = request_memory_block();
    msgbuf *received = NULL;

    kcd_reg->mtype = KCD_REG;
    strncpy(kcd_reg->mtext, "D", 2);
    send_message(PID_KCD, kcd_reg);

    while (1) {
        received = receive_message(&recipient);
				if (!strncmp(received->mtext, "%DR", 3)) {
					  strncpy(display_buffer, "Ready Queues", 12);
					  crt_send_string(display_buffer);
					  cls();
						for (i = 0; i < PNULL; ++i) {
							  strncpy(display_buffer, "> Queue X:", 10);
								display_buffer[8] = i + '0';

							  idx = strlen(display_buffer);
						    for (j = 0; j < NUM_PROCS; ++j) {
                    if (g_queues[i][j] != NULL) {
											  display_buffer[idx++] = ' ';
											  copy_integer(&idx, g_queues[i][j]->pid);
										}
								}
							  crt_send_string(display_buffer);
								cls();
						}
				} else if (!strncmp(received->mtext, "%DBR", 4)) {
					  strncpy(display_buffer, "Blocked on Receive Queue", 25);
					  crt_send_string(display_buffer);
					  cls();

						strncpy(display_buffer, "> Queue:", 8);

						idx = strlen(display_buffer);
						for (i = 0; i < NUM_PROCS; ++i) {
								if (g_queues[PRIORITY_BLOCKED_ON_RECEIVE][i] != NULL) {
										display_buffer[idx++] = ' ';
									  copy_integer(&idx, g_queues[PRIORITY_BLOCKED_ON_RECEIVE][i]->pid);
								}
						}
						crt_send_string(display_buffer);
						cls();
				} else if (!strncmp(received->mtext, "%DBM", 4)) {
					  strncpy(display_buffer, "Blocked on Memory Queue", 24);
					  crt_send_string(display_buffer);
					  cls();

						strncpy(display_buffer, "> Queue:", 8);

						idx = strlen(display_buffer);
						for (i = 0; i < NUM_PROCS; ++i) {
								if (g_queues[PRIORITY_BLOCKED_ON_MEMORY][i] != NULL) {
										display_buffer[idx++] = ' ';
									  copy_integer(&idx, g_queues[PRIORITY_BLOCKED_ON_MEMORY][i]->pid);
								}
						}
						crt_send_string(display_buffer);
						cls();
				}
    }
#endif // _DEBUG_HOTKEYS

		set_process_priority(PID_B, LOWEST);
		while (1) {
			  release_processor();
		}
}
