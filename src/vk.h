#pragma once

#include "result.h"
#include "util.h"

#include <stdint.h>
#include <vulkan/vulkan.h>
#include <vulkan/vulkan_core.h>

/** @brief Map a `VkResult` to a `CuResult`. */
CuResult vk_result_to_cu_result(VkResult result);

/** @brief Queries the Vulkan result and returns it as a `CuResult` if it's not `VK_SUCCESS`. */
#define vk_try(e)                                                                                  \
    do {                                                                                           \
        const VkResult _result = (e);                                                              \
        if (_result != VK_SUCCESS) {                                                               \
            return _result;                                                                        \
        }                                                                                          \
    } while(false)

/** @brief Queries the Vulkan result and returns it if it's not `VK_SUCCESS`. */
#define cu_try_vk(e)                                                                               \
    do {                                                                                           \
        const VkResult _result = (e);                                                              \
        if (_result != VK_SUCCESS) {                                                               \
            return vk_result_to_cu_result(_result);                                                \
        }                                                                                          \
    } while(false)

/**
 * @brief Queries the result and stores it in the local `VkResult result` variable. If it is not
 * `VK_SUCCESS`, jumps to `FAIL`.
 */
#define vk_try_catch(e)                                                                            \
    result = (e);                                                                                  \
    if (result != VK_SUCCESS) {                                                                    \
        goto FAIL;                                                                                 \
    }

/**
 * @brief Queries the Vulkan result and stores it in the local `CuResult result` variable. If it is
 * not a success value, jumps to `FAIL`.
 */
#define cu_try_catch_vk(e)                                                                         \
    cu_try_catch(vk_result_to_cu_result(e))

/** @brief Tries to allocate to the elements and returns `VK_ERROR_OUT_OF_HOST_MEMORY` if it fails. */
#define vk_allocate_n(p, n)                                                                        \
    p = (typeof(*p)*)calloc(n, sizeof(*p));                                                        \
    if (p == nullptr) {                                                                            \
        return VK_ERROR_OUT_OF_HOST_MEMORY;                                                        \
    }

/** @brief Tries to allocate to the pointer and returns `VK_ERROR_OUT_OF_HOST_MEMORY` if it fails. */
#define vk_allocate(p) allocate_n(p, 1)

/** @brief Creates a Vulkan instance. */
VkResult create_vk_instance(
    VkInstance* pInstance,
    const char* appName,
    uint32_t appVersion);

/** @brief Creates a logical device. */
VkResult create_device(
    VkDevice* pDevice,
    VkPhysicalDevice physicalDevice,
    uint32_t iQueueFamily);

/** @brief Creates a command pool. */
VkResult create_command_pool(
    VkCommandPool* pCommandPool,
    VkDevice device,
    uint32_t iQueueFamily);

/** @brief Creates an image view. */
VkResult create_image_view(
    VkImageView* pImageView,
    VkDevice device,
    VkImage image,
    VkFormat format);

/** @brief Creates a semaphore. */
VkResult create_semaphore(
    VkSemaphore* pSemaphore,
    VkDevice device,
    VkSemaphoreType type,
    uint64_t x);
