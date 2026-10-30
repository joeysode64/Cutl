#pragma once

#include "result.h"
#include "window.h"

#include <stddef.h>
#include <stdint.h>
#include <vulkan/vulkan.h>

/** @brief A swapchain image. */
typedef struct CuSwapchainImage_T {
    VkImage _image; /**< The image handle. */

    VkImageView _imageView; /**< The image view handle. */

    VkSemaphore _renderFinished; /**< The "render finished" semaphore handle. */
} CuSwapchainImage;

/** @brief A frame in flight object. */
typedef struct CuFrame_T {
    VkCommandBuffer _commandBuffer; /**< The command buffer handle. */

    VkSemaphore _imageAvailable; /**< The "image available" semaphore handle. */
} CuFrame;

/** @brief A renderer. */
typedef struct CuRenderer_T {
    const CuWindow* _pWindow; /**< A pointer to the target window. */

    uint32_t _minSwapchainImages; /**< The minimum number of swapchain images to
                                     create. */

    VkPresentModeKHR _preferredPresentMode; /**< The preferred present mode. */

    CuSwapchainImage* _pSwapchainImages; /**< A pointer to the swapchain images. */

    uint32_t _nSwapchainImages; /**< The number of swapchain images. */

    CuFrame* _pFramesInFlight; /**< A pointer to the frame in flight objects. */

    uint32_t _nFramesInFlight; /**< The number of frame in flight objects. */

    VkSurfaceKHR _surface; /**< The surface handle. */

    VkSwapchainKHR _swapchain; /**< The swapchain handle. */

    VkExtent2D _swapchainExtent; /**< The swapchain's extent. */

    VkFormat _swapchainFormat; /**< The swapchain format. */

    VkSemaphore _timelineSemaphore; /**< The timeline semaphore. */

    uint64_t _frameCounter; /**< The frame counter. */

    uint32_t _iSwapchainImage; /**< The current target swapchain image's index. */
} CuRenderer;

/** @brief Create info for a renderer. */
typedef struct CuRendererCreateInfo_T {
    size_t maxFramesInFlight; /**< The maximum number of frames in flight. Default
                                 is 3. */

    size_t minSwapchainImages; /**< The minimum number of swapchain images.
                                  Default is 3. */

    VkPresentModeKHR preferredPresentMode; /**< The preferred present mode. Default is
                                              `VK_PRESENT_MODE_FIFO_KHR`, which is also the
                                              fallback. */
} CuRendererCreateInfo;

/** @brief The default renderer create info. */
constexpr CuRendererCreateInfo CU_DEFAULT_RENDERER_CREATE_INFO = {
    .maxFramesInFlight = 3,
    .minSwapchainImages = 3,
    .preferredPresentMode = VK_PRESENT_MODE_FIFO_KHR,
};

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Creates the renderer.
 * @param [in] pRenderer A pointer to the renderer.
 * @param pCreateInfo A pointer to the create info.
 * @param pWindow A pointer to the window target.
 * @return The result of creating the renderer.
 */
CuResult cu_renderer_create(
    CuRenderer* pRenderer,
    const CuRendererCreateInfo* pCreateInfo,
    const CuWindow* pWindow);

/**
 * @brief Destroys the renderer.
 * @param [in, out] pRenderer A pointer to the renderer.
 * @note Passing a null pointer is a safe no-op.
 */
void cu_renderer_destroy(CuRenderer* pRenderer);

/**
 * @brief Begins the next frame, starting and setting the command buffer.
 * @param [in, out] pRenderer A pointerer to the renderer.
 * @param [out] pCommandBuffer A pointer to the command buffer handle.
 * @return The result of beginning the frame.
 * @note If the swapchain has to be recreated, the command buffer is set to null and the frame must
 * be skipped.
 */
CuResult cu_renderer_begin_frame(CuRenderer* pRenderer, VkCommandBuffer* pCommandBuffer);

/**
 * @brief Submits the frame.
 * @param [in, out] pRenderer A pointerer to the renderer.
 * @return The result of submitting the frame.
 * @warning Must be called after `cu_renderer_begin_frame`.
 * @warning The frame's command buffer ends and cannot be used after calling this function.
 */
CuResult cu_renderer_submit_frame(CuRenderer* pRenderer);

/**
 * @brief Begins rendering with the given command buffer.
 * @param [in] pRenderer A pointer to the renderer.
 * @param commandBuffer The command buffer handle.
 */
void cu_renderer_start_rendering(const CuRenderer* pRenderer, VkCommandBuffer commandBuffer);

/**
 * @brief Ends rendering with the given command buffer.
 * @param [in] pRenderer A pointer to the renderer.
 * @param commandBuffer The command buffer handle.
 */
void cu_renderer_end_rendering(const CuRenderer* pRenderer, VkCommandBuffer commandBuffer);

/**
 * @brief Returns the renderer's swapchain's extent.
 * @param [in] pRenderer A pointer to the renderer.
 * @return The renderer's swapchain's extent..
 */
inline static VkExtent2D cu_renderer_get_extent(const CuRenderer* pRenderer)
{
    return pRenderer->_swapchainExtent;
}

/**
 * @brief Returns the renderer's target image format.
 * @param [in] pRenderer A pointer to the renderer.
 * @return The renderer's target image format.
 */
inline static VkFormat cu_renderer_get_format(const CuRenderer* pRenderer)
{
    return pRenderer->_swapchainFormat;
}

/**
 * @brief Returns the renderer's frame index.
 * @param [in] pRenderer A pointer to the renderer.
 * @return The renderer's current frame index.
 */
inline static uint32_t cu_renderer_get_frame_index(const CuRenderer* pRenderer)
{
    return (uint32_t)pRenderer->_frameCounter % pRenderer->_nFramesInFlight;
}

/**
 * @brief Returns a pointer to the renderer's current target frame.
 * @param [in] pRenderer A pointer to the renderer.
 */
inline static CuFrame* cu_renderer_get_current_frame(const CuRenderer* pRenderer)
{
    return &pRenderer->_pFramesInFlight[cu_renderer_get_frame_index(pRenderer)];
}

#ifdef __cplusplus
}
#endif
