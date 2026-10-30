#pragma once

#include "cmd_buffer.h"
#include "color.h"
#include "pipeline.h"

#include <vulkan/vulkan.h>

namespace cu {
    /**
     * @brief Binds the graphics pipeline.
     * @param [in, out] commandBuffer The command buffer.
     * @param graphicsPipeline The graphics pipeline.
     */
    inline void cmd_bind_graphics_pipeline(
        const VkCommandBuffer commandBuffer,
        const CuGraphicsPipeline graphicsPipeline)
    {
        cu_cmd_bind_graphics_pipeline(commandBuffer, graphicsPipeline);
    }

    /**
     * @brief Runs a draw call.
     * @param [in, out] commandBuffer The command buffer.
     * @param nVertices The number of vertices.
     * @param iFirstVertex The index of the first vertex.
     * @param nInstances The number of instances.
     * @param iFirstInstance The index of the first instance.
     */
    inline void cmd_draw(
        const VkCommandBuffer commandBuffer,
        const uint32_t nVertices,
        const uint32_t iFirstVertex,
        const uint32_t nInstances,
        const uint32_t iFirstInstance)
    {
        cu_cmd_draw(commandBuffer, nVertices, iFirstVertex, nInstances, iFirstInstance);
    }

    /**
     * @brief Binds the compute pipeline.
     * @param [in, out] commandBuffer The command buffer.
     * @param computePipeline The graphics pipeline.
     */
    inline void cmd_bind_compute_pipeline(
        const VkCommandBuffer commandBuffer,
        const CuComputePipeline computePipeline)
    {
        cu_cmd_bind_compute_pipeline(commandBuffer, computePipeline);
    }

    /**
     * @brief Dispatches compute workgroups.
     * @param [in, out] commandBuffer The command buffer.
     * @param x The number of local workgroups in X.
     * @param y The number of local workgroups in Y.
     * @param z The number of local workgroups in Z.
     */
    inline void cmd_dispatch(
        const VkCommandBuffer commandBuffer,
        const uint32_t x,
        const uint32_t y,
        const uint32_t z)
    {
        cu_cmd_dispatch(commandBuffer, x, y, z);
    }

    /**
     * @brief Sets the push constants at the given range.
     * @param [in, out] commandBuffer The command buffer.
     * @param pipelineLayout The pipeline layout.
     * @param p A pointer to the push constant data to write.
     * @param z The size of the push constant data.
     * @param o The offset to write the push constants to.
     */
    inline void cmd_write_push_constants(
        const VkCommandBuffer commandBuffer,
        const CuPipelineLayout pipelineLayout,
        const void* const p,
        const size_t z,
        const size_t o)
    {
        cu_cmd_write_push_constants(commandBuffer, pipelineLayout, p, z, o);
    }

    /**
     * @brief Copies data from one buffer to another.
     * @param [in, out] commandBuffer The command buffer.
     * @param dst The destination buffer.
     * @param oDst The write offset.
     * @param src The source buffer.
     * @param oSrc The read offset.
     * @param z The size of the data to copy.
     */
    inline void cmd_buffer_copy(
        const VkCommandBuffer commandBuffer,
        const VkBuffer dst,
        const size_t oDst,
        const VkBuffer src,
        const size_t oSrc,
        const size_t z)
    {
        cu_cmd_buffer_copy(commandBuffer, dst, oDst, src, oSrc, z);
    }

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
    inline void cmd_image_barrier(
        const VkCommandBuffer commandBuffer,
        const VkImage image,
        const VkPipelineStageFlags2 srcStageMask,
        const VkAccessFlags2 srcAccessMask,
        const VkPipelineStageFlags2 dstStageMask,
        const VkAccessFlags2 dstAccessMask,
        const VkImageLayout oldLayout,
        const VkImageLayout newLayout)
    {
        cu_cmd_image_barrier(
            commandBuffer,
            image,
            srcStageMask,
            srcAccessMask,
            dstStageMask,
            dstAccessMask,
            oldLayout,
            newLayout);
    }

    /**
     * @brief Fills the image with a color.
     * @param [in, out] commandBuffer The command buffer.
     * @param [in] pImage A pointer to the image.
     * @param layout The image's current layout.
     * @param color The color to fill the image with.
     */
    inline void cmd_image_fill(
        const VkCommandBuffer commandBuffer,
        const VkImage image,
        const VkImageLayout layout,
        const CuRGBA color)
    {
        cu_cmd_image_fill(commandBuffer, image, layout, color);
    }

    /**
     * @brief Copies data from a buffer into the whole image.
     * @param [in, out] commandBuffer The command buffer.
     * @param [in] pImage A pointer to the image.
     * @param layout The image's current layout.
     * @param w The image's width.
     * @param h The image's height.
     * @param src The source buffer, holding tightly packed pixels.
     * @param oSrc The read offset.
     */
    inline void cmd_image_copy_from_buffer(
        const VkCommandBuffer commandBuffer,
        const VkImage image,
        const VkImageLayout layout,
        const uint32_t w,
        const uint32_t h,
        const VkBuffer src,
        const size_t oSrc)
    {
        cu_cmd_image_copy_from_buffer(commandBuffer, image, layout, w, h, src, oSrc);
    }

    /**
     * @brief Binds the context's image and sampler descriptors for use in shaders.
     * @param [in, out] commandBuffer The command buffer.
     * @param pipelineLayout The pipeline layout.
     * @note Sampled images are at binding 0, samplers at 1 and storage images at 2
     * of set 0.
     */
    inline void cmd_bind_descriptors(
        const VkCommandBuffer commandBuffer,
        const CuPipelineLayout pipelineLayout)
    {
        cu_cmd_bind_descriptors(commandBuffer, pipelineLayout);
    }
} // namespace cu
