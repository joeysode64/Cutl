#include "context.h"

#include "g_context.h"
#include "info.h"
#include "physical_device.h"
#include "result.h"
#include "util.h"
#include "vk.h"

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <vulkan/vulkan.h>

/// @brief Creates the global context's Vulkan instance.
/// @param appName The application name.
/// @param appVersion The application version.
/// @return The result of creating the instance.
static VkResult create_vk_instance(
    const char* appName,
    uint32_t appVersion);

/// @brief Creates the global context's logical device.
/// @return The result of creating the device.
/// @warning This requires the context's device and physical device info to be initialized.
static VkResult create_device();

/// @brief Creates the global context's command pool.
/// @return The result of creating the command pool.
/// @warning This requires the context's device and physical device info to be initialized.
static VkResult create_command_pool();

/// @brief The Cutl version in Vulkan format.
constexpr uint32_t CUTL_VK_VERSION =
    VK_MAKE_VERSION(CU_VERSION_MAJOR, CU_VERSION_MINOR, CU_VERSION_PATCH);

/// @brief The instance extensions.
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

/// @brief The device extensions.
static const char* DEVICE_EXTENSIONS[] = {
    "VK_KHR_swapchain",
#if ON_APPLE
    "VK_KHR_portability_subset",
#endif
};

CuContext gContext = { ._isInitialized = false };

CuContext* cu_context_get()
{
    return &gContext;
}

CuResult cu_context_init(
    const CuContextCreateInfo* pCreateInfo)
{
    CuResult result = CU_ERROR_UNKNOWN;

    cu_assert_catch(glfwInit() == GLFW_TRUE, CU_ERROR_GLFW_INIT);

    const uint32_t appVersion = VK_MAKE_VERSION(
        pCreateInfo->appVersion.major,
        pCreateInfo->appVersion.minor,
        pCreateInfo->appVersion.patch);
    cu_try_catch_vk(create_vk_instance(pCreateInfo->appName, appVersion));
    PhysicalDeviceInfo physicalDeviceInfo = {};
    cu_try_catch(choose_physical_device(&physicalDeviceInfo, gContext._instance));
    gContext._physicalDevice = physicalDeviceInfo.handle;
    gContext._memoryInfo = physicalDeviceInfo.memoryInfo;
    gContext._iQueueFamily = physicalDeviceInfo.iQueueFamily;
    cu_try_catch_vk(create_device());
    cu_try_catch_vk(create_command_pool());
    vkGetDeviceQueue(
        gContext._device,
        gContext._iQueueFamily,
        0,
        &gContext._queue
    );

    gContext._isInitialized = true;
    return CU_SUCCESS;

FAIL:
    cu_context_terminate();
    return result;
}

void cu_context_terminate()
{
    glfwTerminate();

    if (gContext._device != VK_NULL_HANDLE) {
        vkDeviceWaitIdle(gContext._device);
        vkDestroyCommandPool(gContext._device, gContext._commandPool, nullptr);
        vkDestroyDevice(gContext._device, nullptr);
    }

    if (gContext._instance != VK_NULL_HANDLE) {
        vkDestroyInstance(gContext._instance, nullptr);
    }

    gContext = (CuContext){
        ._isInitialized = false,
    };
}

void cu_context_wait_for_idle()
{
    vkDeviceWaitIdle(gContext._device);
}


VkResult create_vk_instance(
    const char* const appName,
    const uint32_t appVersion
) {
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
    return vkCreateInstance(&createInfo, nullptr, &gContext._instance);
}

VkResult create_device() {
    const float queuePriorities[] = { 1.0F };
    const VkDeviceQueueCreateInfo queueCreateInfos[] = {
        {
            .sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO,
            .pNext = nullptr,
            .flags = 0,
            .queueFamilyIndex = gContext._iQueueFamily,
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
    return vkCreateDevice(
        gContext._physicalDevice,
        &createInfo,
        nullptr,
        &gContext._device);
}

VkResult create_command_pool()
{
    const VkCommandPoolCreateInfo createInfo = {
        .sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO,
        .pNext = nullptr,
        .flags = 0,
        .queueFamilyIndex = gContext._iQueueFamily,
    };
    return vkCreateCommandPool(gContext._device, &createInfo, nullptr, &gContext._commandPool);
}
