#include "process.h"
#include "../rtx.h"
#include "sys_process.h"

#include "../printf.h"

void null_proc(void) {
        printf("Running null process\n\r");
    while(1) {
        release_processor();
    }
}
