#include "../printf.h"
#include "process.h"
#include "sys_process.h"

void null_proc(void) {
		printf("Running null process\n\r");
    while(1) {
        release_processor();
    }
}
