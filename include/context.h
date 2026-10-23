#pragma once

#include "image.h"
#include "result.h"

#include <stdint.h>
#include <vulkan/vulkan.h>

/** @brief A physical device's memory type info. */
typedef struct CuPhysicalDeviceMemoryInfo_T {
    uint32_t _mDeviceLocal; /**< A bitmask of the device local memory types. */

    uint32_t _mHostVisible; /**< A bitmask of the host visible memory types. */

    uint32_t _mHostCoherent; /**< A bitmask of the host coherent memory types. */

    uint32_t _mHostCached; /**< A bitmask of the host cached memory types. */
} CuPhysicalDeviceMemoryInfo;

/**
 * @brief A Vulkan context.
 * @note Only one is supposed to exist: the global instance.
 */
typedef struct CuContext_T {
    VkInstance _instance; /**< The instance handle. */

    VkDebugUtilsMessengerEXT _debugMessenger; /**< The (optional) debug messenger handle. */

    VkPhysicalDevice _physicalDevice; /**< The physical device handle. */

    uint32_t _iQueueFamily; /**< The index of the main queue family. This queue
        supports graphics, transfer, compute, and presentation. */

    CuPhysicalDeviceMemoryInfo _memoryInfo; /**< The physical device memory info. */

    VkDevice _device; /**< The logical device handle. */

    VkQueue _queue; /**< The main queue handle. */

    VkCommandPool _commandPool; /**< The command pool handle. */

    VkDescriptorPool _descriptorPool; /**< The descriptor pool handle. */

    VkDescriptorSetLayout _descriptorSetLayout; /**< The descriptor set layout handle. */

    VkDescriptorSet _descriptorSet; /**< The descriptor set handle. */
} CuContext;

/**
 * @brief Create info for the context.
 * @note For extensions, Cutl's defaults are always used. See them in `src/vk.h`.
 */
typedef struct CuContextCreateInfo_T {
    uint32_t nInstanceExtensions; /**< The number of instance extensions to use. Default is 0. */

    const char* const*
        pInstanceExtensions; /**< A pointer to the instance extensions to use. Default is null. */

    uint32_t nDeviceExtensions; /**< The number of instance device to use. Default is 0. */

    const char* const*
        pDeviceExtensions; /**< A pointer to the device extensions to use. Default is null. */

    const char* appName; /**< The application name. Can be null. Default is null. */

    uint32_t appVersion; /**< The application version. Default is v0.0.0. */

    uint32_t nSampledImageDescriptors; /**< The number of sampled image descriptors. Default is 16
                                          (spec minimum). */

    uint32_t nSamplerDescriptors; /**< The number of sampler descriptor slots. Default is 16 (spec
                                     minimum). */

    uint32_t nStorageImageDescriptors; /**< The number of storage image descriptor slogs. Default is
                                          4 (spec minimum). */
} CuContextCreateInfo;

/** @brief The default context create info. */
constexpr CuContextCreateInfo CU_DEFAULT_CONTEXT_CREATE_INFO = {
    .nInstanceExtensions = 0,
    .pInstanceExtensions = 0,
    .nDeviceExtensions = 0,
    .pDeviceExtensions = 0,
    .appName = nullptr,
    .appVersion = 0,
    .nSampledImageDescriptors = 16,
    .nSamplerDescriptors = 16,
    .nStorageImageDescriptors = 4,
};

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Returns a pointer to the global GPU context.
 * @return A pointer to the global GPU Context.
 */
CuContext* cu_context_get();

/**
 * @brief Creates the context
 * @param pCreateInfo A pointer to the create info.
 * @return The result of creating the context.
 */
CuResult cu_context_init(const CuContextCreateInfo* pCreateInfo);

/**
 * @brief Terminates the context.
 * @warning All of the Cutl GPU library relies on the context. Call this only
 * after cleanup.
 */
void cu_context_terminate();

/** @brief Waits for the context to idle. */
void cu_context_wait_for_idle();

/**
 * @brief Binds the image to the given sampled image descriptor.
 * @param pImage A pointer to the image to bind.
 * @param i The index of the descriptor to bind to.
 * @return The result of binding the image.
 */
void cu_context_bind_sampled_image(CuImage* pImage, uint32_t i);

/**
 * @brief Binds the sampler to the given sampler descriptor.
 * @param pSampler A pointer to the sampler to bind.
 * @param i The index of the descriptor to bind to.
 * @return The result of binding the sampler.
 */
void cu_context_bind_sampler(CuSampler* pSampler, uint32_t i);

/**
 * @brief Binds the image to the given storage image descriptor.
 * @param pImage A pointer to the image to bind.
 * @param i The index of the descriptor to bind to.
 * @return The result of binding the image.
 */
void cu_context_bind_storage_image(CuImage* pImage, uint32_t i);

/**
 * @brief Returns whether the context is initialized at all and should be
 * terminated.
 * @return Whether the context is initialized at all.
 */
static inline bool cu_context_is_initialized()
{
    return cu_context_get()->_instance != VK_NULL_HANDLE;
}

#ifdef __cplusplus
}
#endif
