#ifndef STDEFS_H
#define STDEFS_H

#include <system_LPC17xx.h>

#define DEBUG 1

#define NULL ((void *)0)
#define __SVC_0 __svc_indirect(0)

typedef unsigned char U8;
typedef unsigned int U32;

extern U32 Image$$RW_IRAM1$$ZI$$Limit;

#endif // STDEFS_H
