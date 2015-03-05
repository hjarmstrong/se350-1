#ifndef STRING_H
#define STRING_H

void strncpy(char *destination, const char *source, int n);
int strlen(const char *str);
int strncmp(const char *a, const char *b, int n);
void *hash_string(const char* str, int len);

#endif // STRING_H
