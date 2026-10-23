#pragma once

#include "allocation.h"
#include "result.h"

#include <stddef.h>
#include <stdint.h>
#include <vulkan/vulkan.h>

/** @brief An arena allocator slot. */
typedef struct CuArenaSlot_T {
    VkDeviceMemory _memory; /**< The memory allocation handle. */

    void* _pData; /**< A pointer to the mapped memory if relevant. */

    VkDeviceSize _used; /**< The number of bytes used by the slot. */
} CuArenaSlot;

/** @brief A Vulkan arena allocator. */
typedef struct CuArena_T {
    CuArenaSlot _slots[VK_MAX_MEMORY_TYPES]; /**< The allocation slots. */

    VkDeviceSize _size; /**< The size of the slots' allocations. */
} CuArena;

/** @brief The arena allocator callbacks. */
extern const CuAllocatorFns CU_ARENA_ALLOCATOR_FNS;

/**
 * @brief Returns an allocation mode for the arena.
 * @param pArena A pointer to the arena allocator.
 * @return An allocation mode for the arena.
 */
#define cu_arena_mode(pArena)                                                                      \
    ((CuAllocationMode){                                                                           \
        ._pAllocator = (pArena),                                                                   \
        ._pFns = &CU_ARENA_ALLOCATOR_FNS,                                                          \
    })

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Creates the arena allocator.
 * @param [out] pArena A pointer to the arena.
 * @param size The size of the arena's slots.
 * @return The result of the allocation.
 */
CuResult cu_arena_create(CuArena* pArena, size_t size);

/**
 * @brief Destroys the arena allocator.
 * @param [in, out] pArena A pointer to the arena.
 * @warning This makes all allocations depending on the arena become invalid.
 */
void cu_arena_destroy(CuArena* pArena);

#ifdef __cplusplus
}
#endif
