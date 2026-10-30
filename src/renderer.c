#include "renderer.h"

#include "debug.h"
#include "g_context.h"
#include "result.h"
#include "swapchain.h"
#include "util.h"
#include "vk.h"

#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>
#include <vulkan/vulkan.h>

/**
 * @brief Creates the renderer's swapchain images.
 * @param [in, out] pRenderer A pointer to the renderer.
 * @return The result of creating the renderer's swapchain images.
 * @warning Requires the renderer's swapchain info to be set.
 */
static CuResult renderer_create_swapchain_images(CuRenderer* pRenderer)
{
    allocate_n(pRenderer->_pSwapchainImages, pRenderer->_nSwapchainImages);

    VkImage images[pRenderer->_nSwapchainImages];
    cu_try_vk(vkGetSwapchainImagesKHR(
        gContext._device, pRenderer->_swapchain, &pRenderer->_nSwapchainImages, images));

    for (uint32_t i = 0; i < pRenderer->_nSwapchainImages; i++) {
        CuSwapchainImage* const pSwapchainImage = &pRenderer->_pSwapchainImages[i];
        dbg_log_note("Creating swapchain image object for image #%u (%p)", i, (void*)images[i]);

        cu_try(create_image_view(
            &pSwapchainImage->_imageView, images[i], pRenderer->_swapchainFormat));
        dbg_log_create(pSwapchainImage->_imageView);

        cu_try(create_semaphore(&pSwapchainImage->_renderFinished, VK_SEMAPHORE_TYPE_BINARY, 0));
        dbg_log_create(pSwapchainImage->_renderFinished);

        pSwapchainImage->_image = images[i];
    }

    return CU_SUCCESS;
}

/**
 * @brief Destroys the swapchain images.
 * @param [in, out] pSwapchainImages A pointer to the swapchain images.
 * @param nSwapchainImages The number of swapchain images.
 */
static void renderer_destroy_swapchain_images(CuRenderer* pRenderer)
{
    for (uint32_t i = 0; i < pRenderer->_nSwapchainImages; i++) {
        const CuSwapchainImage* const pSwapchainImage = &pRenderer->_pSwapchainImages[i];

        vkDestroyImageView(gContext._device, pSwapchainImage->_imageView, nullptr);
        vkDestroySemaphore(gContext._device, pSwapchainImage->_renderFinished, nullptr);
    }

    free(pRenderer->_pSwapchainImages);
}

/**
 * @brief Destroys the renderer's frame in flight objects.
 * @param [in, out] pRenderer A pointer to the renderer.
 */
static void renderer_destroy_frames(CuRenderer* pRenderer)
{
    VkCommandBuffer commandBuffers[pRenderer->_nFramesInFlight];

    for (size_t i = 0; i < pRenderer->_nFramesInFlight; i++) {
        CuFrame* const pFrame = &pRenderer->_pFramesInFlight[i];

        commandBuffers[i] = pFrame->_commandBuffer;
        vkDestroySemaphore(gContext._device, pFrame->_imageAvailable, nullptr);
    }

    if (pRenderer->_nFramesInFlight > 0) {
        vkFreeCommandBuffers(
            gContext._device, gContext._commandPool, pRenderer->_nFramesInFlight, commandBuffers);
    }

    free(pRenderer->_pFramesInFlight);
}

/**
 * @brief Creates the renderer's frame in flight objects.
 * @param [in, out] pRenderer A pointer to the renderer.
 * @return The result of creating the frames.
 * @warning The renderer's `_nFramesInFlight` must have been set.
 */
static CuResult renderer_create_frames(CuRenderer* const pRenderer)
{
    allocate_n(pRenderer->_pFramesInFlight, pRenderer->_nFramesInFlight);

    CuResult result = CU_SUCCESS;

    VkCommandBuffer commandBuffers[pRenderer->_nFramesInFlight];
    cu_try_catch(allocate_command_buffers(commandBuffers, pRenderer->_nFramesInFlight));

    for (size_t i = 0; i < pRenderer->_nFramesInFlight; i++) {
        CuFrame* const pFrame = &pRenderer->_pFramesInFlight[i];

        cu_try_catch(create_semaphore(&pFrame->_imageAvailable, VK_SEMAPHORE_TYPE_BINARY, 0));
        pFrame->_commandBuffer = commandBuffers[i];
    }

    return CU_SUCCESS;

FAIL:
    renderer_destroy_frames(pRenderer);
    return result;
}

