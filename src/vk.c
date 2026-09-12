#include "vk.h"

#include "info.h"
#include "result.h"
#include "util.h"

#include <assert.h>
#include <stddef.h>
#include <stdint.h>
#include <vulkan/vulkan.h>
#include <vulkan/vulkan_core.h>

/** @brief The Cutl version in Vulkan format. */
constexpr uint32_t CUTL_VK_VERSION =
    VK_MAKE_VERSION(CU_VERSION_MAJOR, CU_VERSION_MINOR, CU_VERSION_PATCH);

/** @brief The instance extensions. */
static const char* INSTANCE_EXTENSIONS[] = {
    "VK_KHR_surface",
#if ON_APPLE
    "VK_EXT_metal_surface",
    "VK_KHR_portability_enumeration",
#elif ON_LINUX
    "VK_KHR_xcb_surface",
#elif ON_WINDOWS
    "VK_KHR_win32_surface",
#endif
};

/** @brief The device extensions. */
static const char* DEVICE_EXTENSIONS[] = {
    "VK_KHR_swapchain",
#if ON_APPLE
    "VK_KHR_portability_subset",
#endif
};

CuResult vk_result_to_cu_result(
    VkResult result)
{
    switch (result) {
        case VK_SUCCESS:
            return CU_SUCCESS;
        case VK_TIMEOUT:
            return CU_ERROR_TIMEOUT;
        case VK_ERROR_OUT_OF_HOST_MEMORY:
            return CU_ERROR_OUT_OF_RAM;
        case VK_ERROR_OUT_OF_DEVICE_MEMORY:
            return CU_ERROR_OUT_OF_VRAM;
        case VK_ERROR_LAYER_NOT_PRESENT:
        case VK_ERROR_EXTENSION_NOT_PRESENT:
        case VK_ERROR_FEATURE_NOT_PRESENT:
        case VK_ERROR_INCOMPATIBLE_DRIVER:
            return CU_ERROR_UNSUPPORTED;
        default:
            return CU_ERROR_UNKNOWN;
    }
}

VkResult create_vk_instance(
    VkInstance* const pInstance,
    const char* const appName,
    const uint32_t appVersion
) {
    assert(pInstance != nullptr);

    const VkApplicationInfo appInfo = {
        .sType = VK_STRUCTURE_TYPE_APPLICATION_INFO,
        .pNext = nullptr,
        .pApplicationName = appName,
        .applicationVersion = appVersion,
        .pEngineName = "Cutl",
        .engineVersion = CUTL_VK_VERSION,
        .apiVersion = VK_API_VERSION_1_4,
    };
    constexpr VkInstanceCreateFlags flags =
        ON_APPLE ? VK_INSTANCE_CREATE_ENUMERATE_PORTABILITY_BIT_KHR : 0;
    const VkInstanceCreateInfo createInfo = {
        .sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO,
        .pNext = nullptr,
        .flags = flags,
        .pApplicationInfo = &appInfo,
        .enabledLayerCount = 0,
        .ppEnabledLayerNames = nullptr,
        .enabledExtensionCount = arr_len(INSTANCE_EXTENSIONS),
        .ppEnabledExtensionNames = INSTANCE_EXTENSIONS,
    };
    return vkCreateInstance(&createInfo, nullptr, pInstance);
}

VkResult create_device(
    VkDevice* const pDevice,
    const VkPhysicalDevice physicalDevice,
    const uint32_t iQueueFamily
) {
    assert(pDevice != nullptr);
    assert(physicalDevice != VK_NULL_HANDLE);

    const float queuePriorities[] = { 1.0F };
    const VkDeviceQueueCreateInfo queueCreateInfos[] = {
        {
            .sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO,
            .pNext = nullptr,
            .flags = 0,
            .queueFamilyIndex = iQueueFamily,
            .queueCount = 1,
            .pQueuePriorities = queuePriorities,
        },
    };

    const VkPhysicalDeviceVulkan14Features features14 = {
        .sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_4_FEATURES,
        .pNext = nullptr,
        .maintenance5 = VK_TRUE,
    };
    const VkPhysicalDeviceVulkan13Features features13 = {
        .sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_3_FEATURES,
        .pNext = (void*)&features14,
        .synchronization2 = VK_TRUE,
    };
    const VkPhysicalDeviceVulkan12Features features12 = {
        .sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_2_FEATURES,
        .pNext = (void*)&features13,
        .bufferDeviceAddress = VK_TRUE,
    };
    const VkPhysicalDeviceVulkan11Features features11 = {
        .sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_1_FEATURES,
        .pNext = (void*)&features12,
    };

    const VkDeviceCreateInfo createInfo = {
        .sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO,
        .pNext = &features11,
        .flags = 0,
        .queueCreateInfoCount = arr_len(queueCreateInfos),
        .pQueueCreateInfos = queueCreateInfos,
        .enabledLayerCount = 0,
        .ppEnabledLayerNames = nullptr,
        .enabledExtensionCount = arr_len(DEVICE_EXTENSIONS),
        .ppEnabledExtensionNames = DEVICE_EXTENSIONS,
    };
    return vkCreateDevice(physicalDevice, &createInfo, nullptr, pDevice);
}

