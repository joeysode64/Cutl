#include "dedicated_allocator.h"

#include "allocation.h"
#include "g_context.h"
#include "physical_device.h"
#include "result.h"
#include "vk.h"

#include <stdint.h>
#include <vulkan/vulkan.h>
#include <vulkan/vulkan_core.h>

CuResult dedicated_allocator_allocate(
    CuAllocation* const pAllocation,
    void** const ppData,
    CuAllocator* const _pAllocator,
    const VkMemoryRequirements* const pRequirements,
    const uint32_t mRequired,
    const uint32_t mPreferred)
{
    (void)_pAllocator;

    const uint32_t mMemoryTypes = find_memory_types(
        &gContext.physicalDeviceInfo.memoryInfo,
        mRequired, mPreferred,
        pRequirements->memoryTypeBits);
    const uint32_t iMemoryType = __builtin_ctz(mMemoryTypes);

    VkDeviceMemory memory = VK_NULL_HANDLE;
    cu_try_vk(allocate_memory(&memory, gContext.device, pRequirements->size, iMemoryType));

    if (ppData != nullptr) {
        cu_try_vk(vkMapMemory(gContext.device, memory, 0, pRequirements->size, 0, ppData));
    }

    pAllocation->_memory = memory;
    pAllocation->_offset = 0;
    
    return CU_SUCCESS;
}

void dedicated_allocator_free(
    CuAllocation* const pAllocation,
    CuAllocator* const _pAllocator)
{
    (void)_pAllocator;

    vkFreeMemory(gContext.device, pAllocation->_memory, nullptr);
}
