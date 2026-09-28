#pragma once

#include "def.h"
#include "result.h"
#include "window.h"

#include <stdint.h>
#include <vulkan/vulkan.h>

/// @brief Create a swapchain.
/// @param pSwapchainInfo A pointer to the swapchain info.
/// @param pnSwapchain Images A pointer to the number of swapchain images.
/// @param minSwapchainImages The minimum number of swapchain images.
/// @param pWindow A pointer to the target window, or null for headless.
/// @param oldSwapchain The old swapchain handle, or null for none.
/// @return The result of creating the swapchain.
CuResult create_swapchain(
    CuSwapchainInfo* pSwapchainInfo,
    uint32_t* pnSwapchainImages,
    uint32_t minSwapchainImages,
    const CuWindow* pWindow,
    VkSwapchainKHR oldSwapchain);

/// @brief Creates N swapchain images.
CuResult create_swapchain_images(
    CuSwapchainImage* pSwapchainImages,
    uint32_t nSwapchainImages,
    const CuSwapchainInfo* pSwapchainInfo);

/// @brief Destroy N swapchain images.
void destroy_swapchain_images(
    CuSwapchainImage* pSwapchainImages,
    uint32_t nSwapchainImages);
