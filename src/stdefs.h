#ifndef STDEFS_H
#define STDEFS_H

#include <system_LPC17xx.h>

#define DEBUG 1

#define NULL ((void *)0)
#define __SVC_0 __svc_indirect(0)

typedef unsigned char U8;
typedef unsigned int U32;

extern U32 Image$$RW_IRAM1$$ZI$$Limit;

/* Return Values */

#define RTX_OK 0
#define RTX_ERROR -1
#define RTX_ERROR_MEMORY_UNALIGNED -2
#define RTX_ERROR_MEMORY_OUT_OF_BOUNDS -3
#define RTX_ERROR_MEMORY_FREEING_UNALLOCATED -4
#define RTX_ERROR_PROCESS_CHANGING_NULL_PROCESS_PRIORITY -5
#define RTX_ERROR_PROCESS_SETTING_NULL_PRIORITY_TO_NON_NULL_PROCESS -6
#define RTX_ERROR_PROCESS_PRIORITY_DOESNT_EXIST -7
#define RTX_ERROR_PROCESS_PID_NOT_FOUND -1
#define RTX_ERROR_PROCESS_SCHEDULER_EMPTY NULL
#define RTX_ERROR_LIST_OUT_OF_MEMORY NULL

#endif // STDEFS_H
