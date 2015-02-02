#include "list.h"
#include "../mem/mem.h"
#include "../proc/process.h"
#include "../proc/scheduler.h"

#ifdef DEBUG
    #include "../printf.h"
#endif

/**
 * Represents a linked list node with NODE_SIZE elements.
 */
typedef struct ListNode {
    struct ListNode *next;
    struct ListNode *prev;
    U8 count;
    /*--- data goes from here up to here + LIST_NODE_SIZE ---*/
} ListNode;

// Size of each ListNode
#define LIST_NODE_SIZE (LIST_MEMORY_SIZE / NUM_QUEUES)

// Maximum number of elements in ListNode
#define NODE_SIZE ((int)((LIST_NODE_SIZE - sizeof(ListNode)) / sizeof(void *)))

// Gives the address of the 0th element in p_node
#define NODE_START(p_node) ((void **)(((U8 *)p_node) + sizeof(ListNode)))

ListNode *request_list_node(void) {
		static void *list_mem_ptr = NULL;

		if (list_mem_ptr == NULL) {
				list_mem_ptr = ((void *)(((U8 *)heap_low_address) - LIST_MEMORY_SIZE));
		}

		ListNode *block = list_mem_ptr;
		list_mem_ptr = ((U8 *)list_mem_ptr) + LIST_NODE_SIZE;
		return block;
}

ListNode *list_nodes[NUM_QUEUES];
int list_node_used[NUM_QUEUES];

void list_init() {
		ListNode *node;

		for (int i = 0; i < NUM_QUEUES; ++i) {
				node = request_list_node();
				list_nodes[i] = node;

				list_node_used[i] = 0;
		}
}

/**
 * Creates a new linked list node with size LIST_NODE_SIZE with NODE_SIZE elements
 * starting at NODE_START. This node is pulled from list_nodes, which holds a pre-
 * defined set of useable list_nodes.
 *
 * The caller must free this node by calling release_list_node(...).
 */
ListNode *list_node_new() {
		ListNode *node;

		for (int i = 0; i < NUM_QUEUES; ++i) {
				if (list_node_used[i] == 0) {
						node = list_nodes[i];
						list_node_used[i] = 1;

						node->next = NULL;
						node->prev = NULL;
						node->count = 0;

						return node;
				}
		}

		return RTX_ERROR_LIST_OUT_OF_MEMORY;
}

void release_list_node(ListNode *node) {
		for (int i = 0; i < NUM_QUEUES; ++i) {
				if (list_nodes[i] == node) {
						list_node_used[i] = 0;
						return;
				}
		}
}

/**
 * ================
 * Begin Public API
 * ================
 */

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
    if (list_empty(list)) {
        list->first = list_node_new();
			  list->last = list->first;
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
				release_list_node(last);
    }
}

/**
 * Returns the last node in a linked list.
 */
void *list_back(List *list) {
    ListNode *last = list->last;
    return NODE_START(last)[last->count - 1];
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

    --first->count;

    for (int i = 0; i < first->count; ++i) {
        NODE_START(first)[i] = NODE_START(first)[i + 1];
    }

    if (!first->count) {
        if (first == list->last) {
            list->first = NULL;
            list->last = NULL;
        } else {
            // there must exist at least two elements, thus first->next is not NULL
            first->next->prev = NULL;
            list->first = first->next;
        }
				release_list_node(first);
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
 * UNTESTED
 */
int list_segment_size(void *start_of_data) {
    ListNode *node = (ListNode *)(((U32 *)start_of_data) - NODE_SIZE);
    return node->count;
}

/**
 * Returns the start of the next segment given the first element in a segment, or NULL if
 * no such next segment exists.
 * UNTESTED
 */
void *list_next_segment(void *start_of_data) {
    ListNode *node = (ListNode *)(((U32 *)start_of_data) - NODE_SIZE);
    return node->next ? node->next + NODE_SIZE : NULL;
}

#ifdef DEBUG
void print_list(List *list) {
	  // TODO: fix when using list for things other than processes.
	
    PCB *process;
		PCB *processes[NUM_PROCS];
		int j = 0;
		while (!list_empty(list)) {
				process = list_front(list);
				list_shift(list);
				printf(" %d", process->pid);
				processes[j] = process;
				++j;
		}
		printf("\n\r");

		for (int k = 0; k < j; ++k) {
				list_push(list, processes[k]);
		}
}
#endif // DEBUG
