#pragma once

#include "allocation.h"
#include "inner.h"
#include "result.h"

#include <stddef.h>
#include <stdint.h>

/// @brief An arena allocator memory type slot.
typedef struct CuArenaSlot_T {
    /// @brief The allocated memory handle.
    cu_vk_t(DeviceMemory) _memory;

    /// @brief The mapped CPU memory pointer.
    /// @note Unused for memory types that aren't device local.
    void* _pData;

    /// @brief The number of bytes used in the arena slot.
    uint64_t _used;
} CuArenaSlot;

/// @brief The number of potential slots in an arena allocator.
/// @note This follows 1 slot per memory type which Vulkan limits at 32.
constexpr size_t CU_N_ARENA_ALLOCATOR_SLOTS = 32;

/// @brief An arena allocator.
typedef struct CuArena_T {
    /// @brief The arena slots.
    CuArenaSlot _slots[CU_N_ARENA_ALLOCATOR_SLOTS];

    /// @brief The arena's slot size.
    size_t _size;
} CuArena;

/// @brief Creates an arena allocator.
/// @param [out] pArena A pointer to the arena allocator.
/// @param size The arena's slot size.
/// @return The result of creating the arena.
CuResult cu_arena_create(
    CuArena* pArena,
    size_t size);

/// @brief Destroys the arena allocator.
/// @param [in, out] pArena A pointer to the arena allocator.
void cu_arena_destroy(
    CuArena* pArena);

/// @brief Creates an allocation mode to use the arena allocator.
/// @param [in, out] pArena A pointer to the arena allocator.
/// @return The allocation mode for the arena allocator.
inline static CuAllocationMode cu_arena_mode(
    CuArena* const pArena)
{
    return (CuAllocationMode){
        ._pAllocator = pArena,
        ._pFns = CU_ARENA_ALLOCATOR_FNS,
    };
}
