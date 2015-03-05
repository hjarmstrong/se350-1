#ifndef UART_H
#define UART_H

#include "../rtx.h"
#include "uart_def.h"

extern U32 uart_irq_init ( U32 n_uart );  /* initialize timer n_timer */
void c_UART0_IRQ_Handler(void);
int get_input_buffer_size(void);
int get_output_buffer_size(void);
void k_crt_write_output_buffer(const char*);

#endif // UART_H
