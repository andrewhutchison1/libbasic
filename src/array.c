#include "array.h"

static int valid_index(basic_array const *array, int index);

basic_array basic_array_move(basic_array *array)
{
    BASIC_ASSERT_PTR_NONNULL(array);
    BASIC_ASSERT(basic_array_isinit(array),
            "cannot move from uninitialised basic_array object");

    basic_array temp = *array;
    *array = BASIC_ARRAY_NULL;
    return temp;
}

basic_array basic_array_clone(basic_array const *array)
{
    BASIC_ASSERT_PTR_NONNULL(array);
    BASIC_ASSERT(basic_array_isnull(array) || basic_array_isinit(array),
            "basic_array object must be null or initialised");

    if (basic_array_isnull(array)) {
        return BASIC_ARRAY_NULL;
    }

    basic_block data = basic_block_clone(&array->data);
    if (basic_block_isnull(&data)) {
        return BASIC_ARRAY_NULL;
    }

    return (basic_array) {
        .data = data,
        .elem_size = array->elem_size
    };
}

basic_array basic_array_alloc(size_t elem_size, int elem_count)
{
    BASIC_ASSERT_NONZERO(elem_size);
    BASIC_ASSERT_POSITIVE(elem_count);

    basic_block data = basic_block_alloc(elem_size * elem_count);
    if (basic_block_isnull(&data)) {
        return BASIC_ARRAY_NULL;
    }

    return (basic_array) {
        .data = basic_block_move(&data),
        .elem_size = elem_size
    };
}

basic_array *basic_array_realloc(basic_array *array, int elem_count)
{
    BASIC_ASSERT_PTR_NONNULL(array);
    BASIC_ASSERT_POSITIVE(elem_count);
    BASIC_ASSERT(basic_array_isinit(array),
            "basic_array object must be initialised");

    size_t const data_size = array->elem_size * elem_count;
    basic_block *data = basic_block_realloc(&array->data, data_size);
    return data ? array : NULL;
}

void basic_array_dealloc(basic_array *array)
{
    BASIC_ASSERT_PTR_NONNULL(array);
    BASIC_ASSERT(basic_array_isnull(array) || basic_array_isinit(array),
            "basic_array object must be null or initialised");

    if (basic_array_isinit(array)) {
        basic_block_dealloc(&array->data);
        *array = BASIC_ARRAY_NULL;
    }
}

basic_array basic_array_fromblock(basic_block *block, size_t elem_size)
{
    BASIC_ASSERT_PTR_NONNULL(block);
    BASIC_ASSERT_NONZERO(elem_size);
    BASIC_ASSERT(basic_block_isnull(block) || basic_block_isinit(block),
            "basic_block object must be null or initialised");
    BASIC_ASSERT(basic_block_isnull(block) || elem_size <= block->size,
            "elem_size (%zu) must be less than or equal to block->size (%zu)",
            elem_size, block->size);

    if (basic_block_isnull(block)) {
        return BASIC_ARRAY_NULL;
    }

    return (basic_array) {
        .data = basic_block_move(block),
        .elem_size = elem_size
    };
}

basic_block basic_array_toblock(basic_array *array)
{
    BASIC_ASSERT_PTR_NONNULL(array);
    BASIC_ASSERT(basic_array_isnull(array) || basic_array_isinit(array),
            "basic_array object must be null or initialised");

    if (basic_array_isnull(array)) {
        return BASIC_BLOCK_NULL;
    }

    basic_block data = basic_block_move(&array->data);
    *array = BASIC_ARRAY_NULL;
    return data;
}

void *basic_array_at(basic_array *array, int index)
{
    BASIC_ASSERT_PTR_NONNULL(array);
    BASIC_ASSERT(valid_index(array, index), "index %d is invalid");
    return (void *)((char *)array->data.ptr
            + (size_t)index * array->elem_size);
}

void const *basic_array_at_c(basic_array const *array, int index)
{
    BASIC_ASSERT_PTR_NONNULL(array);
    BASIC_ASSERT(valid_index(array, index), "index %d is invalid");
    return (void const *)((char const *)array->data.ptr
            + (size_t)index * array->elem_size);
}

int valid_index(basic_array const *array, int index)
{
    return (index > 0) && ((size_t)index < basic_array_cap(array));
}
