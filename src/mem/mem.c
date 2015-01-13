#include "mem.h"
#include "../printf.h"
#include <LPC17xx.h>

typedef struct MemNode {
    struct MemNode *next;
} MemNode;

MemNode *root = START_ADDRESS - sizeof(MemNode);

void k_memory_init() {
    // This goes into the operating system's stack.
    root->next = &Image$$RW_IRAM1$$ZI$$Limit;
}

void* k_request_memory_block(void) {
    __disable_irq();
    for
    __enable_irq();
    return NULL;
}

int k_release_memory_block(void* p_mem_blk) {
    printf("k_release_memory_block: releasing block @ 0x%x\n", p_mem_blk);
    return 0;
}
