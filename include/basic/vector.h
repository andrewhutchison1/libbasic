/**
 * @file vector.h
 */

#ifndef BASIC_VECTOR_H_
#define BASIC_VECTOR_H_

#include <stdbool.h>

#include "assertion.h"
#include "array.h"
#include "span.h"

typedef struct {
    basic_array data;
    int elem_count;
    int elem_cap;
} basic_vector;

#define BASIC_VECTOR_NULL ((basic_vector){BASIC_ARRAY_NULL, 0, 0})

static inline bool basic_vector_isnull(basic_vector const *vector);
static inline bool basic_vector_isinit(basic_vector const *vector);
static inline bool basic_vector_isempty(basic_vector const *vector);

basic_vector basic_vector_move(basic_vector *vector);
basic_vector basic_vector_clone(basic_vector const *vector);

basic_vector basic_vector_new(size_t elem_size, int initial_cap);
void basic_vector_destroy(basic_vector *vector);

bool basic_vector_insert(basic_vector *vector, int index, void *elem);
void basic_vector_remove(basic_vector *vector, int index);

static inline bool basic_vector_insertfront(basic_vector *vector, void *elem);
static inline bool basic_vector_insertback(basic_vector *vector, void *elem);
static inline void basic_vector_removefront(basic_vector *vector);
static inline void basic_vector_removeback(basic_vector *vector);

void *basic_vector_at(basic_vector *vector, int index);
void const *basic_vector_at_c(basic_vector const *vector, int index);

basic_span basic_vector_get(basic_vector *vector, int index);

static inline void *basic_vector_front(basic_vector *vector);
static inline void const *basic_vector_front_c(basic_vector const *vector);
static inline void *basic_vector_back(basic_vector *vector);
static inline void const *basic_vector_back_c(basic_vector const *vector);

bool basic_vector_isnull(basic_vector const *vector)
{
    BASIC_ASSERT_PTR_NONNULL(vector);
    return basic_array_isnull(&vector->data)
        && !vector->elem_count
        && !vector->elem_cap;
}

bool basic_vector_isinit(basic_vector const *vector)
{
    BASIC_ASSERT_PTR_NONNULL(vector);
    return basic_array_isinit(&vector->data)
        && vector->elem_count >= 0
        && vector->elem_cap > 0
        && (vector->elem_cap >= vector->elem_count);
}

bool basic_vector_isempty(basic_vector const *vector)
{
    BASIC_ASSERT_PTR_NONNULL(vector);
    return basic_vector_isinit(vector) && !vector->elem_count;
}

bool basic_vector_insertfront(basic_vector *vector, void *elem)
{
    return basic_vector_insert(vector, 0, elem);
}

bool basic_vector_insertback(basic_vector *vector, void *elem)
{
    BASIC_ASSERT_PTR_NONNULL(vector);
    return basic_vector_insert(vector, vector->elem_count, elem);
}

void basic_vector_removefront(basic_vector *vector)
{
    basic_vector_remove(vector, 0);
}

void basic_vector_removeback(basic_vector *vector)
{
    BASIC_ASSERT_PTR_NONNULL(vector);
    basic_vector_remove(vector, vector->elem_count - 1);
}

void *basic_vector_front(basic_vector *vector)
{
    return basic_vector_at(vector, 0);
}

void const *basic_vector_front_c(basic_vector const *vector)
{
    return basic_vector_at_c(vector, 0);
}

void *basic_vector_back(basic_vector *vector)
{
    BASIC_ASSERT_PTR_NONNULL(vector);
    return basic_vector_at(vector, vector->elem_count - 1);
}

void const *basic_vector_back_c(basic_vector const *vector)
{
    BASIC_ASSERT_PTR_NONNULL(vector);
    return basic_vector_at_c(vector, vector->elem_count - 1);
}

#endif // BASIC_VECTOR_H_
