#pragma once

#include "color.h"
#include "image.h"
#include "pipeline.h"

#include <stddef.h>
#include <stdint.h>
#include <vulkan/vulkan.h>
#include <vulkan/vulkan_core.h>

/**
 * @brief A command buffer.
 * @note This is any type that can be dereferenced as a `VkCommandBuffer`.
 */
typedef void* CuCmdBuffer;

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Binds the graphics pipeline.
 * @param [in, out] commandBuffer The command buffer.
 * @param graphicsPipeline The graphics pipeline.
 */
void cu_cmd_bind_graphics_pipeline(
    CuCmdBuffer commandBuffer,
    CuGraphicsPipeline graphicsPipeline);

/**
 * @brief Runs a draw call.
 * @param [in, out] commandBuffer The command buffer.
 * @param nVertices The number of vertices.
 * @param iFirstVertex The index of the first vertex.
 * @param nInstances The number of instances.
 * @param iFirstInstance The index of the first instance.
 */
void cu_cmd_draw(
    CuCmdBuffer commandBuffer,
    uint32_t nVertices,
    uint32_t iFirstVertex,
    uint32_t nInstances,
    uint32_t iFirstInstance);

/**
 * @brief Binds the compute pipeline.
 * @param [in, out] commandBuffer The command buffer.
 * @param computePipeline The graphics pipeline.
 */
void cu_cmd_bind_compute_pipeline(
    CuCmdBuffer commandBuffer,
    CuComputePipeline computePipeline);

/**
 * @brief Dispatches compute workgroups.
 * @param [in, out] commandBuffer The command buffer.
 * @param x The number of local workgroups in X.
 * @param y The number of local workgroups in Y.
 * @param z The number of local workgroups in Z.
 */
void cu_cmd_dispatch(
    CuCmdBuffer commandBuffer,
    uint32_t x,
    uint32_t y,
    uint32_t z);

/**
 * @brief Sets the push constants at the given range.
 * @param [in, out] commandBuffer The command buffer.
 * @param pipelineLayout The pipeline layout.
 * @param p A pointer to the push constant data to write.
 * @param z The size of the push constant data.
 * @param o The offset to write the push constants to.
 */
void cu_cmd_write_push_constants(
    CuCmdBuffer commandBuffer,
    CuPipelineLayout pipelineLayout,
    const void* p,
    size_t z,
    size_t o);

/**
 * @brief Copies data from one buffer to another.
 * @param [in, out] commandBuffer The command buffer.
 * @param dst The destination buffer.
 * @param oDst The write offset.
 * @param src The source buffer.
 * @param oSrc The read offset.
 * @param z The size of the data to copy.
 */
void cu_cmd_buffer_copy(
    CuCmdBuffer commandBuffer,
    VkBuffer dst,
    size_t oDst,
    VkBuffer src,
    size_t oSrc,
    size_t z);

/**
 * @brief Transitions the image from one layout to another.
 * @param [in, out] commandBuffer The command buffer.
 * @param [in] pImage A pointer to the image.
 * @param srcStageMask The pipeline stages that last used the image.
 * @param srcAccessMask How the image was last accessed.
 * @param dstStageMask The pipeline stages that will use the image next.
 * @param dstAccessMask How the image will be accessed next.
 * @param oldLayout The image's current layout.
 * @param newLayout The layout to transition the image to.
 */
void cu_cmd_image_barrier(
    CuCmdBuffer commandBuffer,
    const CuImage* pImage,
    VkPipelineStageFlags2 srcStageMask,
    VkAccessFlags2 srcAccessMask,
    VkPipelineStageFlags2 dstStageMask,
    VkAccessFlags2 dstAccessMask,
    VkImageLayout oldLayout,
    VkImageLayout newLayout);

/**
 * @brief Fills the image with a color.
 * @param [in, out] commandBuffer The command buffer.
 * @param [in] pImage A pointer to the image.
 * @param layout The image's current layout.
 * @param color The color to fill the image with.
 */
void cu_cmd_image_fill(
    CuCmdBuffer commandBuffer,
    const CuImage* pImage,
    VkImageLayout layout,
    CuRGBA color);

/**
 * @brief Binds the context's image and sampler descriptors for use in shaders.
 * @param [in, out] commandBuffer The command buffer.
 * @param pipelineLayout The pipeline layout.
 * @note Sampled images are at binding 0, samplers at 1 and storage images at 2 of set 0.
 */
void cu_cmd_bind_descriptors(
    CuCmdBuffer commandBuffer,
    CuPipelineLayout pipelineLayout);

#ifdef __cplusplus
}
#endif
