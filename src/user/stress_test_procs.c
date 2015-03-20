#include "../util/list.h"
#include "rtx.h"
#include "../util/string.h"

#include "stress_test_procs.h"

#define TEN_SECONDS 10000

void proc_a(void) {
    msgbuf *kcd_reg = request_memory_block();
    int num = 0;
    msgbuf *p = request_memory_block();
    int sender = 1;
    
    kcd_reg->mtype = KCD_REG;
    strncpy(kcd_reg->mtext, "Z", 2);
    send_message(PID_KCD, kcd_reg);//register to handle %Z commands
    
    while(1){
        p = receive_message(&sender);
        if (!strncmp(p->mtext, "Z", 1)){//if the message contains the %Z command
            release_memory_block(p);
            break;//exit loop
        }else{
            release_memory_block(p);
        }
    }
    
    while(1){
        p = request_memory_block();
        p->mtype = COUNT_REPORT;
        int_to_c_string(num, p->mtext);
        send_message(PID_B, p);
        num = num + 1;
        release_processor();
    }
}

void proc_b(void){//passes a message from A to C
    while(1){
        msgbuf *msg;
        int sender;
        msg = receive_message(&sender);
        send_message(PID_C, msg);
    }
}

void proc_c(void){
    msgbuf *p = request_memory_block();
    int p_data_int;
    msgbuf *wait;
    List queue = list_new(IS_USERSPACE);
    int sender;
    while(1){
        if (list_empty(&queue)){
            p = receive_message(&sender);
        }else{
            p = list_front(&queue);
            list_shift(&queue);
        }
        if (p->mtype == COUNT_REPORT){
            p_data_int = c_string_to_int(p->mtext);
            if (p_data_int % 20 == 0) {
                p->mtype = CALLER_MANAGED_PRINT;//specification says we need to use p
                strncpy(p->mtext, "Process C", 10);
                send_message(PID_CRT, p);
                
                wait = request_memory_block();
                wait->mtype = WAKEUP10;
                delayed_send(PID_C, wait, TEN_SECONDS);//hibernate for 10 sec
                while(1){
                    p = receive_message(&sender); //block and let other processes execute
                    if (p->mtype == WAKEUP10){
                        break;
                    } else {
                        list_push(&queue, p);
                    }
                }
            }
        }
        release_memory_block(p);
        release_processor();
    }
}
