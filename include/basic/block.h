#ifndef BASIC_BLOCK_H_
#define BASIC_BLOCK_H_

#include <stdbool.h>
#include <stddef.h>

#include "assertion.h"

typedef struct {
    void *ptr;
    size_t size;
} basic_block;

#define BASIC_BLOCK_NULL ((basic_block){NULL, 0})

static inline bool basic_block_isnull(basic_block const *block);
static inline bool basic_block_isinit(basic_block const *block);

basic_block basic_block_move(basic_block *block);
basic_block basic_block_clone(basic_block const *block);

basic_block basic_block_alloc(size_t size);
basic_block *basic_block_realloc(basic_block *block, size_t size);
void basic_block_dealloc(basic_block *block);

bool basic_block_isnull(basic_block const *block)
{
    BASIC_ASSERT_PTR_NONNULL(block);
    return !block->ptr && !block->size;
}

bool basic_block_isinit(basic_block const *block)
{
    BASIC_ASSERT_PTR_NONNULL(block);
    return block->ptr && block->size;
}

#endif // BASIC_BLOCK_H_
