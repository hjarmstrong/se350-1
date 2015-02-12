/**
 * @brief timer.h - Timer header file
 * @author Y. Huang
 * @date 2013/02/12
 */
#ifndef _TIMER_H_
#define _TIMER_H_

#include "../rtx.h"

extern volatile U32 g_timer_count; // current time in milliseconds
extern U32 timer_init ( U32 n_timer );  /* initialize timer n_timer */

#endif /* ! _TIMER_H_ */
