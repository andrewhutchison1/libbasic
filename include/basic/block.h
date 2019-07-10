/**
 * @file block.h
 */

#ifndef BASIC_BLOCK_H_
#define BASIC_BLOCK_H_

#include <stdbool.h>
#include <stddef.h>

#include "assertion.h"

/**
 * @struct basic_block
 * @brief Represents a freestore-allocated memory area.
 *
 * The basic_block structure is a foundational object that models freestore-
 * allocated memory areas, and can be thought of as a size and pointer pair.
 * The pointer field always comes from the C allocator, and as such, ownership
 * semantics apply.
 * A basic_block has two states, *init* and *null*.
 *
 * @state_table_begin{basic_block}
 *  @state_table_entry{
 *      null,
 *      A basic_block morally equivalent to a NULL pointer,
 *      basic_block_isnull
 *  }
 *  @state_table_entry{
 *      init,
 *      A basic_block that currently owns a freestore-allocated memory area,
 *      basic_block_isinit
 *  }
 * @state_table_end
 *
 * The state transition diagram for basic_block is as follows:
 *
 * @dotfile basic_block.dot
 *
 * @var basic_block::ptr
 * @brief The pointer to the first byte of the memory area.
 *
 * @var basic_block::size
 * @brief The size, in bytes, of the memory area.
 */
typedef struct {
    void *ptr;
    size_t size;
} basic_block;

/**
 * @brief The value representing a basic_block in the null state.
 */
#define BASIC_BLOCK_NULL ((basic_block){NULL, 0})

/**
 * @brief Returns true if the basic_block pointed to by @c block is in
 *  the null state.
 * 
 * @param[in] block Pointer to the basic_block to query.
 *
 * @pre @c block must be non-NULL.
 *
 * @retval true If the basic_block pointed to by @c block is in the
 *  null state.
 * @retval false If the basic_block pointed to by @c block is not in
 *  the null state.
 */
static inline bool basic_block_isnull(basic_block const *block);

/**
 * @brief Returns true if the basic_block pointed to by @c block is in the
 *  initialised state.
 *
 * @param[in] block Pointer to the basic_block to query.
 *
 * @pre @c block must be non-NULL.
 *
 * @retval true If the basic_block pointed to by @c block is in the initialised
 *  state.
 * @retval false If the basic_block pointed to by @c block is not in the
 *  initialised state.
 */
static inline bool basic_block_isinit(basic_block const *block);

/**
 * @brief Destructively transfers ownership of the memory area
 *  owned by the basic_block pointed to by @c block, returning the new owner.
 *
 * @param[in] block Pointer to the basic_block to move from.
 *
 * @pre @c block must be non-NULL
 * @pre The basic_block pointed to by @c block must be in the initialised state
 * @post The basic_block pointed to by @c block will be set to the null state
 *
 * @returns A basic_block in the initialised state that owns the memory
 *  area previously owned by the basic_block pointed to by @c block.
 */
basic_block basic_block_move(basic_block *block);

/**
 * @brief Performs a deep-copy of the basic_block pointed to by @c block,
 *  returning a basic_block with memory area bytewise-equal to it.
 *
 * @param[in] block Pointer to the basic_block to clone.
 *
 * @pre @c block must be non-NULL
 * @pre The basic_block pointed to by @c block must be in the initialised
 *  or null states.
 *
 * @returns If @c block points to a basic_block in the null state, then
 *  @ref BASIC_BLOCK_NULL is returned.
 *  If @c block points to a basic_block in the initialised state, then
 *  the returned basic_block will be in the initialised state, with equal
 *  size and memory contents as the basic_block pointed to by @c block.
 */
basic_block basic_block_clone(basic_block const *block);

/**
 * @brief Attempts to allocate a memory area of the given size, returning
 *  a basic_block that owns it upon success, or @ref BASIC_BLOCK_NULL on
 *  failure.
 *
 * @param[in] size The size, in bytes, of the requested allocation.
 *
 * @pre @c size must be greater than zero
 * @post If allocation of the memory area succeeds, then it will be
 *  zero-initialised.
 *
 * @returns If the freestore allocation succeeds, then a basic_block
 *  that owns this memory area is returned, and it will be zero-initialised
 *  with size set to the @c size argument.
 *  If the freestore allocation fails, then @ref BASIC_BLOCK_NULL is returned.
 */
basic_block basic_block_alloc(size_t size);

/**
 * @brief Attempts to reallocate the memory area owned by a basic_block
 *  to the given size.
 *
 * @param[in] block Pointer to the basic_block to reallocate to the
 *  given size.
 * @param[in] size The size, in bytes, of the reallocation
 *
 * @pre block must be non-NULL
 * @pre size must be greater than zero
 * @pre The basic_block pointed to by @c block must be in the initialised
 *  state.
 * @post The memory area up to the minimum of @c size and @c block->size
 *  will not be modified
 * @post If @c size is greater than @c block->size, then the memory
 *  in the range [@c size, @c block->size) will be zero-initialised
 * @post If the reallocation fails, then the basic_block pointed to by @c block
 *  will not be modified.
 *
 * @retval block If the reallocation succeeds.
 * @retval NULL If the reallocation fails.
 */
basic_block *basic_block_realloc(basic_block *block, size_t size);

/**
 * @brief Deallocates the memory area represented by the basic_block pointed
 *  to by @c block and sets it to the null state.
 *
 * @param[in] block Pointer to the basic_block to deallocate.
 *
 * @pre @c block must be non-NULL
 * @pre The basic_block pointed to by @c block must be in the null or
 *  initialised states.
 * @post If the basic_block pointed to by @c block is in the initialised state,
 *  then it will be set to the null state.
 */
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
