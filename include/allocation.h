#pragma once

#include "result.h"

#include <stdint.h>
#include <vulkan/vulkan.h>

/** @brief A Vulkan memory allocation. */
typedef struct CuAllocation_T {
    /** @brief The allocation handle. */
    VkDeviceMemory _memory;

    /** @brief The offset from the allocation's base in bytes. */
    VkDeviceSize _offset;
} CuAllocation;

/** @brief A Vulkan memory allocator. */
typedef void CuAllocator;

/**
 * @brief An allocator's allocation callback.
 * @param[out] pAllocation A pointer to the allocation. Must not be null.
 * @param[out] ppData A pointer to the mapped memory. Will be ignored if null.
 * @param[in out] pAllocator A pointer to the allocator. Must not be null.
 * @param pRequirements A pointer to the memory requirements. Must not be null.
 * @param mProperties A bitmap of the memory properties. Must not be 0.
 */
typedef CuResult (*CuAllocateFn)(
    CuAllocation* pAllocation,
    void** ppData,
    CuAllocator* pAllocator,
    const VkMemoryRequirements* pRequirements,
    VkMemoryPropertyFlags mProperties);

/**
 * @brief An allocator's free callback.
 * @param[in out] pAllocation A pointer to the allocation. Must not be null if used.
 * @param[in out] pAllocator A pointer to the allocator. Must not be null if used.
 */
typedef void (*CuFreeFn)(CuAllocation* pAllocation, CuAllocator* pAllocator);

/** @brief A Vulkan memory allocator's callbacks. */
typedef struct CuAllocatorFns_T {
    /** @brief The allocate callback. */
    CuAllocateFn _fAllocate;

    /** @brief The free callback. */
    CuFreeFn _fFree;
} CuAllocatorFns;

/** @brief A Vulkan allocator mode. */
typedef struct CuAllocationMode_T {
    /** @brief A pointer to the allocator. */
    CuAllocator* _pAllocator;

    /** @brief A pointer to the allocator's callbacks. */
    const CuAllocatorFns* _pFns;
} CuAllocationMode;

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Allocates using the allocation mode.
 * @param[out] pAllocation A pointer to the allocation. Must not be null.
 * @param[out] ppData A pointer to the mapped memory. Ignored if null.
 * @param mode The allocation mode.
 * @param pRequirements A pointer to the memory requirements. Must not be null.
 * @param mProperties A bitmap of the memory type properties. Must not be 0.
 */
CU_NODISCARD_RESULT
static inline CuResult mode_allocate(
    CuAllocation* const pAllocation,
    void** const ppData,
    const CuAllocationMode mode,
    const VkMemoryRequirements* const pRequirements,
    const VkMemoryPropertyFlags mProperties)
{
    return mode._pFns->_fAllocate(
        pAllocation, ppData, mode._pAllocator, pRequirements, mProperties);
}

/**
 * @brief Frees an allocation using the allocation mode.
 * @param[in out] pAllocation A pointer to the allocation. Must not be null.
 * @param mode The allocation mode.
 */
static inline void mode_free(CuAllocation* const pAllocation, const CuAllocationMode mode)
{
    mode._pFns->_fFree(pAllocation, mode._pAllocator);
}

#ifdef __cplusplus
}
#endif
