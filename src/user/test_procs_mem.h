#ifndef MEM_TEST_H
#define MEM_TEST_H

void test_add_remove_one(void);
void test_add_remove_many_consecutive(void);
void test_add_remove_separating(void);
void test_add_remove_from_front(void);
void test_add_remove_iterate(void);
void test_merge(void);
void test_release_too_low(void);
void test_release_too_high(void);
void test_release_invalid_offset(void);

void run_mem_tests(void);

#endif // MEM_TEST_H
