#pragma once

#include "result.h"
#include <stdint.h>
#include <vulkan/vulkan.h>

/// @brief A physical device/s memory type info.
typedef struct PhysicalDeviceMemoryInfo_T {
    /// @brief A bitmask of the device local memory types.
    uint32_t mDeviceLocal;

    /// @brief A bitmask of the host visible memory types.
    uint32_t mHostVisible;

    /// @brief A bitmask of the host coherent memory types.
    uint32_t mHostCoherent;

    /// @brief A bitmask of the host cached memory types.
    uint32_t mHostCached;
} PhysicalDeviceMemoryInfo;

/// @brief A physical device's info.
typedef struct PhysicalDeviceInfo_T {
    /// @brief The physical device handle.
    VkPhysicalDevice handle;

    /// @brief The memory info.
    PhysicalDeviceMemoryInfo memoryInfo;

    /// @brief The first queue family index that supports compute, graphics, transfer, and
    /// presentation.
    uint32_t iQueueFamily;
} PhysicalDeviceInfo;

/// @brief Returns a bitmask of the best-fit memory types.
uint32_t find_memory_types(
    const PhysicalDeviceMemoryInfo* pMemoryInfo,
    VkMemoryPropertyFlags mRequired,
    VkMemoryPropertyFlags mPreferred,
    uint32_t mAllowed);

/// @brief Chooses the best-fit physical device.
CuResult choose_physical_device(
    PhysicalDeviceInfo* pPhysicalDeviceInfo,
    VkInstance instance);
