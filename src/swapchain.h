#pragma once

#include "def.h"
#include "window.h"

#include <stdint.h>
#include <vulkan/vulkan.h>

/// @brief Create a swapchain.
VkResult create_swapchain(
    CuSwapchainInfo* pSwapchainInfo,
    uint32_t* pnSwapchainImages,
    uint32_t minSwapchainImages,
    const CuWindow* pWindow,
    VkSwapchainKHR oldSwapchain);

/// @brief Creates N swapchain images.
VkResult create_swapchain_images(
    CuSwapchainImage* pSwapchainImages,
    uint32_t nSwapchainImages,
    const CuSwapchainInfo* pSwapchainInfo);

/// @brief Destroy N swapchain images.
void destroy_swapchain_images(
    CuSwapchainImage* pSwapchainImages,
    uint32_t nSwapchainImages);
