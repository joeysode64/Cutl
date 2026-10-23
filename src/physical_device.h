#pragma once

#include "context.h"
#include "result.h"

#include <stdint.h>
#include <vulkan/vulkan.h>

/** @brief A physical device's info. */
typedef struct PhysicalDeviceInfo_T {
    VkPhysicalDevice handle; /**< The physical device handle. */

    CuPhysicalDeviceMemoryInfo memoryInfo; /**< The memory info. */

    uint32_t iQueueFamily; /**< The index of the main queue family. */
} PhysicalDeviceInfo;

/** @brief Returns a bitmask of the best-fit memory types. */
uint32_t find_memory_types(
    const CuPhysicalDeviceMemoryInfo* pMemoryInfo,
    VkMemoryPropertyFlags mProperties,
    uint32_t mAllowed);

/**
 * @brief Chooses the best-fit physical device.
 * @param [out] pPhysicalDeviceInfo A pointer to the physical device info.
 * @param instance The Vulkan intance handle.
 */
CuResult choose_physical_device(
    PhysicalDeviceInfo* pPhysicalDeviceInfo,
    VkInstance instance);

