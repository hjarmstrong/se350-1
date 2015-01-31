#include "../list/list.h"
#include "mem.h"
#include "../proc/process.h"
#include "../stdefs.h"

#ifdef DEBUG
    #include "../printf.h"
#endif // DEBUG

MemNode *root;

void *heap_high_address;
void *heap_low_address;

void k_memory_init(void) {
    U8 *p_end = (U8 *)&Image$$RW_IRAM1$$ZI$$Limit;

    /* 4 bytes padding */
    p_end += 4;

    /* allocate memory for pcb pointers   */
    gp_pcbs = (PCB **)p_end;
    p_end += NUM_PROCS * sizeof(PCB *);

    for (int i = 0; i < NUM_PROCS; i++ ) {
        gp_pcbs[i] = (PCB *)p_end;
        p_end += sizeof(PCB);
    }

		/* allocate memory for list management */
		// TODO: make this dynamic
		p_end += LIST_MEMORY_SIZE;

    /* Set up Heap building address */
    heap_low_address = p_end;

    /* prepare for k_alloc_stack() to allocate memory for stacks */
    gp_stack = (U32 *)0x10008000;
    if ((U32)gp_stack & 0x04) { /* 8 bytes alignment */
        --gp_stack;
    }
}

U32 *k_alloc_stack(U32 size_b) {
    U32 *sp;
    sp = gp_stack; /* gp_stack is always 8 bytes aligned */

    /* update gp_stack */
    gp_stack = (U32 *)((U8 *)sp - size_b);

    /* 8 bytes alignement adjustment to exception stack frame */
    if ((U32)gp_stack & 0x04) {
        --gp_stack;
    }
    return sp;
}

void* k_request_memory_block(void) {
		static int is_init = 0;

		int free_size = 0;
    int is_free = 1;
    void *mem_blk = NULL;
    MemNode *next_blk = NULL;
    MemNode *prev = NULL;
    MemNode *ptr = NULL;
	  
		if (!is_init) {
				root = (void *)(((U8 *)heap_high_address) - HEADER_SIZE);
				root->next = heap_low_address;
				is_init = 1;
		}

    //__disable_irq();

    while (((U8 *)root) - ((U8 *)root->next) < BLOCK_SIZE && root->next->next == heap_low_address) {
				// TODO: this seems to be adding gp_current_process to priority queue
			  // 0, which may or may not have anything to do with that being
			  // its original queue
				list_push(&g_queues[PRIORITY_BLOCKED_ON_MEMORY], gp_current_process);
				gp_current_process->state = BLOCKED;
				//__enable_irq();
				k_release_processor();
    }

    for (ptr = root; ptr != heap_low_address; prev = ptr, ptr = ptr->next, is_free = !is_free) {
        if (is_free == 1) {
            free_size = ((U8 *)ptr) - ((U8 *)ptr->next);
            if (free_size >= BLOCK_SIZE - 2 * HEADER_SIZE) {
                    break;
            }
        }
    }

    if (ptr->next == heap_low_address) {
        // add first node
        mem_blk = ((U8 *)ptr->next) + HEADER_SIZE;

        // if the last block has only the exact amount of space necessary,
        // this will overwrite a header with an identical one
        next_blk = (MemNode *)((U8 *)mem_blk + BLOCK_SIZE);
        next_blk->next = ptr->next;
        ptr->next = next_blk;
    } else if (ptr->next - ptr < BLOCK_SIZE + HEADER_SIZE) {
        // merge nodes
        // block contains two destroyable headers
        mem_blk = ((U8 *)ptr->next) + HEADER_SIZE;

        if (ptr->next == heap_low_address) {
            prev->next = ptr->next;
        } else {
            prev->next = ptr->next->next;
        }
    } else {
        // expand a node
        mem_blk = ptr->next;

        next_blk = (MemNode *)((U8 *)mem_blk + BLOCK_SIZE);
        if (ptr->next == heap_low_address) {
            next_blk->next = ptr->next;
        } else {
            next_blk->next = ptr->next->next;
        }
        ptr->next = next_blk;
    }

    //__enable_irq();
    return mem_blk;
}

