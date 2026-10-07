#include "context.h"

#include "g_context.h"
#include "info.h"
#include "physical_device.h"
#include "result.h"
#include "util.h"
#include "vk.h"

#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <vulkan/vulkan_core.h>

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <vulkan/vulkan.h>

/// @brief Creates the global context's Vulkan instance.
/// @param appName The application name.
/// @param appVersion The application version.
/// @param enableValidation Whether to enable validation layers.
/// @return The result of creating the instance.
static VkResult create_vk_instance(
    const char* appName,
    uint32_t appVersion,
    bool enableValidation);

/// @brief Sets the functions for creating and destroying a debug messenger.
/// @return Whether both functions were retrieved successfully.
static bool get_debug_messenger_fns();

/// @brief The debug messenger callback.
/// @param severity The message's severity.
/// @param types A bitmask of the message's types.
/// @param pCallbackData A pointer to the message's callback data.
/// @param pUserData The user data pointer given at the messenger's creation.
/// @return Always `VK_FALSE`.
static VKAPI_ATTR VkBool32 VKAPI_CALL debug_messenger_callback(
    VkDebugUtilsMessageSeverityFlagBitsEXT severity,
    VkDebugUtilsMessageTypeFlagsEXT types,
    const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
    void* pUserData);

/// @brief Creates the global context's debug messenger.
/// @return The result of creating the debug messenger.
static VkResult create_debug_messenger();

/// @brief Creates the global context's logical device.
/// @return The result of creating the device.
static VkResult create_device();

/// @brief Creates the global context's command pool.
/// @return The result of creating the command pool.
static VkResult create_command_pool();

/// @brief Creates the global context's descriptor pool.
/// @param nSamplerDescriptors The number of sampler descriptors.
/// @param nSampledImageDescriptors The number of sampled image descriptors.
/// @param nStorageImageDescriptors The number of storage image descriptors.
/// @return The result of creating the descriptor pool.
static VkResult create_descriptor_pool(
    uint32_t nSamplerDescriptors,
    uint32_t nSampledImageDescriptors,
    uint32_t nStorageImageDescriptors);

/// @brief Creates the global context's descriptor set layout.
/// @param nSamplerDescriptors The number of sampler descriptors.
/// @param nSampledImageDescriptors The number of sampled image descriptors.
/// @param nStorageImageDescriptors The number of storage image descriptors.
/// @return the result of creating the descriptor set layout.
static VkResult create_descriptor_set_layout(
    uint32_t nSamplerDescriptors,
    uint32_t nSampledImageDescriptors,
    uint32_t nStorageImageDescriptors);

/// @brief Creates the global context's descriptor set.
/// @return The result of creating the descriptor set.
static VkResult allocate_descriptor_set();

/// @brief The Cutl version in Vulkan format.
constexpr uint32_t CUTL_VK_VERSION =
    VK_MAKE_VERSION(CU_VERSION_MAJOR, CU_VERSION_MINOR, CU_VERSION_PATCH);

/// @brief The instance layers.
static const char* INSTANCE_LAYERS[] = {
    // Debugging layers:
    "VK_LAYER_KHRONOS_validation",
};

/// @brief The number of debugging instance layers.
constexpr uint32_t N_DBG_INSTANCE_LAYERS = 1;

/// @brief The number of instance layers with debugging.
constexpr uint32_t N_INSTANCE_LAYERS_WITH_DBG = arr_len(INSTANCE_LAYERS);

/// @brief The number of instance layers with no debugging.
constexpr uint32_t N_INSTANCE_LAYERS_NO_DBG = N_INSTANCE_LAYERS_WITH_DBG - N_DBG_INSTANCE_LAYERS;

/// @brief The instance extensions.
static const char* INSTANCE_EXTENSIONS[] = {
    "VK_KHR_surface",
    "VK_EXT_headless_surface",
#if ON_APPLE
    "VK_EXT_metal_surface",
    "VK_KHR_portability_enumeration",
#elif ON_LINUX
    "VK_KHR_xcb_surface",
#elif ON_WINDOWS
    "VK_KHR_win32_surface",
#endif
    // Debugging extensions:
    "VK_EXT_debug_utils",
};

/// @brief The number of debugging instance extensions.
constexpr uint32_t N_DBG_INSTANCE_EXTENSIONS = 1;

/// @brief The number of instance extensions with debugging.
constexpr uint32_t N_INSTANCE_EXTENSIONS_WITH_DBG = arr_len(INSTANCE_EXTENSIONS);

/// @brief The number of instance extensions with no debugging.
constexpr uint32_t N_INSTANCE_EXTENSIONS_NO_DBG = N_INSTANCE_EXTENSIONS_WITH_DBG - N_DBG_INSTANCE_EXTENSIONS;

