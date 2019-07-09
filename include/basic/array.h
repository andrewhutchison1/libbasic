#ifndef BASIC_ARRAY_H_
#define BASIC_ARRAY_H_

#include <stdbool.h>
#include <stddef.h>

#include "assertion.h"
#include "block.h"

typedef struct {
    basic_block data;
    size_t elem_size;
} basic_array;

#define BASIC_ARRAY_NULL ((basic_array){BASIC_BLOCK_NULL, 0})

static inline bool basic_array_isnull(basic_array const *array);
static inline bool basic_array_isinit(basic_array const *array);

basic_array basic_array_move(basic_array *array);
basic_array basic_array_clone(basic_array const *array);

basic_array basic_array_alloc(size_t elem_size, int elem_count);
basic_array *basic_array_realloc(basic_array *array, int elem_count);
void basic_array_dealloc(basic_array *array);

basic_array basic_array_fromblock(basic_block *block, size_t elem_size);
basic_block basic_array_toblock(basic_array *array);

static inline int basic_array_cap(basic_array const *array);

void *basic_array_at(basic_array *array, int index);
void const *basic_array_at_c(basic_array const *array, int index);

int basic_array_cap(basic_array const *array)
{
    BASIC_ASSERT_PTR_NONNULL(array);
    BASIC_ASSERT(basic_array_isinit(array),
            "basic_array object must be initialised");

    return (int)(array->data.size / array->elem_size);
}

bool basic_array_isnull(basic_array const *array)
{
    BASIC_ASSERT_PTR_NONNULL(array);
    return basic_block_isnull(&array->data) && !array->elem_size;
}

bool basic_array_isinit(basic_array const *array)
{
    BASIC_ASSERT_PTR_NONNULL(array);
    return !basic_block_isnull(&array->data) && array->elem_size;
}

#endif // BASIC_ARRAY_H_
