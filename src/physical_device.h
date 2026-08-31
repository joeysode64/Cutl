#pragma once

#include "result.h"
#include <stdint.h>
#include <vulkan/vulkan.h>
#include <vulkan/vulkan_core.h>

/** @brief A physical device/s memory type info. */
typedef struct {
    uint32_t mDeviceLocal; /**< A bitmask of the device local memory types. */

    uint32_t mHostVisible; /**< A bitmask of the host visible memory types. */

    uint32_t mHostCoherent; /**< A bitmask of the host coherent memory types. */

    uint32_t mHostCached; /**< A bitmask of the host cached memory types. */
} PhysicalDeviceMemoryInfo;

/** @brief A physical device's info. */
typedef struct {
    VkPhysicalDevice handle; /**< The physical device handle. */

    PhysicalDeviceMemoryInfo memoryInfo; /**< The memory info. */

    uint32_t iQueueFamily; /**< The first queue family index that supports compute, graphics,
        trasnfer, and presentation. */
} PhysicalDeviceInfo;

/** @brief Returns a bitmask of the best-fit memory types. */
uint32_t find_memory_types(
    const PhysicalDeviceMemoryInfo* pMemoryInfo,
    VkMemoryPropertyFlags mRequired,
    VkMemoryPropertyFlags mPreferred,
    uint32_t mAllowed
);

/** @brief Chooses the best-fit physical device. */
CuResult choose_physical_device(PhysicalDeviceInfo* pPhysicalDeviceInfo, VkInstance vkInstance);
