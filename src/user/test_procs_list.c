#include "../list/list.h"
#include "../printf.h"


void test_new_list() {
    List l = list_new();

    ASSERT(list_empty(&l)) // test_new_list FAILED!

    printf("test_new_list PASSED!\n\r");
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

    printf("test_one_item PASSED!\n\r");
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

    printf("test_push_shift_many_items PASSED!\n\r");
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

    printf("test_push_pop_many_items PASSED!\n\r");
}


void run_list_tests() {
    test_new_list();
    test_one_item();
    test_push_shift_many_items();
    test_push_pop_many_items();

    printf("All list tests PASSED!\n\r");
}
