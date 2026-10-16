#pragma once

#include "image.h"

#include <stdint.h>
#include <vulkan/vulkan.h>

typedef struct CuFrame_T CuFrame;

typedef struct CuRenderer_T CuRenderer;

/** @brief A renderer frame. */
struct CuFrame_T {
    VkCommandBuffer _commandBuffer; /**< The command buffer handle. */

    VkSemaphore _imageAvailable; /**< The "image available" semaphore handle. */
};

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Starts rendering to the renderer's target.
 * @param [in, out] pFrame A pointer to the frame.
 * @param [in, out] pRenderer A pointer to the renderer.
 */
void cu_frame_begin_render(
    CuFrame* pFrame,
    CuRenderer* pRenderer);

/**
 * @brief Starts rendering to the image.
 * @param [in, out] pFrame A pointer to the frame.
 * @param pImage A pointer to the image.
 * @param w The image's width.
 * @param h The image's height.
 * @note The image's layout muse be color attachment optimal.
 */
void cu_frame_begin_image_render(
    CuFrame* pFrame,
    CuImage* pImage,
    uint32_t w,
    uint32_t h);

/**
 * @brief Ends the frame's current image render.
 * @param [in, out] pFrame A pointer to the frame.
 * @param pImage A pointer to the image.
 * @param newLayout The layout to leave the image in.
 */
void cu_frame_end_image_render(
    CuFrame* pFrame,
    CuImage* pImage,
    VkImageLayout newLayout);

/**
 * @brief Ends the frame's current render.
 * @param [in, out] pFrame A pointer to the frames.
 * @param [in, out] pRenderer A pointer to the renderer.
 */
void cu_frame_end_render(
    CuFrame* pFrame,
    CuRenderer* pRenderer);

#ifdef __cplusplus
}
#endif
