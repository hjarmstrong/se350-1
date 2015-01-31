#include "process.h"
#include "../stdefs.h"
#include "sys_process.h"

#ifdef DEBUG
    #include "../printf.h"
#endif // DEBUG

void null_proc(void) {
#ifdef DEBUG
		printf("Running null process\n\r");
#endif // DEBUG
    while(1) {
        release_processor();
    }
}
