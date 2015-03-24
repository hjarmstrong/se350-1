#include "rtx.h"
#include "../core/mem.h"
#include "../util/list.h"
#include "../util/string.h"
#include "../sys/uart_polling.h"

#include "stress_test_procs.h"

#define TEN_SECONDS 5000

#define QUEUE_SIZE 200

void *queue[QUEUE_SIZE];
int queue_pointer;

void proc_a(void) {
    msgbuf *kcd_reg = request_memory_block();
    int num = 0;
    msgbuf *p = NULL;
    int sender = 1;

    kcd_reg->mtype = KCD_REG;
    strncpy(kcd_reg->mtext, "Z", 2);
    send_message(PID_KCD, kcd_reg);//register to handle %Z commands

    while(1) {
        p = receive_message(&sender);
        if (!strncmp(p->mtext, "Z", 1)){//if the message contains the %Z command
            release_memory_block(p);
            break;
        } else {
            release_memory_block(p);
        }
    }

    while(1) {
        p = request_memory_block();
        p->mtype = COUNT_REPORT;
        int_to_c_string(num, p->mtext);
        send_message(PID_B, p);
        ++num;
        release_processor();
    }
}

void proc_b(void) {//passes a message from A to C
    msgbuf *msg;
    int sender;

    while(1) {
        msg = receive_message(&sender);
        send_message(PID_C, msg);
    }
}

void proc_c(void){
    msgbuf *p;
    msgbuf *wait = request_memory_block();
    int p_data_int;
    int sender;

    queue_pointer = 0;

    while(1) {
        if (queue_pointer == 0) {
            p = receive_message(&sender);
        } else {
            p = queue[queue_pointer--];
        }

        if (p->mtype == COUNT_REPORT) {
            p_data_int = c_string_to_int(p->mtext);
            if (p_data_int % 20 == 0) {
                p->mtype = DEFAULT;//specification says we need to use p
                strncpy(p->mtext, "Process C\r\n", 12);
                send_message(PID_CRT, p);
                print_memory();

                wait->mtype = WAKEUP10;
                sender = delayed_send(PID_C, wait, TEN_SECONDS);//hibernate for 10 sec
                while(1) {
                    p = receive_message(&sender); //block and let other processes execute
                    if (p->mtype == WAKEUP10) {
                        break;
                    } else {
                        queue[queue_pointer++] = p;
                        if (queue_pointer >= QUEUE_SIZE) {
                            release_memory_block(queue[queue_pointer--]);
                        }
                    }
                }
            } else {
                release_memory_block(p);
            }
        } else {
            release_memory_block(p);
        }

        release_processor();
    }
}
