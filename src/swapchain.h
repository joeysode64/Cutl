#pragma once

#include "inner.h"
#include "result.h"
#include "window.h"

#include <stdint.h>
#include <vulkan/vulkan.h>
#include <vulkan/vulkan_core.h>

/** @brief Create a swapchain. */
VkResult create_swapchain(
    CuSwapchainInfo* pSwapchainInfo,
    uint32_t* pnSwapchainImages,
    uint32_t minSwapchainImages,
    VkInstance instance,
    VkDevice device,
    VkPhysicalDevice physicalDevice,
    const CuWindow* pWindow,
    VkSwapchainKHR oldSwapchain);

/** @brief Creates N swapchain images. */
VkResult create_swapchain_images(
    CuSwapchainImage* pSwapchainImages,
    uint32_t nSwapchainImages,
    VkDevice device,
    const CuSwapchainInfo* pSwapchainInfo);

/** @brief Destroy N swapchain images. */
void destroy_swapchain_images(
    CuSwapchainImage* pSwapchainImages,
    uint32_t nSwapchainImages,
    VkDevice device);