int k_release_memory_block(void* mem_blk) {
    int is_free = 1;
    int offset = 0;
    MemNode *middle_blk = NULL;
    MemNode *next_blk = NULL;
    MemNode *prev = NULL;
    MemNode *ptr = NULL;
		PCB *process = NULL;

    //__disable_irq();

    offset = ((U32)mem_blk) - ((U32)heap_low_address + HEADER_SIZE);
    if (offset % BLOCK_SIZE != 0) {
        //__enable_irq();
        return RTX_ERROR_MEMORY_UNALIGNED;
    }
    if (heap_low_address > mem_blk || (void *)((U8 *)mem_blk + BLOCK_SIZE) > (void *)heap_high_address) {
        //__enable_irq();
        return RTX_ERROR_MEMORY_OUT_OF_BOUNDS;
    }
    for (ptr = root; ptr != heap_low_address; prev = ptr, ptr = ptr->next, is_free = !is_free) {
        // check whether the mem_blk is within the bounds of ptr and ptr->next
        if ((((void *)ptr->next) <= ((void *)((U8 *)mem_blk))) && ((void *)(((U8 *)mem_blk) + BLOCK_SIZE) <= ((void *)ptr))) {
            if (is_free) {
                //__enable_irq();
                return RTX_ERROR_MEMORY_FREEING_UNALLOCATED;
            }
            break;
        }
    }
    if (ptr == heap_low_address) {
#ifdef DEBUG
        printf("ERROR: ptr was set to heap_low_address\n\r");
#endif // DEBUG
        //__enable_irq();
        return RTX_ERROR;
    }

    if (mem_blk == ((U8 *)ptr->next) + HEADER_SIZE) {
        if (mem_blk == ((U8 *)ptr) - BLOCK_SIZE) {
            // complete block
                if (ptr->next == heap_low_address) {
                    // block at start of allocatable memory
                    prev->next = ptr->next;
                } else {
                    // block is anywhere else
                    // this should completely remove the entire block
                    // and force the pointers to skip it
                    prev->next = ptr->next->next;
                }
        } else {
            // at start of block
            next_blk = (MemNode *)(((U8 *)ptr->next) + BLOCK_SIZE);
            if (ptr->next == heap_low_address) {
                // block at start of allocatable memory
                next_blk->next = ptr->next;
            } else {
                // block is anywhere else
                next_blk->next = ptr->next->next;
            }
            ptr->next = next_blk;
        }
    } else if (mem_blk == ((U8 *)ptr) - BLOCK_SIZE) {
        // at end of block
        next_blk = (MemNode *)(((U8 *)ptr) - BLOCK_SIZE);
        next_blk->next = ptr->next;
        prev->next = next_blk;
    } else {
        // TODO: additional validation
        // We should verify that the address to-be-freed is an address
        // we have allocated rather than an address within a block

        // in middle of block
        // allocated left of removed
        next_blk = mem_blk;
        next_blk->next = ptr->next;

        // non-allocated (removed block)
        middle_blk = (MemNode *)(((U8 *)mem_blk) + BLOCK_SIZE - HEADER_SIZE);
        middle_blk->next = next_blk;

        // start of allocated chain
        ptr->next = middle_blk;
    }

		while (!list_empty(&g_queues[PRIORITY_BLOCKED_ON_MEMORY])) {
				process = list_front(&g_queues[PRIORITY_BLOCKED_ON_MEMORY]);
        list_shift(&g_queues[PRIORITY_BLOCKED_ON_MEMORY]);
				process->state = READY;

			  list_push(&g_queues[get_process_priority(process->pid)], process);
		}

    //__enable_irq();

		release_processor();
    return RTX_OK;
}
