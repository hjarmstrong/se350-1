#include "mem.h"
#include "../printf.h"
#include <LPC17xx.h>

typedef struct MemNode {
    struct MemNode *next;
} MemNode;

#define HEADER_SIZE sizeof(MemNode)
#define START_ADDRESS ((void *)(&Image$$RW_IRAM1$$ZI$$Limit))

MemNode *root = (void *)(LAST_ADDRESS - HEADER_SIZE);

void k_memory_init(void) {
    root->next = START_ADDRESS;
}

void* k_request_memory_block(void) {
		int is_free = 1;
		void *mem_blk = NULL;
		MemNode *next_blk = NULL;
		MemNode *prev = NULL;
	  MemNode *ptr = NULL;

    __disable_irq();

	  // while no mem block avail
				// put PCB on blocked_resource_q ;
				// set process state to BLOCKED_ON_RESOURCE ;
				// release_processor ( ) ;

		for (ptr = root; ptr->next != START_ADDRESS; prev = ptr, ptr = ptr->next, is_free = !is_free) {
				if (is_free == 1 && (ptr->next - ptr) >= (BLOCK_SIZE - 2 * HEADER_SIZE)) {
					  break;
				}
		}

		if (ptr->next == START_ADDRESS) {
				// add first node
				mem_blk = ptr->next;

				// if the last block has only the exact amount of space necessary,
			  // this will overwrite a header with an identical one
				next_blk = (MemNode *)((unsigned char *)mem_blk + BLOCK_SIZE);
				next_blk->next = ptr->next;
				ptr->next = next_blk;
		} else if (ptr->next - ptr < BLOCK_SIZE) {
		    // merge nodes
				// block contains two destroyable headers
				mem_blk = ptr->next;

				prev->next = ptr->next->next;
		} else {
				// expand a node
				mem_blk = ptr->next;

				next_blk = (MemNode *)((unsigned char *)mem_blk + BLOCK_SIZE);
				next_blk->next = ptr->next->next;
				ptr->next = next_blk;
		}

    __enable_irq();
    return mem_blk;
}

int k_release_memory_block(void* mem_blk) {
		int is_free = 1;
		MemNode *middle_blk = NULL;
		MemNode *next_blk = NULL;
		MemNode *prev = NULL;
	  MemNode *ptr = NULL;

    __disable_irq();

		if (mem_blk % 4 != 0) {
				return -1;
		}
		if (START_ADDRESS > mem_blk || mem_blk + BLOCK_SIZE > LAST_ADDRESS) {
				return -2;
		}
		for (ptr = root; ptr->next != START_ADDRESS; prev = ptr, ptr = ptr->next, is_free = !is_free) {
				if ((ptr->next <= mem_blk - HEADER_SIZE && mem_blk + BLOCK_SIZE <= ptr)) {
						if (is_free) {
								return -3;
						}
						break;
				}
		}

		if (mem_blk == ptr->next + HEADER_SIZE) {
			  if (mem_blk == ptr - BLOCK_SIZE) {
					  // complete block
					  prev->next = ptr->next;
				} else {
					  // at start of block
						next_blk = ptr->next + BLOCK_SIZE - HEADER_SIZE;
						if (ptr->next == START_ADDRESS) {
							  next_blk->next = ptr->next;
						} else {
								next_blk->next = ptr->next->next;
						}
						ptr->next = next_blk;
				}
		} else if (mem_blk == ptr - BLOCK_SIZE) {
			  // at end of block
				next_blk = ptr - BLOCK_SIZE;
				next_blk->next = ptr->next;
				prev->next = next_blk;
		} else {
			  // in middle of block
				// TODO: additional validation; check that address is aligned
				next_blk = mem_blk;
				next_blk->next = ptr->next;
			
				middle_blk = mem_blk + BLOCK_SIZE - HEADER_SIZE;
				middle_blk->next = next_blk;
			
				ptr->next = middle_blk;
		}

		// if ( blocked on resource q not empty ) f
				// handle_process_ready (pop( blocked resource q ) ) ;

		__enable_irq();
		return 0;
}
