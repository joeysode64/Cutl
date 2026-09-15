#include "allocation.h"

#include "allocation_fns.h"
#include "dedicated_allocator.h"
#include "result.h"

#include <stdint.h>
#include <vulkan/vulkan.h>

/// @brief An array of the allocators' callbacks functions.
static const AllocatorFns ALLOCATOR_FNS[] = {
    // The dedicated allocator:
    {
        .fAllocate = &dedicated_allocator_allocate,
        .fFree = &dedicated_allocator_free,
    },
};

const AllocatorFns* CU_DEDICATED_ALLOCATOR_FNS = &ALLOCATOR_FNS[0];
