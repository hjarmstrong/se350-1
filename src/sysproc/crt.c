#include "crt.h"

#include <LPC17xx.h>

#include "../rtx.h"
#include "../sys/uart.h"
#include "../util/string.h"

void crt_proc(void) {
    msgbuf *buf;
    int pid_from = -1;

    uart_irq_init(0); // interrupt driven

    while(1) {
        buf = receive_message(&pid_from);
        crt_write_output_buffer(buf->mtext);
        if (buf->mtype != CALLER_MANAGED_PRINT) {
            release_memory_block(buf);
        }
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

void crt_send_string(const char* input) {
    msgbuf *buf = request_memory_block();
    buf->mtype = DEFAULT;
    strncpy(buf->mtext, (const char*) input, get_output_buffer_size());
    buf->mtext[get_output_buffer_size()] = 0;
    send_message(PID_CRT, buf);
}
