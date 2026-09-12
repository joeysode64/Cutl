#pragma once

#include "allocation.h"
#include "result.h"

#include <stdint.h>
#include <vulkan/vulkan.h>

/// @brief The dedicated allocator's allocate function.
CuResult dedicated_allocator_allocate(
    CuAllocation* pAllocation,
    void** ppData,
    CuAllocator* _pAllocator,
    const VkMemoryRequirements* pRequirements,
    uint32_t mRequired,
    uint32_t mPreferred);

/// @brief The dedicated allocator's free function.
void dedicated_allocator_free(
    CuAllocation* pAllocation,
    CuAllocator* _pAllocator);
