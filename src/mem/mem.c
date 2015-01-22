#include "mem.h"
#include "../printf.h"
#include <LPC17xx.h>

typedef struct MemNode {
    struct MemNode *next;
} MemNode;

#define HEADER_SIZE sizeof(MemNode)
#define START_ADDRESS ((void *)(&Image$$RW_IRAM1$$ZI$$Limit + 4))

MemNode *root = (void *)(LAST_ADDRESS - HEADER_SIZE);

void k_memory_init(void) {
    root->next = START_ADDRESS;
}

void* k_request_memory_block(void) {
		int is_free = 1;
		void *mem_blk = NULL;
		MemNode *prev = NULL;
	  MemNode *ptr = NULL;
		MemNode *next_blk = NULL;

    __disable_irq();

	  // while no mem block avail
				// put PCB on blocked_resource_q ;
				// set process state to BLOCKED_ON_RESOURCE ;
				// release_processor ( ) ;

		for (ptr = root; ptr->next != START_ADDRESS; prev = ptr, ptr = ptr->next, is_free = !is_free) {
				if (is_free == 1 && (ptr->next - ptr) >= (BLOCK_SIZE - HEADER_SIZE)) {
					  break;
				}
		}

		if (ptr->next == START_ADDRESS) {
				// add first node
				mem_blk = ptr->next;

				// if the last block has only the exact amount of space necessary,
			  // this will overwrite a header with an identical one
				next_blk = (MemNode *)((void *)mem_blk) + BLOCK_SIZE;
				next_blk->next = ptr->next;
				ptr->next = next_blk;
		} else if ((ptr->next - ptr) < (BLOCK_SIZE + HEADER_SIZE)) {
		    // merge nodes
				mem_blk = ptr->next;

				// ptr->next->next must exist or we would be in case 1
				prev->next = ptr->next->next;
		} else {
				// expand a node
				mem_blk = ptr->next;

				next_blk = (MemNode *)((void *)mem_blk) + BLOCK_SIZE;
				next_blk->next = ptr->next->next;
				ptr->next = next_blk;
		}

    __enable_irq();
    return mem_blk;
}

int k_release_memory_block(void* p_mem_blk) {
    __disable_irq();
		// if ( memory block pointer is not valid )
				// return -1;
		// put memory_block into heap ;
		// if ( blocked on resource q not empty ) f
				// handle_process_ready (pop( blocked resource q ) ) ;
		__enable_irq();
		return 0;
}
