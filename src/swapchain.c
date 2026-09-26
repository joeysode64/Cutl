#include "swapchain.h"

#include "enumerator.h"
#include "inner.h"
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

/// @return The extent best fit for the window.
static VkExtent2D get_extent(
    const VkSurfaceCapabilitiesKHR* pCapabilities,
    const CuWindow* pWindow);

/// @brief Chooses the surface format best suited for the swapchain.
static VkResult choose_surface_format(
    VkSurfaceFormatKHR* pSurfaceFormat,
    VkSurfaceKHR surface,
    VkPhysicalDevice physicalDevice);

/// @brief Chooses the present mode best suited for the swapchain.
static VkResult choose_present_mode(
    VkPresentModeKHR* pPresentMode,
    VkSurfaceKHR surface,
    VkPhysicalDevice physicalDevice);

VkResult create_swapchain(
    CuSwapchainInfo* const pSwapchainInfo,
    uint32_t* const pnSwapchainImages,
    const uint32_t minSwapchainImages,
    const VkInstance instance,
    const VkDevice device,
    const VkPhysicalDevice physicalDevice,
    const CuWindow* const pWindow,
    const VkSwapchainKHR oldSwapchain)
{
    VkSurfaceKHR surface = VK_NULL_HANDLE;
    vk_try(glfwCreateWindowSurface(instance, pWindow->_handle, nullptr, &surface));
    VkSurfaceCapabilitiesKHR capabilities = {};
    vk_try(vkGetPhysicalDeviceSurfaceCapabilitiesKHR(physicalDevice, surface, &capabilities));
    VkSurfaceFormatKHR surfaceFormat = {};
    vk_try(choose_surface_format(&surfaceFormat, surface, physicalDevice));
    VkPresentModeKHR presentMode = 0;
    vk_try(choose_present_mode(&presentMode, surface, physicalDevice));
    const VkExtent2D extent = get_extent(&capabilities, pWindow);

    const VkSwapchainCreateInfoKHR createInfo = {
        .sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR,
        .pNext = nullptr,
        .flags = 0,
        .surface = surface,
        .minImageCount = minSwapchainImages,
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
    vk_try(vkCreateSwapchainKHR(device, &createInfo, nullptr, &pSwapchainInfo->_handle));

    vk_try(vkGetSwapchainImagesKHR(
        device, pSwapchainInfo->_handle, pnSwapchainImages, nullptr));

    pSwapchainInfo->_surface = surface;
    pSwapchainInfo->_format = surfaceFormat.format;
    pSwapchainInfo->_w = extent.width;
    pSwapchainInfo->_h = extent.height;

    return VK_SUCCESS;
}

VkResult create_swapchain_images(
    CuSwapchainImage* const pSwapchainImages,
    uint32_t nSwapchainImages,
    const VkDevice device,
    const CuSwapchainInfo* const pSwapchainInfo)
{
    VkImage images[nSwapchainImages];
    vk_try(vkGetSwapchainImagesKHR(device, pSwapchainInfo->_handle, &nSwapchainImages, images));

    for (uint32_t i = 0; i < nSwapchainImages; i++) {
        CuSwapchainImage* const pSwapchainImage = &pSwapchainImages[i];

        vk_try(create_image_view(
            &pSwapchainImage->_imageView, device, images[i], pSwapchainInfo->_format));
        vk_try(create_semaphore(
            &pSwapchainImage->_renderFinished, device, VK_SEMAPHORE_TYPE_BINARY, 0));
        pSwapchainImage->_image = images[i];
    }

    return VK_SUCCESS;
}

void destroy_swapchain_images(
    CuSwapchainImage* const pSwapchainImages,
    const uint32_t nSwapchainImages,
    const VkDevice device)
{
    for (uint32_t i = 0; i < nSwapchainImages; i++) {
        const CuSwapchainImage* const pSwapchainImage = &pSwapchainImages[i];

        vkDestroyImageView(device, pSwapchainImage->_imageView, nullptr);
        vkDestroySemaphore(device, pSwapchainImage->_renderFinished, nullptr);
    }
}

VkExtent2D get_extent(
    const VkSurfaceCapabilitiesKHR* const pCapabilities,
    const CuWindow* const pWindow)
{
    const bool isExtentDefined = (pCapabilities->currentExtent.width != UINT32_MAX) ||
        (pCapabilities->currentExtent.height != UINT32_MAX);
    if (isExtentDefined) {
        return pCapabilities->currentExtent;
    } else {
        return (VkExtent2D){
            .width = clamp(
                pWindow->_w,
                pCapabilities->minImageExtent.width,
                pCapabilities->maxImageExtent.width),
            .height = clamp(
                pWindow->_h,
                pCapabilities->minImageExtent.height,
                pCapabilities->maxImageExtent.height),
        };
    }
}

VkResult choose_surface_format(
    VkSurfaceFormatKHR* const pSurfaceFormat,
    const VkSurfaceKHR surface,
    const VkPhysicalDevice physicalDevice)
{
    Enumerator(VkSurfaceFormatKHR) surfaceFormats ENUMERATOR_AUTO_FREE = {};
    enumerate_vk(surfaceFormats, vkGetPhysicalDeviceSurfaceFormatsKHR, physicalDevice, surface);

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

    return VK_SUCCESS;
}

VkResult choose_present_mode(
    VkPresentModeKHR* const pPresentMode,
    const VkSurfaceKHR surface,
    const VkPhysicalDevice physicalDevice)
{
    Enumerator(VkPresentModeKHR) presentModes ENUMERATOR_AUTO_FREE = {};
    enumerate_vk(presentModes, vkGetPhysicalDeviceSurfacePresentModesKHR, physicalDevice, surface);

    *pPresentMode = VK_PRESENT_MODE_FIFO_KHR;
    for (uint32_t i = 0; i < presentModes.n; i++) {
        if (presentModes.p[i] == VK_PRESENT_MODE_MAILBOX_KHR) {
            *pPresentMode = VK_PRESENT_MODE_MAILBOX_KHR;
            break;
        }
    }

    return VK_SUCCESS;
}
