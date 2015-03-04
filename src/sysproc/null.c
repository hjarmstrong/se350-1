#include <LPC17xx.h>

#include "../rtx.h"
#include "../sys/uart_polling.h"

void null_proc(void) {
    uart1_put_string("Running null process\n\r");
    while(1) {
        release_processor();
    }
}
