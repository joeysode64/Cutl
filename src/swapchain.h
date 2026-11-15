#pragma once

#include "result.h"
#include "window.h"

#include <stdint.h>
#include <vulkan/vulkan.h>

/**
 * @brief A swapchain's information.
 * @note Used for the return value of `create_swapchain`.
 */
typedef struct CuSwapchainInfo_T {
    /** @brief The swapchain handle. */
    VkSwapchainKHR _handle;

    /** @brief The swapchain's extent. */
    VkExtent2D _extent;

    /** @brief The surface's format. */
    VkFormat _format;
} CuSwapchainInfo;

/**
 * @brief Creates a swapchain.
 * @param[out] pSwapchainInfo A pointer to the swapchain info. Must not be null.
 * @param minSwapchainImages The minimum number of swapchain images. Clamped to the surface's
 * capabilities.
 * @param pWindow A pointer to the target window. If null, the swapchain is headless.
 * @param surface The surface to create the swapchain for.
 * @param preferredPresentMode The preferred present mode. Falls back to FIFO if unsupported.
 * @param oldSwapchain The swapchain being replaced, or `VK_NULL_HANDLE` for none.
 */
CU_NODISCARD_RESULT
CuResult create_swapchain(
    CuSwapchainInfo* pSwapchainInfo,
    uint32_t minSwapchainImages,
    const CuWindow* pWindow,
    VkSurfaceKHR surface,
    VkPresentModeKHR preferredPresentMode,
    VkSwapchainKHR oldSwapchain);
