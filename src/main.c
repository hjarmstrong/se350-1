#include <LPC17xx.h>

#include "mem/mem.h"
#include "proc/process.h"
#include "stdefs.h"
#include "uart_polling.h"

#ifdef DEBUG
    #include "printf.h"

    #include "test/list_test.h"
    #include "test/mem_test.h"
#endif // DEBUG

int main() {
    SystemInit();

		uart0_init();
#ifdef DEBUG
    init_printf(NULL);
#endif // DEBUG

    memory_init();
		list_init();
		process_init();

#ifdef DEBUG
    // These need to be refactored since calling release_processor() from
    // these functions (ie. when they use memory management) causes a hard
    // fault
    //run_list_tests();
    //run_mem_tests();
#endif // DEBUG

		release_processor();
    return 0;
}
