#ifndef BASIC_STRING_VECTOR_H_
#define BASIC_STRING_VECTOR_H_

#include <stdbool.h>

#include "assertion.h"
#include "array.h"

typedef struct {
    basic_array chunk_data;
    int chunk_count;
    int string_count;
} basic_string_vector;

#define BASIC_STRING_VECTOR_NULL \
    ((basic_string_vector){BASIC_ARRAY_NULL, 0, 0})

static inline bool basic_string_vector_isnull(
        basic_string_vector const *string_vector);

static inline bool basic_string_vector_isinit(
        basic_string_vector const *string_vector);

static inline bool basic_string_vector_isempty(
        basic_string_vector const *string_vector);

basic_string_vector basic_string_vector_move(
        basic_string_vector *string_vector);

basic_string_vector basic_string_vector_clone(
        basic_string_vector const *string_vector);

basic_string_vector basic_string_vector_new(size_t chunk_size, int chunk_cap);
void basic_string_vector_destroy(basic_string_vector *string_vector);

bool basic_string_vector_insert(
        basic_string_vector *string_vector,
        int index,
        char const *string);

void basic_string_vector_remove(
        basic_string_vector *string_vector,
        int index);

static inline bool basic_string_vector_insertback(
        basic_string_vector *string_vector,
        char const *string);

static inline bool basic_string_vector_insertfront(
        basic_string_vector *string_vector,
        char const *string);

static inline void basic_string_vector_removefront(
        basic_string_vector *string_vector);

static inline void basic_string_vector_removeback(
        basic_string_vector *string_vector);

char const *basic_string_vector_at(
        basic_string_vector const *string_vector,
        int index);

static inline char const *basic_string_vector_front(
        basic_string_vector const *string_vector);

static inline char const *basic_string_vector_back(
        basic_string_vector const *string_vector);

bool basic_string_vector_isnull(basic_string_vector const *string_vector)
{
    BASIC_ASSERT_PTR_NONNULL(string_vector);
    return basic_array_isnull(&string_vector->chunk_data)
        && !string_vector->chunk_count
        && !string_vector->string_count;
}

bool basic_string_vector_isinit(basic_string_vector const *string_vector)
{
    BASIC_ASSERT_PTR_NONNULL(string_vector);
    return basic_array_isinit(&string_vector->chunk_data)
        && string_vector->chunk_count >= 0
        && string_vector->string_count >= 0
        && string_vector->string_count <= string_vector->chunk_count;
}

bool basic_string_vector_isempty(basic_string_vector const *string_vector)
{
    BASIC_ASSERT_PTR_NONNULL(string_vector);
    return basic_string_vector_isinit(string_vector)
        && string_vector->string_count == 0;
}

bool basic_string_vector_insertback(
        basic_string_vector *string_vector,
        char const *string)
{
    BASIC_ASSERT_PTR_NONNULL(string_vector);
    return basic_string_vector_insert(
            string_vector,
            string_vector->string_count,
            string);
}

bool basic_string_vector_insertfront(
        basic_string_vector *string_vector,
        char const *string)
{
    return basic_string_vector_insert(string_vector, 0, string);
}

void basic_string_vector_removefront(basic_string_vector *string_vector)
{
    basic_string_vector_remove(string_vector, 0);
}

void basic_string_vector_removeback(basic_string_vector *string_vector)
{
    BASIC_ASSERT_PTR_NONNULL(string_vector);
    basic_string_vector_remove(string_vector, string_vector->string_count);
}

char const *basic_string_vector_front(
        basic_string_vector const *string_vector)
{
    return basic_string_vector_at(string_vector, 0);
}

char const *basic_string_vector_back(basic_string_vector const *string_vector)
{
    BASIC_ASSERT_PTR_NONNULL(string_vector);
    return basic_string_vector_at(string_vector, string_vector->string_count);
}

#endif // BASIC_STRING_VECTOR_H_
