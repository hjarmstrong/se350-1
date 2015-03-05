#include "crt.h"

#include <LPC17xx.h>

#include "../rtx.h"
#include "../sys/uart.h"
#include "../util/string.h"

void crt_proc(void) {
    msgbuf *buf;
    int pid_from = -1;

    uart_irq_init(0); // interrupt driven

    set_process_priority(PID_KCD, HIGH);
    while(1) {
        buf = receive_message(&pid_from);
        k_crt_write_output_buffer(buf->mtext);
        release_memory_block(buf->mtext);
        release_processor();
    }
}

void crt_send_char(char c) {
    msgbuf *buf = request_memory_block();
    buf->mtype = DEFAULT;
    buf->mtext[0] = c;
    buf->mtext[1] = '\0';
    send_message(PID_CRT, buf);
}

void crt_send_string(const unsigned char* input) {
    msgbuf *buf = request_memory_block();
    buf->mtype = DEFAULT;
    strncpy(buf->mtext, (const char*) input, 64);
    buf->mtext[64] = 0;
    send_message(PID_CRT, buf);
}
