#pragma once

#include "context.h"
#include "result.h"

#include <stdint.h>
#include <vulkan/vulkan.h>

/** @brief A physical device's info. */
typedef struct PhysicalDeviceInfo_T {
    /** @brief The physical device handle. */
    VkPhysicalDevice handle;

    /** @brief The physical device's memory type info. */
    CuPhysicalDeviceMemoryInfo memoryInfo;

    /**
     * @brief The index of the main queue family. This queue supports graphics, transfer, compute,
     * and presentation.
     */
    uint32_t iQueueFamily;
} PhysicalDeviceInfo;

/**
 * @brief Returns a bitmask of the best-fit memory types.
 * @param pMemoryInfo A pointer to the physical device's memory type info. Must not be null.
 * @param mProperties A bitmap of the required memory properties.
 * @param mAllowed A bitmask of the memory types to choose from, e.g.
 * `VkMemoryRequirements::memoryTypeBits`.
 */
uint32_t find_memory_types(
    const CuPhysicalDeviceMemoryInfo* pMemoryInfo,
    VkMemoryPropertyFlags mProperties,
    uint32_t mAllowed);

/**
 * @brief Chooses the best-fit physical device.
 * @param[out] pPhysicalDeviceInfo A pointer to the physical device info. Must not be null.
 * @param instance The Vulkan instance to choose from.
 */
CU_NODISCARD_RESULT
CuResult choose_physical_device(PhysicalDeviceInfo* pPhysicalDeviceInfo, VkInstance instance);
