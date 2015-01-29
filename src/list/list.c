#include "list.h"
#include "../mem/mem.h"

/**
 * Represents a linked list node with NODE_SIZE elements.
 * To be punned as the start of a block returned by request_memory_block(...).
 */
typedef struct ListNode {
    struct ListNode *next;
    struct ListNode *prev;
    short count;
    /*--- data goes from here up to here + BLOCK_SIZE ---*/
} ListNode;

// Maximum number of elements in node_size
#define NODE_SIZE ((int)(BLOCK_SIZE/sizeof(void *) - sizeof(ListNode)/sizeof(void *)))

// Gives the address of the 0th element in p_node
#define NODE_START(p_node) ((void **) (((char *)p_node) + sizeof(ListNode)))

/**
 * Creates a new linked list node with size BLOCK_SIZE with NODE_SIZE elements
 * starting at NODE_START.
 *
 * The caller must free this node by calling release_memory_block(...).
 */
ListNode *list_node_new() {
    ListNode *node = (ListNode *) request_memory_block();

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
    l.first = NULL;
    l.last = NULL;
    return l;
}

/**
 * Adds a node to the end of a linked list.
 */
void list_push(List *list, void *data) {
    if (!list->last) {
        list->first = list->last = list_node_new();
    }
    if (list->last->count == NODE_SIZE) {
        ListNode *new_node = list_node_new();

        list->last->next = new_node;
        new_node->prev = list->last;
        list->last = new_node;
    }
    ++list->last->count;

    NODE_START(list->last)[list->last->count - 1] = data;
}

/**
 * Removes the last node from a linked list.
 */
void list_pop(List *list) {
    ListNode *last = list->last;

    --last->count;
    if (!last->count) {
        if (last == list->first) {
            list->last = NULL;
            list->first = NULL;
        } else {
            last->prev->next = NULL;
            list->last = last->prev;
        }
        release_memory_block(last);
    }
}

/**
 * Returns the last node in a linked list.
 */
void *list_back(List *list) {
    ListNode *last = list->last;
    return NODE_START(list->last)[list->last->count - 1];
}


/**
 * Prepend a node to a linked list.
 */
void list_unshift(List *list, void *data) {
    // NOT IMPLEMENTED!!!
}

/**
 * Removes the first node in a linked list.
 */
void list_shift(List *list) {
    ListNode *first = list->first;
    void *data = NODE_START(first)[0];

    --first->count;

    for (int i = 0; i < first->count; ++i) {
        NODE_START(first)[i] = NODE_START(first)[i + 1];
    }

    if (!first->count) {
        if(first == list->last) {
            list->first = NULL;
            list->last = NULL;
        } else {
            first->next->prev = NULL;
            list->first = first->next;
        }
        release_memory_block(first);
    }
}

/**
 * Returns the first element in a linked list.
 */
void *list_front(List *list) {
    ListNode *first = list->first;
    return NODE_START(first)[0];
}

/**
 * Returns 1 if list is empty, 0 otherwise.
 */
int list_empty(List *list) {
    return !list->last;
}

/**
 * Returns the number of elements in the segment with first element start_of_data.
 */
int list_segment_size(void *start_of_data) {
    ListNode *node = start_of_data - NODE_SIZE;
    return node->count;
}

/**
 * Returns the start of the next segment given the first element in a segment, or NULL if
 * no such next segment exists.
 */
void *list_next_segment(void *start_of_data) {
    ListNode *node = start_of_data - NODE_SIZE;
    return node->next ? node->next + NODE_SIZE : NULL;
}
