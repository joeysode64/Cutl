#include "vk.h"

#include "allocation.h"
#include "g_context.h"
#include "result.h"
#include "util.h"

#include <stddef.h>
#include <stdint.h>
#include <vulkan/vulkan.h>

CuResult create_image_view(
    VkImageView* const pImageView,
    const VkDevice device,
    const VkImage image,
    const VkFormat format)
{
    const VkComponentMapping componentMapping = {
        .r = VK_COMPONENT_SWIZZLE_IDENTITY,
        .g = VK_COMPONENT_SWIZZLE_IDENTITY,
        .b = VK_COMPONENT_SWIZZLE_IDENTITY,
        .a = VK_COMPONENT_SWIZZLE_IDENTITY,
    };
    const VkImageSubresourceRange subresourceRange = {
        .aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,
        .baseMipLevel = 0,
        .levelCount = 1,
        .baseArrayLayer = 0,
        .layerCount = 1,
    };
    const VkImageViewCreateInfo createInfo = {
        .sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO,
        .pNext = nullptr,
        .flags = 0,
        .image = image,
        .viewType = VK_IMAGE_VIEW_TYPE_2D,
        .format = format,
        .components = componentMapping,
        .subresourceRange = subresourceRange,
    };
    return cu_vk_result(vkCreateImageView(device, &createInfo, nullptr, pImageView));
}

CuResult allocate_command_buffers(
    VkCommandBuffer* const pCommandBuffers,
    const size_t nCommandBuffers)
{
    const VkCommandBufferAllocateInfo allocateInfo = {
        .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO,
        .pNext = nullptr,
        .commandPool = gContext._commandPool,
        .level = VK_COMMAND_BUFFER_LEVEL_PRIMARY,
        .commandBufferCount = nCommandBuffers,
    };
    return cu_vk_result(vkAllocateCommandBuffers(gContext._device, &allocateInfo, pCommandBuffers));
}

CuResult create_semaphore(
    VkSemaphore* const pSemaphore,
    const VkDevice device,
    const VkSemaphoreType type,
    const uint64_t x)
{
    const VkSemaphoreTypeCreateInfo typeCreateInfo = {
        .sType = VK_STRUCTURE_TYPE_SEMAPHORE_TYPE_CREATE_INFO,
        .pNext = nullptr,
        .semaphoreType = type,
        .initialValue = x,
    };
    const VkSemaphoreCreateInfo createInfo = {
        .sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO,
        .pNext = &typeCreateInfo,
        .flags = 0,
    };
    return cu_vk_result(vkCreateSemaphore(device, &createInfo, nullptr, pSemaphore));
}

CuResult allocate_memory(
    VkDeviceMemory* const pMemory,
    const VkDevice device,
    const uint64_t size,
    const uint32_t i)
{
    const VkMemoryAllocateFlagsInfo allocateFlagsInfo = {
        .sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_FLAGS_INFO,
        .pNext = nullptr,
        .flags = VK_MEMORY_ALLOCATE_DEVICE_ADDRESS_BIT,
        .deviceMask = 1,
    };
    const VkMemoryAllocateInfo allocateInfo = {
        .sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO,
        .pNext = &allocateFlagsInfo,
        .allocationSize = size,
        .memoryTypeIndex = i,
    };
    return cu_vk_result(vkAllocateMemory(device, &allocateInfo, nullptr, pMemory));
}

CuResult create_buffer(
    VkBuffer* const pBuffer,
    CuAllocation* const pAllocation,
    void** const ppData,
    const size_t z,
    const VkBufferUsageFlags usage,
    const CuAllocationMode mode,
    const VkMemoryPropertyFlags mRequired,
    const VkMemoryPropertyFlags mPreferred)
{
    CuResult result = CU_SUCCESS;

    const VkBufferCreateInfo bufferCreateInfo = {
        .sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO,
        .pNext = nullptr,
        .flags = 0,
        .size = z,
        .usage = usage,
        .sharingMode = VK_SHARING_MODE_EXCLUSIVE,
        .queueFamilyIndexCount = 0,
        .pQueueFamilyIndices = nullptr,
    };
    VkBuffer buffer = VK_NULL_HANDLE;
    cu_try_catch_vk(vkCreateBuffer(gContext._device, &bufferCreateInfo, nullptr, &buffer));

    VkMemoryRequirements memoryRequirements = {};
    vkGetBufferMemoryRequirements(gContext._device, buffer, &memoryRequirements);

    cu_try_catch(mode_allocate(
        pAllocation,
        ppData,
        mode,
        &memoryRequirements,
        mRequired,
        mPreferred));
    
    cu_try_catch_vk(vkBindBufferMemory(
        gContext._device,
        buffer,
        pAllocation->_memory,
        pAllocation->_offset));

    *pBuffer = buffer;

    return CU_SUCCESS;

FAIL:
    vkDestroyBuffer(gContext._device, buffer, nullptr);
    mode_free(pAllocation, mode);
    return result;
}

VkDeviceAddress get_buffer_device_address(
    const VkBuffer buffer)
{
    const VkBufferDeviceAddressInfo info = {
        .sType = VK_STRUCTURE_TYPE_BUFFER_DEVICE_ADDRESS_INFO,
        .pNext = nullptr,
        .buffer = buffer,
    };
    return vkGetBufferDeviceAddress(gContext._device, &info);
}

CuResult create_frames(
    CuFrame* const pFrames,
    const size_t nFrames,
    const VkDevice device,
    const VkCommandPool commandPool)
{
    CuResult result = CU_SUCCESS;

    VkCommandBuffer commandBuffers[nFrames];
    cu_try_catch(allocate_command_buffers(commandBuffers, nFrames));

    for (size_t i = 0; i < nFrames; i++) {
        CuFrame* const pFrame = &pFrames[i];

        cu_try_catch(create_semaphore(
            &pFrame->_imageAvailable, device, VK_SEMAPHORE_TYPE_BINARY, 0));
        pFrame->_commandBuffer = commandBuffers[i];
    }

    return CU_SUCCESS;

FAIL:
    destroy_frames(pFrames, nFrames, device, commandPool);
    return result;
}

void destroy_frames(
    CuFrame* const pFrames,
    const size_t nFrames,
    const VkDevice device,
    const VkCommandPool commandPool)
{
    VkCommandBuffer commandBuffers[nFrames];

    for (size_t i = 0; i < nFrames; i++) {
        CuFrame* const pFrame = &pFrames[i];

        commandBuffers[i] = pFrame->_commandBuffer;
        vkDestroySemaphore(device, pFrame->_imageAvailable, nullptr);
    }

    vkFreeCommandBuffers(device, commandPool, nFrames, commandBuffers);
}
