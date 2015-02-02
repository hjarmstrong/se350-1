#include "../src/mem/mem.h"
#include "../src/proc/process.h"
#include "../src/proc/scheduler.h"

/* ----- Definitations ----- */
#define RTX_ERR -1
#define NUM_TEST_PROCS 6

/* Process Priority. The bigger the number is, the lower the priority is*/
#define HIGH    0
#define MEDIUM  1
#define LOW     2
#define LOWEST  3

/* Process IDs */
#define PID_NULL 0
#define PID_P1   1
#define PID_P2   2
#define PID_P3   3
#define PID_P4   4
#define PID_P5   5
#define PID_P6   6
#define PID_A    7
#define PID_B    8
#define PID_C    9
#define PID_SET_PRIO     10
#define PID_CLOCK        11
#define PID_KCD          12
#define PID_CRT          13
#define PID_TIMER_IPROC  14
#define PID_UART_IPROC   15
