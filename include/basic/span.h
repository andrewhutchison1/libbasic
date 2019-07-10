/**
 * @file span.h
 */

#ifndef BASIC_SPAN_H_
#define BASIC_SPAN_H_

#include <stdbool.h>

#include "assertion.h"

/**
 * @struct basic_span
 * @brief Represents a non-owning view into a memory area.
 *
 * A basic_span is superficially similar to a basic_block.
 * The major difference is semantic; a basic_block always represents (owns)
 * a freestore allocated memory area, while a basic_span may represent
 * any memory area.
 * As such, the lifetime of a basic_span object is not explicitly encoded in the
 * functions that operate upon it, but care must still be taken to e.g. not
 * return a basic_span object that represents an automatic memory area from
 * a function.
 *
 * @var basic_span::ptr
 * @brief A pointer to the beginning of the memory area.
 *
 * @var basic_span::size
 * @brief The size, in bytes, of the memory area.
 */
typedef struct {
    void *ptr;
    size_t size;
} basic_span;

/**
 * @brief The value of a basic_span object in the null state
 */
#define BASIC_SPAN_NULL ((basic_span){NULL, 0})

/**
 * @brief Returns true if the basic_span pointed to by @c span is in
 *  the null state.
 *
 * @param[in] span A pointer to the basic_span to query.
 *
 * @pre @c span is non-NULL
 *
 * @retval true If the basic_span pointed to by @c span is in the null
 *  state.
 * @retval false If the basic_span pointed to by @c span is not in the
 *  null state.
 */
static inline bool basic_span_isnull(basic_span const *span);

/**
 * @brief Returns true if the basic_span pointed to by @c span is in
 *  the init state.
 *
 * @param[in] span A pointer to the basic_span to query.
 *
 * @pre @c span is non-NULL
 *
 * @retval true If the basic_span pointed to by @c span is in the init 
 *  state.
 * @retval false If the basic_span pointed to by @c span is not in the
 *  init state.
 */
static inline bool basic_span_isinit(basic_span const *span);

/**
 * @brief Copies the data from the memory area represented by the basic_span
 *  pointed to by @c src to the memory area represented by the basic_span
 *  pointed to by @c dest, where the memory areas are assumed not to overlap.
 *
 * If the size fields of the basic_span objects pointed to by @c dest and
 * @c src are not equal, then the minimum of both sizes is copied.
 * The memory areas of the basic_span objects pointed to by @c dest and
 * @c src must not overlap. If it is possible that these memory areas overlap,
 * then @ref basic_span_move should be used instead.
 *
 * @param[in] dest Pointer to the basic_span whose memory area will be copied
 *  to.
 * @param[in] src Pointer to the basic_span whose memory area will be copied
 *  from.
 *
 * @pre dest is non-NULL and points to an initialised basic_span
 * @pre src is non-NULL and points to an initialised basic_span
 * @pre The memory areas represented by the basic_span objects pointed to
 *  by @c dest and @c src do not overlap
 *
 * @returns @c dest
 */
basic_span *basic_span_copy(basic_span *dest, basic_span const *src);

/**
 * @brief Copies the data from the memory area represented by the basic_span
 *  pointed to by @c src to the memory area represented by the basic_span
 *  pointed to by @c dest, where the memory areas may overlap.
 *
 * If the size fields of the basic_span objects pointed to by @c dest and
 * @c src are not equal, then the minimum of both sizes is copied.
 * If the memory areas represented by the basic_span objects pointed to by
 * @c dest and @c src are known to not overlap, then @ref basic_span_copy
 * might result in some performance gain.
 *
 * @param[in] dest Pointer to the basic_span whose memory area will be copied
 *  to.
 * @param[in] src Pointer to the basic_span whose memory area will be copied
 *  from.
 *
 * @pre dest is non-NULL and points to an initialised basic_span
 * @pre src is non-NULL and points to an initialised basic_span
 *
 * @returns @c dest
 */
basic_span *basic_span_move(basic_span *dest, basic_span const *src);

/**
 * @brief Fills the memory area represented by the basic_span pointed to by
 *  @c span with the constant byte @c c.
 *
 * @param[in] span Pointer to the basic_span whose memory area will be set.
 * @param[in] c The value to set each byte of the memory area to.
 *
 * @pre span is non-NULL and points to an initialised basic_span
 */
void basic_span_set(basic_span *span, int c);

/**
 * @brief Zeros the memory area represented by the basic_span pointed to by
 *  @c span.
 *
 * This function is equivalent to @ref basic_span_set with a value of 0.
 *
 * @param[in] span Pointer to the basic_span whose memory area will be zeroed.
 *
 * @pre span is non-NULL and points to an initialised basic_span
 */
static inline void basic_span_zero(basic_span *span);

/**
 * @brief Returns true if the basic_span objects pointed to by @c lhs and
 *  @c rhs are equal.
 *
 * The basic_span objects pointed to by @c lhs and @c rhs are equal if
 * they are both in the null state, or if they have the same @c size field
 * and their memory areas compare bytewise equal.
 *
 * @param[in] lhs Pointer to the first basic_span object to compare
 * @param[in] rhs Pointer to the second basic_span object to compare
 *
 * @pre lhs is non-NULL and points to a basic_span object in the null or
 *  init state
 * @pre rhs is non-NULL and points to a basic_span object in the null or
 *  init state
 *
 * @retval true If the basic_span objects pointed to by @c lhs and
 *  @c rhs are equal
 * @retval false If the basic_span objects pointed to by @c lhs and
 *  @c rhs are not equal
 */
bool basic_span_equal(basic_span const *lhs, basic_span const *rhs);

bool basic_span_isnull(basic_span const *span)
{
    BASIC_ASSERT_PTR_NONNULL(span);
    return !span->ptr && !span->size;
}

bool basic_span_isinit(basic_span const *span)
{
    BASIC_ASSERT_PTR_NONNULL(span);
    return span->ptr && span->size;
}

void basic_span_zero(basic_span *span)
{
    BASIC_ASSERT_PTR_NONNULL(span);
    BASIC_ASSERT(basic_span_isinit(span),
            "basic_span object must be initialised");

    basic_span_set(span, 0);
}

#endif // BASIC_SPAN_H_
