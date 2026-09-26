#pragma once

#include "allocation.h"
#include "result.h"

#include <stdint.h>
#include <vulkan/vulkan.h>

/// @brief The dedicated allocator allocation callback functions.
extern const CuAllocatorFns CU_DEDICATED_ALLOCATOR_FNS;

/// @brief The dedicated allocator allocation mode.
static const CuAllocationMode CU_DEDICATED_ALLOCATOR_MODE = {
    ._pAllocator = nullptr,
    ._pFns = &CU_DEDICATED_ALLOCATOR_FNS,
};
