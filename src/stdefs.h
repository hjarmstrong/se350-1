#ifndef STDEFS_H
#define STDEFS_H

#include <system_LPC17xx.h>

#define NULL ((void *)0)

typedef unsigned int U32;

#define __SVC_0  __svc_indirect(0)

extern U32 Image$$RW_IRAM1$$ZI$$Limit;

#endif // STDEFS_H
