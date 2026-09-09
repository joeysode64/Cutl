#include "frame.h"

#include "renderer.h"

#include <vulkan/vulkan.h>

void cu_frame_begin_render(CuFrame* const pFrame, CuRenderer* const pRenderer) {
    const VkRect2D area = {
        .offset =
            {
                .x = 0,
                .y = 0,
            },
        .extent =
            {
                .width = pRenderer->_swapchainInfo._w,
                .height = pRenderer->_swapchainInfo._h,
            },
    };

    // Set the viewport and scissor.
    const VkViewport viewport = {
        .x = 0.0F,
        .y = 0.0F,
        .width = (float)area.extent.width,
        .height = (float)area.extent.height,
        .minDepth = 0.0F,
        .maxDepth = 1.0F,
    };
    vkCmdSetViewport(pFrame->_commandBuffer, 0, 1, &viewport);
    vkCmdSetScissor(pFrame->_commandBuffer, 0, 1, &area);

    // Convert image for rendering.
    const VkImageSubresourceRange subresourceRange = {
        .aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,
        .baseMipLevel = 0,
        .levelCount = 1,
        .baseArrayLayer = 0,
        .layerCount = 1,
    };
    CuSwapchainImage* const pSwapchainImage =
        &pRenderer->_pSwapchainImages[pRenderer->_iSwapchainImage];
    const VkImageMemoryBarrier2 imageBarrier = {
        .sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER_2,
        .pNext = NULL,
        .srcStageMask = VK_PIPELINE_STAGE_2_NONE,
        .srcAccessMask = VK_ACCESS_2_NONE,
        .dstStageMask = VK_PIPELINE_STAGE_2_COLOR_ATTACHMENT_OUTPUT_BIT,
        .dstAccessMask = VK_ACCESS_2_COLOR_ATTACHMENT_WRITE_BIT,
        .oldLayout = VK_IMAGE_LAYOUT_UNDEFINED,
        .newLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
        .srcQueueFamilyIndex = 0,
        .dstQueueFamilyIndex = 0,
        .image = (VkImage)pSwapchainImage->_image,
        .subresourceRange = subresourceRange,
    };
    const VkDependencyInfo dependencyInfo = {
        .sType = VK_STRUCTURE_TYPE_DEPENDENCY_INFO,
        .pNext = NULL,
        .dependencyFlags = 0,
        .memoryBarrierCount = 0,
        .pMemoryBarriers = NULL,
        .bufferMemoryBarrierCount = 0,
        .pBufferMemoryBarriers = NULL,
        .imageMemoryBarrierCount = 1,
        .pImageMemoryBarriers = &imageBarrier,
    };
    vkCmdPipelineBarrier2(pFrame->_commandBuffer, &dependencyInfo);

    // Begin rendering.
    const VkClearValue clearValue = {
        .color.float32 = {0.0F, 0.0F, 0.0F, 1.0F},
    };
    const VkRenderingAttachmentInfo colorAttachment = {
        .sType = VK_STRUCTURE_TYPE_RENDERING_ATTACHMENT_INFO,
        .pNext = NULL,
        .imageView = (VkImageView)pSwapchainImage->_imageView,
        .imageLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
        .resolveMode = VK_RESOLVE_MODE_NONE,
        .resolveImageView = VK_NULL_HANDLE,
        .resolveImageLayout = VK_IMAGE_LAYOUT_UNDEFINED,
        .loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR,
        .storeOp = VK_ATTACHMENT_STORE_OP_STORE,
        .clearValue = clearValue,
    };
    const VkRenderingInfo renderingInfo = {
        .sType = VK_STRUCTURE_TYPE_RENDERING_INFO,
        .pNext = NULL,
        .flags = 0,
        .renderArea = area,
        .layerCount = 1,
        .viewMask = 0,
        .colorAttachmentCount = 1,
        .pColorAttachments = &colorAttachment,
        .pDepthAttachment = NULL,
        .pStencilAttachment = NULL,
    };
    vkCmdBeginRendering(pFrame->_commandBuffer, &renderingInfo);
}

void cu_frame_end_render(CuFrame* const pFrame, CuRenderer* const pRenderer) {
    // End the render.
    vkCmdEndRendering(pFrame->_commandBuffer);

    // Convert the swapchain image to presentable.
    const VkImageSubresourceRange subresourceRange = {
        .aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,
        .baseMipLevel = 0,
        .levelCount = 1,
        .baseArrayLayer = 0,
        .layerCount = 1,
    };
    const VkImageMemoryBarrier2 imageBarrier = {
        .sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER_2,
        .pNext = NULL,
        .srcStageMask = VK_PIPELINE_STAGE_2_COLOR_ATTACHMENT_OUTPUT_BIT,
        .srcAccessMask = VK_ACCESS_2_COLOR_ATTACHMENT_WRITE_BIT,
        .dstStageMask = VK_PIPELINE_STAGE_2_NONE,
        .dstAccessMask = VK_ACCESS_2_NONE,
        .oldLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
        .newLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR,
        .srcQueueFamilyIndex = 0,
        .dstQueueFamilyIndex = 0,
        .image = (VkImage)pRenderer->_pSwapchainImages[pRenderer->_iSwapchainImage]._image,
        .subresourceRange = subresourceRange,
    };
    const VkDependencyInfo dependencyInfo = {
        .sType = VK_STRUCTURE_TYPE_DEPENDENCY_INFO,
        .pNext = NULL,
        .dependencyFlags = 0,
        .memoryBarrierCount = 0,
        .pMemoryBarriers = NULL,
        .bufferMemoryBarrierCount = 0,
        .pBufferMemoryBarriers = NULL,
        .imageMemoryBarrierCount = 1,
        .pImageMemoryBarriers = &imageBarrier,
    };
    vkCmdPipelineBarrier2(pFrame->_commandBuffer, &dependencyInfo);
}