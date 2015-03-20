/**
 * list.c -- Simple segmented linked list used by the RTX
 */

#ifndef LIST_H
#define LIST_H

#include "../rtx.h"

#define IS_KERNEL 1
#define IS_USERSPACE 0

/*---- List API Methods -----------------------------------------------------*/

struct ListNode;

typedef struct List {
    struct ListNode *first;
    struct ListNode *last;
    int is_kernel;
} List;

List list_new(int is_kernel);

void list_push(List *, void *data);
void list_pop(List *);
void *list_back(List *);

void list_shift(List *);
void *list_front(List *);

int list_empty(List *);

/**
 * Debug method. no-op if DEBUG is set to 0.
 */
void print_list(List *);

#endif // LIST_H
