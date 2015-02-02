#include <LPC17xx.h>

#include "mem/mem.h"
#include "printf.h"
#include "proc/process.h"
#include "proc/scheduler.h"
#include "stdefs.h"
#include "uart_polling.h"

#ifdef DEBUG
    #include "test/list_test.h"
    #include "test/mem_test.h"
#endif // DEBUG

int main() {
    SystemInit();

		uart0_init();
    init_printf(NULL);

    memory_init();
    list_init();

#ifdef DEBUG
    run_list_tests();
#endif // DEBUG

		process_init();
    scheduler_init();

#ifdef DEBUG
    // TODO: This needs to be refactored since calling release_processor()
    // from these functions (ie. when they use memory management) causes a
    // hard fault
    //run_mem_tests();
#endif // DEBUG

		release_processor();
    return RTX_OK;
}
