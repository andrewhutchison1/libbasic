#include "span.h"

#include <string.h>

static size_t min_size(basic_span const *lhs, basic_span const *rhs);

basic_span *basic_span_copy(basic_span *dest, basic_span const *src)
{
    BASIC_ASSERT_PTR_NONNULL(dest);
    BASIC_ASSERT_PTR_NONNULL(src);
    BASIC_ASSERT(basic_span_isinit(dest),
            "dest basic_span must be initialised");
    BASIC_ASSERT(basic_span_isinit(src),
            "src basic_span must be initialised");

    memcpy(dest->ptr, src->ptr, min_size(dest, src));
    return dest;
}

basic_span *basic_span_move(basic_span *dest, basic_span const *src)
{
    BASIC_ASSERT_PTR_NONNULL(dest);
    BASIC_ASSERT_PTR_NONNULL(src);
    BASIC_ASSERT(basic_span_isinit(dest),
            "dest basic_span must be initialised");
    BASIC_ASSERT(basic_span_isinit(src),
            "src basic_span must be initialised");

    memmove(dest->ptr, src->ptr, min_size(dest, src));
    return dest;
}

void basic_span_set(basic_span *span, int c)
{
    BASIC_ASSERT_PTR_NONNULL(span);
    BASIC_ASSERT(basic_span_isinit(span),
            "basic_span object must be initialised");

    memset(span->ptr, c, span->size);
}

bool basic_span_equal(basic_span const *lhs, basic_span const *rhs)
{
    BASIC_ASSERT_PTR_NONNULL(lhs);
    BASIC_ASSERT_PTR_NONNULL(rhs);
    BASIC_ASSERT(basic_span_isnull(lhs) || basic_span_isinit(lhs),
            "lhs basic_span must be null or initialised");
    BASIC_ASSERT(basic_span_isnull(rhs) || basic_span_isinit(rhs),
            "rhs basic_span must be null or initialised");

    if (lhs == rhs) {
        return true;
    } else if (basic_span_isnull(lhs) && basic_span_isnull(rhs)) {
        return true;
    } else if (basic_span_isnull(lhs) && !basic_span_isnull(rhs)) {
        return false;
    } else if (!basic_span_isnull(lhs) && basic_span_isnull(rhs)) {
        return false;
    } else if (lhs->size != rhs->size) {
        return false;
    }

    return memcmp(lhs->ptr, rhs->ptr, lhs->size) == 0;
}

size_t min_size(basic_span const *lhs, basic_span const *rhs)
{
    if (lhs->size <= rhs->size) {
        return lhs->size;
    } else {
        return rhs->size;
    }
}
