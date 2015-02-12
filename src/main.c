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
#include "uart_polling.h"

#if DEBUG
    #include "test/list_test.h"
    #include "test/mem_test.h"
#endif // DEBUG

int main() {
    SystemInit();
    uart0_init();
  
    // Printf operations are no-ops unless DEBUG is set.
    init_printf(NULL);
    printf("\n\r\n\r");

    memory_init();

#if DEBUG
    // run_list_tests();
#endif // DEBUG

    process_init();
    scheduler_init();

#if DEBUG
    // TODO: This needs to be refactored since calling release_processor()
    // from these functions (ie. when they use memory management) causes a
    // hard fault
    //run_mem_tests();
#endif // DEBUG

    release_processor();
    return RTX_OK;
}
