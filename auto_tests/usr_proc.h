/**
 * Definitions that auto tests must export.
 */

#ifndef USER_PROCESS_H
#define USER_PROCESS_H

/* User Process Methods  */
extern PROC_INIT g_test_procs[NUM_TEST_PROCS];

void set_test_procs(void);

void proc1(void);
void proc2(void);
void proc3(void);
void proc4(void);
void proc5(void);
void proc6(void);

#endif // USER_PROCESS_H
