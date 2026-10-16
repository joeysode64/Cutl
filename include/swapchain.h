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

    VkSurfaceKHR _surface; /**< The surface handle. */

    VkExtent2D _extent; /**< The swapchain's extent. */

    uint32_t _nImages; /**< The number of swapchain images. */

    VkFormat _format; /**< The surface's format. */
} CuSwapchainInfo;

/** @brief A swapchain image. */
typedef struct CuSwapchainImage_T {
    VkImage _image; /**< The image handle. */

    VkImageView _imageView; /**< The image view handle. */

    VkSemaphore _renderFinished; /**< The "render finished" semaphore handle. */
} CuSwapchainImage;

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Creates a swapchain.
 * @param [out] pSwapchainInfo A pointer to the swapchain info.
 * @param minSwapchainImages The minimum number of swapchain images.
 * @param pWindow A pointer to the target window, or null for headless.
 * @param oldSwapchain The old swapchain handle, or null for none.
 * @return The result of creating the swapchain.
 */
CuResult cu_vk_create_swapchain(
    CuSwapchainInfo* pSwapchainInfo,
    uint32_t minSwapchainImages,
    const CuWindow* pWindow,
    VkSwapchainKHR oldSwapchain);

/**
 * @brief Creates the swapchain images.
 * @param [out] pSwapchainImages A pointer to the swapchain images.
 * @param nSwapchainImages The number of swapchain images.
 * @param swapchain The swapchain handle.
 * @param format The swapchain's format.
 * @return The result of creating the swapchain images.
 */
CuResult cu_swapchain_images_create(
    CuSwapchainImage* pSwapchainImages,
    uint32_t nSwapchainImages,
    const VkSwapchainKHR swapchain,
    const VkFormat format);

/**
 * @brief Destroys the swapchain images.
 * @param [in, out] pSwapchainImages A pointer to the swapchain images.
 * @param nSwapchainImages The number of swapchain images.
 */
void cu_swapchain_images_destroy(
    CuSwapchainImage* pSwapchainImages,
    uint32_t nSwapchainImages);

#ifdef __cplusplus
}
#endif
