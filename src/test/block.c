#include <stdarg.h>
#include <stddef.h>
#include <setjmp.h>
#include <string.h>
#include <cmocka.h>

#include "block.h"

static int dummy = 1;
static basic_block block_bad_ptr = {NULL, sizeof dummy};
static basic_block block_bad_size = {&dummy, 0};
static basic_block block_good = {&dummy, sizeof dummy};

static void test_block_isnull(void **state)
{
    (void) state;

    // Passing NULL to basic_block_isnull should assert
    expect_assert_failure(basic_block_isnull(NULL));

    // Passing a null block to basic_block_isnull should return true
    assert_true(basic_block_isnull(&BASIC_BLOCK_NULL));

    // Passing a non-null block to basic_block_isnull should return false
    assert_false(basic_block_isnull(&block_good));
}

static void test_block_isinit(void **state)
{
    (void) state;

    // Passing NULL to basic_block_isinit should assert
    expect_assert_failure(basic_block_isinit(NULL));

    // Passing a null block to basic_block_isinit should return false
    assert_false(basic_block_isinit(&BASIC_BLOCK_NULL));

    // Passing blocks with ptr NULL and size nonzero, and ptr non-NULL
    // and size zero (resp.) should return false
    assert_false(basic_block_isinit(&block_bad_ptr));
    assert_false(basic_block_isinit(&block_bad_size));

    // Passing a block with ptr non-NULL and size nonzero should return true
    assert_true(basic_block_isinit(&block_good));
}

static void test_block_move(void **state)
{
    (void) state;

    // Passing NULL to basic_block_move should assert
    expect_assert_failure(basic_block_move(NULL));

    // Attempting to move from a non-initialised block object should assert
    expect_assert_failure(basic_block_move(&BASIC_BLOCK_NULL));
    expect_assert_failure(basic_block_move(&block_bad_ptr));
    expect_assert_failure(basic_block_move(&block_bad_size));

    basic_block src = block_good;
    basic_block dest = basic_block_move(&src);

    // Moving from an initialised block should return an initialised block
    assert_true(basic_block_isinit(&dest));

    // ...and set the argument block object to null
    assert_true(basic_block_isnull(&src));

    // ...and have the same ptr and size member as the argument block object
    assert_ptr_equal(dest.ptr, block_good.ptr);
    assert_true(dest.size == block_good.size);
}

static void test_block_clone(void **state)
{
    (void) state;

    // Passing NULL to basic_block_clone should assert
    expect_assert_failure(basic_block_clone(NULL));

    // Attempting to clone a non-initialised and non-null block object should
    // assert
    expect_assert_failure(basic_block_clone(&block_bad_ptr));
    expect_assert_failure(basic_block_clone(&block_bad_size));

    // Attempting to clone a null block object should result in a null
    // block object
    basic_block const should_be_null = basic_block_clone(&BASIC_BLOCK_NULL);
    assert_true(basic_block_isnull(&should_be_null));

    basic_block src = block_good;
    basic_block dest = basic_block_clone(&src);

    // Moving from an initialised block should return an initialised block
    assert_true(basic_block_isinit(&dest));

    // ...and leave the block argument initialised
    assert_true(basic_block_isinit(&src));

    // ...and have the same size as the block argument
    assert_true(dest.size == block_good.size);

    // ...and have equal memory contents as the block argument
    assert_memory_equal(src.ptr, dest.ptr, src.size);
}

static void test_block_alloc(void **state)
{
    (void) state;
    enum {
        allocation_size = 512
    };

    // Passing 0 as the size parameter should assert
    expect_assert_failure(basic_block_alloc(0));

    basic_block block = basic_block_alloc(allocation_size);
    if (basic_block_isnull(&block)) {
        fail_msg("Failed to allocate block for testing");
    }

    // The block should be initialised
    assert_true(basic_block_isinit(&block));

    // ...and have size equal to the requested allocation size
    assert_true(block.size == allocation_size);

    // ...and have zero-initialised memory
    static char const zero[allocation_size] = {0};
    assert_memory_equal(block.ptr, zero, allocation_size);
}

