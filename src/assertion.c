#include "assertion.h"

#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>

void basic_emit_assert(
        char const *const file,
        char const *const func,
        int line,
        char const *const cond,
        char const *const msg,
        ...)
{
    va_list args;
    va_start(args, msg);

    fprintf(stderr, "%s:%d Assertion \"%s\" failed in %s: ",
            file, line, cond, func);
    vfprintf(stderr, msg, args);
    fputs("\n", stderr);

    va_end(args);
}

void basic_emit_panic(
        char const *const file,
        char const *const func,
        int line,
        char const *const msg,
        ...)
{
    va_list args;
    va_start(args, msg);

    fprintf(stderr, "%s:%d Panic in %s: ", file, line, func);
    vfprintf(stderr, msg, args);
    fputs("\n", stderr);
    va_end(args);
}
