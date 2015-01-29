#define NUM_TEST_PROCS 6

#ifndef USER_PROCESS_H
#define USER_PROCESS_H

#include "process.h"

extern struct PROC_INIT g_test_procs[NUM_TEST_PROCS];

void set_test_procs(void);

void proc_1(void);
void proc_2(void);
void proc_3(void);
void proc_4(void);
void proc_5(void);
void proc_6(void);

#endif // USER_PROCESS_H
