#include "./mem.h"
#include "../printf.h"

#define NUM_BLOCKS 150

/**
 * Crashes with error `error` if `condition` is equal to `0`.
 */
void assert(int condition, const char* error) {
    int i = 0;
    if (condition == 0) {
        printf("\n\r########################################\n\r%s\n\r########################################\n\r",
            error);

        // try to crash
        i = i / 0;

        // if we didn't crash for whatever reason, we still shouldn't return
        while(1) {
            /* pass */
        }
    }
}

/**
 * A proxy for release_memory_block which asserts success.
 */
int assert_release_memory_block(void *mem_block) {
    int exit_code = release_memory_block(mem_block);
    assert(exit_code == 0, "assert_release_memory_block FAILED!");
    return exit_code;
}

/**
 * Tests memory block reuse when creating and removing a single memory block
 * from a single memory node.
 */
void test_add_remove_one(void) {
    void *address_1;

    address_1 = request_memory_block();
    assert_release_memory_block(address_1);

    assert(address_1 == request_memory_block(), "test_add_remove_one FAILED!");
    assert_release_memory_block(address_1);
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
            address_1 = request_memory_block();
        } else if (i == 1) {
            address_2 = request_memory_block();
            block_size = ((char *) address_2) - ((char *) address_1);
        } else {
            address_2 = request_memory_block();
            assert(address_2 == ((char *) address_1) + block_size*i, "expected consecutive blocks");
        }
    }
    for (i = NUM_BLOCKS - 1; i >= 0; --i) {
        assert_release_memory_block(((char *) address_1) + block_size*i);
    }

    address_2 = request_memory_block();
    assert_release_memory_block(address_2);

    assert(address_1 == address_2, "test_add_remove FAILED!");
}

/**
 * Tests memory block reuse when separating blocks.
 */
void test_add_remove_separating(void) {
    void *address_1;
    void *address_2;
    void *address_3;

    address_1 = request_memory_block();
    address_2 = request_memory_block();
    address_3 = request_memory_block();

    assert_release_memory_block(address_2);
    assert_release_memory_block(address_1);
    assert_release_memory_block(address_3);

    assert(request_memory_block() == address_1, "test_add_remove_separating FAILED!");
    assert(request_memory_block() == address_2, "test_add_remove_separating FAILED!");
    assert(request_memory_block() == address_3, "test_add_remove_separating FAILED!");

    assert_release_memory_block(address_3);
    assert_release_memory_block(address_2);
    assert_release_memory_block(address_1);
}

/**
 * Tests memory block reuse when removing from front.
 */
void test_add_remove_from_front(void) {
    void *address_1;
    void *address_2;
    void *address_3;

    address_1 = request_memory_block();
    address_2 = request_memory_block();
    address_3 = request_memory_block();

    assert_release_memory_block(address_1);
    assert_release_memory_block(address_2);
    assert_release_memory_block(address_3);

    assert(request_memory_block() == address_1, "test_add_remove_separating FAILED!");
    assert(request_memory_block() == address_2, "test_add_remove_separating FAILED!");
    assert(request_memory_block() == address_3, "test_add_remove_separating FAILED!");

    assert_release_memory_block(address_3);
    assert_release_memory_block(address_2);
    assert_release_memory_block(address_1);
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
            address_1 = request_memory_block();
        } else if (i == 1) {
            address_2 = request_memory_block();
            block_size = ((char *) address_2) - ((char *) address_1);
        } else {
            address_2 = request_memory_block();
            assert(address_2 == ((char *) address_1) + block_size*i, "expected consecutive blocks");
        }
    }

    assert_release_memory_block(address_1);
    assert_release_memory_block(((char *) address_1) + block_size);

    assert(request_memory_block() == address_1, "test_add_remove_iterate FAILED or is incorrect. Check NUM_BLOCKS.");

    for (i = NUM_BLOCKS - 1; i >= 2; --i) {
        assert_release_memory_block(((char *) address_1) + block_size*i);
    }

    address_2 = request_memory_block();
    assert_release_memory_block(address_2);

    assert(address_1 == address_2, "test_add_remove_iterate FAILED!");
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
            address_1 = request_memory_block();
        } else if (i == 1) {
            address_2 = request_memory_block();
            block_size = ((char *) address_2) - ((char *) address_1);
        } else {
            address_2 = request_memory_block();
            assert(address_2 == ((char *) address_1) + block_size*i, "expected consecutive blocks");
        }
    }

    assert_release_memory_block(((char *) address_1) + block_size*40);
    assert(request_memory_block() == ((char *) address_1) + block_size*40, "test_merge FAILED!");

    for (i = NUM_BLOCKS - 1; i >= 0; --i) {
        assert_release_memory_block(((char *) address_1) + block_size*i);
    }
}

/**
 * Tests that you cannot deallocate the first memory block that has not been allocated.
 */
void test_release_too_low(void) {
    void *address_1;

    address_1 = request_memory_block();
    assert_release_memory_block(address_1);

    assert(release_memory_block(address_1) != 0, "test_release_too_low FAILED!");
}

/**
 * Tests that you cannot deallocate past a block.
 */
void test_release_too_high(void) {
    void *address_1;
    void *address_2;
    int block_size;

    address_1 = request_memory_block();
    address_2 = request_memory_block();

    block_size = ((char *) address_1) - ((char *) address_2);

    assert(release_memory_block(((char *) address_1) + block_size * 3), "test_release_too_high FAILED!");
    assert(release_memory_block(((char *) address_1) + block_size * 2), "test_release_too_high FAILED!");

    assert_release_memory_block(address_1);
    assert_release_memory_block(address_2);
}

void test_release_invalid_offset(void) {
    void *address_1;
    void *address_2;
    int block_size;

    address_1 = request_memory_block();
    address_2 = request_memory_block();

    block_size = ((char *) address_1) - ((char *) address_2);

    assert(release_memory_block(((char *) address_1) + 1), "test_release_invalid_offset FAILED!");
    assert(release_memory_block(((char *) address_1) + 4), "test_release_invalid_offset FAILED!");
    assert(release_memory_block(((char *) address_1) + block_size/2), "test_release_invalid_offset FAILED!");
    assert(release_memory_block(((char *) address_2) - 1), "test_release_invalid_offset FAILED!");
    assert(release_memory_block(((char *) address_2) - 4), "test_release_invalid_offset FAILED!");
    assert(release_memory_block(((char *) address_2) - block_size/2), "test_release_invalid_offset FAILED!");

    assert_release_memory_block(address_1);
    assert_release_memory_block(address_2);
}

void run_mem_tests(void) {
		test_add_remove_one();
		test_add_remove_many_consecutive();
		test_add_remove_separating();
	  test_add_remove_from_front();
		test_add_remove_iterate();
		test_merge();
		test_release_too_low();
		test_release_too_high();
		test_release_invalid_offset();
}
