#include <LPC17xx.h>
#include "mem/mem.h"
#include "printf.h"
#include "stdefs.h"
#include "uart_polling.h"

int main() {
    SystemInit();
	  k_memory_init();

    uart0_init();
    uart0_put_string("------------\n\r");
    uart0_put_string("Hello World!\n\r");
	
	  init_printf(NULL);
    printf("Hello from printf!\n\r");

    request_memory_block();
    return 0;
}
