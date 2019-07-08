#include <stdarg.h>
#include <stddef.h>
#include <setjmp.h>
#include <cmocka.h>

#include "array.h"

static void test_array_isnull(void **state)
{
    (void) state;

    // Passing NULL to basic_array_isnull should assert
    expect_assert_failure(basic_array_isnull(NULL));
}

static void test_array_isinit(void **state)
{
    (void) state;

    // Passing NULL to basic_array_isnull should assert
    expect_assert_failure(basic_array_isnull(NULL));
}

static void test_array_move(void **state)
{
    (void) state;

    // TODO
    fail_msg("Unimplemented");
}

static void test_array_clone(void **state)
{
    (void) state;
    
    // TODO
    fail_msg("Unimplemented");
}

static void test_array_alloc(void **state)
{
    (void) state;
    
    // TODO
    fail_msg("Unimplemented");
}

static void test_array_realloc(void **state)
{
    (void) state;
    
    // TODO
    fail_msg("Unimplemented");
}

static void test_array_dealloc(void **state)
{
    (void) state;
    
    // TODO
    fail_msg("Unimplemented");
}

static void test_array_fromblock(void **state)
{
    (void) state;
    
    // TODO
    fail_msg("Unimplemented");
}

static void test_array_toblock(void **state)
{
    (void) state;
    
    // TODO
    fail_msg("Unimplemented");
}

static void test_array_cap(void **state)
{
    (void) state;
    
    // TODO
    fail_msg("Unimplemented");
}

static void test_array_at(void **state)
{
    (void) state;
    
    // TODO
    fail_msg("Unimplemented");
}

static void test_array_at_c(void **state)
{
    (void) state;
    
    // TODO
    fail_msg("Unimplemented");
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
