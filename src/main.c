#include <LPC17xx.h>
#include "mem/mem.h"
#include "mem/test.h"
#include "printf.h"
#include "proc/process.h"
#include "stdefs.h"
#include "uart_polling.h"

int main() {
	//int exit_code;
    //int i;
	//void *addr;

    SystemInit();
    k_memory_init();
    process_init();
    queue_init();

    uart0_init();
    uart0_put_string("------------\n\r");
    uart0_put_string("Hello World!\n\r");

    init_printf(NULL);
    printf("Hello from printf!\n\r");

		// TODO: make this into a real test case
    //printf("Starting address is 0x%x\n\r", ((void *)(&Image$$RW_IRAM1$$ZI$$Limit)));
    //for (i = 0; i < 100; ++i) {
		//		addr = request_memory_block();
    //    printf("request_memory_block: 0x%x\n\r", addr);

		//		if (i % 5 == 0) {
		//				exit_code = release_memory_block(addr);
		//				printf("Exit code from release_memory_block: %d\n\r", exit_code);
		//		}
    //}

		run_mem_tests();

    return 0;
}
