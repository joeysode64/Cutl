#pragma once

#include "physical_device.h"

#include <stdint.h>
#include <vulkan/vulkan.h>

/// @brief A Vulkan context.
typedef struct {
    /// @brief The Vulkan instance handle.
    VkInstance instance;

    /// @brief The physical device info.
    PhysicalDeviceInfo physicalDeviceInfo;

    /// @brief The logical device handle.
    VkDevice device;

    /// @brief The device queue handle.
    VkQueue queue;

    /// @brief The command pool handle.
    VkCommandPool commandPool;

    /// @brief Whether the context is fully initialized.
    bool isInitialized;
} Context;

/// @brief An uninitialized context.
#define CU_NULL_CONTEXT ((Context){ .isInitialized = false, })

/// @brief The global Vulkan context.
extern Context gContext;
