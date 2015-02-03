/**
 * list.c -- Simple segmented linked list used by the RTX
 */

#ifndef LIST_H
#define LIST_H

#include "../rtx.h"

/*---- Configuration --------------------------------------------------------*/

/**
 * Amount of memory preallocated for all list nodes.
 * TODO: Move lists back to the heap, if possible
 */
#define LIST_MEMORY_SIZE 1024

/*---- List API Methods -----------------------------------------------------*/

struct ListNode;

typedef struct List {
    struct ListNode *first;
    struct ListNode *last;
} List;

List list_new(void);

void list_init(void);

void list_push(List *, void *data);
void list_pop(List *);
void *list_back(List *);

void list_unshift(List *, void *data);
void list_shift(List *);
void *list_front(List *);

int list_empty(List *);

int list_segment_size(void *);
void *list_next_segment(void *);

/**
 * Debug method. no-op if DEBUG is set to 0.
 */
void print_list(List *);

#endif // LIST_H
