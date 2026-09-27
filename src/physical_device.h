#pragma once

#include "def.h"
#include "result.h"

#include <stdint.h>
#include <vulkan/vulkan.h>

/// @brief A physical device's info.
typedef struct PhysicalDeviceInfo_T {
    /// @brief The physical device handle.
    VkPhysicalDevice handle;

    /// @brief The memory info.
    CuPhysicalDeviceMemoryInfo memoryInfo;

    /// @brief The first queue family index that supports compute, graphics, transfer, and
    /// presentation.
    uint32_t iQueueFamily;
} PhysicalDeviceInfo;

/// @brief Returns a bitmask of the best-fit memory types.
uint32_t find_memory_types(
    const CuPhysicalDeviceMemoryInfo* pMemoryInfo,
    VkMemoryPropertyFlags mRequired,
    VkMemoryPropertyFlags mPreferred,
    uint32_t mAllowed);

/// @brief Chooses the best-fit physical device.
CuResult choose_physical_device(
    PhysicalDeviceInfo* pPhysicalDeviceInfo,
    VkInstance instance);
