#include "frame_t.h"

#include "g_context.h"
#include "vk.h"

#include <stddef.h>
#include <vulkan/vulkan.h>
#include <vulkan/vulkan_core.h>

VkResult create_frames(
    CuFrame_T* const pFrames,
    const size_t nFrames,
    const VkDevice device,
    const VkCommandPool commandPool)
{
    VkResult result = VK_ERROR_UNKNOWN;

    VkCommandBuffer commandBuffers[nFrames];
    const VkCommandBufferAllocateInfo allocateInfo = {
        .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO,
        .pNext = nullptr,
        .commandPool = commandPool,
        .level = VK_COMMAND_BUFFER_LEVEL_PRIMARY,
        .commandBufferCount = nFrames,
    };
    vk_try_catch(vkAllocateCommandBuffers(device, &allocateInfo, commandBuffers));

    for (size_t i = 0; i < nFrames; i++) {
        CuFrame_T* const pFrame = &pFrames[i];

        const VkSemaphoreCreateInfo semaphoreCreateInfo = {
            .sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO,
            .pNext = nullptr,
            .flags = 0,
        };
        vk_try_catch(vkCreateSemaphore(
            gContext.device,
            &semaphoreCreateInfo,
            nullptr,
            &pFrame->imageAvailable
        ));
        vk_try_catch(create_semaphore(
            &pFrame->imageAvailable, device, VK_SEMAPHORE_TYPE_BINARY, 0));
        pFrame->commandBuffer = commandBuffers[i];
    }

    return VK_SUCCESS;

FAIL:
    destroy_frames(pFrames, nFrames, device);
    return result;
}

void destroy_frames(
    CuFrame_T* const pFrames,
    const size_t nFrames,
    const VkDevice device)
{
    VkCommandBuffer commandBuffers[nFrames];

    for (size_t i = 0; i < nFrames; i++) {
        CuFrame_T* const pFrame = &pFrames[i];

        commandBuffers[i] = pFrame->commandBuffer;
        vkDestroySemaphore(device, pFrame->imageAvailable, nullptr);
    }

    vkFreeCommandBuffers(device, gContext.commandPool, nFrames, commandBuffers);
}