/// @brief The device extensions.
static const char* DEVICE_EXTENSIONS[] = {
    "VK_KHR_swapchain",
#if ON_APPLE
    "VK_KHR_portability_subset",
#endif
};

/// @brief The callback for creating a debug messenger.
static PFN_vkCreateDebugUtilsMessengerEXT fCreateDebugMessenger = nullptr;

/// @brief The callback for destroying a debug messenger.
static PFN_vkDestroyDebugUtilsMessengerEXT fDestroyDebugMessenger = nullptr;

CuContext gContext = { ._isInitialized = false };

CuContext* cu_context_get()
{
    return &gContext;
}

CuResult cu_context_init(
    const CuContextCreateInfo* pCreateInfo)
{
    CuResult result = CU_SUCCESS;

    cu_assert_catch(glfwInit() == GLFW_TRUE, cu_glfw_error());

    const uint32_t appVersion = VK_MAKE_VERSION(
        pCreateInfo->appVersion.major,
        pCreateInfo->appVersion.minor,
        pCreateInfo->appVersion.patch);
    cu_try_catch_vk(create_vk_instance(pCreateInfo->appName, appVersion, pCreateInfo->enableValidation));
    if (pCreateInfo->enableValidation) {
        cu_try_catch_vk(create_debug_messenger());
    }
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
    cu_try_catch_vk(create_descriptor_pool(
        pCreateInfo->nSampledImageDescriptors,
        pCreateInfo->nSamplerDescriptors,
        pCreateInfo->nStorageImageDescriptors));
    cu_try_catch_vk(create_descriptor_set_layout(
        pCreateInfo->nSampledImageDescriptors,
        pCreateInfo->nSamplerDescriptors,
        pCreateInfo->nStorageImageDescriptors));
    cu_try_catch_vk(allocate_descriptor_set());

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
        vkDestroyDescriptorSetLayout(gContext._device, gContext._descriptorSetLayout, nullptr);
        vkDestroyDescriptorPool(gContext._device, gContext._descriptorPool, nullptr);
        vkDestroyCommandPool(gContext._device, gContext._commandPool, nullptr);
        vkDestroyDevice(gContext._device, nullptr);
    }

    if (gContext._instance != VK_NULL_HANDLE) {
        if (gContext._debugMessenger != VK_NULL_HANDLE) {
            fDestroyDebugMessenger(gContext._instance, gContext._debugMessenger, nullptr);
        }
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
    const uint32_t appVersion,
    const bool enableValidation)
{
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
        .enabledLayerCount = enableValidation ? N_INSTANCE_LAYERS_WITH_DBG : N_INSTANCE_LAYERS_NO_DBG,
        .ppEnabledLayerNames = INSTANCE_LAYERS,
        .enabledExtensionCount = enableValidation ? N_INSTANCE_EXTENSIONS_WITH_DBG : N_INSTANCE_EXTENSIONS_NO_DBG,
        .ppEnabledExtensionNames = INSTANCE_EXTENSIONS,
    };
    return vkCreateInstance(&createInfo, nullptr, &gContext._instance);
}

bool get_debug_messenger_fns()
{
    fCreateDebugMessenger = (PFN_vkCreateDebugUtilsMessengerEXT)vkGetInstanceProcAddr(
        gContext._instance, "vkCreateDebugUtilsMessengerEXT");
    cu_assert(fCreateDebugMessenger != nullptr, false);
    fDestroyDebugMessenger = (PFN_vkDestroyDebugUtilsMessengerEXT)vkGetInstanceProcAddr(
        gContext._instance, "vkDestroyDebugUtilsMessengerEXT");
    cu_assert(fDestroyDebugMessenger != nullptr, false);

    return true;
}

VKAPI_ATTR VkBool32 VKAPI_CALL debug_messenger_callback(
    const VkDebugUtilsMessageSeverityFlagBitsEXT _severity,
    const VkDebugUtilsMessageTypeFlagsEXT _types,
    const VkDebugUtilsMessengerCallbackDataEXT* const pCallbackData,
    void* const _pUserData)
{
    (void)_severity;
    (void)_types;
    (void)_pUserData;

    fputs(pCallbackData->pMessage, stderr);

    return VK_FALSE;
}

VkResult create_debug_messenger()
{
    cu_assert(get_debug_messenger_fns(), VK_ERROR_EXTENSION_NOT_PRESENT);

    const VkDebugUtilsMessengerCreateInfoEXT createInfo = {
        .sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT,
        .pNext = nullptr,
        .flags = 0,
        .messageSeverity =
            VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT |
            VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT,
        .messageType =
            VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT |
            VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT |
            VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT,
        .pfnUserCallback = &debug_messenger_callback,
        .pUserData = nullptr,
    };
    return fCreateDebugMessenger(gContext._instance, &createInfo, nullptr, &gContext._debugMessenger);
}

