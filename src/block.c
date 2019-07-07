#include "block.h"

#include <stdlib.h>
#include <string.h>

basic_block basic_block_move(basic_block *block)
{
    BASIC_ASSERT_PTR_NONNULL(block);
    BASIC_ASSERT(basic_block_isinit(block),
            "cannot move from uninitialised basic_block object");

    basic_block temp = *block;
    *block = BASIC_BLOCK_NULL;
    return temp;
}

basic_block basic_block_clone(basic_block const *block)
{
    BASIC_ASSERT_PTR_NONNULL(block);
    BASIC_ASSERT(basic_block_isnull(block) || basic_block_isinit(block),
            "basic_block object must be null or initialised");

    if (basic_block_isnull(block)) return BASIC_BLOCK_NULL;

    void *ptr = calloc(1, block->size);
    if (!ptr) {
        return BASIC_BLOCK_NULL;
    }

    memcpy(ptr, block->ptr, block->size);
    return (basic_block) {
        .ptr = ptr,
        .size = block->size
    };
}

basic_block basic_block_alloc(size_t size)
{
    BASIC_ASSERT_NONZERO(size);

    void *ptr = calloc(1, size);
    if (!ptr) {
        return BASIC_BLOCK_NULL;
    }

    return (basic_block) {
        .ptr = ptr,
        .size = size
    };
}

basic_block *basic_block_realloc(basic_block *block, size_t size)
{
    BASIC_ASSERT_PTR_NONNULL(block);
    BASIC_ASSERT_NONZERO(size);
    BASIC_ASSERT(basic_block_isnull(block) || basic_block_isinit(block),
            "basic_object object must be null or initialised");

    if (basic_block_isnull(block)) {
        *block = basic_block_alloc(size);
        return block;
    }

    void *ptr = realloc(block->ptr, size);
    if (!ptr) {
        return NULL;
    }

    if (size > block->size) {
        memset((void *)((char *)ptr + block->size), 0, size - block->size);
    }

    block->ptr = ptr;
    block->size = size;
    return block;
}

void basic_block_dealloc(basic_block *block)
{
    BASIC_ASSERT_PTR_NONNULL(block);
    BASIC_ASSERT(basic_block_isnull(block) || basic_block_isinit(block),
            "basic_block object must be null or initialised");

    if (basic_block_isinit(block)) {
        free(block->ptr);
        *block = BASIC_BLOCK_NULL;
    }
}
