#include <LPC17xx.h>
#include "mem/mem.h"
#include "printf.h"
#include "stdefs.h"
#include "uart_polling.h"

int main() {
    unsigned int end_addr = (unsigned int) &Image$$RW_IRAM1$$ZI$$Limit;

    SystemInit();
    uart0_init();
    uart0_put_string("------------\n\r");
    uart0_put_string("Hello World!\n\r");
	
	  init_printf(NULL);
    printf("Hello from printf!\n\r");
    
    //printf("k_request_memory_block: image ends at 0x%x\n", end_addr);

    request_memory_block();
    return 0;
}
