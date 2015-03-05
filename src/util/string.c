#include "string.h"

void strncpy(char *destination, const char *source, int n) {
    int i;
    for (i = 0; source[i] && i < n; ++i) {
        destination[i] = source[i];
    }
    // note that as per C spec, the string may NOT null-terminated
    for (; i < n; ++i) {
        destination[i] = 0;
    }
}
