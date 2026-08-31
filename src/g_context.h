#pragma once

#include "physical_device.h"

#include <stdint.h>
#include <vulkan/vulkan.h>
#include <vulkan/vulkan_core.h>

/** @brief A Vulkan context. */
typedef struct {
    VkInstance instance; /**< The Vulkan instance handle. */

    PhysicalDeviceInfo physicalDeviceInfo; /**< The physical device info. */

    VkDevice device; /**< The logical device handle. */

    VkQueue queue; /**< The device queue handle. */

    VkCommandPool commandPool; /**< The command pool handle. */

    bool isInitialized; /**< Whether the context is fully initialized. */
} Context;

/** @brief An uninitialized context. */
#define CU_NULL_CONTEXT ((Context){ .isInitialized = false, })

/** @brief The global Vulkan context. */
extern Context gContext;
