#include "arena_allocator.h"

#include "allocation.h"
#include "g_context.h"
#include "physical_device.h"
#include "result.h"
#include "util.h"
#include "vk.h"

#include <stddef.h>
#include <stdint.h>
#include <vulkan/vulkan.h>
#include <vulkan/vulkan_core.h>

/// @brief Returns whether the arena slot has room for the allocation.
/// @param [in] pArena A pointer to the arena.
/// @param i The index of the arena slot.
/// @param z The size of the allocation.
/// @return Whetehr the allocation will fit in the arena slot.
static bool does_arena_slot_have_room(
    const CuArena* pArena,
    size_t i,
    size_t z);

/// @brief Allocates data from the arena slot into the allocation.
/// @param [out] pAllocation A pointer to the allocation.
/// @param [out] ppData A pointer to the mapped data.
/// @param [in, out] pArena a pointer to the arena.
/// @param i The index of the arena slot and memory type.
/// @param size The allocation's size.
/// @param align The allocation's alignment.
/// @return The result of the allocation.
/// @note This will allocate the slot if it hasn't been already.
static CuResult allocate_to_slot(
    CuAllocation* pAllocation,
    void** ppData,
    CuArena* pArena,
    size_t i,
    uint64_t size,
    uint64_t align);

/// @brief Allocates the arena slot.
/// @param [in, out] pArena A pointer to the arena.
/// @param i The index of the arena slot and memory type.
/// @return The result of the allocation.
static CuResult allocate_slot(
    CuArena* pArena,
    size_t i);

CuResult cu_arena_create(
    CuArena* const pArena,
    const size_t size)
{
    for (size_t i = 0; i < CU_N_ARENA_ALLOCATOR_SLOTS; i++) {
        pArena->_slots[i]._memory = VK_NULL_HANDLE;
    }
    pArena->_size = size;

    return CU_SUCCESS;
}

void cu_arena_destroy(
    CuArena* const pArena)
{
    for (size_t i = 0; i < CU_N_ARENA_ALLOCATOR_SLOTS; i++) {
        vkFreeMemory(gContext.device, pArena->_slots[i]._memory, nullptr);
    }
}

CuResult arena_allocator_allocate(
    CuAllocation* const pAllocation,
    void** const ppData,
    CuAllocator* const pAllocator,
    const VkMemoryRequirements* const pRequirements,
    const uint32_t mRequired, 
    const uint32_t mPreferred)
{
    CuArena* const pArena = (CuArena*)pAllocator;

    if (pRequirements->size > pArena->_size) {
        return CU_ERROR_ALLOCATION_TOO_LARGE;
    }

    uint32_t mMemoryTypes = find_memory_types(
        &gContext.physicalDeviceInfo.memoryInfo,
        mRequired,
        mPreferred,
        pRequirements->memoryTypeBits);

    // Find the first suitable slot that has room.
    while (mMemoryTypes != 0) {
        const uint32_t i = __builtin_ctz(mMemoryTypes);
        mMemoryTypes &= ~(1U << i);

        if (does_arena_slot_have_room(pArena, i, pRequirements->size)) {
            return allocate_to_slot(
                pAllocation,
                ppData,
                (CuArena*)pAllocator,
                i,
                pRequirements->size,
                pRequirements->alignment);
        }
    }

    return CU_ERROR_ARENA_FULL;
}

void arena_allocator_free(
    CuAllocation* const _pAllocation,
    CuAllocator* const _pAllocator)
{
    (void)_pAllocation;
    (void)_pAllocator;
}

bool does_arena_slot_have_room(
    const CuArena* pArena,
    size_t i,
    size_t z)
{
    const CuArenaSlot* const pSlot = &pArena->_slots[i];
    return (pSlot->_memory == VK_NULL_HANDLE) || (pSlot->_used + z < pArena->_size);
}

CuResult allocate_to_slot(
    CuAllocation* const pAllocation,
    void** const ppData,
    CuArena* const pArena,
    const size_t i,
    const uint64_t size,
    const uint64_t align)
{
    CuArenaSlot* const pSlot = &pArena->_slots[i];

    if (pSlot->_memory == VK_NULL_HANDLE) {
        cu_try(allocate_slot(pArena, i));
    }

    const uint64_t offset = align_up(pSlot->_used, align);

    pAllocation->_memory = pSlot->_memory;
    pAllocation->_offset = offset;

    if (ppData != nullptr && pSlot->_pData != nullptr) {
        *ppData = (uint8_t*)pSlot->_pData + offset;
    }

    pSlot->_used = offset + size;

    return CU_SUCCESS;
}

CuResult allocate_slot(
    CuArena* pArena,
    size_t i)
{
    CuArenaSlot* const pSlot = &pArena->_slots[i];

    cu_try_vk(allocate_memory(&pSlot->_memory, gContext.device, pArena->_size, i));
    if (ones_overlap(gContext.physicalDeviceInfo.memoryInfo.mHostVisible, 1u << i)) {
        cu_try_vk(vkMapMemory(gContext.device, pSlot->_memory, 0, pArena->_size, 0, &pSlot->_pData));
    }
    pSlot->_used = 0;

    return CU_SUCCESS;
}
