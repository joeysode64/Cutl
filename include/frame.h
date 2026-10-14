#pragma once

#include "def.h"
#include "image.h"

#include <stdint.h>

/// @brief A renderer frame.
struct CuFrame_T {
    /// @brief The command buffer handle.
    cu_vk_dispatch_t(CommandBuffer) _commandBuffer;

    /// @brief The "image available" semaphore handle.
    cu_vk_t(Semaphore) _imageAvailable;
};

#ifdef __cplusplus
extern "C" {
#endif

/// @brief Starts rendering to the renderer's target.
/// @param [in, out] pFrame A pointer to the frame.
/// @param [in, out] pRenderer A pointer to the renderer.
void cu_frame_begin_render(
    CuFrame* pFrame,
    CuRenderer* pRenderer);

/// @brief Starts rendering to the image.
/// @param [in, out] pFrame A pointer to the frame.
/// @param pImage A pointer to the image.
/// @param w The image's width.
/// @param h The image's height.
/// @note The image's layout muse be color attachment optimal.
void cu_frame_begin_image_render(
    CuFrame* pFrame,
    CuImage* pImage,
    uint32_t w,
    uint32_t h);

/// @brief Ends the frame's current image render.
/// @param [in, out] pFrame A pointer to the frame.
/// @param pImage A pointer to the image.
/// @param newLayout The layout to leave the image in.
void cu_frame_end_image_render(
    CuFrame* pFrame,
    CuImage* pImage,
    CuImageLayout newLayout);

/// @brief Ends the frame's current render.
/// @param [in, out] pFrame A pointer to the frames.
/// @param [in, out] pRenderer A pointer to the renderer.
void cu_frame_end_render(
    CuFrame* pFrame,
    CuRenderer* pRenderer);

#ifdef __cplusplus
}
#endif
