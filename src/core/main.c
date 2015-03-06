/**
 * main.c -- Launches the RTX
 */

#include <LPC17xx.h>
#include "../rtx.h"

#include "../core/mem.h"
#include "../core/process.h"
#include "../core/scheduler.h"
#include "../sys/timer.h"
#include "../sys/uart_polling.h"

int main() {
    SystemInit();
	  __disable_irq();
    uart1_init();     // polling
	  __enable_irq();

    memory_init();

    process_init();
    scheduler_init();
    timer_init(0);

    release_processor();
    ASSERT(0); // Not reached
}
