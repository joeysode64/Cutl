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
    CuAllocation* pAllocation,
    void** ppData,
    CuAllocator* _pAllocator,
    const VkMemoryRequirements* pRequirements,
    uint32_t mRequired,
    uint32_t mPreferred)
{
    (void)_pAllocator;

    const uint32_t mMemoryTypes = find_memory_types(
        &gContext.physicalDeviceInfo.memoryInfo,
        mRequired, mPreferred,
        pRequirements->memoryTypeBits);
    const uint32_t iMemoryType = __builtin_ctz(mMemoryTypes);

    const VkMemoryAllocateFlagsInfo allocateFlagsInfo = {
        .sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_FLAGS_INFO,
        .pNext = nullptr,
        .flags = VK_MEMORY_ALLOCATE_DEVICE_ADDRESS_BIT,
        .deviceMask = 1,
    };
    const VkMemoryAllocateInfo allocateInfo = {
        .sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO,
        .pNext = &allocateFlagsInfo,
        .allocationSize = pRequirements->size,
        .memoryTypeIndex = iMemoryType,
    };
    VkDeviceMemory memory = VK_NULL_HANDLE;
    cu_try_vk(vkAllocateMemory(gContext.device, &allocateInfo, nullptr, &memory));

    if (ppData != nullptr) {
        cu_try_vk(vkMapMemory(gContext.device, memory, 0, pRequirements->size, 0, ppData));
    }

    pAllocation->_memory = memory;
    pAllocation->_offset = 0;
    
    return CU_SUCCESS;
}

void dedicated_allocator_free(
    CuAllocation* pAllocation,
    CuAllocator* _pAllocator)
{
    (void)_pAllocator;

    vkFreeMemory(gContext.device, pAllocation->_memory, nullptr);
}
