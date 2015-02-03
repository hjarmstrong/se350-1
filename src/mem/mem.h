/**
 * mem/mem.h -- Internal memory API
 */

#ifndef MEM_H
#define MEM_H

#include "../rtx.h"

/*---- Types ----------------------------------------------------------------*/

typedef struct MemNode {
    struct MemNode *next;
} MemNode;


/*---- Constants ------------------------------------------------------------*/

#define HEADER_SIZE sizeof(MemNode)


/*---- Initialization -------------------------------------------------------*/

/**
 * Highest address that can be used by the heap, inclusive.
 */
extern void *heap_high_address;

/**
 * Lowest address that can be used by the heap, inclusive.
 */
extern void *heap_low_address;


/*---- Private API ----------------------------------------------------------*/

/**
 * Initializes memory.
 * Must be called BEFORE request_memory_block and k_alloc_stack!
 *
 * @SVC
 */
extern void k_memory_init(void);
#define memory_init() _memory_init((U32)k_memory_init)
extern void _memory_init(U32 p_func) __SVC_0;

/**
 * Allocates memory for a process stack with 'size' bytes.
 * Must be called AFTER memory_init.
 *
 * @param size size of process stack in bytes.
 */
U32 *k_alloc_stack(U32 size);

/**
 * Prints debug information about the current state of the heap and stack.
 */
void print_memory(void);

#endif // MEM_H
