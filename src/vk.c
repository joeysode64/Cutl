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
constexpr uint32_t CU_VK_VERSION =
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

CuResult vk_result_to_cu_result(VkResult result) {
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
        .engineVersion = CU_VK_VERSION,
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
    const VkDeviceCreateInfo createInfo = {
        .sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO,
        .pNext = nullptr,
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

VkResult create_command_pool(VkCommandPool* pCommandPool, VkDevice device, uint32_t iQueueFamily) {
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
