#pragma once

#include "allocation.h"
#include "result.h"

#include <stdint.h>
#include <vulkan/vulkan.h>

/// @brief The arena allocator's allocate function.
CuResult arena_allocator_allocate(
    CuAllocation* pAllocation,
    void** ppData,
    CuAllocator* pAllocator,
    const VkMemoryRequirements* pRequirements,
    uint32_t mRequired,
    uint32_t mPreferred);

/// @brief The arena allocator's free function.
void arena_allocator_free(
    CuAllocation* _pAllocation,
    CuAllocator* _pAllocator);
