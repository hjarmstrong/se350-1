/**
 * main.c -- Launches the RTX
 */

#include <LPC17xx.h>
#include <system_LPC17xx.h>

#include "mem/mem.h"
#include "printf.h"
#include "proc/process.h"
#include "proc/scheduler.h"
#include "rtx.h"
#include "timer/timer.h"
#include "crt/crt.h"
#include "uart_polling.h"

#if DEBUG
    #include "test/list_test.h"
    #include "test/mem_test.h"
#endif // DEBUG

int main() {
    SystemInit();
	uart_irq_init(0);   // uart0, interrupt-driven 
	uart1_init();       // uart1, polling
  
    // Printf operations are no-ops unless DEBUG is set.
    init_printf(NULL);
    printf("\n\r\n\r");

    memory_init();

#if DEBUG
    // run_list_tests();
#endif // DEBUG

    process_init();
    scheduler_init();
		timer_init(0);

#if DEBUG
    // TODO: This needs to be refactored since calling release_processor()
    // from these functions (ie. when they use memory management) causes a
    // hard fault
    //run_mem_tests();
#endif // DEBUG

    release_processor();
    return RTX_OK;
}
