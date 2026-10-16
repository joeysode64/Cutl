#include "cmd_buffer.h"

#include "color.h"
#include "g_context.h"
#include "image.h"
#include "pipeline.h"

#include <stdint.h>
#include <vulkan/vulkan.h>
#include <vulkan/vulkan_core.h>

/** @return The dereferenced command buffer. */
#define deref(p) (*(VkCommandBuffer*)p)

void cu_cmd_bind_graphics_pipeline(
    const CuCmdBuffer commandBuffer,
    const CuGraphicsPipeline graphicsPipeline)
{
    vkCmdBindPipeline(
        deref(commandBuffer), VK_PIPELINE_BIND_POINT_GRAPHICS, (VkPipeline)graphicsPipeline
    );
}

void cu_cmd_draw(
    const CuCmdBuffer commandBuffer,
    const uint32_t nVertices,
    const uint32_t iFirstVertex,
    const uint32_t nInstances,
    const uint32_t iFirstInstance)
{
    vkCmdDraw(deref(commandBuffer), nVertices, nInstances, iFirstVertex, iFirstInstance);
}

void cu_cmd_bind_compute_pipeline(
    const CuCmdBuffer commandBuffer,
    const CuComputePipeline computePipeline)
{
    vkCmdBindPipeline(
        deref(commandBuffer), VK_PIPELINE_BIND_POINT_COMPUTE, (VkPipeline)computePipeline
    );
}

void cu_cmd_dispatch(
    const CuCmdBuffer commandBuffer,
    const uint32_t x,
    const uint32_t y,
    const uint32_t z)
{
    vkCmdDispatch(deref(commandBuffer), x, y, z);
}

void cu_cmd_write_push_constants(
    const CuCmdBuffer commandBuffer,
    const CuPipelineLayout pipelineLayout,
    const void* const p,
    const size_t z,
    const size_t o)
{
    vkCmdPushConstants(
        deref(commandBuffer), (VkPipelineLayout)pipelineLayout, VK_SHADER_STAGE_ALL, o, z, p
    );
}

void cu_cmd_buffer_copy(
    CuCmdBuffer commandBuffer,
    VkBuffer dst,
    size_t oDst,
    VkBuffer src,
    size_t oSrc,
    size_t z)
{
    const VkBufferCopy2 region = {
        .sType = VK_STRUCTURE_TYPE_BUFFER_COPY_2,
        .pNext = nullptr,
        .srcOffset = oSrc,
        .dstOffset = oDst,
        .size = z,
    };
    const VkCopyBufferInfo2 copyInfo = {
        .sType = VK_STRUCTURE_TYPE_COPY_BUFFER_INFO_2,
        .pNext = nullptr,
        .srcBuffer = src,
        .dstBuffer = dst,
        .regionCount = 1,
        .pRegions = &region,
    };
    vkCmdCopyBuffer2(deref(commandBuffer), &copyInfo);
}

void cu_cmd_image_barrier(
    const CuCmdBuffer commandBuffer,
    const CuImage* const pImage,
    const VkPipelineStageFlags2 srcStageMask,
    const VkAccessFlags2 srcAccessMask,
    const VkPipelineStageFlags2 dstStageMask,
    const VkAccessFlags2 dstAccessMask,
    const VkImageLayout oldLayout,
    const VkImageLayout newLayout)
{
    const VkImageSubresourceRange subresourceRange = {
        .aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,
        .baseMipLevel = 0,
        .levelCount = 1,
        .baseArrayLayer = 0,
        .layerCount = 1,
    };
    const VkImageMemoryBarrier2 imageBarrier = {
        .sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER_2,
        .pNext = nullptr,
        .srcStageMask = srcStageMask,
        .srcAccessMask = srcAccessMask,
        .dstStageMask = dstStageMask,
        .dstAccessMask = dstAccessMask,
        .oldLayout = oldLayout,
        .newLayout = newLayout,
        .srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
        .dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
        .image = pImage->_image,
        .subresourceRange = subresourceRange,
    };
    const VkDependencyInfo dependencyInfo = {
        .sType = VK_STRUCTURE_TYPE_DEPENDENCY_INFO,
        .pNext = nullptr,
        .dependencyFlags = 0,
        .memoryBarrierCount = 0,
        .pMemoryBarriers = nullptr,
        .bufferMemoryBarrierCount = 0,
        .pBufferMemoryBarriers = nullptr,
        .imageMemoryBarrierCount = 1,
        .pImageMemoryBarriers = &imageBarrier,
    };
    vkCmdPipelineBarrier2(deref(commandBuffer), &dependencyInfo);
}

void cu_cmd_image_fill(
    const CuCmdBuffer commandBuffer,
    const CuImage* const pImage,
    const VkImageLayout layout,
    const CuRGBA color)
{
    const VkClearColorValue clearColor = {
        .float32 = {
            (float)color.r / 255.0F,
            (float)color.g / 255.0F,
            (float)color.b / 255.0F,
            (float)color.a / 255.0F,
        },
    };
    const VkImageSubresourceRange subresourceRange = {
        .aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,
        .baseMipLevel = 0,
        .levelCount = 1,
        .baseArrayLayer = 0,
        .layerCount = 1,
    };
    vkCmdClearColorImage(
        deref(commandBuffer),
        pImage->_image,
        (VkImageLayout)layout,
        &clearColor,
        1,
        &subresourceRange
    );
}

void cu_cmd_bind_descriptors(
    const CuCmdBuffer commandBuffer,
    const CuPipelineLayout pipelineLayout)
{
    const VkPipelineBindPoint bindPoints[] = {
        VK_PIPELINE_BIND_POINT_GRAPHICS,
        VK_PIPELINE_BIND_POINT_COMPUTE,
    };
    for (size_t i = 0; i < sizeof(bindPoints) / sizeof(bindPoints[0]); i++) {
        vkCmdBindDescriptorSets(
            deref(commandBuffer),
            bindPoints[i],
            (VkPipelineLayout)pipelineLayout,
            0,
            1,
            &gContext._descriptorSet,
            0,
            nullptr
        );
    }
}