VkResult create_command_pool(
    VkCommandPool* pCommandPool,
    VkDevice device,
    uint32_t iQueueFamily)
{
    assert(pCommandPool != nullptr);
    assert(device != VK_NULL_HANDLE);

    const VkCommandPoolCreateInfo createInfo = {
        .sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO,
        .pNext = nullptr,
        .flags = 0,
        .queueFamilyIndex = iQueueFamily,
    };
    return vkCreateCommandPool(device, &createInfo, nullptr, pCommandPool);
}

VkResult create_image_view(
    VkImageView* const pImageView,
    const VkDevice device,
    const VkImage image,
    const VkFormat format)
{
    assert(pImageView != nullptr);
    assert(device != VK_NULL_HANDLE);
    assert(image != VK_NULL_HANDLE);

    const VkComponentMapping componentMapping = {
        .r = VK_COMPONENT_SWIZZLE_IDENTITY,
        .g = VK_COMPONENT_SWIZZLE_IDENTITY,
        .b = VK_COMPONENT_SWIZZLE_IDENTITY,
        .a = VK_COMPONENT_SWIZZLE_IDENTITY,
    };
    const VkImageSubresourceRange subresourceRange = {
        .aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,
        .baseMipLevel = 0,
        .levelCount = 1,
        .baseArrayLayer = 0,
        .layerCount = 1,
    };
    const VkImageViewCreateInfo createInfo = {
        .sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO,
        .pNext = nullptr,
        .flags = 0,
        .image = image,
        .viewType = VK_IMAGE_VIEW_TYPE_2D,
        .format = format,
        .components = componentMapping,
        .subresourceRange = subresourceRange,
    };
    return vkCreateImageView(device, &createInfo, nullptr, pImageView);
}

VkResult create_semaphore(
    VkSemaphore* const pSemaphore,
    const VkDevice device,
    const VkSemaphoreType type,
    const uint64_t x)
{
    assert(pSemaphore != nullptr);
    assert(device != VK_NULL_HANDLE);

    const VkSemaphoreTypeCreateInfo typeCreateInfo = {
        .sType = VK_STRUCTURE_TYPE_SEMAPHORE_TYPE_CREATE_INFO,
        .pNext = nullptr,
        .semaphoreType = type,
        .initialValue = x,
    };
    const VkSemaphoreCreateInfo createInfo = {
        .sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO,
        .pNext = &typeCreateInfo,
        .flags = 0,
    };
    return vkCreateSemaphore(device, &createInfo, nullptr, pSemaphore);
}

VkResult allocate_memory(
    VkDeviceMemory* const pMemory,
    const VkDevice device,
    const uint64_t size,
    const uint32_t i)
{
    const VkMemoryAllocateFlagsInfo allocateFlagsInfo = {
        .sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_FLAGS_INFO,
        .pNext = nullptr,
        .flags = VK_MEMORY_ALLOCATE_DEVICE_ADDRESS_BIT,
        .deviceMask = 1,
    };
    const VkMemoryAllocateInfo allocateInfo = {
        .sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO,
        .pNext = &allocateFlagsInfo,
        .allocationSize = size,
        .memoryTypeIndex = i,
    };
    return vkAllocateMemory(device, &allocateInfo, nullptr, pMemory);
}

VkResult create_frames(
    CuFrame* const pFrames,
    const size_t nFrames,
    const VkDevice device,
    const VkCommandPool commandPool)
{
    assert(pFrames != nullptr);
    assert(nFrames > 0);
    assert(device != VK_NULL_HANDLE);
    assert(commandPool != VK_NULL_HANDLE);

    VkResult result = VK_ERROR_UNKNOWN;

    VkCommandBuffer commandBuffers[nFrames];
    const VkCommandBufferAllocateInfo allocateInfo = {
        .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO,
        .pNext = nullptr,
        .commandPool = commandPool,
        .level = VK_COMMAND_BUFFER_LEVEL_PRIMARY,
        .commandBufferCount = nFrames,
    };
    vk_try_catch(vkAllocateCommandBuffers(device, &allocateInfo, commandBuffers));

    for (size_t i = 0; i < nFrames; i++) {
        CuFrame* const pFrame = &pFrames[i];

        const VkSemaphoreCreateInfo semaphoreCreateInfo = {
            .sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO,
            .pNext = nullptr,
            .flags = 0,
        };
        vk_try_catch(vkCreateSemaphore(
            device,
            &semaphoreCreateInfo,
            nullptr,
            &pFrame->_imageAvailable
        ));
        vk_try_catch(create_semaphore(
            &pFrame->_imageAvailable, device, VK_SEMAPHORE_TYPE_BINARY, 0));
        pFrame->_commandBuffer = commandBuffers[i];
    }

    return VK_SUCCESS;

FAIL:
    destroy_frames(pFrames, nFrames, device, commandPool);
    return result;
}

void destroy_frames(
    CuFrame* const pFrames,
    const size_t nFrames,
    const VkDevice device,
    const VkCommandPool commandPool)
{
    assert(pFrames != nullptr);
    assert(nFrames > 0);
    assert(device != VK_NULL_HANDLE);

    VkCommandBuffer commandBuffers[nFrames];

    for (size_t i = 0; i < nFrames; i++) {
        CuFrame* const pFrame = &pFrames[i];

        commandBuffers[i] = pFrame->_commandBuffer;
        vkDestroySemaphore(device, pFrame->_imageAvailable, nullptr);
    }

    vkFreeCommandBuffers(device, commandPool, nFrames, commandBuffers);
}

