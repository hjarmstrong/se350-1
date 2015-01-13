#include "mem.h"
#include "../printf.h"
#include <LPC17xx.h>

typedef struct MemNode {
    struct MemNode *next;
    U32 length;
} MemNode;

MemNode root;

void* k_request_memory_block(void) {
    unsigned int end_addr = (unsigned int) &Image$$RW_IRAM1$$ZI$$Limit;
    printf("k_request_memory_block: image ends at 0x%x\n", end_addr);
    return NULL;
}

int k_release_memory_block(void* p_mem_blk) {
    printf("k_release_memory_block: releasing block @ 0x%x\n", p_mem_blk);
    return 0;
}
