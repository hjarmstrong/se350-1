#include "./list.h"
#include <stdlib.h>
#include <assert.h>
#include <stdio.h>

int main() {
    List l = list_new();
    assert(list_empty(&l));
    void *first = (void *)4;
    list_push(&l, first);
    assert(list_front(&l) == 4);
    assert(list_back(&l) == 4);
    assert(!list_empty(&l));
    list_pop(&l);
    assert(list_empty(&l));

    for (unsigned int i = 1; i < 500; ++i) {
        list_push(&l, (void *)i);
    }
    for (unsigned int i = 1; i < 500; ++i) {
        assert(list_front(&l) == (void*)i);
        list_shift(&l);
    }

    assert(list_empty(&l));

    for (unsigned int i = 1; i < 500; ++i) {
        list_push(&l, (void *)i);
        assert(list_back(&l) == (void*)i);
    }
    for (unsigned int i = 500 - 1; i >= 1; --i) {
        assert(list_back(&l) == (void*)i);
        list_pop(&l);
    }

    assert(list_empty(&l));

    return 0;
}
