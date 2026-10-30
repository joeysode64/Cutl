#pragma once

#include "result.h"
#include "window.h"

#include <stdint.h>
#include <vulkan/vulkan.h>

/**
 * @brief A swapchain's information.
 * @note Used for the return value of `cu_vk_create_swapchain`.
 */
typedef struct CuSwapchainInfo_T {
    VkSwapchainKHR _handle; /**< The swapchain handle. */

    VkExtent2D _extent; /**< The swapchain's extent. */

    uint32_t _nImages; /**< The number of swapchain images. */

    VkFormat _format; /**< The surface's format. */
} CuSwapchainInfo;

/**
 * @brief Creates a swapchain.
 * @param [out] pSwapchainInfo A pointer to the swapchain info.
 * @param minSwapchainImages The minimum number of swapchain images.
 * @param pWindow A pointer to the target window, or null for headless.
 * @param surface The surface handle.
 * @param preferredPresentMode The preferred present mode.
 * @param oldSwapchain The old swapchain handle, or null for none.
 * @return The result of creating the swapchain.
 */
CuResult create_swapchain(
    CuSwapchainInfo* pSwapchainInfo,
    uint32_t minSwapchainImages,
    const CuWindow* pWindow,
    VkSurfaceKHR surface,
    VkPresentModeKHR preferredPresentMode,
    VkSwapchainKHR oldSwapchain);
