/**
 * @brief timer.h - Timer header file
 * @author Y. Huang
 * @date 2013/02/12
 */
#ifndef _CRT_H_
#define _CRT_H_

#include "../rtx.h"

extern volatile U32 g_timer_count; // current time in milliseconds
extern U32 uart_irq_init ( U32 n_uart );  /* initialize timer n_timer */
void c_UART0_IRQ_Handler(void);
int get_input_buffer_size(void);

#endif /* ! _TIMER_H_ */
