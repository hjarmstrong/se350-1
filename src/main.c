#include <LPC17xx.h>

#include "mem/mem.h"
#include "printf.h"
#include "proc/process.h"
#include "stdefs.h"
#include "uart_polling.h"

#include "test/list_test.h"
#include "test/mem_test.h"

int main() {
    SystemInit();

		uart0_init();
    init_printf(NULL);

    memory_init();
		initialize_list_nodes();
		process_init();

    //run_list_tests();
    //run_mem_tests();

		release_processor();
    return 0;
}
