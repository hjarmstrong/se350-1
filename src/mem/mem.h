#ifndef MEM_H
#define MEM_H

#include "../stdefs.h"

extern void k_memory_init(void);

extern void *k_request_memory_block(void);
#define request_memory_block() _request_memory_block((U32)k_request_memory_block)
extern void *_request_memory_block(U32 p_func) __SVC_0;
//__SVC_0 can also be put at the end of the function declaration

extern int k_release_memory_block(void *);
#define release_memory_block(mem_blk) _release_memory_block((U32)k_release_memory_block, mem_blk)
extern int _release_memory_block(U32 p_func, void *mem_blk) __SVC_0;

#define BLOCK_SIZE 0x80
#define LAST_ADDRESS 0x10007FFF

#endif // MEM_H
