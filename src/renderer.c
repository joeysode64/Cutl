#include "renderer.h"

#include "frame_t.h"
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
    const size_t zFramesInFlight = pCreateInfo->maxFramesInFlight * sizeof(CuFrame_T);
    allocate_z(pRenderer->_pData, zSwapchainImages + zFramesInFlight);

    pRenderer->_pSwapchainImages = (CuSwapchainImage*)pRenderer->_pData;
    cu_try_catch_vk(create_swapchain_images(
        pRenderer->_pSwapchainImages,
        pRenderer->_nSwapchainImages,
        gContext.device,
        &pRenderer->_swapchainInfo));

    pRenderer->_pFramesInFlight =
        (CuFrame_T*)((uint8_t*)pRenderer->_pSwapchainImages + zSwapchainImages);
    cu_try_catch_vk(create_frames(
        pRenderer->_pFramesInFlight,
        pCreateInfo->maxFramesInFlight,
        gContext.device,
        gContext.commandPool));
    pRenderer->_nFramesInFlight = pCreateInfo->maxFramesInFlight;

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

    destroy_frames(pRenderer->_pFramesInFlight, pRenderer->_nFramesInFlight, gContext.device);
    destroy_swapchain_images(
        pRenderer->_pSwapchainImages, pRenderer->_nSwapchainImages, gContext.device);
    vkDestroySwapchainKHR(gContext.device, pRenderer->_swapchainInfo._handle, nullptr);
    vkDestroySurfaceKHR(gContext.instance, pRenderer->_swapchainInfo._surface, nullptr);
    free(pRenderer->_pData);
}
