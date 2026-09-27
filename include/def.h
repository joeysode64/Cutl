#pragma once

#include "format.h"

#include <stdint.h>

#if (VK_USE_64_BIT_PTR_DEFINES == 1) || (SIZE_MAX == UINT64_MAX)
/// @brief Maps a non-dispatchable Vulkan type name to its defined Vulkan type.
#define cu_vk_t(t) struct Vk##t##_T*
#else
/// @brief Maps a non-dispatchable Vulkan type name to its defined Vulkan type.
#define cu_vk_t(t) uint64_t
#endif

/// @brief Maps a dispatchable Vulkan type name to its defined Vulkan type.
#define cu_vk_dispatch_t(t) struct Vk##t##_T*

typedef struct CuFrame_T CuFrame;

typedef struct CuRenderer_T CuRenderer;

/// @brief A physical device's memory type info.
typedef struct CuPhysicalDeviceMemoryInfo_T {
    /// @brief A bitmask of the device local memory types.
    uint32_t _mDeviceLocal;

    /// @brief A bitmask of the host visible memory types.
    uint32_t _mHostVisible;

    /// @brief A bitmask of the host coherent memory types.
    uint32_t _mHostCoherent;

    /// @brief A bitmask of the host cached memory types.
    uint32_t _mHostCached;
} CuPhysicalDeviceMemoryInfo;

/// @brief A swapchain's information.
typedef struct CuSwapchainInfo_T {
    /// @brief The swapchain handle.
    cu_vk_t(SwapchainKHR) _handle;

    /// @brief The target surface handle.
    cu_vk_t(SurfaceKHR) _surface;

    /// @brief The swapchain's format.
    CuFormat _format;

    /// @brief The swapchain's width.
    uint32_t _w;

    /// @brief The swapchain's height.
    uint32_t _h;
} CuSwapchainInfo;

/// @brief A swapchain image.
typedef struct CuSwapchainImage_T {
    /// @brief The image handle.
    cu_vk_t(Image) _image;

    /// @brief The image view handle.
    cu_vk_t(ImageView) _imageView;

    /// @brief The "render finished" semaphore handle.
    cu_vk_t(Semaphore) _renderFinished;
} CuSwapchainImage;
