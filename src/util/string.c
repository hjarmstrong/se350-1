#include "string.h"
#include "../rtx.h"

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

    while (1) {
        c = *str++;
        if (!c || !len--) {
            break;
        }
        hash = ((hash << 5) + hash) + c; /* hash * 33 + c + 1 */
    }

    if (!hash) {
        ++hash;
    }

    return (void *) hash;
}

/**
 * Read a decimal number starting at (*str_ptr)[0], and advance str_ptr to just
 * after the number. If no number is found, returns INVALID_NUMBER without advancing
 * the string.
 */
int read_num(const char **str_ptr) {
    int num = 0;
    int read_num = 0;
    while ((*str_ptr)[0] >= '0' && (*str_ptr)[0] <= '9') {
        read_num = 1;
        num *= 10;
        num += (*str_ptr)[0] - '0';
        ++(*str_ptr);
    }
    return read_num ? num : INVALID_NUMBER;
}

/**
 * Advance str_ptr to just after any tabs or spaces.
 */
void read_whitespace(const char **str_ptr) {
    while ((*str_ptr)[0] == ' ' || (*str_ptr)[0] == '\t') {
        ++(*str_ptr);
    }
}

void int_to_c_string(int input, char *output){//works for numbers < 999
    int index = 0;//current position in the string
    if(input >= 100){
        output[index] = input / 100;
        index++;
        input %= 100;
    }
    if(input >= 10 || index == 1){//If there was a hundreds digit, we can't skip the tens even if it is 0
        output[index] = input / 10;
        index++;
        input %= 10;
    }
    output[index] = input;
    output[index + 1] = '\0';
}

int c_string_to_int(char *input){//input must be NULL terminated
    int char_offset = 48;
    int index;//current position in the string
    int multiplier = 1;
    int output;
    for(index = 0; input[index] != '\0'; index++){}//index will be one past the end of the string
    index--;//last character
    for(;index >= 0; index--, multiplier *= 10){
        output += (input[index] - char_offset) * multiplier;
    }
    return output;
}
