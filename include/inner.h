#pragma once

#include "format.h"

#include <stdint.h>
#include <vulkan/vulkan_core.h>

#if (VK_USE_64_BIT_PTR_DEFINES == 1) || (SIZE_MAX == UINT64_MAX)
/** @brief Maps a non-dispatchable Vulkan type name to its defined Vulkan type. */
#define cu_vk_t(t) struct Vk##t##_T*
#else
/** @brief Maps a non-dispatchable Vulkan type name to its defined Vulkan type. */
#define cu_vk_t(t) uint64_t
#endif

/** @brief Maps a dispatchable Vulkan type name to its defined Vulkan type. */
#define cu_vk_dispatch_t(t) struct Vk##t##_T*

/** @brief A swapchain's information. */
typedef struct CuSwapchainInfo_T {
    cu_vk_t(SwapchainKHR) _handle; /**< The swapchain handle. */

    cu_vk_t(SurfaceKHR) _surface; /**< The target surface handle. */

    CuFormat _format; /**< The swapchain's format. */

    uint32_t _w; /**< The swapchain's width. */

    uint32_t _h; /**< The swapchain's height. */
} CuSwapchainInfo;

/** @brief A swapchain image. */
typedef struct CuSwapchainImage_T {
    cu_vk_t(Image) _image; /**< The image handle. */

    cu_vk_t(ImageView) _imageView; /**< The image view handle. */

    cu_vk_t(Semaphore) _renderFinished; /**< The "render finished" semaphore handle. */
} CuSwapchainImage;