/**
 * @brief Recreates the renderer's swapchain.
 * @param [in, out] pRenderer A pointer to the renderer.
 * @return The result of creating the new swapchain.
 */
static CuResult renderer_recreate_swapchain(CuRenderer* const pRenderer)
{
    dbg_log_note("Recreating the swapchain %p", (void*)pRenderer->_swapchain);

    CuSwapchainInfo swapchainInfo = {};
    cu_try(create_swapchain(
        &swapchainInfo, pRenderer->_minSwapchainImages, pRenderer->_pWindow, pRenderer->_surface,
        pRenderer->_preferredPresentMode, pRenderer->_swapchain));
    dbg_log_create(swapchainInfo._handle);

    vkQueueWaitIdle(gContext._queue);
    vkDestroySwapchainKHR(gContext._device, pRenderer->_swapchain, nullptr);
    dbg_log_destroy(pRenderer->_swapchain);
    renderer_destroy_swapchain_images(pRenderer);

    pRenderer->_swapchain = swapchainInfo._handle;
    pRenderer->_swapchainFormat = swapchainInfo._format;
    pRenderer->_swapchainExtent = swapchainInfo._extent;
    pRenderer->_nSwapchainImages = swapchainInfo._nImages;

    cu_try(renderer_create_swapchain_images(pRenderer));

    return CU_SUCCESS;
}

CuResult cu_renderer_create(
    CuRenderer* const pRenderer,
    const CuRendererCreateInfo* pCreateInfo,
    const CuWindow* const pWindow)
{
    CuResult result = CU_SUCCESS;

    if (pWindow != nullptr) {
        cu_try_catch(create_surface(&pRenderer->_surface, pWindow->_handle));
    } else {
        cu_try_catch(create_headless_surface(&pRenderer->_surface));
    }
    dbg_log_create(pRenderer->_surface);

    CuSwapchainInfo swapchainInfo = {};
    cu_try_catch(create_swapchain(
        &swapchainInfo, pCreateInfo->minSwapchainImages, pWindow, pRenderer->_surface,
        pCreateInfo->preferredPresentMode, VK_NULL_HANDLE));
    pRenderer->_swapchain = swapchainInfo._handle;
    pRenderer->_swapchainFormat = swapchainInfo._format;
    pRenderer->_swapchainExtent = swapchainInfo._extent;
    pRenderer->_nSwapchainImages = swapchainInfo._nImages;
    cu_try_catch(renderer_create_swapchain_images(pRenderer));

    pRenderer->_nFramesInFlight = pCreateInfo->maxFramesInFlight;
    cu_try_catch(renderer_create_frames(pRenderer));

    cu_try_catch(create_semaphore(&pRenderer->_timelineSemaphore, VK_SEMAPHORE_TYPE_TIMELINE, 0));
    dbg_log_create(pRenderer->_timelineSemaphore);

    pRenderer->_pWindow = pWindow;
    pRenderer->_minSwapchainImages = pCreateInfo->minSwapchainImages;
    pRenderer->_preferredPresentMode = pCreateInfo->preferredPresentMode;
    pRenderer->_frameCounter = (uint64_t)pRenderer->_nFramesInFlight - 1;
    pRenderer->_iSwapchainImage = 0;

    return CU_SUCCESS;

FAIL:
    dbg_log_fail(result);
    cu_renderer_destroy(pRenderer);
    return result;
}

void cu_renderer_destroy(CuRenderer* const pRenderer)
{
    if (pRenderer == nullptr) {
        return;
    }

    vkDeviceWaitIdle(gContext._device);

    vkDestroySemaphore(gContext._device, pRenderer->_timelineSemaphore, nullptr);
    dbg_log_destroy(pRenderer->_timelineSemaphore);
    renderer_destroy_frames(pRenderer);
    renderer_destroy_swapchain_images(pRenderer);
    vkDestroySwapchainKHR(gContext._device, pRenderer->_swapchain, nullptr);
    dbg_log_destroy(pRenderer->_swapchain);
    vkDestroySurfaceKHR(gContext._instance, pRenderer->_surface, nullptr);
    dbg_log_destroy(pRenderer->_surface);
}

