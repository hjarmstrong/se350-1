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

int strlen(const char *str) {
    int i;
    for (i = 0; str[i]; ++i) {
        // pass
    }
    return i;
}

int strncmp(const char *a, const char *b, int n) {
    int i;
    for (i = 0; (a[i] || b[i]) && i < n; ++i) {
        if (a[i] < b[i]) {
            return -1;
        }
        if (a[i] > b[i]) {
            return 1;
        }
    }
    return 0;
}

/**
 * Hashes a string using the djb2 algorithm.
 * Modified from code on http://www.cse.yorku.ca/~oz/hash.html
 */
void *hash_string(const char* str, int len) {
    unsigned long hash = 5381;
    int c;

    while ((c = *str++) && (len--)) {
        hash = ((hash << 5) + hash) + c; /* hash * 33 + c + 1 */
    }

    if (!hash) {
        ++hash;
    }

    return (void *) hash;
}
