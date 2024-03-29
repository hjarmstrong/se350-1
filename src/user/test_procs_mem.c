#include "../core/mem.h"
#include "../sys/uart_polling.h"

#define NUM_BLOCKS (((((unsigned int)heap_high_address) - ((unsigned int)heap_low_address) - 2 * HEADER_SIZE) / BLOCK_SIZE) - 5)


/**
 * A proxy for k_release_memory_block which asserts success.
 */
int assert_k_release_memory_block(void *mem_block) {
    int exit_code = k_release_memory_block(mem_block);
    ASSERT(exit_code == 0) // assert_k_release_memory_block FAILED!
    return exit_code;
}

/**
 * Tests memory block reuse when creating and removing a single memory block
 * from a single memory node.
 */
void test_add_remove_one(void) {
    void *address_1;

    address_1 = k_request_memory_block();
    assert_k_release_memory_block(address_1);

    ASSERT(address_1 == k_request_memory_block()) // test_add_remove_one FAILED!
    assert_k_release_memory_block(address_1);
    uart1_put_string("test_add_remove_one PASSED!\n\r");
}

/**
 * Tests memory block reuse when adding and removing many memory blocks
 * from a single memory node.
 */
void test_add_remove_many_consecutive(void) {
    void *address_1;
    void *address_2;
    int block_size;
    int i;

    for (i = 0; i < NUM_BLOCKS; ++i) {
        if (!i) {
            address_1 = k_request_memory_block();
        } else if (i == 1) {
            address_2 = k_request_memory_block();
            block_size = ((U8 *) address_2) - ((U8 *) address_1);
        } else {
            address_2 = k_request_memory_block();
            ASSERT(address_2 == ((U8 *) address_1) + block_size*i) // expected consecutive blocks
        }
    }
    for (i = NUM_BLOCKS - 1; i >= 0; --i) {
        assert_k_release_memory_block(((U8 *) address_1) + block_size*i);
    }

    address_2 = k_request_memory_block();
    assert_k_release_memory_block(address_2);

    ASSERT(address_1 == address_2) // test_add_remove_many_consecutive FAILED!
    uart1_put_string("test_add_remove_many_consecutive PASSED!\n\r");
}

/**
 * Tests memory block reuse when separating blocks.
 */
void test_add_remove_separating(void) {
    void *address_1;
    void *address_2;
    void *address_3;

    address_1 = k_request_memory_block();
    address_2 = k_request_memory_block();
    address_3 = k_request_memory_block();

    assert_k_release_memory_block(address_2);
    assert_k_release_memory_block(address_1);
    assert_k_release_memory_block(address_3);

    ASSERT(k_request_memory_block() == address_1) // test_add_remove_separating (1) FAILED!
    ASSERT(k_request_memory_block() == address_2) // test_add_remove_separating (2) FAILED!
    ASSERT(k_request_memory_block() == address_3) // test_add_remove_separating (3) FAILED!

    assert_k_release_memory_block(address_3);
    assert_k_release_memory_block(address_2);
    assert_k_release_memory_block(address_1);
    uart1_put_string("test_add_remove_separating PASSED!\n\r");
}

/**
 * Tests memory block reuse when removing from front.
 */
void test_add_remove_from_front(void) {
    void *address_1;
    void *address_2;
    void *address_3;

    address_1 = k_request_memory_block();
    address_2 = k_request_memory_block();
    address_3 = k_request_memory_block();

    assert_k_release_memory_block(address_1);
    assert_k_release_memory_block(address_2);
    assert_k_release_memory_block(address_3);

    ASSERT(k_request_memory_block() == address_1) // test_add_remove_from_front (1) FAILED!
    ASSERT(k_request_memory_block() == address_2) // test_add_remove_from_front (2) FAILED!
    ASSERT(k_request_memory_block() == address_3) // test_add_remove_from_front (3) FAILED!

    assert_k_release_memory_block(address_3);
    assert_k_release_memory_block(address_2);
    assert_k_release_memory_block(address_1);
    uart1_put_string("test_add_remove_from_front PASSED!\n\r");
}

/**
 * Tests memory block reuse when there is no memory at the end.
 *
 * NOTE: for this test to work correctly, requesting NUM_BLOCKS + 1 blocks should fail.
 * Test that manually.
 */
void test_add_remove_iterate(void) {
    void *address_1;
    void *address_2;
    int block_size;
    int i;

    for (i = 0; i < NUM_BLOCKS; ++i) {
        if (!i) {
            address_1 = k_request_memory_block();
        } else if (i == 1) {
            address_2 = k_request_memory_block();
            block_size = ((U8 *) address_2) - ((U8 *) address_1);
        } else {
            address_2 = k_request_memory_block();
            ASSERT(address_2 == ((U8 *) address_1) + block_size*i) // expected consecutive blocks
        }
    }

    assert_k_release_memory_block(address_1);
    assert_k_release_memory_block(((U8 *) address_1) + block_size);

    ASSERT(k_request_memory_block() == address_1) // test_add_remove_iterate FAILED!
    assert_k_release_memory_block(address_1);

    for (i = NUM_BLOCKS - 1; i >= 2; --i) {
        assert_k_release_memory_block(((U8 *) address_1) + block_size*i);
    }

    address_2 = k_request_memory_block();
    assert_k_release_memory_block(address_2);

    ASSERT(address_1 == address_2) // test_add_remove_iterate FAILED!
    uart1_put_string("test_add_remove_iterate PASSED!\n\r");
}

