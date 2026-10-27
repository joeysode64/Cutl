#include "swapchain.h"

#include "debug.h"
#include "enumerator.h"
#include "g_context.h"
#include "result.h"
#include "util.h"
#include "vk.h"
#include "window.h"
#include <vulkan/vulkan_core.h>

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <stddef.h>
#include <stdint.h>
#include <vulkan/vulkan.h>

/** @brief The preferred extent for a headless surface's swapchain. */
constexpr VkExtent2D PREFERRED_HEADLESS_EXTENT = {
    .width = 1280,
    .height = 720,
};

/**
 * @brief Chooses a surface format for the surface.
 * @param [out] pSurfaceFormat A pointer to the surface format.
 * @param surface The surface handle.
 * @return The result of choosing a surface format.
 * @note Chooses The first 32-bit format with a non-linear SRGB color space, or
 * the first surface format if none are found.
 */
static CuResult choose_surface_format(
    VkSurfaceFormatKHR* const pSurfaceFormat,
    const VkSurfaceKHR surface)
{
    Enumerator(VkSurfaceFormatKHR) surfaceFormats ENUMERATOR_AUTO_FREE = {};
    enumerate_vk(
        surfaceFormats, vkGetPhysicalDeviceSurfaceFormatsKHR, gContext._physicalDevice, surface);

    *pSurfaceFormat = surfaceFormats.p[0];
    for (uint32_t i = 0; i < surfaceFormats.n; i++) {
        const VkSurfaceFormatKHR surfaceFormat = surfaceFormats.p[i];
        const bool hasGoodFormat = (surfaceFormat.format == VK_FORMAT_B8G8R8A8_SRGB) ||
                                   (surfaceFormat.format == VK_FORMAT_R8G8B8A8_SRGB);
        const bool hasGoodColorSpace =
            surfaceFormat.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR;
        if (hasGoodFormat && hasGoodColorSpace) {
            *pSurfaceFormat = surfaceFormat;
            break;
        }
    }

    dbg_log_note(
        "Chose surface format with format: 0x%X, and color space: 0x%X", pSurfaceFormat->format,
        pSurfaceFormat->colorSpace);
    return CU_SUCCESS;
}

/**
 * @brief Chooses the present mode for the surface.
 * @param [out] pPresentMode A pointer to the present mode.
 * @param surface The surface handle.
 * @param targetPresentMode The target present mode.
 * @return The result of choosing a present mode.
 * @note Chooses the target present mode if present, falls back to FIFO.
 */
static CuResult choose_present_mode(
    VkPresentModeKHR* const pPresentMode,
    const VkSurfaceKHR surface,
    const VkPresentModeKHR targetPresentMode)
{
    Enumerator(VkPresentModeKHR) presentModes ENUMERATOR_AUTO_FREE = {};
    enumerate_vk(
        presentModes, vkGetPhysicalDeviceSurfacePresentModesKHR, gContext._physicalDevice, surface);

    *pPresentMode = VK_PRESENT_MODE_FIFO_KHR;
    for (uint32_t i = 0; i < presentModes.n; i++) {
        if (presentModes.p[i] == targetPresentMode) {
            *pPresentMode = targetPresentMode;
            break;
        }
    }

    dbg_log_note("Chose present mode 0x%X", *pPresentMode);
    return CU_SUCCESS;
}

/**
 * @brief Returns the preferred extent clamped within the surface capabilities.
 * @param pCapabilities A pointer to the capabilities.
 * @param preferred The preferred extent.
 * @return The preferred extent clamped within the surface capabilities.
 */
static VkExtent2D get_extent(
    const VkSurfaceCapabilitiesKHR* const pCapabilities,
    const VkExtent2D preferred)
{
    const bool isExtentDefined = (pCapabilities->currentExtent.width != UINT32_MAX) ||
                                 (pCapabilities->currentExtent.height != UINT32_MAX);
    if (isExtentDefined) {
        return pCapabilities->currentExtent;
    } else {
        return (VkExtent2D){
            .width = clamp(
                preferred.width, pCapabilities->minImageExtent.width,
                pCapabilities->maxImageExtent.width),
            .height = clamp(
                preferred.height, pCapabilities->minImageExtent.height,
                pCapabilities->maxImageExtent.height),
        };
    }
}

/**
 * @brief Returns the number of swapchain images to request.
 * @param minSwapchainImages The minimum number of swapchain images.
 * @param pCapabilities A pointer to the surface capabilities.
 * @return The number of swapchain images to request.
 */
