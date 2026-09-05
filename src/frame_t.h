#pragma once

#include <stddef.h>
#include <vulkan/vulkan.h>
#include <vulkan/vulkan_core.h>

typedef struct CuFrame_T {
    VkCommandBuffer commandBuffer; /**< The command buffer handle. */

    VkSemaphore imageAvailable; /**< The "image available" semaphore handle. */
} CuFrame_T;

/** @brief Creates N frames. */
VkResult create_frames(
    CuFrame_T* pFrames,
    size_t nFrames,
    VkDevice device,
    VkCommandPool commandPool);

/** @brief Destroys N frames. */
void destroy_frames(
    CuFrame_T* pFrames,
    size_t nFrames,
    VkDevice device);
