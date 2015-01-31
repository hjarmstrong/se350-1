#ifndef USER_PROCESS_H
#define USER_PROCESS_H

// must be defined before process.h
#define NUM_TEST_PROCS 6

#include "process.h"

/* Global Variables */

#define NUM_TESTS 4
extern struct PROC_INIT g_test_procs[NUM_TEST_PROCS];

/* User Process Methods  */

void set_test_procs(void);

/* User Processes */

void proc_1(void);
void proc_2(void);
void proc_3(void);
void proc_4(void);
void proc_5(void);
void proc_6(void);

#endif // USER_PROCESS_H
