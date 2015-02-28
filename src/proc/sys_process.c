#include <LPC17xx.h>

#include "process.h"
#include "../rtx.h"
#include "sys_process.h"
#include "scheduler.h"
#include "../uart.h"
#include "../uart_polling.h"
#include "../printf.h"

char gp_input_buffer[] = "WE RESERVE ENOUGH SPACE TO SEND 51 BYTES TO THE KCD";
char gp_output_buffer[] = "WE RESERVE ENOUGH SPACE TO SEND 52 BYTES TO THE UART";

U8 gp_buffer_index = 0;

void null_proc(void) {
        printf("Running null process\n\r");
    while(1) {
        release_processor();
    }
}

// Pre condition, interupts are disabled when the ISR schedules this process.
void uart0_iproc(void) {

    
    
}

void timer_iproc(void) {
}
