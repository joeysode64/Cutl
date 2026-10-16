#include "physical_device.h"

#include "enumerator.h"
#include "log.h"
#include "result.h"
#include "util.h"

#include <stdint.h>
#include <string.h>
#include <vulkan/vulkan.h>

/** @brief Returns a bitmask of the memory types that contain all the memory properties. */
static uint32_t get_memory_types(
    const CuPhysicalDeviceMemoryInfo* const pMemoryInfo,
    const VkMemoryPropertyFlags flags)
{
    uint32_t m = 0;
    if (ones_overlap(flags, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT)) {
        m |= pMemoryInfo->_mDeviceLocal;
    }
    if (ones_overlap(flags, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT)) {
        m |= pMemoryInfo->_mHostVisible;
    }
    if (ones_overlap(flags, VK_MEMORY_PROPERTY_HOST_COHERENT_BIT)) {
        m |= pMemoryInfo->_mHostCoherent;
    }
    if (ones_overlap(flags, VK_MEMORY_PROPERTY_HOST_CACHED_BIT)) {
        m |= pMemoryInfo->_mHostCached;
    }
    return m;
}

/** @brief Returns whether the physical device supports all the extensions. */
static bool does_physical_device_support_extensions(
    VkPhysicalDevice physicalDevice,
    const char* pExtensions[],
    uint32_t nExtensions)
{
    Enumerator(VkExtensionProperties) extensions ENUMERATOR_AUTO_FREE = {};
    enumerate(extensions, false, vkEnumerateDeviceExtensionProperties, physicalDevice, nullptr);
    for (uint32_t i = 0; i < nExtensions; i++) {
        bool found = false;
        for (uint32_t j = 0; j < extensions.n; j++) {
            if (strcmp(pExtensions[i], extensions.p[j].extensionName) == 0) {
                found = true;
                break;
            }
        } 
        if (!found) {
            return false;
        }
    }

    return true;
}

/** @brief Returns whether the physical device supports all the required features. */
static bool does_physical_device_support_features(
    VkPhysicalDevice physicalDevice)
{
    VkPhysicalDeviceVulkan14Features features1_4 = {
        .sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_4_FEATURES,
        .pNext = nullptr,
    };
    VkPhysicalDeviceVulkan13Features features1_3 = {
        .sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_3_FEATURES,
        .pNext = &features1_4,
    };
    VkPhysicalDeviceVulkan12Features features1_2 = {
        .sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_2_FEATURES,
        .pNext = &features1_3,
    };
    VkPhysicalDeviceVulkan11Features features1_1 = {
        .sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_1_FEATURES,
        .pNext = &features1_2,
    };
    VkPhysicalDeviceFeatures2 features1_0 = {
        .sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FEATURES_2,
        .pNext = &features1_1,
    };
    vkGetPhysicalDeviceFeatures2(physicalDevice, &features1_0);
    return features1_2.shaderSampledImageArrayNonUniformIndexing == VK_TRUE &&
        features1_2.descriptorBindingSampledImageUpdateAfterBind == VK_TRUE &&
        features1_2.descriptorBindingStorageImageUpdateAfterBind == VK_TRUE &&
        features1_2.descriptorBindingUpdateUnusedWhilePending == VK_TRUE &&
        features1_2.descriptorBindingPartiallyBound == VK_TRUE &&
        features1_2.runtimeDescriptorArray == VK_TRUE &&
        features1_2.timelineSemaphore == VK_TRUE &&
        features1_2.bufferDeviceAddress == VK_TRUE &&
        features1_3.synchronization2 == VK_TRUE &&
        features1_3.dynamicRendering == VK_TRUE &&
        features1_4.maintenance5 == VK_TRUE;
}

/** @brief Returns whether the physical device is suitable. */
static bool is_physical_device_suitable(
    const VkPhysicalDevice physicalDevice,
    const char* pExtensions[],
    const uint32_t nExtensions)
{
    return does_physical_device_support_extensions(physicalDevice, pExtensions, nExtensions) &&
        does_physical_device_support_features(physicalDevice);
}

/** @brief Returns whether the queue family at the index supports presentation. */
static bool queue_family_supports_presentation(
    const uint32_t i, 
    const VkInstance instance,
    const VkPhysicalDevice physicalDevice)
{
    (void)i;
    (void)instance;
    (void)physicalDevice;

#if ON_APPLE
    // All queues support presentation with MoltenVK.
    return true;
#elif ON_LINUX
    return true;
#else
    return false;
#endif
}