VkResult create_device()
{
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
        .dynamicRendering = VK_TRUE,
    };
    const VkPhysicalDeviceVulkan12Features features12 = {
        .sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_2_FEATURES,
        .pNext = (void*)&features13,
        .shaderSampledImageArrayNonUniformIndexing = VK_TRUE,
        .descriptorBindingSampledImageUpdateAfterBind = VK_TRUE,
        .descriptorBindingStorageImageUpdateAfterBind = VK_TRUE,
        .descriptorBindingUpdateUnusedWhilePending = VK_TRUE,
        .descriptorBindingPartiallyBound = VK_TRUE,
        .runtimeDescriptorArray = VK_TRUE,
        .timelineSemaphore = VK_TRUE,
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
        .flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT,
        .queueFamilyIndex = gContext._iQueueFamily,
    };
    return vkCreateCommandPool(gContext._device, &createInfo, nullptr, &gContext._commandPool);
}

VkResult create_descriptor_pool(
    const uint32_t nSampledImageDescriptors,
    const uint32_t nSamplerDescriptors,
    const uint32_t nStorageImageDescriptors)
{
    const VkDescriptorPoolSize poolSizes[] = {
        {
            .type = VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE,
            .descriptorCount = nSampledImageDescriptors,
        },
        {
            .type = VK_DESCRIPTOR_TYPE_SAMPLER,
            .descriptorCount = nSamplerDescriptors,
        },
        {
            .type = VK_DESCRIPTOR_TYPE_STORAGE_IMAGE,
            .descriptorCount = nStorageImageDescriptors,
        },
    };
    constexpr uint32_t nPoolSizes = arr_len(poolSizes);
    const VkDescriptorPoolCreateInfo createInfo = {
        .sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO,
        .pNext = nullptr,
        .flags = VK_DESCRIPTOR_POOL_CREATE_UPDATE_AFTER_BIND_BIT,
        .maxSets = 1,
        .poolSizeCount = nPoolSizes,
        .pPoolSizes = poolSizes,
    };
    return vkCreateDescriptorPool(gContext._device, &createInfo, nullptr, &gContext._descriptorPool);
}

VkResult create_descriptor_set_layout(
    const uint32_t nSampledImageDescriptors,
    const uint32_t nSamplerDescriptors,
    const uint32_t nStorageImageDescriptors)
{
    const VkDescriptorSetLayoutBinding bindings[] = {
        {
            .binding = 0,
            .descriptorType = VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE,
            .descriptorCount = nSampledImageDescriptors,
            .stageFlags = VK_SHADER_STAGE_ALL,
            .pImmutableSamplers = nullptr,
        },
        {
            .binding = 1,
            .descriptorType = VK_DESCRIPTOR_TYPE_SAMPLER,
            .descriptorCount = nSamplerDescriptors,
            .stageFlags = VK_SHADER_STAGE_ALL,
            .pImmutableSamplers = nullptr,
        },
        {
            .binding = 2,
            .descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_IMAGE,
            .descriptorCount = nStorageImageDescriptors,
            .stageFlags = VK_SHADER_STAGE_ALL,
            .pImmutableSamplers = nullptr,
        },
    };
    constexpr uint32_t nBindings = arr_len(bindings);
    constexpr VkDescriptorBindingFlags flags = 
        VK_DESCRIPTOR_BINDING_UPDATE_AFTER_BIND_BIT |
        VK_DESCRIPTOR_BINDING_UPDATE_UNUSED_WHILE_PENDING_BIT |
        VK_DESCRIPTOR_BINDING_PARTIALLY_BOUND_BIT;
    const VkDescriptorBindingFlags bindingFlags[nBindings] = {
        flags, flags, flags,
    };
    const VkDescriptorSetLayoutBindingFlagsCreateInfo bindingFlagsCreateInfo = {
        .sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_BINDING_FLAGS_CREATE_INFO,
        .pNext = nullptr,
        .bindingCount = nBindings,
        .pBindingFlags = bindingFlags,
    };
    const VkDescriptorSetLayoutCreateInfo createInfo = {
        .sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO,
        .pNext = &bindingFlagsCreateInfo,
        .flags = VK_DESCRIPTOR_SET_LAYOUT_CREATE_UPDATE_AFTER_BIND_POOL_BIT,
        .bindingCount = nBindings,
        .pBindings = bindings,
    };
    return vkCreateDescriptorSetLayout(
        gContext._device, &createInfo, nullptr, &gContext._descriptorSetLayout);
}

VkResult allocate_descriptor_set()
{
    const VkDescriptorSetAllocateInfo allocateInfo = {
        .sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO,
        .pNext = nullptr,
        .descriptorPool = gContext._descriptorPool,
        .descriptorSetCount = 1,
        .pSetLayouts = &gContext._descriptorSetLayout,
    };
    return vkAllocateDescriptorSets(gContext._device, &allocateInfo, &gContext._descriptorSet);
}
