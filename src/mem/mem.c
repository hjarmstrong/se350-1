#include "mem.h"
#include "../printf.h"
#include <LPC17xx.h>

MemNode *root = (void *)(LAST_ADDRESS - HEADER_SIZE);

void k_memory_init(void) {
    root->next = START_ADDRESS;
}

void* k_request_memory_block(void) {
		int free_size = 0;
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
		//if ((void *)((unsigned char *)root - HEADER_SIZE) == (void *)root->next && root->next->next == START_ADDRESS) {
		//	  __enable_irq();
		//		return NULL;
		//}

    for (ptr = root; ptr != START_ADDRESS; prev = ptr, ptr = ptr->next, is_free = !is_free) {
				// TODO: BLOCK_SIZE - 2 * HEADER_SIZE ???
        if (is_free == 1) {
						free_size = ((unsigned char *)ptr) - ((unsigned char *)ptr->next);
						if (free_size >= BLOCK_SIZE - 2 * HEADER_SIZE) {
								break;
						}
        }
    }

    if (ptr->next == START_ADDRESS) {
        // add first node
        mem_blk = ((unsigned char *)ptr->next) + HEADER_SIZE;

        // if the last block has only the exact amount of space necessary,
        // this will overwrite a header with an identical one
        next_blk = (MemNode *)((unsigned char *)mem_blk + BLOCK_SIZE);
        next_blk->next = ptr->next;
        ptr->next = next_blk;
		// TODO: remove HEADER_SIZE ???
    } else if (ptr->next - ptr < BLOCK_SIZE + HEADER_SIZE) {
        // merge nodes
        // block contains two destroyable headers
        mem_blk = ((unsigned char *)ptr->next) + HEADER_SIZE;

        if (ptr->next == START_ADDRESS) {
						prev->next = ptr->next;
				} else {
						prev->next = ptr->next->next;
				}
    } else {
        // expand a node
        mem_blk = ptr->next;

        next_blk = (MemNode *)((unsigned char *)mem_blk + BLOCK_SIZE);
				if (ptr->next == START_ADDRESS) {
						next_blk->next = ptr->next;
				} else {
						next_blk->next = ptr->next->next;
				}
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

    if ((U32)mem_blk % 4 != 0) {
        return -1;
    }
    if (START_ADDRESS > mem_blk || (void *)((unsigned char *)mem_blk + BLOCK_SIZE) > (void *)LAST_ADDRESS) {
        return -2;
    }
    for (ptr = root; ptr != START_ADDRESS; prev = ptr, ptr = ptr->next, is_free = !is_free) {
				// check whether the mem_blk is within the bounds of ptr and ptr->next
        if ((void *)ptr->next <= (void *)((unsigned char *)mem_blk - HEADER_SIZE) && (void *)((unsigned char *)mem_blk + BLOCK_SIZE) <= (void *)ptr) {
            if (is_free) {
                return -3;
            }
            break;
        }
    }

    if (mem_blk == ((unsigned char *)ptr->next) + HEADER_SIZE) {
        if (mem_blk == ((unsigned char *)ptr) - BLOCK_SIZE) {
            // complete block
						if (ptr->next == START_ADDRESS) {
								// block at start of allocatable memory
								prev->next = ptr->next;
						} else {
								// anywhere else
							  // this should completely remove the entire block
							  // and force the pointers to skip it
								prev->next = ptr->next->next;
						}
        } else {
            // at start of block
            next_blk = (MemNode *)(((unsigned char *)ptr->next) + BLOCK_SIZE);
						if (ptr->next == START_ADDRESS) {
								// block at start of allocatable memory
								next_blk->next = ptr->next;
						} else {
								// block is anywhere else
								next_blk->next = ptr->next->next;
						}
            ptr->next = next_blk;
        }
    } else if (mem_blk == ((unsigned char *)ptr) - BLOCK_SIZE) {
        // at end of block
        next_blk = (MemNode *)(((unsigned char *)ptr) - BLOCK_SIZE);
        next_blk->next = ptr->next;
        prev->next = next_blk;
    } else {
        // in middle of block
        // TODO: additional validation; check that address is aligned
				// allocated left of removed
				next_blk = mem_blk;
        next_blk->next = ptr->next;

				// non-allocated (removed block)
        middle_blk = (MemNode *)(((unsigned char *)mem_blk) + BLOCK_SIZE - HEADER_SIZE);
        middle_blk->next = next_blk;

				// start of allocated chain
        ptr->next = middle_blk;
    }

    // if ( blocked on resource q not empty ) f
        // handle_process_ready (pop( blocked resource q ) ) ;

    __enable_irq();
    return 0;
}
