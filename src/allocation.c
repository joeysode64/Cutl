#include "allocation.h"

#include "dedicated_allocator.h"
#include "result.h"

#include <stdint.h>
#include <vulkan/vulkan.h>

/// @brief An allocator's allocate callback.
/// @param [out] pAllocation A pointer to the allocation.
/// @param [out] ppData A pointer to the mapped memory. Ignored if null.
/// @param [in, out] pAllocator A pointer to the allocator.
/// @param pRequirements A pointer to the memory requirements.
/// @param mRequired A bitmask of the required memory types.
/// @param mPreferred A bitmask of the preferred (but not required) memory types.
/// @return The result of the allocation.
typedef CuResult(*AllocateFn)(
    CuAllocation* pAllocation,
    void** ppData,
    CuAllocator* pAllocator,
    const VkMemoryRequirements* pRequirements,
    uint32_t mRequired,
    uint32_t mPreferred);

/// @brief An allocator's free callback.
/// @param [in, out] pAllocation A pointer to the allocation.
/// @param [in, out] pAllocator A pointer to the allocator.
typedef void(*FreeFn)(
    CuAllocation* pAllocation,
    CuAllocator* pAllocator);

typedef struct CuAllocatorFns_T {
    /// @brief The allocate callback function.
    AllocateFn fAllocate;

    /// @brief The free callback function.
    FreeFn fFree;
} AllocatorFns;

/// @brief An array of the allocators' callbacks functions.
static const AllocatorFns ALLOCATOR_FNS[] = {
    // The dedicated allocator:
    {
        .fAllocate = &dedicated_allocator_allocate,
        .fFree = &dedicated_allocator_free,
    },
};

const AllocatorFns* CU_DEDICATED_ALLOCATOR_FNS = &ALLOCATOR_FNS[0];
