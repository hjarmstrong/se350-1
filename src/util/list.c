/**
 * list.c -- Simple segmented linked list used by the RTX
 */

#include "list.h"
#include "../core/mem.h"
#include "../core/process.h"
#include "../core/scheduler.h"
#include "../sys/uart_polling.h"

/**
 * Represents a linked list node with LIST_BUCKET_SIZE elements.
 */
typedef struct ListNode {
    struct ListNode *next;
    struct ListNode *prev;
    U8 count;
    void *data;
} ListNode;

// Maximum number of elements in ListNode.
#define LIST_BUCKET_SIZE ((int)(BLOCK_SIZE/sizeof(void *) - sizeof(ListNode)/sizeof(void *) + 1))

/**
 * Creates a new linked list node with LIST_BUCKET_SIZE elements
 * starting at list_node->data.
 *
 * The caller must free this node by calling k_release_memory_block(...).
 */
ListNode *list_node_new() {
    ListNode *node = (ListNode *) k_request_memory_block();

    node->next = NULL;
    node->prev = NULL;
    node->count = 0;

    return node;
}

/**
 * Creates a new stack-based linked list with 0 elements.
 */
List list_new() {
    List l;
    l.first = list_node_new();
    l.last = l.first;
    return l;
}

/**
 * Adds a node to the end of a linked list.
 */
void list_push(List *list, void *data) {
    if (list->last->count == LIST_BUCKET_SIZE) {
        ListNode *new_node = list_node_new();

        list->last->next = new_node;
        new_node->prev = list->last;
        list->last = new_node;
    }
    ++list->last->count;

    (&list->last->data)[list->last->count - 1] = data;
}

/**
 * Removes the last node from a linked list.
 */
void list_pop(List *list) {
    ListNode *last = list->last;

    --last->count;
    if (!last->count) {
        if (last != list->first) {
            last->prev->next = NULL;
            list->last = last->prev;
					  k_release_memory_block(last);
        }
    }
}

/**
 * Returns the last node in a linked list.
 */
void *list_back(List *list) {
    ListNode *last = list->last;
    return (&last->data)[last->count - 1];
}

/**
 * Removes the first node in a linked list.
 */
void list_shift(List *list) {
    ListNode *first = list->first;
    int i;

    --first->count;

    for (i = 0; i < first->count; ++i) {
        (&first->data)[i] = (&first->data)[i + 1];
    }

    if (!first->count) {
        if (first != list->last) {
            // there must exist at least two elements, thus first->next is not NULL
            first->next->prev = NULL;
            list->first = first->next;
					  k_release_memory_block(first);
        }
    }
}

/**
 * Returns the first element in a linked list.
 */
void *list_front(List *list) {
    ListNode *first = list->first;
    return (&first->data)[0];
}

/**
 * Returns 1 if list is empty, 0 otherwise.
 */
int list_empty(List *list) {
    return list->last->count == 0;
}

void print_list(List *list) {
#if DEBUG
    // TODO: fix when using list for things other than processes.
    int i;

    if (list->first) {
        for (i = 0; i < list->first->count; ++i) {
            uart1_put_char(((PCB *)(&list->first->data)[i])->pid + '0');
        }
    }
    uart1_put_string("\n\r");
#endif
}