CuResult cu_renderer_begin_frame(CuRenderer* const pRenderer, VkCommandBuffer* const pCommandBuffer)
{
    const CuFrame* const pFrame = cu_renderer_get_current_frame(pRenderer);

    // Wait for the image to be available.
    const uint64_t waitValue = pRenderer->_frameCounter + 1 - pRenderer->_nFramesInFlight;
    const VkSemaphoreWaitInfo waitInfo = {
        .sType = VK_STRUCTURE_TYPE_SEMAPHORE_WAIT_INFO,
        .pNext = nullptr,
        .flags = 0,
        .semaphoreCount = 1,
        .pSemaphores = &pRenderer->_timelineSemaphore,
        .pValues = &waitValue,
    };
    cu_try_vk(vkWaitSemaphores(gContext._device, &waitInfo, TIMEOUT_NANOS));

    // Get the next target swapchain image's index.
    const VkResult acquireResult = vkAcquireNextImageKHR(
        gContext._device, pRenderer->_swapchain, TIMEOUT_NANOS, pFrame->_imageAvailable,
        VK_NULL_HANDLE, &pRenderer->_iSwapchainImage);
    if (acquireResult == VK_ERROR_OUT_OF_DATE_KHR) {
        *pCommandBuffer = VK_NULL_HANDLE;
        return renderer_recreate_swapchain(pRenderer);
    } else if (acquireResult != VK_SUCCESS && acquireResult != VK_SUBOPTIMAL_KHR) {
        return cu_vk_error(acquireResult);
    }

    // Begin the frame's command buffer.
    const VkCommandBufferBeginInfo beginInfo = {
        .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,
        .pNext = nullptr,
        .flags = 0,
        .pInheritanceInfo = nullptr,
    };
    cu_try_vk(vkBeginCommandBuffer(pFrame->_commandBuffer, &beginInfo));

    *pCommandBuffer = pFrame->_commandBuffer;
    return CU_SUCCESS;
}

CuResult cu_renderer_submit_frame(CuRenderer* const pRenderer)
{
    const CuFrame* const pFrame = cu_renderer_get_current_frame(pRenderer);

    // End the frame's command buffer.
    cu_try_vk(vkEndCommandBuffer(pFrame->_commandBuffer));

    CuSwapchainImage* const pSwapchainImage =
        &pRenderer->_pSwapchainImages[pRenderer->_iSwapchainImage];

    // Submit the command buffer.
    const VkSemaphoreSubmitInfoKHR semaphoreSubmitInfo = {
        .sType = VK_STRUCTURE_TYPE_SEMAPHORE_SUBMIT_INFO,
        .pNext = nullptr,
        .semaphore = pFrame->_imageAvailable,
        .value = 0,
        .stageMask = VK_PIPELINE_STAGE_2_COLOR_ATTACHMENT_OUTPUT_BIT,
        .deviceIndex = 0,
    };
    const VkCommandBufferSubmitInfo commandBufferSubmitInfo = {
        .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_SUBMIT_INFO,
        .pNext = nullptr,
        .commandBuffer = pFrame->_commandBuffer,
        .deviceMask = 1,
    };
    const VkSemaphoreSubmitInfo signalSemaphoreInfos[2] = {
        {
            .sType = VK_STRUCTURE_TYPE_SEMAPHORE_SUBMIT_INFO,
            .pNext = nullptr,
            .semaphore = pSwapchainImage->_renderFinished,
            .value = 0,
            .stageMask = VK_PIPELINE_STAGE_2_BOTTOM_OF_PIPE_BIT,
            .deviceIndex = 0,
        },
        {
            .sType = VK_STRUCTURE_TYPE_SEMAPHORE_SUBMIT_INFO,
            .pNext = nullptr,
            .semaphore = pRenderer->_timelineSemaphore,
            .value = pRenderer->_frameCounter + 1,
            .stageMask = VK_PIPELINE_STAGE_2_BOTTOM_OF_PIPE_BIT,
            .deviceIndex = 0,
        },
    };
    const VkSubmitInfo2 submitInfo = {
        .sType = VK_STRUCTURE_TYPE_SUBMIT_INFO_2,
        .pNext = nullptr,
        .flags = 0,
        .waitSemaphoreInfoCount = 1,
        .pWaitSemaphoreInfos = &semaphoreSubmitInfo,
        .commandBufferInfoCount = 1,
        .pCommandBufferInfos = &commandBufferSubmitInfo,
        .signalSemaphoreInfoCount = 2,
        .pSignalSemaphoreInfos = signalSemaphoreInfos,
    };
    cu_try_vk(vkQueueSubmit2(gContext._queue, 1, &submitInfo, VK_NULL_HANDLE));
    pRenderer->_frameCounter += 1;

    // Submit presentation.
    const VkPresentInfoKHR presentInfo = {
        .sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR,
        .pNext = nullptr,
        .waitSemaphoreCount = 1,
        .pWaitSemaphores = &pSwapchainImage->_renderFinished,
        .swapchainCount = 1,
        .pSwapchains = &pRenderer->_swapchain,
        .pImageIndices = &pRenderer->_iSwapchainImage,
        .pResults = nullptr,
    };
    const VkResult presentResult = vkQueuePresentKHR(gContext._queue, &presentInfo);
    if (presentResult == VK_ERROR_OUT_OF_DATE_KHR || presentResult == VK_SUBOPTIMAL_KHR) {
        cu_try(renderer_recreate_swapchain(pRenderer));
    } else if (presentResult != VK_SUCCESS) {
        return cu_vk_error(presentResult);
    }

    return CU_SUCCESS;
}

