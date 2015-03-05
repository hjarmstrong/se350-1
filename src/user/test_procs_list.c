#include "../util/list.h"
#include "../sys/uart_polling.h"

void test_new_list() {
    List l = list_new();

    ASSERT(list_empty(&l)) // test_new_list FAILED!

    uart1_put_string("test_new_list PASSED!\n\r");
}

void test_one_item() {
    List l = list_new();
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
    List l = list_new();
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
    List l = list_new();
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

int k_run_ktests() {
    test_new_list();
    test_one_item();
    test_push_shift_many_items();
    test_push_pop_many_items();
    test_finished();
    return 0;
}

#include "ktest.fragment.c"
