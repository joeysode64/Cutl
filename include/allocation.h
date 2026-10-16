#pragma once

#include "result.h"

#include <stdint.h>
#include <vulkan/vulkan.h>
#include <vulkan/vulkan_core.h>

/** @brief A Vulkan memory allocation. */
typedef struct CuAllocation_T {
    VkDeviceMemory _memory; /**< The memory allocation handle. */

    VkDeviceSize _offset; /**< The offset from the allocation's base. */
} CuAllocation;

/** @brief A Vulkan memory allocator. */
typedef void CuAllocator;

/**
 * @brief An allocator's allocation callback.
 * @param [out] pAllocation A pointer to the allocation.
 * @param [out] ppData A pointer the mapped memory, or null to ignore.
 * @param [in, out] pAllocator A pointer to the allocator.
 * @param pRequirements A pointer to the memory requirements.
 * @param mProperties A bitmap of the memory properties.
 */
typedef CuResult(*CuAllocateFn)(
    CuAllocation* pAllocation,
    void** ppData,
    CuAllocator* pAllocator,
    const VkMemoryRequirements* pRequirements,
    VkMemoryPropertyFlags mProperties);

/**
 * @brief An allocator's free callback.
 * @param [in, out] pAllocation A pointer to the allocation.
 * @param [in, out] pAllocator A pointer to the allocator.
 */
typedef void(*CuFreeFn)(
    CuAllocation* pAllocation,
    CuAllocator* pAllocator);

/** @brief A Vulkan memory allocator's callbacks. */
typedef struct CuAllocatorFns_T {
    CuAllocateFn _fAllocate; /**< The allocate callback. */

    CuFreeFn _fFree; /**< The free callback. */
} CuAllocatorFns;

/** @brief A Vulkan allocator mode. */
typedef struct CuAllocationMode_T {
    CuAllocator* _pAllocator; /**< A pointer to the allocator. */

    const CuAllocatorFns* _pFns; /**< A pointer to the allocator's callbacks. */
} CuAllocationMode;

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Allocates using the allocation mode.
 * @param [out] pAllocation A pointer to the allocation.
 * @param [out] ppData A pointer to the mapped memory. Ignored if null.
 * @param mode The allocation mode.
 * @param pRequirements A pointer to the memory requirements.
 * @param mProperties A bitmap of the memory type properties.
 * @return The result of the allocation.
 */
static inline CuResult mode_allocate(
    CuAllocation* const pAllocation,
    void** const ppData,
    const CuAllocationMode mode,
    const VkMemoryRequirements* const pRequirements,
    const VkMemoryPropertyFlags mProperties)
{
    return mode._pFns->_fAllocate(
        pAllocation,
        ppData,
        mode._pAllocator,
        pRequirements,
        mProperties);
}

/**
 * @brief Frees an allocation using the allocation mode.
 * @param [in, out] pAllocation A pointer to the allocation.
 * @param mode The allocation mode.
 */
static inline void mode_free(
    CuAllocation* const pAllocation,
    const CuAllocationMode mode)
{
    mode._pFns->_fFree(pAllocation, mode._pAllocator);
}

#ifdef __cplusplus
}
#endif
