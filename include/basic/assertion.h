#ifndef BASIC_ASSERTION_H_
#define BASIC_ASSERTION_H_

#include <stdlib.h>

#include "basic.h"

BASIC_PRINTF_FMT(5, 6)
void basic_emit_assert(
        char const *const file,
        char const *const func,
        int line,
        char const *const cond,
        char const *const msg,
        ...);

#ifdef BASIC_DEBUG
    #ifndef BASIC_TESTING
        #define BASIC_ASSERT(cond, ...) \
            ((void)(!(cond) \
                ? ((void)basic_emit_assert( \
                        __FILE__, __func__, __LINE__, #cond, __VA_ARGS__), \
                    abort()) \
                : (void)0))
    #else
        extern void mock_assert(
                const int result,
                const char* const expression,
                const char * const file,
                const int line);

        #define BASIC_ASSERT(cond, ...) \
            mock_assert(!!(cond), #cond, __FILE__, __LINE__)
    #endif // BASIC_TESTING
#else
    #define BASIC_ASSERT(cond, ...) // Nothing
#endif // BASIC_DEBUG

#define BASIC_ASSERT_PTR_NONNULL(ptr) \
    BASIC_ASSERT((ptr) != NULL, "pointer must be non-NULL")

#define BASIC_ASSERT_NONZERO(x) \
    BASIC_ASSERT((x) != 0, #x " must be nonzero")

#define BASIC_ASSERT_POSITIVE(x) \
    BASIC_ASSERT((x) > 0, #x " must be positive")

BASIC_PRINTF_FMT(4, 5)
void basic_emit_panic(
        char const *const file,
        char const *const func,
        int line,
        char const *const msg,
        ...);

#define BASIC_PANIC(...) \
    do { \
        basic_emit_panic(__FILE__, __func__, __LINE__, __VA_ARGS__); \
        abort(); \
    } while (0)

#endif // BASIC_ASSERTION_H_
