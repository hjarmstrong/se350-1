/**
 * rtx.h -- RTX OS Public API definitions
 */

#ifndef RTX_H
#define RTX_H

/*---- Configuration --------------------------------------------------------*/

/**
 * If '1', print verbose DEBUG information to UART.
 */
#define DEBUG 1

/**
 * Size of memory block returned by request_memory_block, in bytes.
 */
#define BLOCK_SIZE 0x80

/**
 * Number of processes, not including system process.
 */
#define NUM_TEST_PROCS 6


/*---- Types and Short Forms ------------------------------------------------*/

#define NULL ((void *)0)
#define TRUE 1
#define FALSE 0

#define __SVC_0 __svc_indirect(0)

typedef unsigned char U8;
typedef unsigned int U32;


/*---- Return Values --------------------------------------------------------*/

#define RTX_OK 0
#define RTX_ERR -1

#define RTX_ERROR_LIST_OUT_OF_MEMORY NULL

#define RTX_ERROR_MEMORY_FREEING_UNALLOCATED -2
#define RTX_ERROR_MEMORY_OUT_OF_BOUNDS -3
#define RTX_ERROR_MEMORY_UNALIGNED -4

#define RTX_ERROR_SCHEDULER_CHANGING_NULL_PROCESS_PRIORITY -5
#define RTX_ERROR_SCHEDULER_EMPTY NULL
#define RTX_ERROR_SCHEDULER_PID_NOT_FOUND -1
#define RTX_ERROR_SCHEDULER_PRIORITY_DOESNT_EXIST -6
#define RTX_ERROR_SCHEDULER_SETTING_NULL_PRIORITY_TO_NON_NULL_PROCESS -7
#define RTX_ERROR_SCHEDULER_UNBLOCKING_NON_BLOCK_QUEUE -8
#define RTX_ERROR_SCHEDULER_UNDEFINED_STATE -9


/*---- Priorities -----------------------------------------------------------*/

#define HIGH    0
#define MEDIUM  1
#define LOW     2
#define LOWEST  3
#define PNULL   4


/*---- Section 3.4 -- Process IDs -------------------------------------------*/

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

#define PID_FIRST_IPROC  PID_TIMER_IPROC
#define PID_LAST_IPROC   PID_UART_IPROC

/*---- Assertions -----------------------------------------------------------*/

/**
 * Cheap person's assert.
 */
#define ASSERT(x) while(!(x));


/*---- Section 2.1 -- Memory Management -------------------------------------*/

/*
 * The RTX supports a simple memory management scheme. The memory is divided
 * into blocks of fixed size (128 bytes minimum). The size and the number of
 * these * blocks is a configuration parameter at compile time 1. The blocks
 * can be used by the requesting processes for storing local variables or as
 * envelopes for messages sent to other processes. A block which is no
 * longer needed must be returned to the RTX. Two primitives are to be
 * provided.
 */

/**
 * The primitive returns a pointer to a memory block to the calling process.
 * If no memory block is available, the calling process is blocked until a
 * memory block becomes available. If several processes are waiting for a memory
 * block and a block becomes available, the highest priority waiting process will
 * get it.
 *
 * Implemented in mem/mem.c
 *
 * @SVC
 */

extern void *k_request_memory_block(void);
#define request_memory_block() _request_memory_block((U32)k_request_memory_block)
extern void *_request_memory_block(U32 p_func) __SVC_0;

/**
 * This primitive returns the memory block to the RTX. If there are processes
 * waiting for a block, the block is given to the highest priority process,
 * which is then unblocked. The caller of this primitive never blocks, but
 * could be preempted. Thus, it may affect the currently executing process.
 *
 * Implemented in mem/mem.c
 *
 * @SVC
 */

extern int k_release_memory_block(void *);
#define release_memory_block(mem_blk) _release_memory_block((U32)k_release_memory_block, mem_blk)
extern int _release_memory_block(U32 p_func, void *mem_blk) __SVC_0;


/*---- Section 2.2 -- Processor Management ----------------------------------*/

/**
 * This primitive transfers the control to the RTX (the calling process
 * voluntarily releases the processor). The invoking process remains ready to
 * execute and is put at the end of the ready queue of the same priority.
 * Another process may possibly be selected for execution.
 *
 * Implemented in proc/process.c
 *
 * @SVC
 */
extern int k_release_processor(void);
#define release_processor() _release_processor((U32)k_release_processor)
extern int __SVC_0 _release_processor(U32 p_func);

typedef struct PROC_INIT {
    int m_pid;
    int m_priority;
    int m_stack_size;
    void (*mpf_start_pc) ();
} PROC_INIT;


/*---- Section 2.3 -- Interprocess Communication ----------------------------*/

extern int k_send_message(int process_id, void *message_envelope);
#define send_message(process_id, message_envelope) _send_message((U32)k_send_message, process_id, message_envelope)
extern int _send_message(U32 p_func, int process_id, void *message_envelope) __SVC_0;

extern void *k_receive_message(int *sender_id);
#define receive_message(sender_id) _receive_message((U32)k_receive_message, sender_id)
extern void *_receive_message(U32 p_func, int *sender_id) __SVC_0;

typedef struct msgbuf {
    int mtype;
    char mtext[1];
} msgbuf;

#define DEFAULT 0
#define KCD_REG 1
#define CALLER_MANAGED_PRINT 2 // Like DEFAULT for CRT process, but user is responsible for memory management
#define COUNT_REPORT 3 //Used in stress tests to send a count from A to C
#define WAKEUP10 4 //Used in stress tests for delayed messages sent by C to itself

/*---- Section 2.4 -- Timing Services ---------------------------------------*/

extern int k_delayed_send(int process_id, void *message_envelope, int delay);
#define delayed_send(process_id, message_envelope, delay) _delayed_send((U32)k_delayed_send, process_id, message_envelope, delay)
extern int _delayed_send(U32 p_func, int process_id, void *message_envelope, int delay) __SVC_0;

/**
 * For testing timing services only. Use delayed_send instead.
 */
extern int k_get_time(void);
#define get_time(void) _get_time((U32)k_get_time)
extern int _get_time(U32 p_func) __SVC_0;

/*---- Section 2.5 -- Process Priority --------------------------------------*/

/**
 * Process priorities have an integer priority value (0, 1, 2, 3, 4) where 0 is
 * the highest priority level. Two primitives are to be provided to set and get
 * the process priority.
 */

/**
 * This primitive sets the priority of the process with process_id to the value
 * given in priority. A process may change priority of any process (including
 * itself) except for i-processes (see Section 3.2). The priority of the null
 * process may not be changed from level 4 and it is the only process that can
 * be assigned to level 4 (see Section 3.1.1). The caller of this primitive
 * never blocks, but could be preempted. This preemption may affect the
 * currently executing process.
 *
 * Implemented in proc/scheduler.c
 */
extern int k_set_process_priority(int pid, int prio);
#define set_process_priority(pid, prio) _set_process_priority((U32)k_set_process_priority, pid, prio)
extern int _set_process_priority(U32 p_func, int pid, int prio) __SVC_0;

/**
 * This primitive returns the priority of the process specified by the
 * process_id parameter. For an invalid process_id, the primitive returns -1.
 *
 * Implemented in proc/scheduler.c
 */
extern int k_get_process_priority(int pid);
#define get_process_priority(pid) _get_process_priority((U32)k_get_process_priority, pid)
extern int _get_process_priority(U32 p_func, int pid) __SVC_0;

#endif // RTX_H
