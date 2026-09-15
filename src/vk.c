#include "vk.h"

#include "allocation.h"
#include "allocation_fns.h"
#include "g_context.h"
#include "info.h"
#include "result.h"
#include "util.h"

#include <assert.h>
#include <stddef.h>
#include <stdint.h>
#include <vulkan/vk_platform.h>
#include <vulkan/vulkan.h>
#include <vulkan/vulkan_core.h>

CuResult vk_result_to_cu_result(
    VkResult result)
{
    switch (result) {
        case VK_SUCCESS:
            return CU_SUCCESS;
        case VK_TIMEOUT:
            return CU_ERROR_TIMEOUT;
        case VK_ERROR_OUT_OF_HOST_MEMORY:
            return CU_ERROR_OUT_OF_RAM;
        case VK_ERROR_OUT_OF_DEVICE_MEMORY:
            return CU_ERROR_OUT_OF_VRAM;
        case VK_ERROR_LAYER_NOT_PRESENT:
        case VK_ERROR_EXTENSION_NOT_PRESENT:
        case VK_ERROR_FEATURE_NOT_PRESENT:
        case VK_ERROR_INCOMPATIBLE_DRIVER:
            return CU_ERROR_UNSUPPORTED;
        default:
            return CU_ERROR_UNKNOWN;
    }
}

VkResult create_image_view(
    VkImageView* const pImageView,
    const VkDevice device,
    const VkImage image,
    const VkFormat format)
{
    assert(pImageView != nullptr);
    assert(device != VK_NULL_HANDLE);
    assert(image != VK_NULL_HANDLE);

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
    return vkCreateImageView(device, &createInfo, nullptr, pImageView);
}

VkResult allocate_command_buffers(
    VkCommandBuffer* const pCommandBuffers,
    const size_t nCommandBuffers)
{
    const VkCommandBufferAllocateInfo allocateInfo = {
        .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO,
        .pNext = nullptr,
        .commandPool = gContext.commandPool,
        .level = VK_COMMAND_BUFFER_LEVEL_PRIMARY,
        .commandBufferCount = nCommandBuffers,
    };
    return vkAllocateCommandBuffers(gContext.device, &allocateInfo, pCommandBuffers);
}

VkResult create_semaphore(
    VkSemaphore* const pSemaphore,
    const VkDevice device,
    const VkSemaphoreType type,
    const uint64_t x)
{
    assert(pSemaphore != nullptr);
    assert(device != VK_NULL_HANDLE);

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
    return vkCreateSemaphore(device, &createInfo, nullptr, pSemaphore);
}

VkResult allocate_memory(
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
    return vkAllocateMemory(device, &allocateInfo, nullptr, pMemory);
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
    CuResult result = CU_ERROR_UNKNOWN;

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
    cu_try_catch_vk(vkCreateBuffer(gContext.device, &bufferCreateInfo, nullptr, &buffer));

    VkMemoryRequirements memoryRequirements = {};
    vkGetBufferMemoryRequirements(gContext.device, buffer, &memoryRequirements);

    cu_try_catch(mode_allocate(
        pAllocation,
        ppData,
        mode,
        &memoryRequirements,
        mRequired,
        mPreferred));
    
    cu_try_catch_vk(vkBindBufferMemory(
        gContext.device,
        buffer,
        pAllocation->_memory,
        pAllocation->_offset));

    *pBuffer = buffer;

    return CU_SUCCESS;

FAIL:
    vkDestroyBuffer(gContext.device, buffer, nullptr);
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
    return vkGetBufferDeviceAddress(gContext.device, &info);
}

VkResult create_frames(
    CuFrame* const pFrames,
    const size_t nFrames,
    const VkDevice device,
    const VkCommandPool commandPool)
{
    assert(pFrames != nullptr);
    assert(nFrames > 0);
    assert(device != VK_NULL_HANDLE);
    assert(commandPool != VK_NULL_HANDLE);

    VkResult result = VK_ERROR_UNKNOWN;

    VkCommandBuffer commandBuffers[nFrames];
    vk_try_catch(allocate_command_buffers(commandBuffers, nFrames));

    for (size_t i = 0; i < nFrames; i++) {
        CuFrame* const pFrame = &pFrames[i];

        const VkSemaphoreCreateInfo semaphoreCreateInfo = {
            .sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO,
            .pNext = nullptr,
            .flags = 0,
        };
        vk_try_catch(vkCreateSemaphore(
            device,
            &semaphoreCreateInfo,
            nullptr,
            &pFrame->_imageAvailable
        ));
        vk_try_catch(create_semaphore(
            &pFrame->_imageAvailable, device, VK_SEMAPHORE_TYPE_BINARY, 0));
        pFrame->_commandBuffer = commandBuffers[i];
    }

    return VK_SUCCESS;

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
    assert(pFrames != nullptr);
    assert(nFrames > 0);
    assert(device != VK_NULL_HANDLE);

    VkCommandBuffer commandBuffers[nFrames];

    for (size_t i = 0; i < nFrames; i++) {
        CuFrame* const pFrame = &pFrames[i];

        commandBuffers[i] = pFrame->_commandBuffer;
        vkDestroySemaphore(device, pFrame->_imageAvailable, nullptr);
    }

    vkFreeCommandBuffers(device, commandPool, nFrames, commandBuffers);
}
