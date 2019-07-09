#include <stdarg.h>
#include <stddef.h>
#include <setjmp.h>
#include <cmocka.h>
#include <string.h>

#include "array.h"

enum { dummy_size = 8 };

static void test_array_isnull(void **state)
{
    (void) state;

    int dummy[dummy_size] =  {0};
    basic_array array_good = {{&dummy, sizeof dummy}, sizeof(int)};
    basic_array array_bad_size = {{&dummy, sizeof dummy}, 0};
    basic_array array_bad_data = {BASIC_BLOCK_NULL, sizeof(int)};

    // Passing NULL to basic_array_isnull should assert
    expect_assert_failure(basic_array_isnull(NULL));

    // Passing a null basic_array to basic_array_isnull should return true
    assert_true(basic_array_isnull(&BASIC_ARRAY_NULL));

    // Passing a non-null basic_array to basic_array_isnull should return
    // false
    assert_false(basic_array_isnull(&array_good));

    // Passing non-null, non-init array to basic_array_isnull should return
    // false
    assert_false(basic_array_isnull(&array_bad_size));
    assert_false(basic_array_isnull(&array_bad_data));
}

static void test_array_isinit(void **state)
{
    (void) state;

    int dummy[dummy_size] =  {0};
    basic_array array_good = {{&dummy, sizeof dummy}, sizeof(int)};
    basic_array array_bad_size = {{&dummy, sizeof dummy}, 0};
    basic_array array_bad_data = {BASIC_BLOCK_NULL, sizeof(int)};

    // Passing NULL to basic_array_isinit should assert
    expect_assert_failure(basic_array_isinit(NULL));

    // Passing an initialised block to basic_array_isinit should return true
    assert_true(basic_array_isinit(&array_good));

    // Passing a null block to basic_array_isinit should return false
    assert_false(basic_array_isinit(&BASIC_ARRAY_NULL));

    // Passing non-null, non-init arrays to basic_array_isnull should
    // return false
    assert_false(basic_array_isinit(&array_bad_size));
    assert_false(basic_array_isinit(&array_bad_data));
}

static void test_array_move(void **state)
{
    (void) state;

    int dummy[dummy_size] =  {0};
    basic_array array_good = {{&dummy, sizeof dummy}, sizeof(int)};
    basic_array array_good_copy = array_good;
    basic_array array_bad_size = {{&dummy, sizeof dummy}, 0};
    basic_array array_bad_data = {BASIC_BLOCK_NULL, sizeof(int)};

    // Passing NULL, a pointer to a null block, or pointers to non-null,
    // non-init arrays should assert
    expect_assert_failure(basic_array_move(NULL));
    expect_assert_failure(basic_array_move(&BASIC_ARRAY_NULL));
    expect_assert_failure(basic_array_move(&array_bad_size));
    expect_assert_failure(basic_array_move(&array_bad_data));

    // Passing an initialised array to basic_array_move should succeed,
    // and the original block should be set to null, and the result
    // of the move should be fieldwise equal to array_good and
    // initialised
    basic_array move_result = basic_array_move(&array_good);
    assert_true(basic_array_isnull(&array_good));
    assert_true(basic_array_isinit(&move_result));
    assert_true(move_result.data.ptr == array_good_copy.data.ptr);
    assert_true(move_result.data.size == array_good_copy.data.size);
    assert_true(move_result.elem_size == array_good_copy.elem_size);
}

