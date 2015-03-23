#include "../util/list.h"
#include "../sys/uart_polling.h"
#include "../sysproc/null.h"

PROC_INIT g_test_procs[NUM_TEST_PROCS];

void test_new_list() {
    List l = list_new(IS_USERSPACE);

    ASSERT(list_empty(&l)) // test_new_list FAILED!

    uart1_put_string("test_new_list PASSED!\n\r");
}

void test_one_item() {
    List l = list_new(IS_USERSPACE);
    void *item = (void *)4;

    list_push(&l, item);

    ASSERT(list_front(&l) == (void *)4) // test_one_item (1) FAILED!
    ASSERT(list_back(&l) == (void *)4) // test_one_item (2) FAILED!
    ASSERT(!list_empty(&l)) // test_one_item (3) FAILED!

    list_pop(&l);

    ASSERT(list_empty(&l)) // test_one_item (4) FAILED!

    uart1_put_string("test_one_item PASSED!\n\r");
}

void test_push_shift_many_items() {
    List l = list_new(IS_USERSPACE);
    unsigned int i;

    for (i = 1; i < 100; ++i) {
        list_push(&l, (void *)i);
    }

    for (i = 1; i < 100; ++i) {
        ASSERT(list_front(&l) == (void*)i) // test_push_shift_many_items (1) FAILED!
        list_shift(&l);
    }

    ASSERT(list_empty(&l)) // test_push_shift_many_items (2) FAILED!

    uart1_put_string("test_push_shift_many_items PASSED!\n\r");
}

void test_push_pop_many_items() {
    List l = list_new(IS_USERSPACE);
    unsigned int i;

    for (i = 1; i < 100; ++i) {
        list_push(&l, (void *)i);
        ASSERT(list_back(&l) == (void*)i) // test_push_pop_many_items (1) FAILED!
    }

    for (i = 99; i >= 1; --i) {
        ASSERT(list_back(&l) == (void*)i) // test_push_pop_many_items (2) FAILED!
        list_pop(&l);
    }

    ASSERT(list_empty(&l)) // test_push_pop_many_items (3) FAILED!

    uart1_put_string("test_push_pop_many_items PASSED!\n\r");
}


void test_finished() {
    uart1_put_string("All list tests PASSED!\n\r");
}

void run_tests() {
    test_new_list();
    test_one_item();
    test_push_shift_many_items();
    test_push_pop_many_items();
    test_finished();
}

void set_test_procs() {
    int i;
    for( i = 0; i < NUM_TEST_PROCS; i++ ) {
        g_test_procs[i].m_pid=(U32)(i+1);
        g_test_procs[i].m_stack_size = 0x100;
    }
  
    g_test_procs[0].mpf_start_pc = &run_tests;
    g_test_procs[0].m_priority   = HIGH;
    g_test_procs[0].m_stack_size = 0x3000;
    
    g_test_procs[1].mpf_start_pc = &null_proc;
    g_test_procs[1].m_priority   = PNULL;
    
    g_test_procs[2].mpf_start_pc = &null_proc;
    g_test_procs[2].m_priority   = PNULL;
    
    g_test_procs[3].mpf_start_pc = &null_proc;
    g_test_procs[3].m_priority   = PNULL;
    
    g_test_procs[4].mpf_start_pc = &null_proc;
    g_test_procs[4].m_priority   = PNULL;
    
    g_test_procs[5].mpf_start_pc = &null_proc;
    g_test_procs[5].m_priority   = PNULL;
}