static void test_block_realloc(void **state)
{
    (void) state;
    enum {
        allocation_size = 128,
        grow_size = allocation_size * 2,
        shrink_size = allocation_size / 2
    };

    // Passing NULL as the basic_block pointer should assert
    expect_assert_failure(basic_block_realloc(NULL, 1));

    // Passing a non-null and non-initialised block object should assert
    expect_assert_failure(basic_block_realloc(&block_bad_size, 1));
    expect_assert_failure(basic_block_realloc(&block_bad_ptr, 1));

    // Passing 0 as the size should assert
    basic_block block = block_good;
    expect_assert_failure(basic_block_realloc(&block, 0));

    // Passing a null block should defer to basic_block_alloc, we check
    // here that the block (after reallocation) is initialised, and has
    // the appropriate size
    basic_block null_block = BASIC_BLOCK_NULL;
    assert_non_null(basic_block_realloc(&null_block, allocation_size));
    assert_true(basic_block_isinit(&null_block));
    assert_true(null_block.size == allocation_size);

    // Allocate a block for testing
    basic_block block_to_grow = basic_block_alloc(allocation_size);
    if (basic_block_isnull(&block_to_grow)) {
        fail_msg("Failed to allocate block for testing");
    }

    memset(block_to_grow.ptr, 1, block_to_grow.size);
    char grow_unchanged[allocation_size] = {0};
    char grow_extra[grow_size - allocation_size] = {0};
    memset(grow_unchanged, 1, allocation_size);

    // Growing allocation: ensure that the returned block is initialised,
    // basic_block_realloc returns a non-null pointer, the size has been
    // updated correctly, the memory up to the grow size is unchanged,
    // and the extra space allocated is zero-initialised
    assert_non_null(basic_block_realloc(&block_to_grow, grow_size));
    assert_true(basic_block_isinit(&block_to_grow));
    assert_true(block_to_grow.size == grow_size);
    assert_memory_equal(block_to_grow.ptr, grow_unchanged, allocation_size);
    assert_memory_equal((char *)block_to_grow.ptr + allocation_size,
            grow_extra,
            grow_size - allocation_size);

    // Allocate a block for testing
    basic_block block_to_shrink = basic_block_alloc(allocation_size);
    if (basic_block_isnull(&block_to_shrink)) {
        fail_msg("Failed to allocate block for testing");
    }

    memset(block_to_shrink.ptr, 1, block_to_shrink.size);
    char shrink_unchanged[shrink_size] = {0};
    memset(shrink_unchanged, 1, shrink_size);

    // Shrinking allocation: ensure that the returned block is
    // initialised, basic_block_realloc returns a non-null pointer,
    // the size has been updated correctly, and the memory up to the shrink
    // size is unchanged
    assert_non_null(basic_block_realloc(&block_to_shrink, shrink_size));
    assert_true(basic_block_isinit(&block_to_shrink));
    assert_true(block_to_shrink.size == shrink_size);
    assert_memory_equal(block_to_shrink.ptr, shrink_unchanged, shrink_size);
}

static void test_block_dealloc(void **state)
{
    (void) state;

    // Passing NULL to basic_block_dealloc should assert
    expect_assert_failure(basic_block_dealloc(NULL));

    // Attempting to deallocate a non-initialised and non-null block
    // should assert
    expect_assert_failure(basic_block_dealloc(&block_bad_ptr));
    expect_assert_failure(basic_block_dealloc(&block_bad_size));

    // We need to get an allocated block to test against, fail the
    // test if the allocation failed.
    basic_block block = basic_block_alloc(sizeof dummy);
    if (basic_block_isnull(&block)) {
        fail_msg("Failed to allocate block for testing");
    }

    // Now deallocate the block. It should be null after deallocation.
    basic_block_dealloc(&block);
    assert_true(basic_block_isnull(&block));
}

int main(int argc, char **argv)
{
    (void) argc;
    (void) argv;

    struct CMUnitTest const tests[] = {
        cmocka_unit_test(test_block_isnull),
        cmocka_unit_test(test_block_isinit),
        cmocka_unit_test(test_block_move),
        cmocka_unit_test(test_block_clone),
        cmocka_unit_test(test_block_alloc),
        cmocka_unit_test(test_block_realloc),
        cmocka_unit_test(test_block_dealloc)
    };

    return cmocka_run_group_tests(tests, NULL, NULL);
}
