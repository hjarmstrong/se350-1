#include "mem.h"
#include "../printf.h"
#include "../proc/process.h"

MemNode *root = (void *)(LAST_ADDRESS - HEADER_SIZE);

void *heap_high_address;
void *heap_low_address;

void k_memory_init(void) {
    root->next = START_ADDRESS;
	
    char *p_end = (char *)&Image$$RW_IRAM1$$ZI$$Limit;
    
	/* 4 bytes padding */
    // Can remove later if necissary.
	p_end += 4;

	/* allocate memory for pcb pointers   */
	gp_pcbs = (PCB **)p_end;
	p_end += NUM_PROCS * sizeof(PCB *);
  
	for (int i = 0; i < NUM_PROCS; i++ ) {
		gp_pcbs[i] = (PCB *)p_end;
		p_end += sizeof(PCB); 
	}
    
    /* Set up Heap building address */
    heap_low_address = p_end;
    
	printf("gp_pcbs[0] = 0x%x \n", gp_pcbs[0]);
	printf("gp_pcbs[1] = 0x%x \n", gp_pcbs[1]);

	
	/* prepare for alloc_stack() to allocate memory for stacks */
	
	gp_stack = (U32 *)LAST_ADDRESS;
	if ((U32)gp_stack & 0x04) { /* 8 bytes alignment */
		--gp_stack; 
	}
}

U32 *alloc_stack(U32 size_b) 
{
	U32 *sp;
	sp = gp_stack; /* gp_stack is always 8 bytes aligned */
	
	/* update gp_stack */
	gp_stack = (U32 *)((char *)sp - size_b);
	
	/* 8 bytes alignement adjustment to exception stack frame */
	if ((U32)gp_stack & 0x04) {
		--gp_stack; 
	}
	return sp;
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
		//if ((void *)((unsigned char *)root - HEADER_SIZE) == (void *)root->next && root->next->next == heap_low_address) {
		//	  __enable_irq();
		//		return NULL;
		//}

    for (ptr = root; ptr != heap_low_address; prev = ptr, ptr = ptr->next, is_free = !is_free) {
				// TODO: BLOCK_SIZE - 2 * HEADER_SIZE ???
        if (is_free == 1) {
						free_size = ((unsigned char *)ptr) - ((unsigned char *)ptr->next);
						if (free_size >= BLOCK_SIZE - 2 * HEADER_SIZE) {
								break;
						}
        }
    }

    if (ptr->next == heap_low_address) {
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

        if (ptr->next == heap_low_address) {
						prev->next = ptr->next;
				} else {
						prev->next = ptr->next->next;
				}
    } else {
        // expand a node
        mem_blk = ptr->next;

        next_blk = (MemNode *)((unsigned char *)mem_blk + BLOCK_SIZE);
				if (ptr->next == heap_low_address) {
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
	int offset = 0;
    MemNode *middle_blk = NULL;
    MemNode *next_blk = NULL;
    MemNode *prev = NULL;
    MemNode *ptr = NULL;

    __disable_irq();

		offset = ((U32)mem_blk) - ((U32)heap_low_address + HEADER_SIZE);
    if (offset % BLOCK_SIZE != 0) {
				// unaligned exception
				__enable_irq();
        return -1;
    }
    if (heap_low_address > mem_blk || (void *)((unsigned char *)mem_blk + BLOCK_SIZE) > (void *)heap_high_address) {
        // out of memory bounds exceptions
				__enable_irq();
				return -2;
    }
    for (ptr = root; ptr != heap_low_address; prev = ptr, ptr = ptr->next, is_free = !is_free) {
				// check whether the mem_blk is within the bounds of ptr and ptr->next
        if ((((void *)ptr->next) <= ((void *)((unsigned char *)mem_blk))) && ((void *)(((unsigned char *)mem_blk) + BLOCK_SIZE) <= ((void *)ptr))) {
            if (is_free) {
								// freeing unallocated memory
								__enable_irq();
                return -3;
            }
            break;
        }
    }
		if (ptr == heap_low_address) {
				// This should never happen
				printf("ERROR: ptr was set to heap_low_address\n\r");
				__enable_irq();
				return -4;
		}

    if (mem_blk == ((unsigned char *)ptr->next) + HEADER_SIZE) {
        if (mem_blk == ((unsigned char *)ptr) - BLOCK_SIZE) {
            // complete block
						if (ptr->next == heap_low_address) {
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
						if (ptr->next == heap_low_address) {
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

