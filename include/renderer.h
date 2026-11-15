#pragma once

#include "result.h"
#include "window.h"

#include <stddef.h>
#include <stdint.h>
#include <vulkan/vulkan.h>

/** @brief A swapchain image. */
typedef struct CuSwapchainImage_T {
    /** @brief The image handle. */
    VkImage _image;

    /** @brief The image view handle. */
    VkImageView _imageView;

    /** @brief The "render finished" semaphore handle. */
    VkSemaphore _renderFinished;
} CuSwapchainImage;

/** @brief A frame in flight object. */
typedef struct CuFrame_T {
    /** @brief The command buffer handle. */
    VkCommandBuffer _commandBuffer;

    /** @brief The "image available" semaphore handle. */
    VkSemaphore _imageAvailable;
} CuFrame;

/** @brief A renderer's configuration. */
typedef struct CuRendererConfig_T {
    /** @brief The target number of swapchain images. */
    uint32_t nSwapchainImages;

    /** @brief The number of MSAA samples. */
    uint32_t nMsaaSamples;

    /** @brief The preferred present mode. */
    VkPresentModeKHR preferredPresentMode;

    /** @brief Whether to create a depth image. */
    bool enableDepth;
} CuRendererConfig;

/** @brief A renderer. */
typedef struct CuRenderer_T {
    /** @brief A pointer to the swapchain images. */
    CuSwapchainImage* _pSwapchainImages;

    /** @brief The number of swapchain images. */
    uint32_t _nSwapchainImages;

    /** @brief A pointer to the frame in flight objects. */
    CuFrame* _pFramesInFlight;

    /** @brief The number of frame in flight objects. */
    uint32_t _nFramesInFlight;

    /** @brief A pointer to the window target, or null if headless. */
    const CuWindow* _pWindow;

    /** @brief The renderer's configuration. */
    CuRendererConfig _config;

    /** @brief The swapchain handle. */
    VkSwapchainKHR _swapchain;

    /** @brief The swapchain extent. */
    VkExtent2D _extent;

    /** @brief The swapchain format. */
    VkFormat _format;

    /** @brief The MSAA image handle. */
    VkImage _msaaImage;

    /** @brief The MSAA image view handle. */
    VkImageView _msaaImageView;

    /** @brief The depth image handle. */
    VkImage _depthImage;

    /** @brief The depth image view handle. */
    VkImageView _depthImageView;

    /** @brief The memory allocation shared by the MSAA and depth images. */
    VkDeviceMemory _imageMemory;

    /** @brief The surface handle. */
    VkSurfaceKHR _surface;

    /** @brief The timeline semaphore. */
    VkSemaphore _timelineSemaphore;

    /** @brief The frame counter. */
    uint64_t _frameCounter;

    /** @brief The current target swapchain image's index. */
    uint32_t _iSwapchainImage;
} CuRenderer;

/** @brief Create info for a renderer. */
typedef struct CuRendererCreateInfo_T {
    /**
     * @brief The maximum number of frames in flight.
     * @note Default is 3.
     */
    size_t maxFramesInFlight;

    /**
     * @brief The minimum number of swapchain images.
     * @note Default is 3.
     */
    size_t minSwapchainImages;

    /**
     * @brief The preferred present mode. Falls back to `VK_PRESENT_MODE_FIFO_KHR` if unsupported.
     * @note Default is `VK_PRESENT_MODE_FIFO_KHR`.
     */
    VkPresentModeKHR preferredPresentMode;

    /**
     * @brief The number of MSAA samples.
     * @note Default is 1 (no anti-aliasing).
     */
    uint32_t nMsaaSamples;

    /**
     * @brief Whether to create a depth image.
     * @note Default is false.
     */
    bool enableDepth;
} CuRendererCreateInfo;

/** @brief The default renderer create info. */
constexpr CuRendererCreateInfo CU_DEFAULT_RENDERER_CREATE_INFO = {
    .maxFramesInFlight = 3,
    .minSwapchainImages = 3,
    .preferredPresentMode = VK_PRESENT_MODE_FIFO_KHR,
    .nMsaaSamples = 1,
    .enableDepth = false,
};

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Creates the renderer.
 * @param[out] pRenderer A pointer to the renderer. Must not be null.
 * @param pCreateInfo A pointer to the create info. Must not be null.
 * @param pWindow A pointer to the window target. If null, the renderer is headless. Must outlive
 * the renderer.
 * @note On failure, the renderer is destroyed automatically.
 */