/** @brief Choose the queue family. Returns whether one was found. */
static bool choose_queue_family(
    uint32_t* const piQueueFamily,
    const VkInstance instance,
    const VkPhysicalDevice physicalDevice)
{
    Enumerator(VkQueueFamilyProperties) queueFamilies ENUMERATOR_AUTO_FREE = {};
    enumerate(queueFamilies, false, vkGetPhysicalDeviceQueueFamilyProperties, physicalDevice);

    for (uint32_t i = 0; i < queueFamilies.n; i++) {
        constexpr VkQueueFlags flags =
            VK_QUEUE_GRAPHICS_BIT | VK_QUEUE_COMPUTE_BIT | VK_QUEUE_TRANSFER_BIT;
        const bool hasFlags = ones_match(queueFamilies.p[i].queueFlags, flags);

        if (hasFlags && queue_family_supports_presentation(i, instance, physicalDevice)) {
            *piQueueFamily = i;
            return true;
        }
    }

    return false;
}

/** @brief Returns a score for the physical device. A higher value is better. */
static uint32_t grade_physical_device(
    VkPhysicalDevice physicalDevice)
{
    uint32_t score = 0;
    VkPhysicalDeviceProperties properties = {};
    vkGetPhysicalDeviceProperties(physicalDevice, &properties);

    constexpr uint32_t TOP_BIT = 1U << 31;
    if (ones_overlap(properties.deviceType, VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU)) {
        score |= TOP_BIT;
    } else if (ones_overlap(properties.deviceType, VK_PHYSICAL_DEVICE_TYPE_INTEGRATED_GPU)) {
        score |= TOP_BIT >> 1;
    }

    return score;
}

/** @brief Returns the physical device's memory type info. */
static CuPhysicalDeviceMemoryInfo get_physical_device_memory_info(
    VkPhysicalDevice physicalDevice)
{
    VkPhysicalDeviceMemoryProperties memoryProperties = {};
    vkGetPhysicalDeviceMemoryProperties(physicalDevice, &memoryProperties);

    CuPhysicalDeviceMemoryInfo memoryInfo = {};
    for (uint32_t i = 0; i < memoryProperties.memoryTypeCount; i++) {
        const VkMemoryPropertyFlags flags = memoryProperties.memoryTypes[i].propertyFlags;
        const uint32_t b = 1u << i;
        if (ones_overlap(flags, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT)) {
            memoryInfo._mDeviceLocal |= b;
        }
        if (ones_overlap(flags, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT)) {
            memoryInfo._mHostVisible |= b;
        }
        if (ones_overlap(flags, VK_MEMORY_PROPERTY_HOST_COHERENT_BIT)) {
            memoryInfo._mHostCoherent |= b;
        }
        if (ones_overlap(flags, VK_MEMORY_PROPERTY_HOST_CACHED_BIT)) {
            memoryInfo._mHostCached |= b;
        }
    }

    return memoryInfo;
}

uint32_t find_memory_types(
    const CuPhysicalDeviceMemoryInfo* const pMemoryInfo,
    const VkMemoryPropertyFlags mProperties,
    const uint32_t mAllowed)
{
    return get_memory_types(pMemoryInfo, mProperties) & mAllowed;
}

/** @brief Chooses the best-fit physical device. */
CuResult choose_physical_device(
    PhysicalDeviceInfo* const pPhysicalDeviceInfo,
    const VkInstance instance,
    const char* pExtensions[],
    const uint32_t nExtensions)
{
    Enumerator(VkPhysicalDevice) physicalDevices ENUMERATOR_AUTO_FREE = {};
    enumerate_cu(physicalDevices, vkEnumeratePhysicalDevices, instance);

    uint64_t bestScore = 0;
    bool found = false;
    for (uint32_t i = 0; i < physicalDevices.n; i++) {
        const VkPhysicalDevice contestant = physicalDevices.p[i];

#if defined(CU_DEBUGGING)
        VkPhysicalDeviceProperties properties = {};
        vkGetPhysicalDeviceProperties(contestant, &properties);
        cu_log_note("Querying device %p (%s):", (void*)contestant, properties.deviceName);
#endif

        if (!is_physical_device_suitable(contestant, pExtensions, nExtensions)) {
            cu_log_note("Device not suitable");
            continue;
        }

        uint32_t iQueueFamily = 0;
        if (!choose_queue_family(&iQueueFamily, instance, contestant)) {
            cu_log_note("Device has no suitable queue family");
            continue;
        }

        const uint32_t newScore = grade_physical_device(contestant);
        cu_log_note("Device was scored %u", newScore);
        if (!found || newScore > bestScore) {
            *pPhysicalDeviceInfo = (PhysicalDeviceInfo){
                .handle = contestant,
                .iQueueFamily = iQueueFamily,
                .memoryInfo = get_physical_device_memory_info(contestant),
            };
            bestScore = newScore;
            found = true;
        }
    }

    if (found) {
        cu_log_note("Chose device %p with queue family #%u",
            (void*)pPhysicalDeviceInfo->handle,
            pPhysicalDeviceInfo->iQueueFamily);
        return CU_SUCCESS;
    } else {
        cu_log_note("No suitable devies found");
        return cu_error(CU_ERROR_NO_VALID_DEVICE);
    }
}
