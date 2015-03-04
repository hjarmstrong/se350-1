/**
 * main.c -- Launches the RTX
 */

#include <LPC17xx.h>
#include <system_LPC17xx.h>
#include "../rtx.h"

#include "../core/mem.h"
#include "../core/process.h"
#include "../core/scheduler.h"
#include "../sys/timer.h"
#include "../sys/crt.h"
#include "../sys/uart_polling.h"

//TODO: remove this
#include "../sys/uart_def.h"
#include "../sys/uart.h"

int main() {
    LPC_UART_TypeDef *pUart;
	
    SystemInit();
	  __disable_irq();
    uart_irq_init(0); // interrupt driven
    uart1_init();     // polling
	  __enable_irq();

    memory_init();

    process_init();
    scheduler_init();
    timer_init(0);

    // TODO: THIS SHOULD BE SET IN THE CRT PROCESS
    pUart = (LPC_UART_TypeDef *) LPC_UART0;    
    pUart->IER = IER_RLS | IER_RBR; // Right now we only interput on data input
    // This is the command for interupts on output register clear (leaves reading interupts on)
    //pUart->IER = IER_THRE | IER_RLS | IER_RBR;

    release_processor();
    return RTX_OK;
}
