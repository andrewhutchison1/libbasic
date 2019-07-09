#include "string_vector.h"

#include <string.h>
#include <stdio.h> // FIXME

enum {
    grow_factor = 2
};

static bool string_vector_grow(
        basic_string_vector *string_vector,
        int n_chunks);

static int chunks_required_for(
        basic_string_vector const *string_vector,
        char const *string);

static bool chunk_is_terminal(
        basic_string_vector const *string_vector,
        int chunk_index);

static int string_index_to_chunk_index(
        basic_string_vector const *string_vector,
        int string_index);

static void shift_chunks_left(
        basic_string_vector *string_vector,
        int chunk_index,
        int shift_by);

static void shift_chunks_right(
        basic_string_vector *string_vector,
        int chunk_index,
        int shift_by);

basic_string_vector basic_string_vector_move(
        basic_string_vector *string_vector)
{
    BASIC_ASSERT_PTR_NONNULL(string_vector);
    BASIC_ASSERT(basic_string_vector_isinit(string_vector),
            "basic_string_vector object must be initialised");

    basic_string_vector temp = *string_vector;
    *string_vector = BASIC_STRING_VECTOR_NULL;
    return temp;
}

basic_string_vector basic_string_vector_clone(
        basic_string_vector const *string_vector)
{
    BASIC_ASSERT_PTR_NONNULL(string_vector);
    BASIC_ASSERT(basic_string_vector_isnull(string_vector)
            || basic_string_vector_isinit(string_vector),
            "basic_string_vector object must be null or initialised");

    if (basic_string_vector_isnull(string_vector)) {
        return BASIC_STRING_VECTOR_NULL;
    }

    basic_array chunk_data = basic_array_clone(&string_vector->chunk_data);
    if (basic_array_isnull(&chunk_data)) {
        return BASIC_STRING_VECTOR_NULL;
    }

    return (basic_string_vector) {
        .chunk_data = basic_array_move(&chunk_data),
        .chunk_count = string_vector->chunk_count,
        .string_count = string_vector->string_count
    };
}

basic_string_vector basic_string_vector_new(size_t chunk_size, int chunk_cap)
{
    BASIC_ASSERT_NONZERO(chunk_size);

    basic_array chunk_data = basic_array_alloc(chunk_size, chunk_cap);
    if (basic_array_isnull(&chunk_data)) {
        return BASIC_STRING_VECTOR_NULL;
    }

    return (basic_string_vector) {
        .chunk_data = basic_array_move(&chunk_data),
        .chunk_count = 0,
        .string_count = 0
    };
}

void basic_string_vector_destroy(basic_string_vector *string_vector)
{
    BASIC_ASSERT_PTR_NONNULL(string_vector);
    BASIC_ASSERT(basic_string_vector_isnull(string_vector)
            || basic_string_vector_isinit(string_vector),
            "basic_string_vector object must be null or initialised");
    
    if (basic_string_vector_isinit(string_vector)) {
        basic_array_dealloc(&string_vector->chunk_data);
        *string_vector = BASIC_STRING_VECTOR_NULL;
    }
}

bool basic_string_vector_insert(
        basic_string_vector *string_vector,
        int index,
        char const *string)
{
    BASIC_ASSERT_PTR_NONNULL(string_vector);
    BASIC_ASSERT(basic_string_vector_isinit(string_vector),
            "basic_string_vector object must be initialised");
    BASIC_ASSERT(index >= 0 && index <= string_vector->string_count,
            "index %d out of range", index);
    BASIC_ASSERT_PTR_NONNULL(string);

    // Determine how many blocks we need and how many we have available
    int const string_chunks = chunks_required_for(string_vector, string);
    int const chunks_required = string_chunks + string_vector->chunk_count;

    // Grow the string_vector if needed
    if (basic_array_cap(&string_vector->chunk_data) < chunks_required) {
        if (!string_vector_grow(string_vector, chunks_required)) {
            return false;
        }
    }

    BASIC_ASSERT(basic_array_cap(&string_vector->chunk_data)
            >= chunks_required,
            "Insufficient chunks (%d), need %d",
            basic_array_cap(&string_vector->chunk_data),
            chunks_required);

    if (index == string_vector->string_count) {
        // At this point we definitely have enough space to write the string.
        // If the index is at the end of the string_vector (ie. index ==
        // string_vector->string_count) then we do not need to shift blocks.
        strcpy(basic_array_at(&string_vector->chunk_data,
                    string_vector->chunk_count),
                string);
    } else {
        // Otherwise, we need to shift some chunks rightward `string_chunks` units
        int const chunk_index = string_index_to_chunk_index(string_vector, index);
        shift_chunks_right(string_vector, chunk_index, string_chunks);

        // At which point we can write the string in
        strcpy(basic_array_at(&string_vector->chunk_data, chunk_index),
                string);
    }

    ++string_vector->string_count;
    string_vector->chunk_count += string_chunks;
    return true;
}

