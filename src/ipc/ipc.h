#ifndef IPC_H
#define IPC_H

typedef struct msgbuf {
		int mtype; /* user defined message type */
		char mtext[1]; /* body of the message */
} msgbuf;

#endif // IPC_H
