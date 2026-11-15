#pragma once

#include "color.h"
#include "pipeline.h"

#include <stddef.h>
#include <stdint.h>
#include <vulkan/vulkan.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Binds the graphics pipeline.
 * @param[in out] commandBuffer The command buffer to record to. Must be in the recording state.
 * @param graphicsPipeline The graphics pipeline to bind.
 */
void cu_cmd_bind_graphics_pipeline(
    VkCommandBuffer commandBuffer,
    CuGraphicsPipeline graphicsPipeline);

/**
 * @brief Runs a draw call.
 * @param[in out] commandBuffer The command buffer to record to. Must be in the recording state.
 * @param nVertices The number of vertices to draw.
 * @param iFirstVertex The index of the first vertex to draw.
 * @param nInstances The number of instances to draw. Use 1 for non-instanced drawing.
 * @param iFirstInstance The index of the first instance to draw.
 */
void cu_cmd_draw(
    VkCommandBuffer commandBuffer,
    uint32_t nVertices,
    uint32_t iFirstVertex,
    uint32_t nInstances,
    uint32_t iFirstInstance);

/**
 * @brief Binds the compute pipeline.
 * @param[in out] commandBuffer The command buffer to record to. Must be in the recording state.
 * @param computePipeline The compute pipeline to bind.
 */
void cu_cmd_bind_compute_pipeline(VkCommandBuffer commandBuffer, CuComputePipeline computePipeline);

/**
 * @brief Dispatches compute workgroups.
 * @param[in out] commandBuffer The command buffer to record to. Must be in the recording state.
 * @param x The number of local workgroups in X.
 * @param y The number of local workgroups in Y.
 * @param z The number of local workgroups in Z.
 */
void cu_cmd_dispatch(VkCommandBuffer commandBuffer, uint32_t x, uint32_t y, uint32_t z);

/**
 * @brief Sets the push constants at the given range.
 * @param[in out] commandBuffer The command buffer to record to. Must be in the recording state.
 * @param pipelineLayout The layout of the pipeline the push constants are used by.
 * @param p A pointer to the push constant data to write. Must not be null.
 * @param z The size of the push constant data in bytes. Must be a multiple of 4.
 * @param o The offset to write the push constants to in bytes. Must be a multiple of 4.
 */
void cu_cmd_write_push_constants(
    VkCommandBuffer commandBuffer,
    CuPipelineLayout pipelineLayout,
    const void* p,
    size_t z,
    size_t o);

/**
 * @brief Copies data from one buffer to another.
 * @param[in out] commandBuffer The command buffer to record to. Must be in the recording state.
 * @param dst The buffer to copy to.
 * @param oDst The offset into the destination buffer in bytes.
 * @param src The buffer to copy from.
 * @param oSrc The offset into the source buffer in bytes.
 * @param z The size of the data to copy in bytes. Must not be 0.
 */
void cu_cmd_buffer_copy(
    VkCommandBuffer commandBuffer,
    VkBuffer dst,
    size_t oDst,
    VkBuffer src,
    size_t oSrc,
    size_t z);

/**
 * @brief Transitions the image from one layout to another.
 * @param[in out] commandBuffer The command buffer to record to. Must be in the recording state.
 * @param image The image to transition.
 * @param aspectMask A bitmap of the image aspects to transition.
 * @param srcStageMask The pipeline stages that last used the image.
 * @param srcAccessMask How the image was last accessed.
 * @param dstStageMask The pipeline stages that will use the image next.
 * @param dstAccessMask How the image will be accessed next.
 * @param oldLayout The image's current layout.
 * @param newLayout The layout to transition the image to.
 */
void cu_cmd_image_barrier(
    VkCommandBuffer commandBuffer,
    VkImage image,
    VkImageAspectFlags aspectMask,
    VkPipelineStageFlags2 srcStageMask,
    VkAccessFlags2 srcAccessMask,
    VkPipelineStageFlags2 dstStageMask,
    VkAccessFlags2 dstAccessMask,
    VkImageLayout oldLayout,
    VkImageLayout newLayout);

/**
 * @brief Fills the image with a color.
 * @param[in out] commandBuffer The command buffer to record to. Must be in the recording state.
 * @param image The image to fill.
 * @param layout The image's current layout. Must be `VK_IMAGE_LAYOUT_GENERAL` or
 * `VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL`.
 * @param color The color to fill the image with.
 */
void cu_cmd_image_fill(
    VkCommandBuffer commandBuffer,
    VkImage image,
    VkImageLayout layout,
    CuRGBA color);

/**
 * @brief Copies data from a buffer into the whole image.
 * @param[in out] commandBuffer The command buffer to record to. Must be in the recording state.
 * @param image The image to copy to.
 * @param layout The image's current layout. Must be `VK_IMAGE_LAYOUT_GENERAL` or
 * `VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL`.
 * @param w The image's width in pixels.
 * @param h The image's height in pixels.
 * @param src The buffer to copy from, holding tightly packed pixels.
 * @param oSrc The offset into the source buffer in bytes.
 */
void cu_cmd_image_copy_from_buffer(
    VkCommandBuffer commandBuffer,
    VkImage image,
    VkImageLayout layout,
    uint32_t w,
    uint32_t h,
    VkBuffer src,
    size_t oSrc);

/**
 * @brief Binds the context's image and sampler descriptors for use in shaders.
 * @param[in out] commandBuffer The command buffer to record to. Must be in the recording state.
 * @param pipelineLayout The layout of the pipeline the descriptors are used by.
 * @note Sampled images are at binding 0, samplers at 1 and storage images at 2
 * of set 0.
 */
void cu_cmd_bind_descriptors(VkCommandBuffer commandBuffer, CuPipelineLayout pipelineLayout);

#ifdef __cplusplus
}
#endif
