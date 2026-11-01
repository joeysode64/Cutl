#pragma once

#include "color.h"
#include "command_buffer.h"
#include "pipeline.h"
#include "wrapper.hpp"

#include <vulkan/vulkan.h>

namespace cu {
    /** @brief A command buffer wrapper. */
    struct CommandBufferWrapper : public CWrapper<VkCommandBuffer> {
        /** @ref cu_cmd_bind_graphics_pipeline() */
        inline void cmd_bind_graphics_pipeline(const CuGraphicsPipeline graphicsPipeline)
        {
            cu_cmd_bind_graphics_pipeline(*this, graphicsPipeline);
        }

        /** @ref cu_cmd_draw() */
        inline void cmd_draw(
            const uint32_t nVertices,
            const uint32_t iFirstVertex,
            const uint32_t nInstances,
            const uint32_t iFirstInstance)
        {
            cu_cmd_draw(*this, nVertices, iFirstVertex, nInstances, iFirstInstance);
        }

        /** @ref cu_cmd_bind_compute_pipeline() */
        inline void cmd_bind_compute_pipeline(const CuComputePipeline computePipeline)
        {
            cu_cmd_bind_compute_pipeline(*this, computePipeline);
        }

        /** @ref cu_cmd_dispatch() */
        inline void cmd_dispatch(const uint32_t x, const uint32_t y, const uint32_t z)
        {
            cu_cmd_dispatch(*this, x, y, z);
        }

        /** @ref cu_cmd_write_push_constants() */
        inline void cmd_write_push_constants(
            const CuPipelineLayout pipelineLayout,
            const void* const p,
            const size_t z,
            const size_t o)
        {
            cu_cmd_write_push_constants(*this, pipelineLayout, p, z, o);
        }

        /** @ref cu_cmd_buffer_copy() */
        inline void cmd_buffer_copy(
            const VkBuffer dst,
            const size_t oDst,
            const VkBuffer src,
            const size_t oSrc,
            const size_t z)
        {
            cu_cmd_buffer_copy(*this, dst, oDst, src, oSrc, z);
        }

        /** @ref cu_cmd_image_barrier() */
        inline void cmd_image_barrier(
            const VkImage image,
            const VkImageAspectFlags aspectMask,
            const VkPipelineStageFlags2 srcStageMask,
            const VkAccessFlags2 srcAccessMask,
            const VkPipelineStageFlags2 dstStageMask,
            const VkAccessFlags2 dstAccessMask,
            const VkImageLayout oldLayout,
            const VkImageLayout newLayout)
        {
            cu_cmd_image_barrier(
                *this,
                image,
                aspectMask,
                srcStageMask,
                srcAccessMask,
                dstStageMask,
                dstAccessMask,
                oldLayout,
                newLayout);
        }

        /** @ref cu_cmd_image_fill() */
        inline void cmd_image_fill(
            const VkImage image,
            const VkImageLayout layout,
            const CuRGBA color)
        {
            cu_cmd_image_fill(*this, image, layout, color);
        }

        /** @ref cu_cmd_image_copy_from_buffer() */
        inline void cmd_image_copy_from_buffer(
            const VkImage image,
            const VkImageLayout layout,
            const uint32_t w,
            const uint32_t h,
            const VkBuffer src,
            const size_t oSrc)
        {
            cu_cmd_image_copy_from_buffer(*this, image, layout, w, h, src, oSrc);
        }

        /** @ref cu_cmd_bind_descriptors() */
        inline void cmd_bind_descriptors(const CuPipelineLayout pipelineLayout)
        {
            cu_cmd_bind_descriptors(*this, pipelineLayout);
        }
    };

    /** @brief A command buffer. */
    class CommandBuffer : public CommandBufferWrapper {
    public:
        /**
         * @brief Creates the command buffer.
         * @param commandBuffer The command buffer handle.
         */
        inline CommandBuffer(const VkCommandBuffer commandBuffer)
        {
            _v = commandBuffer;
        }
    };
} // namespace cu