static void test_array_clone(void **state)
{
    (void) state;

    int dummy[dummy_size] =  {0};
    memset(dummy, 1, sizeof dummy);
    basic_array array_good = {{&dummy, sizeof dummy}, sizeof(int)};
    basic_array array_bad_size = {{&dummy, sizeof dummy}, 0};
    basic_array array_bad_data = {BASIC_BLOCK_NULL, sizeof(int)};

    // Passing NULL, or pointers to non-null, non-init arrays should assert
    expect_assert_failure(basic_array_clone(NULL));
    expect_assert_failure(basic_array_clone(&array_bad_size));
    expect_assert_failure(basic_array_clone(&array_bad_data));

    // Passing a null array to basic_array_clone should return a null
    // array
    basic_array clone_null_result = basic_array_clone(&BASIC_ARRAY_NULL);
    assert_true(basic_array_isnull(&clone_null_result));

    // Passing an initialised array should return an initialised array,
    // with equal elem_size field, equal data.size field,
    // and contents bitwise equal to the contents of the argument array
    basic_array clone_result = basic_array_clone(&array_good);
    assert_true(basic_array_isinit(&clone_result));
    assert_true(clone_result.elem_size == array_good.elem_size);
    assert_true(clone_result.data.size == array_good.data.size);
    assert_memory_equal(
            array_good.data.ptr,
            clone_result.data.ptr,
            clone_result.data.size);

    basic_array_dealloc(&clone_result);
}

static void test_array_alloc(void **state)
{
    (void) state;

    // Passing 0 as the elem_size parameter or a
    // non-positive elem_count parameter should assert
    expect_assert_failure(basic_array_alloc(0, dummy_size));
    expect_assert_failure(basic_array_alloc(sizeof(int), 0));
    expect_assert_failure(basic_array_alloc(sizeof(int), -1));

    basic_array array = basic_array_alloc(sizeof(int), dummy_size);
    if (basic_array_isnull(&array)) {
        fail_msg("Failed to allocate block for testing");
    }

    // The allocated array should be initialised, have elem_size equal
    // to what was requested, and have elem_count equal to what was
    // requested
    assert_true(basic_array_isinit(&array));
    assert_true(array.elem_size == sizeof(int));
    assert_true(array.data.size == sizeof(int) * dummy_size);

    // and the memory should be zero-initialised
    static int const zero[dummy_size] = {0};
    assert_memory_equal(array.data.ptr, zero, sizeof zero);

    basic_array_dealloc(&array);
}

static void test_array_realloc(void **state)
{
    (void) state;

    enum {
        shrink_size = dummy_size / 2,
        grow_size = dummy_size * 2
    };

    int dummy[dummy_size] =  {0};
    memset(dummy, 1, sizeof dummy);
    basic_array array_bad_size = {{&dummy, sizeof dummy}, 0};
    basic_array array_bad_data = {BASIC_BLOCK_NULL, sizeof(int)};

    basic_array array_good = basic_array_alloc(sizeof(int), dummy_size);
    if (basic_array_isnull(&array_good)) {
        fail_msg("Failed to allocate array for testing");
    }

    memset(basic_array_at(&array_good, 0), 1, sizeof(int) * dummy_size);

    // Passing NULL or a non-initialised array, or a zero or negative
    // elem_count to basic_array_realloc should assert
    expect_assert_failure(basic_array_realloc(NULL, grow_size));
    expect_assert_failure(basic_array_realloc(&BASIC_ARRAY_NULL, grow_size));
    expect_assert_failure(basic_array_realloc(&array_bad_size, grow_size));
    expect_assert_failure(basic_array_realloc(&array_bad_data, grow_size));

    // Growing reallocation
    // Passing an initialised array to basic_array_realloc with a valid
    // elem_count should succeed, and the returned array should be
    // initialised, and the new capacity should be equal to the requested,
    // and the extra data should be zero initialised, and the existing
    // data should be untouched
    int grow_extra[grow_size - dummy_size] = {0};
    basic_array *array_grow = basic_array_realloc(&array_good, grow_size);
    assert_non_null(array_grow);
    assert_true(basic_array_isinit(array_grow));
    assert_true(basic_array_cap(array_grow) == grow_size);
    assert_memory_equal(
            basic_array_at_c(array_grow, dummy_size),
            grow_extra,
            grow_size - dummy_size);
    assert_memory_equal(
            basic_array_at_c(array_grow, 0),
            dummy,
            dummy_size);

    // Shrinking reallocation
    // Passing an initialised array to basic_array_realloc with a valid
    // elem_count should succeed, and the returned array should be
    // initialised, and the new capacity should be equal to the request,
    // and the existing data should be untouched
    basic_array *array_shrink = basic_array_realloc(&array_good, shrink_size);
    assert_non_null(array_shrink);
    assert_true(basic_array_isinit(array_shrink));
    assert_true(basic_array_cap(array_shrink) == shrink_size);
    assert_memory_equal(
            basic_array_at_c(array_shrink, 0),
            dummy,
            dummy_size / 2);

    basic_array_dealloc(array_shrink);
}