static uint32_t get_n_swapchain_images(
    const uint32_t minSwapchainImages,
    const VkSurfaceCapabilitiesKHR* pCapabilities)
{
    const uint32_t n = max(minSwapchainImages, pCapabilities->minImageCount);
    return (pCapabilities->maxImageCount == 0) ? n : min(n, pCapabilities->maxImageCount);
}

CuResult cu_vk_create_swapchain(
    CuSwapchainInfo* const pSwapchainInfo,
    const uint32_t minSwapchainImages,
    const CuWindow* const pWindow,
    const VkSurfaceKHR surface,
    const VkPresentModeKHR preferredPresentMode,
    const VkSwapchainKHR oldSwapchain)
{
    VkSurfaceCapabilitiesKHR capabilities = {};
    cu_try_vk(vkGetPhysicalDeviceSurfaceCapabilitiesKHR(
        gContext._physicalDevice, surface, &capabilities));
    VkSurfaceFormatKHR surfaceFormat = {};
    cu_try(choose_surface_format(&surfaceFormat, surface));
    VkPresentModeKHR presentMode = 0;
    cu_try(choose_present_mode(&presentMode, surface, preferredPresentMode));
    const VkExtent2D preferred = (pWindow != nullptr)
                                     ? (VkExtent2D){.width = pWindow->_w, .height = pWindow->_h}
                                     : PREFERRED_HEADLESS_EXTENT;
    const VkExtent2D extent = get_extent(&capabilities, preferred);
    const uint32_t nSwapchainImages = get_n_swapchain_images(minSwapchainImages, &capabilities);
    const VkSwapchainCreateInfoKHR createInfo = {
        .sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR,
        .pNext = nullptr,
        .flags = 0,
        .surface = surface,
        .minImageCount = nSwapchainImages,
        .imageFormat = surfaceFormat.format,
        .imageColorSpace = surfaceFormat.colorSpace,
        .imageExtent = extent,
        .imageArrayLayers = 1,
        .imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT,
        .imageSharingMode = VK_SHARING_MODE_EXCLUSIVE,
        .queueFamilyIndexCount = 0,
        .pQueueFamilyIndices = nullptr,
        .preTransform = capabilities.currentTransform,
        .compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR,
        .presentMode = presentMode,
        .clipped = VK_TRUE,
        .oldSwapchain = oldSwapchain,
    };
    cu_try_vk(
        vkCreateSwapchainKHR(gContext._device, &createInfo, nullptr, &pSwapchainInfo->_handle));
    dbg_log_create(pSwapchainInfo->_handle);

    cu_try_vk(vkGetSwapchainImagesKHR(
        gContext._device, pSwapchainInfo->_handle, &pSwapchainInfo->_nImages, nullptr));
    dbg_log_note(
        "Swapchain %p was created with %u swapchain images", (void*)pSwapchainInfo->_handle,
        pSwapchainInfo->_nImages);

    pSwapchainInfo->_format = surfaceFormat.format;
    pSwapchainInfo->_extent = extent;

    return CU_SUCCESS;
}

CuResult cu_swapchain_images_create(
    CuSwapchainImage* const pSwapchainImages,
    uint32_t nSwapchainImages,
    const VkSwapchainKHR swapchain,
    const VkFormat format)
{
    VkImage images[nSwapchainImages];
    cu_try_vk(vkGetSwapchainImagesKHR(gContext._device, swapchain, &nSwapchainImages, images));

    for (uint32_t i = 0; i < nSwapchainImages; i++) {
        CuSwapchainImage* const pSwapchainImage = &pSwapchainImages[i];

        cu_try(create_image_view(&pSwapchainImage->_imageView, images[i], format));
        cu_try(create_semaphore(&pSwapchainImage->_renderFinished, VK_SEMAPHORE_TYPE_BINARY, 0));
        pSwapchainImage->_image = images[i];
    }

    return CU_SUCCESS;
}

void cu_swapchain_images_destroy(
    CuSwapchainImage* const pSwapchainImages,
    const uint32_t nSwapchainImages)
{
    for (uint32_t i = 0; i < nSwapchainImages; i++) {
        const CuSwapchainImage* const pSwapchainImage = &pSwapchainImages[i];

        vkDestroyImageView(gContext._device, pSwapchainImage->_imageView, nullptr);
        vkDestroySemaphore(gContext._device, pSwapchainImage->_renderFinished, nullptr);
    }
}
