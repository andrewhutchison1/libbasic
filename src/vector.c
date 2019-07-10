#include "vector.h"

#include <string.h>

enum {
    vector_grow_factor = 2
};

static bool vector_isfull(basic_vector const *vector);
static basic_vector *vector_grow(basic_vector *vector);

static void vector_shift_elem_right(
        basic_vector *vector,
        int index,
        int shift_by);

static void vector_shift_elem_left(
        basic_vector *vector,
        int index,
        int shift_by);

static void vector_write_elem(
        basic_vector *vector,
        int index,
        void *elem);

basic_vector basic_vector_move(basic_vector *vector)
{
    BASIC_ASSERT_PTR_NONNULL(vector);
    BASIC_ASSERT(basic_vector_isinit(vector),
            "basic_vector object must be initialised");

    basic_vector temp = *vector;
    *vector = BASIC_VECTOR_NULL;
    return temp;
}

basic_vector basic_vector_clone(basic_vector const *vector)
{
    BASIC_ASSERT_PTR_NONNULL(vector);
    BASIC_ASSERT(basic_vector_isnull(vector) || basic_vector_isinit(vector),
            "basic_vector object must be null or initialised");

    if (basic_vector_isnull(vector)) {
        return BASIC_VECTOR_NULL;
    }

    basic_array data = basic_array_clone(&vector->data);
    if (basic_array_isnull(&data)) {
        return BASIC_VECTOR_NULL;
    }

    return (basic_vector) {
        .data = basic_array_move(&data),
        .elem_count = vector->elem_count,
        .elem_cap = vector->elem_cap
    };
}

basic_vector basic_vector_new(size_t elem_size, int initial_cap)
{
    BASIC_ASSERT_NONZERO(elem_size);
    BASIC_ASSERT_POSITIVE(initial_cap);

    basic_array data = basic_array_alloc(elem_size, (size_t)initial_cap);
    if (basic_array_isnull(&data)) {
        return BASIC_VECTOR_NULL;
    }

    return (basic_vector) {
        .data = basic_array_move(&data),
        .elem_count = 0,
        .elem_cap = initial_cap
    };
}

void basic_vector_destroy(basic_vector *vector)
{
    BASIC_ASSERT_PTR_NONNULL(vector);

    if (basic_vector_isinit(vector)) {
        basic_array_dealloc(&vector->data);
        *vector = BASIC_VECTOR_NULL;
    }
}

bool basic_vector_insert(basic_vector *vector, int index, void *elem)
{
    BASIC_ASSERT_PTR_NONNULL(vector);
    BASIC_ASSERT_PTR_NONNULL(elem);
    BASIC_ASSERT(basic_vector_isinit(vector),
            "basic_vector object must be initialised");
    BASIC_ASSERT_POSITIVE(index);
    BASIC_ASSERT(index <= vector->elem_count,
            "insert index %d out of range",
            index);

    // Grow the vector if necessary
    if (vector_isfull(vector) && !vector_grow(vector)) {
        return false;
    }

    // If the index to insert at is not at the end of the vector,
    // then we need to shift all elements after the given index
    // right by 1 unit
    if (index != vector->elem_count) {
        vector_shift_elem_right(vector, index, 1);
    }

    // Now we can write the element into the vector at the given
    // index, and bump the elem_count
    vector_write_elem(vector, index, elem);
    ++vector->elem_count;
    return true;
}

void basic_vector_remove(basic_vector *vector, int index)
{
    BASIC_ASSERT_PTR_NONNULL(vector);
    BASIC_ASSERT(basic_vector_isinit(vector),
            "basic_vector object must be initialised");
    BASIC_ASSERT_POSITIVE(index);
    BASIC_ASSERT(index < vector->elem_count,
            "index %d out of range",
            index);

    if (index != vector->elem_count - 1) {
        vector_shift_elem_left(vector, index + 1, 1);
    }

    --vector->elem_count;
}

void *basic_vector_at(basic_vector *vector, int index)
{
    BASIC_ASSERT_PTR_NONNULL(vector);
    BASIC_ASSERT(basic_vector_isinit(vector),
            "basic_vector object must be initialised");
    BASIC_ASSERT_POSITIVE(index);
    BASIC_ASSERT(index < vector->elem_count,
            "index %d out of range",
            index);

    return basic_array_at(&vector->data, index);
}

void const *basic_vector_at_c(basic_vector const *vector, int index)
{
    BASIC_ASSERT_PTR_NONNULL(vector);
    BASIC_ASSERT(basic_vector_isinit(vector),
            "basic_vector object must be initialised");
    BASIC_ASSERT_POSITIVE(index);
    BASIC_ASSERT(index < vector->elem_count,
            "index %d out of range",
            index);

    return basic_array_at_c(&vector->data, index);
}

basic_span basic_vector_get(basic_vector *vector, int index)
{
    BASIC_ASSERT_PTR_NONNULL(vector);
    BASIC_ASSERT(basic_vector_isinit(vector),
            "basic_vector object must be initialised");
    BASIC_ASSERT_POSITIVE(index);
    BASIC_ASSERT(index < vector->elem_count,
            "index %d out of range",
            index);

    return basic_array_get(&vector->data, index);
}

bool vector_isfull(basic_vector const *vector)
{
    return vector->elem_count == vector->elem_cap;
}

basic_vector *vector_grow(basic_vector *vector)
{
    int const new_elem_cap = vector->elem_cap * vector_grow_factor;
    if (!basic_array_realloc(&vector->data, new_elem_cap)) {
        return NULL;
    }

    vector->elem_cap = new_elem_cap;
    return vector;
}

void vector_shift_elem_right(
        basic_vector *vector,
        int index,
        int shift_by)
{
    void *const dest = basic_array_at(&vector->data, index + shift_by);
    void const *const src = basic_array_at_c(&vector->data, index);
    size_t const n = (vector->elem_count - index) * vector->data.elem_size;
    memmove(dest, src, n);
}

void vector_shift_elem_left(
        basic_vector *vector,
        int index,
        int shift_by)
{
    void *const dest = basic_array_at(&vector->data, index - shift_by);
    void const *const src = basic_array_at(&vector->data, index);
    size_t const n = (vector->elem_count - index) * vector->data.elem_size;
    memmove(dest, src, n);
}

void vector_write_elem(
        basic_vector *vector,
        int index,
        void *elem)
{
    void *const write_ptr = basic_array_at(&vector->data, index);
    memcpy(write_ptr, elem, vector->data.elem_size);
}

