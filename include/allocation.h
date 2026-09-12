#pragma once

#include "inner.h"

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

/// @brief A GPU memory allocator's callback functions.
typedef struct CuAllocatorFns_T CuAllocatorFns;

/// @brief A GPU allocation mode.
typedef struct CuAllocationMode_T {
    /// @brief A pointer to the allocator.
    CuAllocator* _pAllocator;

    /// @brief The allocator's callback functions.
    const CuAllocatorFns* _pFns;
} CuAllocationMode;

/// @brief The dedicated allocator callback functions.
extern const CuAllocatorFns* CU_DEDICATED_ALLOCATOR_FNS;

/// @brief The dedicated allocator allocation mode.
#define CU_DEDICATED_ALLOCATOR_MODE \
    ((CuAllocationMode){._pAllocator = nullptr, ._pFns = CU_DEDICATED_ALLOCATOR_FNS})

/// @brief The arena allocator callback functions.
extern const CuAllocatorFns* CU_ARENA_ALLOCATOR_FNS;
