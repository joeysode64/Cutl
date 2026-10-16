#pragma once

#include "result.h"
#include "swapchain.h"
#include "window.h"

#include <stddef.h>
#include <stdint.h>
#include <vulkan/vulkan.h>

typedef struct CuFrame_T CuFrame;

typedef struct CuRenderer_T CuRenderer;

/** @brief A renderer. */
struct CuRenderer_T {
    void* _pData; /**< A pointer to the allocated data. */

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
};

/** @brief Create info for a renderer. */
typedef struct CuRendererCreateInfo_T {
    size_t maxFramesInFlight; /**< The maximum number of frames in flight. Default is 3. */

    size_t minSwapchainImages; /**< The minimum number of swapchain images. Default is 3. */
} CuRendererCreateInfo;

/** @brief The default renderer create info. */
constexpr CuRendererCreateInfo CU_DEFAULT_RENDERER_CREATE_INFO = {
    .maxFramesInFlight = 3,
    .minSwapchainImages = 3,
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
void cu_renderer_destroy(
    CuRenderer* pRenderer);

/**
 * @brief Begins the next frame.
 * @param [in, out] pRenderer A pointerer to the renderer.
 * @param [out] ppFrame A double pointer to the frame.
 * @return The result of beginning the frame.
 */
CuResult cu_renderer_begin_frame(
    CuRenderer* pRenderer,
    CuFrame** ppFrame);

/**
 * @brief Submits the frame.
 * @param [in, out] pRenderer A pointerer to the renderer.
 * @param [out] pFrame A pointer to the frame.
 * @return The result of submitting the frame.
 * @warning The frame becoems unusable as left.
 */
CuResult cu_renderer_submit_frame(
    CuRenderer* pRenderer,
    CuFrame* pFrame);

/**
 * @brief Returns the renderer's swapchain's extent.
 * @param [in] pRenderer A pointer to the renderer.
 * @return The renderer's swapchain's extent..
 */
inline static VkExtent2D cu_renderer_get_extent(
    const CuRenderer* pRenderer)
{
    return pRenderer->_swapchainExtent;
}

/**
 * @brief Returns the renderer's target image format.
 * @param [in] pRenderer A pointer to the renderer.
 * @return The renderer's target image format.
 */
inline static VkFormat cu_renderer_get_format(
    const CuRenderer* pRenderer)
{
    return pRenderer->_swapchainFormat;
}

/**
 * @brief Returns the renderer's frame index.
 * @param [in] pRenderer A pointer to the renderer.
 * @return The renderer's current frame index.
 */
inline static uint32_t cu_renderer_get_frame_index(
    const CuRenderer *pRenderer)
{
    return (uint32_t)pRenderer->_frameCounter % pRenderer->_nFramesInFlight;
}

#ifdef __cplusplus
}
#endif
