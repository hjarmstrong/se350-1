#ifndef STRING_H
#define STRING_H

#define INVALID_NUMBER -1

void strncpy(char *destination, const char *source, int n);
int strlen(const char *str);
int strncmp(const char *a, const char *b, int n);
void *hash_string(const char* str, int len);
int read_num(const char **str_ptr);
void read_whitespace(const char **str_ptr);

#endif // STRING_H
