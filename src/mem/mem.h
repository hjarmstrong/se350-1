#ifndef MEM_H
#define MEM_H

#include "../stdefs.h"

/* Global Variables */

typedef struct MemNode {
    struct MemNode *next;
} MemNode;

#define HEADER_SIZE sizeof(MemNode)
#define BLOCK_SIZE 0x80

extern void *heap_high_address;
extern void *heap_low_address;

/* Kernel Memory Methods */

extern void k_memory_init(void);
#define memory_init() _memory_init((U32)k_memory_init)
extern int _memory_init(U32 p_func) __SVC_0;

extern void *k_request_memory_block(void);
#define request_memory_block() _request_memory_block((U32)k_request_memory_block)
extern void *_request_memory_block(U32 p_func) __SVC_0;

extern int k_release_memory_block(void *);
#define release_memory_block(mem_blk) _release_memory_block((U32)k_release_memory_block, mem_blk)
extern int _release_memory_block(U32 p_func, void *mem_blk) __SVC_0;

/* Memory Methods */

U32 *k_alloc_stack(U32 size_b);

#endif // MEM_H
