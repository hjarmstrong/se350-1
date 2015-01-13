/* @brief: rtx.h User API prototype, this is an example only
 * @author: Yiqing Huang
 * @date: 2014/01/07
 */
#ifndef RTX_H_
#define RTX_H_

#include "stdefs.h"
#include "mem/mem.h"

extern int k_release_processor(void);
#define release_processor() _release_processor((U32)k_release_processor)
extern int __SVC_0 _release_processor(U32 p_func);

#endif // !RTX_H_
