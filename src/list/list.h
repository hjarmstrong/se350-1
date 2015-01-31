#ifndef LIST_H
#define LIST_H

/* Global Variables */

#define LIST_MEMORY_SIZE 1024

struct ListNode;

typedef struct List {
    struct ListNode *first;
    struct ListNode *last;
} List;

/* List API Methods */

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

#endif // LIST_H