static void test_array_dealloc(void **state)
{
    (void) state;

    int dummy[dummy_size] = {0};
    basic_array array_bad_size = {{&dummy, sizeof dummy}, 0};
    basic_array array_bad_data = {BASIC_BLOCK_NULL, sizeof(int)};

    // Passing NULL to basic_array_dealloc should assert
    expect_assert_failure(basic_array_dealloc(NULL));

    // Attempting to deallocate a non-initialised and non-null array
    // should assert
    expect_assert_failure(basic_array_dealloc(&array_bad_size));
    expect_assert_failure(basic_array_dealloc(&array_bad_data));

    // Allocate an array to test against
    basic_array array = basic_array_alloc(sizeof(int), dummy_size);
    if (basic_array_isnull(&array)) {
        fail_msg("Failed to allocate an array for testing");
    }

    // Now deallocate the array. It should be null after deallocation.
    basic_array_dealloc(&array);
    assert_true(basic_array_isnull(&array));
}

static void test_array_fromblock(void **state)
{
    (void) state;

    int dummy[dummy_size] = {0};
    basic_block block_good = {dummy, sizeof dummy};
    basic_block block_bad_ptr = {NULL, sizeof dummy};
    basic_block block_bad_size = {dummy, 0};

    // Passing NULL, or a non-null and non-init block, or a zero
    // elem_size, or an elem_size greater than the size of the block
    // should assert
    expect_assert_failure(basic_array_fromblock(NULL, sizeof(int)));
    expect_assert_failure(basic_array_fromblock(&block_bad_ptr, sizeof(int)));
    expect_assert_failure(basic_array_fromblock(&block_bad_size, sizeof(int)));
    expect_assert_failure(basic_array_fromblock(&block_good, 0));
    expect_assert_failure(basic_array_fromblock(&block_good, sizeof dummy + 1));

    // Passing an initialised block should return an initialised array, and
    // passing a null block should return a null array, and the original
    // block should be null, and the elem_size should be correct
    basic_array should_be_null = basic_array_fromblock(&BASIC_BLOCK_NULL, sizeof(int));
    basic_array array = basic_array_fromblock(&block_good, sizeof(int));
    assert_true(basic_array_isinit(&array));
    assert_true(basic_array_isnull(&should_be_null));
    assert_true(basic_block_isnull(&block_good));
    assert_true(array.elem_size == sizeof(int));
}

static void test_array_toblock(void **state)
{
    (void) state;

    int dummy[dummy_size] =  {0};
    basic_array array_good = {{&dummy, sizeof dummy}, sizeof(int)};
    basic_array array_bad_size = {{&dummy, sizeof dummy}, 0};
    basic_array array_bad_data = {BASIC_BLOCK_NULL, sizeof(int)};
    basic_block data = array_good.data;

    // Passing NULL, or a non-null and non-init block, or a zero
    // elem_size, or an elem_size greater than the size of the block
    // should assert
    expect_assert_failure(basic_array_toblock(NULL));
    expect_assert_failure(basic_array_toblock(&array_bad_size));
    expect_assert_failure(basic_array_toblock(&array_bad_data));

    // If the array is null, then the returned block should be null
    basic_block null_result = basic_array_toblock(&BASIC_ARRAY_NULL);
    assert_true(basic_block_isnull(&null_result));

    // If the array is initialised, then the returned block should be
    // initialised, the array should be null, and the block should
    // have the same ptr and size fields as the array's data
    basic_block result = basic_array_toblock(&array_good);
    assert_true(basic_block_isinit(&result));
    assert_true(basic_array_isnull(&array_good));
    assert_true(result.ptr == data.ptr);
    assert_true(result.size == data.size);
}

