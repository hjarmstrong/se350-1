#ifndef IPC_H
#define IPC_H

#include "../rtx.h"

typedef struct msg_metadata {
		int sender_pid;
		int destination_pid;
		U32 min_delivery_time;
} msg_metadata;
	
#endif // IPC_H
