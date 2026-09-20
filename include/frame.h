#pragma once

#include "inner.h"

/// @brief A renderer frame.
struct CuFrame_T {
    /// @brief The command buffer handle.
    cu_vk_dispatch_t(CommandBuffer) _commandBuffer;

    /// @brief The "image available" semaphore handle.
    cu_vk_t(Semaphore) _imageAvailable;
};

/// @brief Starts rendering to the renderer's target.
/// @param [in, out] pFrame A pointer to the frame.
/// @param [in, out] pRenderer A pointer to the renderer.
void cu_frame_begin_render(
    CuFrame* pFrame,
    CuRenderer* pRenderer);

/// @brief Ends the frame's current render.
/// @param [in, out] pFrame A pointer to the frames.
/// @param [in, out] pRenderer A pointer to the renderer.
void cu_frame_end_render(
    CuFrame* pFrame,
    CuRenderer* pRenderer);