static void test_array_cap(void **state)
{
    (void) state;

    int dummy[dummy_size] =  {0};
    basic_array array_good = {{&dummy, sizeof dummy}, sizeof(int)};
    basic_array array_bad_size = {{&dummy, sizeof dummy}, 0};
    basic_array array_bad_data = {BASIC_BLOCK_NULL, sizeof(int)};

    // Passing NULL or a non-initialised array to basic_array_cap should
    // assert
    expect_assert_failure(basic_array_cap(NULL));
    expect_assert_failure(basic_array_cap(&array_bad_size));
    expect_assert_failure(basic_array_cap(&array_bad_data));

    // basic_array_cap should correctly return the capacity of the array
    assert_true(basic_array_cap(&array_good) == dummy_size);
}

static void test_array_at(void **state)
{
    (void) state;

    int dummy[dummy_size] =  {0};
    basic_array array_good = {{&dummy, sizeof dummy}, sizeof(int)};
    basic_array array_bad_size = {{&dummy, sizeof dummy}, 0};
    basic_array array_bad_data = {BASIC_BLOCK_NULL, sizeof(int)};
    
    // Passing NULL or a non-initialised array should assert
    expect_assert_failure(basic_array_at(NULL, 0));
    expect_assert_failure(basic_array_at(&BASIC_ARRAY_NULL, 0));
    expect_assert_failure(basic_array_at(&array_bad_size, 0));
    expect_assert_failure(basic_array_at(&array_bad_data, 0));

    // Passing a negative index or index greater than the array capacity
    // should assert
    expect_assert_failure(basic_array_at(&array_good, -1));
    expect_assert_failure(basic_array_at(&array_good, dummy_size));

    // A valid index and initialised array should return a non-null pointer
    // that is equal to the address of the array element
    void *result = NULL;
    for (int i = 0; i < (int)dummy_size; ++i) {
        result = basic_array_at(&array_good, i);
        assert_non_null(result);
        assert_ptr_equal(result, &dummy[i]);
    }
}

static void test_array_at_c(void **state)
{
    (void) state;
    
    int dummy[dummy_size] =  {0};
    basic_array array_good = {{&dummy, sizeof dummy}, sizeof(int)};
    basic_array array_bad_size = {{&dummy, sizeof dummy}, 0};
    basic_array array_bad_data = {BASIC_BLOCK_NULL, sizeof(int)};
    
    // Passing NULL or a non-initialised array should assert
    expect_assert_failure(basic_array_at_c(NULL, 0));
    expect_assert_failure(basic_array_at_c(&BASIC_ARRAY_NULL, 0));
    expect_assert_failure(basic_array_at_c(&array_bad_size, 0));
    expect_assert_failure(basic_array_at_c(&array_bad_data, 0));

    // Passing a negative index or index greater than the array capacity
    // should assert
    expect_assert_failure(basic_array_at_c(&array_good, -1));
    expect_assert_failure(basic_array_at_c(&array_good, dummy_size));

    // A valid index and initialised array should return a non-null pointer
    // that is equal to the address of the array element
    void const *result = NULL;
    for (int i = 0; i < (int)dummy_size; ++i) {
        result = basic_array_at_c(&array_good, i);
        assert_non_null(result);
        assert_ptr_equal(result, &dummy[i]);
    }
}

int main(int argc, char **argv)
{
    (void) argc;
    (void) argv;

    struct CMUnitTest const tests[] = {
        cmocka_unit_test(test_array_isnull),
        cmocka_unit_test(test_array_isinit),
        cmocka_unit_test(test_array_move),
        cmocka_unit_test(test_array_clone),
        cmocka_unit_test(test_array_alloc),
        cmocka_unit_test(test_array_realloc),
        cmocka_unit_test(test_array_dealloc),
        cmocka_unit_test(test_array_fromblock),
        cmocka_unit_test(test_array_toblock),
        cmocka_unit_test(test_array_cap),
        cmocka_unit_test(test_array_at),
        cmocka_unit_test(test_array_at_c),
    };

    return cmocka_run_group_tests(tests, NULL, NULL);
}
