#include "test.h"

/**
 * Crashes with error `error` if `condition` is equal to `0`.
 */
void assert(int condition, const char *error) {
    if (condition == 0) {
        printf("\n\r########################################\n\r%s\n\r########################################\n\r",
            error);

        // try to crash
        *((unsigned int*)0) = 0xdeadbeef;

        // if we didn't crash for whatever reason, we still shouldn't return
        while(1) {
            /* pass */
        }
    }
}
