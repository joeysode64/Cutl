#pragma once

#include "allocation.h"
#include "result.h"

#include <stddef.h>
#include <stdint.h>
#include <vulkan/vulkan.h>

/** @brief An arena allocator slot. */
typedef struct CuArenaSlot_T {
    /** @brief The memory allocation handle. */
    VkDeviceMemory _memory;

    /** @brief A pointer to the mapped memory (if relevant). */
    void* _pData;

    /** @brief The number of bytes already used in the slot. */
    VkDeviceSize _used;
} CuArenaSlot;

/** @brief A Vulkan memory arena allocator. */
typedef struct CuArena_T {
    /** @brief The allocation slots. */
    CuArenaSlot _slots[VK_MAX_MEMORY_TYPES];

    /** @brief The size of the arena's slots. */
    VkDeviceSize _size;
} CuArena;

/** @brief The arena allocator callbacks. */
extern const CuAllocatorFns CU_ARENA_ALLOCATOR_FNS;

/**
 * @brief Returns an allocation mode for the arena.
 * @param pArena A pointer to the arena allocator. Must not be null.
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
 * @param[out] pArena A pointer to the arena. Must not be null.
 * @param size The size of the arena's slots.
 */
CU_NODISCARD_RESULT
CuResult cu_arena_create(CuArena* pArena, size_t size);

/**
 * @brief Destroys the arena allocator.
 * @param[in out] pArena A pointer to the arena. Must not be null.
 * @warning This makes all allocations depending on the arena become invalid.
 */
void cu_arena_destroy(CuArena* pArena);

#ifdef __cplusplus
}
#endif
