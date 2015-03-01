#ifndef IPC_H
#define IPC_H

#include "../rtx.h"

typedef struct msg_metadata {
		int sender_pid;
		int destination_pid;
	  int send_time;
} msg_metadata;

msg_metadata *get_message_metadata(void * message_envelope);
	
#endif // IPC_H
