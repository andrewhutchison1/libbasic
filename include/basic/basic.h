#ifndef BASIC_BASIC_H_
#define BASIC_BASIC_H_

#if defined(__GNUC__) || defined(__clang__)
    #define BASIC_PRINTF_FMT(fmt, arg) \
        __attribute__((__format__(__printf__, (fmt), (arg))))

    #define BASIC_VPRINTF_FMT(fmt) \
        __attribute__((__format__(__printf__, (fmt), 0)))
#else
    #define BASIC_PRINTF_FMT(fmt, arg)  // Nothing
    #define BASIC_VPRINTF_FMT(fmt)      // Nothing
#endif

#endif // BASIC_BASIC_H_
