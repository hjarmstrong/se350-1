#include "../list/list.h"
#include "../printf.h"

#include "test.h"


void test_new_list() {
    List l = list_new();

    assert(list_empty(&l), "test_new_list FAILED!\n\r");

    printf("test_new_list PASSED!\n\r");
}

void test_one_item() {
    List l = list_new();
    void *item = (void *)4;

    list_push(&l, item);

    assert(list_front(&l) == (void *)4, "test_one_item (1) FAILED!\n\r");
    assert(list_back(&l) == (void *)4, "test_one_item (2) FAILED!\n\r");
    assert(!list_empty(&l), "test_one_item (3) FAILED!\n\r");

    list_pop(&l);

    assert(list_empty(&l), "test_one_item (4) FAILED!\n\r");

    printf("test_one_item PASSED!\n\r");
}

void test_push_shift_many_items() {
    List l = list_new();

    for (unsigned int i = 1; i < 500; ++i) {
        list_push(&l, (void *)i);
    }

    for (unsigned int i = 1; i < 500; ++i) {
        assert(list_front(&l) == (void*)i, "test_push_shift_many_items (1) FAILED!\n\r");
        list_shift(&l);
    }

    assert(list_empty(&l), "test_push_shift_many_items (2) FAILED!\n\r");

    printf("test_push_shift_many_items PASSED!\n\r");
}

void test_push_pop_many_items() {
    List l = list_new();

    for (unsigned int i = 1; i < 500; ++i) {
        list_push(&l, (void *)i);
        assert(list_back(&l) == (void*)i, "test_push_pop_many_items (1) FAILED!\n\r");
    }

    for (unsigned int i = 499; i >= 1; --i) {
        assert(list_back(&l) == (void*)i, "test_push_pop_many_items (2) FAILED!\n\r");
        list_pop(&l);
    }

    assert(list_empty(&l), "test_push_pop_many_items (3) FAILED!\n\r");

    printf("test_push_pop_many_items PASSED!\n\r");
}


void run_list_tests() {
    test_new_list();
    test_one_item();
    test_push_shift_many_items();
    test_push_pop_many_items();

    printf("All list tests PASSED!\n\r");
}
