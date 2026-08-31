#pragma once

#include "result.h"
#include "util.h"

#include <stdint.h>
#include <vulkan/vulkan.h>
#include <vulkan/vulkan_core.h>

/** @brief Map a `VkResult` to a `CuResult`. */
CuResult vk_result_to_cu_result(VkResult result);

/**
 * @brief Queries the Vulkan result and stores it in the local `CuResult result` variable. If it is
 * not a success value, jumps to `FAIL`.
 */
#define cu_try_vk(e)                                                                               \
    cu_try(vk_result_to_cu_result(e))                                                              \

/** @brief Creates a Vulkan instance. */
VkResult create_vk_instance(VkInstance* pInstance, const char* appName, uint32_t appVersion);

/** @brief Creates a logical device. */
VkResult create_device(VkDevice* pDevice, VkPhysicalDevice physicalDevice, uint32_t iQueueFamily);

/** @brief Creates a command pool. */
VkResult create_command_pool(VkCommandPool* pCommandPool, VkDevice device, uint32_t iQueueFamily);