void basic_string_vector_remove(
        basic_string_vector *string_vector,
        int index)
{
    BASIC_ASSERT_PTR_NONNULL(string_vector);
    BASIC_ASSERT(basic_string_vector_isinit(string_vector) &&
            !basic_string_vector_isempty(string_vector),
            "basic_string_vector object must be initialised and "
            "non-empty");
    BASIC_ASSERT(index >= 0 && index < string_vector->string_count,
            "index %d out of range", index);

    int const chunk_index = string_index_to_chunk_index(
            string_vector,
            index + 1);
    int const chunks_occupied = chunks_required_for(
            string_vector,
            basic_string_vector_at(string_vector, index));

    shift_chunks_left(string_vector, chunk_index, chunks_occupied);
    --string_vector->string_count;
    string_vector->chunk_count -= chunks_occupied;
}

char const *basic_string_vector_at(
        basic_string_vector const *string_vector,
        int index)
{
    BASIC_ASSERT_PTR_NONNULL(string_vector);
    BASIC_ASSERT(index >= 0 && index < string_vector->string_count,
            "index %d out of range", index);

    return basic_array_at_c(&string_vector->chunk_data,
            string_index_to_chunk_index(string_vector, index));
}

bool string_vector_grow(
        basic_string_vector *string_vector,
        int n_chunks)
{
    int chunks_total = basic_array_cap(&string_vector->chunk_data);

    while (chunks_total < n_chunks) {
        chunks_total *= grow_factor;
    }

    return basic_array_realloc(&string_vector->chunk_data, chunks_total);
}

int chunks_required_for(
        basic_string_vector const *string_vector,
        char const *string)
{
    size_t const size = strlen(string) + 1;
    return (size / string_vector->chunk_data.elem_size)
        + (size % string_vector->chunk_data.elem_size != 0);
}

bool chunk_is_terminal(
        basic_string_vector const *string_vector,
        int chunk_index)
{
    char const *const chunk_begin = basic_array_at_c(&string_vector->chunk_data,
            chunk_index);
    return !chunk_begin[string_vector->chunk_data.elem_size - 1];
}

int string_index_to_chunk_index(
        basic_string_vector const *string_vector,
        int string_index)
{
    // If the string index is zero, then so is the chunk index
    if (string_index == 0) {
        return 0;
    }

    // If the string_vector has equal number of chunks and strings,
    // then the string->chunk index transform is a bijection
    if (string_vector->string_count == string_vector->chunk_count) {
        return string_index;
    }

    // Otherwise, the index transform requires a linear search
    // (linear in the number of chunks)
    int chunk_index = 0;
    for (; chunk_index < string_vector->chunk_count; ++chunk_index) {
        if (!string_index) {
            break;
        }

        if (chunk_is_terminal(string_vector, chunk_index)) {
            --string_index;
        }
    }

    return chunk_index;
}

void shift_chunks_left(
        basic_string_vector *string_vector,
        int chunk_index,
        int shift_by)
{
    void *const dest = basic_array_at(&string_vector->chunk_data,
            chunk_index - shift_by);
    void const *const src = basic_array_at(&string_vector->chunk_data,
            chunk_index);
    size_t const n = (basic_array_cap(&string_vector->chunk_data) - chunk_index)
            * string_vector->chunk_data.elem_size;

    memmove(dest, src, n);
    memset(basic_array_at(&string_vector->chunk_data,
                basic_array_cap(&string_vector->chunk_data) - shift_by),
            0,
            shift_by * string_vector->chunk_data.elem_size);
}

void shift_chunks_right(
        basic_string_vector *string_vector,
        int chunk_index,
        int shift_by)
{
    void *const dest = basic_array_at(&string_vector->chunk_data,
            chunk_index + shift_by);
    void const *const src = basic_array_at(&string_vector->chunk_data,
            chunk_index);
    size_t const n = basic_array_cap(&string_vector->chunk_data) - chunk_index;
    memmove(dest, src, n);
}