/**
 * Tests that memory blocks can merge.
 *
 * NOTE: for this test to work correctly, requesting NUM_BLOCKS + 1 blocks should fail.
 * Test that manually.
 */
void test_merge(void) {
    void *address_1;
    void *address_2;
    int block_size;
    int i;

    for (i = 0; i < NUM_BLOCKS; ++i) {
        if (!i) {
            address_1 = k_request_memory_block();
        } else if (i == 1) {
            address_2 = k_request_memory_block();
            block_size = ((U8 *) address_2) - ((U8 *) address_1);
        } else {
            address_2 = k_request_memory_block();
            ASSERT(address_2 == ((U8 *) address_1) + block_size*i) // expected consecutive blocks
        }
    }

    assert_k_release_memory_block(((U8 *) address_1) + block_size*40);
    ASSERT(k_request_memory_block() == ((U8 *) address_1) + block_size*40) // test_merge FAILED!

    for (i = NUM_BLOCKS - 1; i >= 0; --i) {
        assert_k_release_memory_block(((U8 *) address_1) + block_size*i);
    }
    uart1_put_string("test_merge PASSED!\n\r");
}

/**
 * Tests that you cannot deallocate the first memory block that has not been allocated.
 */
void test_release_too_low(void) {
    void *address_1;

    address_1 = k_request_memory_block();
    assert_k_release_memory_block(address_1);

    ASSERT(k_release_memory_block(address_1) == RTX_ERROR_MEMORY_FREEING_UNALLOCATED) // test_release_too_low FAILED!
    uart1_put_string("test_release_too_low PASSED!\n\r");
}

/**
 * Tests that you cannot deallocate past a block.
 */
void test_release_too_high(void) {
    void *address_1;
    void *address_2;
    int block_size;

    address_1 = k_request_memory_block();
    address_2 = k_request_memory_block();

    block_size = ((U8 *) address_2) - ((U8 *) address_1);

    ASSERT(k_release_memory_block(((U8 *) address_1) + block_size * 3) == RTX_ERROR_MEMORY_FREEING_UNALLOCATED) // test_release_too_high FAILED!
    ASSERT(k_release_memory_block(((U8 *) address_1) + block_size * 2) == RTX_ERROR_MEMORY_FREEING_UNALLOCATED) // test_release_too_high FAILED!

    assert_k_release_memory_block(address_1);
    assert_k_release_memory_block(address_2);
    uart1_put_string("test_release_too_high PASSED!\n\r");
}

void test_release_invalid_offset(void) {
    void *address_1;
    void *address_2;
    int block_size;

    address_1 = k_request_memory_block();
    address_2 = k_request_memory_block();

    block_size = ((U8 *) address_2) - ((U8 *) address_1);

    ASSERT(k_release_memory_block(((U8 *) address_1) + 1)) // test_release_invalid_offset (1) FAILED!
    ASSERT(k_release_memory_block(((U8 *) address_1) + 4)) // test_release_invalid_offset (2) FAILED!
    ASSERT(k_release_memory_block(((U8 *) address_1) + block_size/2)) // test_release_invalid_offset (3) FAILED!
    ASSERT(k_release_memory_block(((U8 *) address_2) - 1)) // test_release_invalid_offset (4) FAILED!
    ASSERT(k_release_memory_block(((U8 *) address_2) - 4)) // test_release_invalid_offset (5) FAILED!
    ASSERT(k_release_memory_block(((U8 *) address_2) - block_size/2)) // test_release_invalid_offset (6) FAILED!

    assert_k_release_memory_block(address_1);
    assert_k_release_memory_block(address_2);
    uart1_put_string("test_release_invalid_offset PASSED!\n\r");
}

void test_add_remove_some(void) {
    // TODO: make this into a real test case
    //int exit_code;
    //int i;
    //void *addr;

    //printf("Starting address is 0x%x\n\r", ((void *)(&Image$$RW_IRAM1$$ZI$$Limit)));
    //for (i = 0; i < 100; ++i) {
    //    addr = k_request_memory_block();
    //    printf("k_request_memory_block: 0x%x\n\r", addr);

    //    if (i % 5 == 0) {
    //        exit_code = k_release_memory_block(addr);
    //        printf("Exit code from k_release_memory_block: %d\n\r", exit_code);
    //    }
    //}
}


void k_run_ktests(void) {
    test_add_remove_one();
    test_add_remove_many_consecutive();
    test_add_remove_separating();
    test_add_remove_from_front();
    test_add_remove_iterate();
    test_merge();
    test_release_too_low();
    test_release_too_high();
    test_release_invalid_offset();
    test_add_remove_some();

    uart1_put_string("All memory tests PASSED!\n\r");
}

#include "ktest.fragment.c"
