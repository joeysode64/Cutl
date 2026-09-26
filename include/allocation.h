#pragma once

#include "inner.h"
#include "result.h"

#include <stdint.h>

/// @brief A GPU memory allocation.
typedef struct CuAllocation_T {
    /// @brief The memory handle.
    cu_vk_t(DeviceMemory) _memory;

    /// @brief The memory offset.
    uint64_t _offset;
} CuAllocation;

/// @brief A GPU memory allocator.
typedef void CuAllocator;

/// @brief An allocator's allocate callback.
/// @param [out] pAllocation A pointer to the allocation.
/// @param [out] ppData A pointer to the mapped memory. Ignored if null.
/// @param [in, out] pAllocator A pointer to the allocator.
/// @param pRequirements A pointer to the memory requirements.
/// @param mRequired A bitmask of the required memory types.
/// @param mPreferred A bitmask of the preferred (but not required) memory types.
/// @return The result of the allocation.
typedef CuResult(*CuAllocateFn)(
    CuAllocation* pAllocation,
    void** ppData,
    CuAllocator* pAllocator,
    const VkMemoryRequirements* pRequirements,
    uint32_t mRequired,
    uint32_t mPreferred);

/// @brief An allocator's free callback.
/// @param [in, out] pAllocation A pointer to the allocation.
/// @param [in, out] pAllocator A pointer to the allocator.
typedef void(*CuFreeFn)(
    CuAllocation* pAllocation,
    CuAllocator* pAllocator);

/// @brief A GPU memory allocator's callback functions.
typedef struct CuAllocatorFns_T {
    /// @brief The allocation function.
    CuAllocateFn _fAllocate;

    /// @brief The free function.
    CuFreeFn _fFree;
} CuAllocatorFns;

/// @brief A GPU allocation mode.
typedef struct CuAllocationMode_T {
    /// @brief A pointer to the allocator.
    CuAllocator* _pAllocator;

    /// @brief The allocator's callback functions.
    const CuAllocatorFns* _pFns;
} CuAllocationMode;
