#include "renderer.h"

#include "debug.h"
#include "frame.h"
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
 * @brief Recreates the renderer's swapchain.
 * @param [in, out] pRenderer A pointer to the renderer.
 * @return The result of creating the new swapchain.
 */
static CuResult renderer_recreate_swapchain(CuRenderer* const pRenderer)
{
    dbg_log_note("Recreating the swapchain %p", (void*)pRenderer->_swapchain);

    CuSwapchainInfo swapchainInfo = {};
    cu_try(cu_vk_create_swapchain(
        &swapchainInfo, pRenderer->_minSwapchainImages, pRenderer->_pWindow, pRenderer->_surface,
        pRenderer->_preferredPresentMode, pRenderer->_swapchain));
    dbg_log_create(swapchainInfo._handle);

    vkQueueWaitIdle(gContext._queue);
    vkDestroySwapchainKHR(gContext._device, pRenderer->_swapchain, nullptr);
    dbg_log_destroy(pRenderer->_swapchain);
    cu_swapchain_images_destroy(pRenderer->_pSwapchainImages, pRenderer->_nSwapchainImages);

    if (swapchainInfo._nImages != pRenderer->_nSwapchainImages) {
        free(pRenderer->_pSwapchainImages);
        allocate_n(pRenderer->_pSwapchainImages, swapchainInfo._nImages);
    }

    pRenderer->_swapchain = swapchainInfo._handle;
    pRenderer->_swapchainFormat = swapchainInfo._format;
    pRenderer->_swapchainExtent = swapchainInfo._extent;
    pRenderer->_nSwapchainImages = swapchainInfo._nImages;

    cu_try(cu_swapchain_images_create(
        pRenderer->_pSwapchainImages, pRenderer->_nSwapchainImages, pRenderer->_swapchain,
        pRenderer->_swapchainFormat));

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
    cu_try_catch(cu_vk_create_swapchain(
        &swapchainInfo, pCreateInfo->minSwapchainImages, pWindow, pRenderer->_surface,
        pCreateInfo->preferredPresentMode, VK_NULL_HANDLE));
    pRenderer->_swapchain = swapchainInfo._handle;
    pRenderer->_swapchainFormat = swapchainInfo._format;
    pRenderer->_swapchainExtent = swapchainInfo._extent;
    pRenderer->_nSwapchainImages = swapchainInfo._nImages;

    allocate_n(pRenderer->_pSwapchainImages, pRenderer->_nSwapchainImages);
    cu_try_catch(cu_swapchain_images_create(
        pRenderer->_pSwapchainImages, pRenderer->_nSwapchainImages, pRenderer->_swapchain,
        pRenderer->_swapchainFormat));

    pRenderer->_nFramesInFlight = pCreateInfo->maxFramesInFlight;
    allocate_n(pRenderer->_pFramesInFlight, pRenderer->_nFramesInFlight);
    cu_try_catch(create_frames(pRenderer->_pFramesInFlight, pRenderer->_nFramesInFlight));

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
    destroy_frames(pRenderer->_pFramesInFlight, pRenderer->_nFramesInFlight);
    free(pRenderer->_pFramesInFlight);
    cu_swapchain_images_destroy(pRenderer->_pSwapchainImages, pRenderer->_nSwapchainImages);
    free(pRenderer->_pSwapchainImages);
    vkDestroySwapchainKHR(gContext._device, pRenderer->_swapchain, nullptr);
    dbg_log_destroy(pRenderer->_swapchain);
    vkDestroySurfaceKHR(gContext._instance, pRenderer->_surface, nullptr);
    dbg_log_destroy(pRenderer->_surface);
}

CuResult cu_renderer_begin_frame(CuRenderer* const pRenderer, CuFrame** const ppFrame)
{
    const uint32_t iFrame = cu_renderer_get_frame_index(pRenderer);
    CuFrame* const pFrame = &pRenderer->_pFramesInFlight[iFrame];

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
        *ppFrame = nullptr;
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

    *ppFrame = pFrame;
    return CU_SUCCESS;
}

CuResult cu_renderer_submit_frame(CuRenderer* const pRenderer, CuFrame* pFrame)
{
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
