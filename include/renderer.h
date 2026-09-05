#pragma once

#include "frame.h"
#include "inner.h"
#include "result.h"
#include "window.h"

#include <stddef.h>

/** @brief A renderer. */
typedef struct {
    CuSwapchainInfo _swapchainInfo; /**< The swapchain info. */

    CuSwapchainImage* _pSwapchainImages; /**< A pointer to the swapchain images. */

    uint32_t _nSwapchainImages; /**< The number of swapchain images. */

    struct CuFrame_T* _pFramesInFlight; /**< A pointer to the frame in flight objects. */

    uint32_t _nFramesInFlight; /**< The number of frame in flight objects. */

    void* _pData; /**< A pointer to the allocated data. */
} CuRenderer;

/** @brief Create info for a renderer. */
typedef struct {
    size_t maxFramesInFlight; /**< The maximum number of frames in flight. Default is 3. */

    size_t minSwapchainImages; /**< The minimum number of swapchain images. Default is 3. */
} CuRendererCreateInfo;

/** @brief The default renderer create info. */
constexpr CuRendererCreateInfo CU_DEFAULT_RENDERER_CREATE_INFO = {
    .maxFramesInFlight = 3,
    .minSwapchainImages = 3,
};

/**
 * @brief Creates the renderer.
 *
 * @param [in] pRenderer A pointer to the renderer.
 * @param pCreateInfo A pointer to the create info, or a nullpointer for default.
 * @param pWindow A pointer to the window target.
 *
 * @return The result of creating the renderer.
 */
CuResult cu_renderer_create(
    CuRenderer* pRenderer,
    const CuRendererCreateInfo* pCreateInfo,
    const CuWindow* pWindow);

/** 
 * @brief Destroys the renderer.
 *
 * @param [in, out] pRenderer A pointer to the renderer.
 *
 * @note Passing a null pointer is a safe no-op.
 */
void cu_renderer_destroy(
    CuRenderer* pRenderer);