CU_NODISCARD_RESULT
CuResult cu_renderer_create(
    CuRenderer* pRenderer,
    const CuRendererCreateInfo* pCreateInfo,
    const CuWindow* pWindow);

/**
 * @brief Destroys the renderer.
 * @param[in out] pRenderer A pointer to the renderer. Ignored if null.
 */
void cu_renderer_destroy(CuRenderer* pRenderer);

/**
 * @brief Recreates the renderer's swapchain.
 * @param[in out] pRenderer A pointer to the renderer. Must not be null.
 * @note This function is called internally as needed. Only call manually when you change a
 * configuration.
 * @warning This function should not be called between a frame's start and submission.
 */
CU_NODISCARD_RESULT
CuResult cu_renderer_recreate_swapchain(CuRenderer* pRenderer);

/**
 * @brief Begins the next frame, starting and setting the command buffer.
 * @param[in out] pRenderer A pointer to the renderer. Must not be null.
 * @param[out] pCommandBuffer A pointer to the frame's command buffer handle. Must not be null.
 * @note If the swapchain has to be recreated, the command buffer is set to null and the frame must
 * be skipped.
 */
CU_NODISCARD_RESULT
CuResult cu_renderer_begin_frame(CuRenderer* pRenderer, VkCommandBuffer* pCommandBuffer);

/**
 * @brief Submits the frame.
 * @param[in out] pRenderer A pointer to the renderer. Must not be null.
 * @warning Must be called after `cu_renderer_begin_frame`.
 * @warning The frame's command buffer ends and cannot be used after calling this function.
 */
CU_NODISCARD_RESULT
CuResult cu_renderer_submit_frame(CuRenderer* pRenderer);

/**
 * @brief Begins rendering with the given command buffer.
 * @param[in] pRenderer A pointer to the renderer. Must not be null.
 * @param commandBuffer The frame's command buffer from `cu_renderer_begin_frame`.
 */
void cu_renderer_start_rendering(const CuRenderer* pRenderer, VkCommandBuffer commandBuffer);

/**
 * @brief Ends rendering with the given command buffer.
 * @param[in] pRenderer A pointer to the renderer. Must not be null.
 * @param commandBuffer The command buffer `cu_renderer_start_rendering` was called with.
 */
void cu_renderer_end_rendering(const CuRenderer* pRenderer, VkCommandBuffer commandBuffer);

/**
 * @brief Returns a pointer to the renderer's config.
 * @param[in out] pRenderer A pointer to the renderer. Must not be null.
 * @note Changing any values will require recreating the swapchain.
 */
inline static CuRendererConfig* cu_renderer_get_config(CuRenderer* pRenderer)
{
    return &pRenderer->_config;
}

/**
 * @brief Returns the renderer's swapchain's extent.
 * @param[in] pRenderer A pointer to the renderer. Must not be null.
 */
inline static VkExtent2D cu_renderer_get_extent(const CuRenderer* pRenderer)
{
    return pRenderer->_extent;
}

/**
 * @brief Returns the renderer's target image format.
 * @param[in] pRenderer A pointer to the renderer. Must not be null.
 */
inline static VkFormat cu_renderer_get_format(const CuRenderer* pRenderer)
{
    return pRenderer->_format;
}

/**
 * @brief Returns the renderer's frame index.
 * @param[in] pRenderer A pointer to the renderer. Must not be null.
 */
inline static uint32_t cu_renderer_get_frame_index(const CuRenderer* pRenderer)
{
    return (uint32_t)pRenderer->_frameCounter % pRenderer->_nFramesInFlight;
}

/**
 * @brief Returns a pointer to the renderer's current target frame.
 * @param[in] pRenderer A pointer to the renderer. Must not be null.
 */
inline static CuFrame* cu_renderer_get_current_frame(const CuRenderer* pRenderer)
{
    return &pRenderer->_pFramesInFlight[cu_renderer_get_frame_index(pRenderer)];
}

#ifdef __cplusplus
}
#endif