void cu_renderer_start_rendering(
    const CuRenderer* const pRenderer,
    const VkCommandBuffer commandBuffer)
{
    // Set the viewport and scissor.
    const VkRect2D area = {
        .offset =
            {
                .x = 0,
                .y = 0,
            },
        .extent = pRenderer->_swapchainExtent,
    };
    const VkViewport viewport = {
        .x = 0.0F,
        .y = (float)area.extent.height,
        .width = (float)area.extent.width,
        .height = -(float)area.extent.height,
        .minDepth = 0.0F,
        .maxDepth = 1.0F,
    };
    vkCmdSetViewport(commandBuffer, 0, 1, &viewport);
    vkCmdSetScissor(commandBuffer, 0, 1, &area);

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
        .pNext = nullptr,
        .srcStageMask = VK_PIPELINE_STAGE_2_COLOR_ATTACHMENT_OUTPUT_BIT,
        .srcAccessMask = VK_ACCESS_2_NONE,
        .dstStageMask = VK_PIPELINE_STAGE_2_COLOR_ATTACHMENT_OUTPUT_BIT,
        .dstAccessMask = VK_ACCESS_2_COLOR_ATTACHMENT_WRITE_BIT,
        .oldLayout = VK_IMAGE_LAYOUT_UNDEFINED,
        .newLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
        .srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
        .dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
        .image = pSwapchainImage->_image,
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
    vkCmdPipelineBarrier2(commandBuffer, &dependencyInfo);

    // Begin rendering.
    const VkClearValue clearValue = {
        .color.float32 = {0.0F, 0.0F, 0.0F, 1.0F},
    };
    const VkRenderingAttachmentInfo colorAttachment = {
        .sType = VK_STRUCTURE_TYPE_RENDERING_ATTACHMENT_INFO,
        .pNext = nullptr,
        .imageView = pSwapchainImage->_imageView,
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
        .pNext = nullptr,
        .flags = 0,
        .renderArea = area,
        .layerCount = 1,
        .viewMask = 0,
        .colorAttachmentCount = 1,
        .pColorAttachments = &colorAttachment,
        .pDepthAttachment = nullptr,
        .pStencilAttachment = nullptr,
    };
    vkCmdBeginRendering(commandBuffer, &renderingInfo);
}

void cu_renderer_end_rendering(
    const CuRenderer* const pRenderer,
    const VkCommandBuffer commandBuffer)
{
    // End the render.
    vkCmdEndRendering(commandBuffer);

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
        .pNext = nullptr,
        .srcStageMask = VK_PIPELINE_STAGE_2_COLOR_ATTACHMENT_OUTPUT_BIT,
        .srcAccessMask = VK_ACCESS_2_COLOR_ATTACHMENT_WRITE_BIT,
        .dstStageMask = VK_PIPELINE_STAGE_2_NONE,
        .dstAccessMask = VK_ACCESS_2_NONE,
        .oldLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
        .newLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR,
        .srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
        .dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
        .image = pRenderer->_pSwapchainImages[pRenderer->_iSwapchainImage]._image,
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
    vkCmdPipelineBarrier2(commandBuffer, &dependencyInfo);
}
