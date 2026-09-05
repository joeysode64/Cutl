#include "physical_device.h"

#include "result.h"
#include "util.h"

#include <stdint.h>
#include <vulkan/vulkan_core.h>

/** @brief Returns a bitmask of the memory types that contain all the memory properties. */
static uint32_t get_memory_types(
    const PhysicalDeviceMemoryInfo* pMemoryInfo,
    VkMemoryPropertyFlags flags);

/** @brief Returns whether the physical device is suitable. */
static bool is_physical_device_suitable(
    VkPhysicalDevice physicalDevice);

/** @brief Choose the queue family. Returns whether one was found. */
static bool choose_queue_family(
    uint32_t* piQueueFamily,
    VkInstance vkInstance,
    VkPhysicalDevice physicalDevice);

/** @brief Returns whether the queue family at the index supports presentation. */
static bool queue_family_supports_presentation(
    uint32_t i, 
    VkInstance vkInstance, 
    VkPhysicalDevice physicalDevice);

/** @brief Returns a score for the physical device. A higher value is better. */
static uint32_t grade_physical_device(
    VkPhysicalDevice physicalDevice);

/** @brief Returns the physical device's memory type info. */
static PhysicalDeviceMemoryInfo get_physical_device_memory_info(
    VkPhysicalDevice physicalDevice);

uint32_t find_memory_types(
    const PhysicalDeviceMemoryInfo* const pMemoryInfo,
    const VkMemoryPropertyFlags mRequired,
    const VkMemoryPropertyFlags mPreferred,
    const uint32_t mAllowed)
{
    const uint32_t mSuitable = get_memory_types(pMemoryInfo, mRequired) & mAllowed;
    const uint32_t mIdeal = get_memory_types(pMemoryInfo, mPreferred) & mSuitable;

    return (mIdeal != 0) ? mIdeal : mSuitable;
}

/** @brief Chooses the best-fit physical device. */
CuResult choose_physical_device(
    PhysicalDeviceInfo* const pPhysicalDeviceInfo,
    const VkInstance vkInstance)
{
    VkPhysicalDevice* pPhysicalDevices AUTO_FREE = nullptr;
    uint32_t nPhysicalDevices = 0;
    vkEnumeratePhysicalDevices(vkInstance, &nPhysicalDevices, nullptr);
    allocate_n(pPhysicalDevices, nPhysicalDevices);
    vkEnumeratePhysicalDevices(vkInstance, &nPhysicalDevices, pPhysicalDevices);

    uint64_t bestScore = 0;
    bool found = false;
    for (uint32_t i = 0; i < nPhysicalDevices; i++) {
        const VkPhysicalDevice contestant = pPhysicalDevices[i];
        if (!is_physical_device_suitable(contestant)) {
            continue;
        }

        uint32_t iQueueFamily = 0;
        if (!choose_queue_family(&iQueueFamily, vkInstance, contestant)) {
            continue;
        }

        const uint32_t newScore = grade_physical_device(contestant);
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

    return found ? CU_SUCCESS : CU_ERROR_NO_DEVICE;
}

uint32_t get_memory_types(
    const PhysicalDeviceMemoryInfo* const pMemoryInfo,
    const VkMemoryPropertyFlags flags)
{
    uint32_t m = 0;
    if (ones_overlap(flags, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT)) {
        m |= pMemoryInfo->mDeviceLocal;
    }
    if (ones_overlap(flags, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT)) {
        m |= pMemoryInfo->mHostVisible;
    }
    if (ones_overlap(flags, VK_MEMORY_PROPERTY_HOST_COHERENT_BIT)) {
        m |= pMemoryInfo->mHostCoherent;
    }
    if (ones_overlap(flags, VK_MEMORY_PROPERTY_HOST_CACHED_BIT)) {
        m |= pMemoryInfo->mHostCached;
    }
    return m;
}

bool is_physical_device_suitable(
    const VkPhysicalDevice physicalDevice)
{
    (void)physicalDevice;
    return true;
}

bool choose_queue_family(
    uint32_t* const piQueueFamily,
    const VkInstance vkInstance,
    const VkPhysicalDevice physicalDevice)
{
    VkQueueFamilyProperties* pQueueFamilies AUTO_FREE = nullptr;
    uint32_t nQueueFamilies = 0;
    vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, &nQueueFamilies, nullptr);
    allocate_n(pQueueFamilies, nQueueFamilies);
    vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, &nQueueFamilies, pQueueFamilies);

    for (uint32_t i = 0; i < nQueueFamilies; i++) {
        constexpr VkQueueFlags flags =
            VK_QUEUE_GRAPHICS_BIT | VK_QUEUE_COMPUTE_BIT | VK_QUEUE_TRANSFER_BIT;
        const bool hasFlags = ones_match(pQueueFamilies[i].queueFlags, flags);

        if (hasFlags && queue_family_supports_presentation(i, vkInstance, physicalDevice)) {
            *piQueueFamily = i;
            return true;
        }
    }

    return false;
}

bool queue_family_supports_presentation(
    const uint32_t i, 
    const VkInstance vkInstance, 
    const VkPhysicalDevice physicalDevice)
{
    (void)i;
    (void)vkInstance;
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

uint32_t grade_physical_device(
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

PhysicalDeviceMemoryInfo get_physical_device_memory_info(
    VkPhysicalDevice physicalDevice)
{
    VkPhysicalDeviceMemoryProperties memoryProperties = {};
    vkGetPhysicalDeviceMemoryProperties(physicalDevice, &memoryProperties);

    PhysicalDeviceMemoryInfo memoryInfo = {};
    for (uint32_t i = 0; i < memoryProperties.memoryTypeCount; i++) {
        const VkMemoryPropertyFlags flags = memoryProperties.memoryTypes[i].propertyFlags;
        const uint32_t b = 1u << i;
        if (ones_overlap(flags, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT)) {
            memoryInfo.mDeviceLocal |= b;
        }
        if (ones_overlap(flags, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT)) {
            memoryInfo.mHostVisible |= b;
        }
        if (ones_overlap(flags, VK_MEMORY_PROPERTY_HOST_COHERENT_BIT)) {
            memoryInfo.mHostCoherent |= b;
        }
        if (ones_overlap(flags, VK_MEMORY_PROPERTY_HOST_CACHED_BIT)) {
            memoryInfo.mHostCached |= b;
        }
    }

    return memoryInfo;
}
