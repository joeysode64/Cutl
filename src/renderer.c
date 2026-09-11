#include "renderer.h"

#include "frame.h"
#include "g_context.h"
#include "inner.h"
#include "result.h"
#include "swapchain.h"
#include "util.h"
#include "vk.h"

#include <assert.h>
#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>
#include <vulkan/vulkan_core.h>

CuResult cu_renderer_create(
    CuRenderer* const  pRenderer,
    const CuRendererCreateInfo* pCreateInfo,
    const CuWindow* const pWindow)
{
    assert(pRenderer != nullptr);
    assert(pWindow != nullptr);

    CuResult result = CU_ERROR_UNKNOWN;

    if (pCreateInfo == nullptr) {
        pCreateInfo = &CU_DEFAULT_RENDERER_CREATE_INFO;
    }

    assert(pCreateInfo->maxFramesInFlight > 0);
    assert(pCreateInfo->minSwapchainImages > 0);

    cu_try_catch_vk(create_swapchain(
        &pRenderer->_swapchainInfo,
        &pRenderer->_nSwapchainImages,
        pCreateInfo->minSwapchainImages,
        gContext.instance,
        gContext.device,
        gContext.physicalDeviceInfo.handle,
        pWindow,
        VK_NULL_HANDLE));
    
    const size_t zSwapchainImages = pRenderer->_nSwapchainImages * sizeof(CuSwapchainImage);
    const size_t zFramesInFlight = pCreateInfo->maxFramesInFlight * sizeof(CuFrame);
    allocate_z(pRenderer->_pData, zSwapchainImages + zFramesInFlight);

    pRenderer->_pSwapchainImages = (CuSwapchainImage*)pRenderer->_pData;
    cu_try_catch_vk(create_swapchain_images(
        pRenderer->_pSwapchainImages,
        pRenderer->_nSwapchainImages,
        gContext.device,
        &pRenderer->_swapchainInfo));

    pRenderer->_pFramesInFlight =
        (CuFrame*)((uint8_t*)pRenderer->_pSwapchainImages + zSwapchainImages);
    cu_try_catch_vk(create_frames(
        pRenderer->_pFramesInFlight,
        pCreateInfo->maxFramesInFlight,
        gContext.device,
        gContext.commandPool));
    pRenderer->_nFramesInFlight = pCreateInfo->maxFramesInFlight;

    cu_try_catch_vk(create_semaphore(
        &pRenderer->_timelineSemaphore,
        gContext.device,
        VK_SEMAPHORE_TYPE_TIMELINE,
        0));
    
    pRenderer->_frameCounter = (uint64_t)pRenderer->_nFramesInFlight - 1;
    pRenderer->_iSwapchainImage = 0;

    return CU_SUCCESS;

FAIL:
    cu_renderer_destroy(pRenderer);
    return result;
}

void cu_renderer_destroy(
    CuRenderer* const pRenderer)
{
    if (pRenderer == nullptr) {
        return;
    }

    vkDeviceWaitIdle(gContext.device);

    destroy_frames(
        pRenderer->_pFramesInFlight,
        pRenderer->_nFramesInFlight,
        gContext.device,
        gContext.commandPool);
    destroy_swapchain_images(
        pRenderer->_pSwapchainImages, pRenderer->_nSwapchainImages, gContext.device);
    vkDestroySwapchainKHR(gContext.device, pRenderer->_swapchainInfo._handle, nullptr);
    vkDestroySurfaceKHR(gContext.instance, pRenderer->_swapchainInfo._surface, nullptr);
    free(pRenderer->_pData);
}


CuResult cu_renderer_begin_frame(
    CuRenderer* const pRenderer,
    CuFrame** const ppFrame)
{
    CuResult result = CU_SUCCESS;

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
    cu_try_catch_vk(vkWaitSemaphores(gContext.device, &waitInfo, TIMEOUT_NANOS));

    // Get the next target swapchain image's index.
    cu_try_catch_vk(vkAcquireNextImageKHR(
        gContext.device,
        pRenderer->_swapchainInfo._handle,
        TIMEOUT_NANOS,
        pFrame->_imageAvailable,
        VK_NULL_HANDLE,
        &pRenderer->_iSwapchainImage));

    // Begin the frame's command buffer.
    const VkCommandBufferBeginInfo beginInfo = {
        .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,
        .pNext = nullptr,
        .flags = 0,
        .pInheritanceInfo = nullptr,
    };
    cu_try_catch_vk(vkBeginCommandBuffer(pFrame->_commandBuffer, &beginInfo));

    *ppFrame = pFrame;
    return CU_SUCCESS;

FAIL:
    return result;
}

CuResult cu_renderer_submit_frame(
    CuRenderer* const pRenderer,
    CuFrame* pFrame)
{
    CuResult result = CU_SUCCESS;

    // End the frame's command buffer.
    cu_try_catch_vk(vkEndCommandBuffer(pFrame->_commandBuffer));

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
    cu_try_catch_vk(vkQueueSubmit2(gContext.queue, 1, &submitInfo, VK_NULL_HANDLE));
    pRenderer->_frameCounter += 1;

    // Submit presentation.
    const VkPresentInfoKHR presentInfo = {
        .sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR,
        .pNext = nullptr,
        .waitSemaphoreCount = 1,
        .pWaitSemaphores = &pSwapchainImage->_renderFinished,
        .swapchainCount = 1,
        .pSwapchains = &pRenderer->_swapchainInfo._handle,
        .pImageIndices = &pRenderer->_iSwapchainImage,
        .pResults = nullptr,
    };
    cu_try_catch_vk(vkQueuePresentKHR(gContext.queue, &presentInfo));

    return CU_SUCCESS;

FAIL:
    return result;
}
