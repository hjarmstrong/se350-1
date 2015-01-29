#include "process.h"
#include "sys_process.h"

void null_proc(void) {
    while(1) {
        release_processor();
    }
}
