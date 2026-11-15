#pragma once

#include "image.h"
#include "result.h"

#include <stdint.h>
#include <vulkan/vulkan.h>

/** @brief A physical device's memory type info. */
typedef struct CuPhysicalDeviceMemoryInfo_T {
    /** @brief A bitmask of the device local memory types. */
    uint32_t _mDeviceLocal;

    /** @brief A bitmask of the host visible memory types. */
    uint32_t _mHostVisible;

    /** @brief A bitmask of the host coherent memory types. */
    uint32_t _mHostCoherent;

    /** @brief A bitmask of the host cached memory types. */
    uint32_t _mHostCached;
} CuPhysicalDeviceMemoryInfo;

/**
 * @brief A Vulkan context.
 * @note Only one is supposed to exist: the global instance.
 */
typedef struct CuContext_T {
    /** @brief The instance handle. */
    VkInstance _instance;

    /** @brief The (optional) debug messenger handle. */
    VkDebugUtilsMessengerEXT _debugMessenger;

    /** @brief The physical device handle. */
    VkPhysicalDevice _physicalDevice;

    /**
     * @brief The index of the main queue family. This queue supports graphics, transfer, compute,
     * and presentation.
     */
    uint32_t _iQueueFamily;

    /** @brief The physical device memory info. */
    CuPhysicalDeviceMemoryInfo _memoryInfo;

    /** @brief The logical device handle. */
    VkDevice _device;

    /** @brief The main queue handle. */
    VkQueue _queue;

    /** @brief The command pool handle. */
    VkCommandPool _commandPool;

    /** @brief The descriptor pool handle. */
    VkDescriptorPool _descriptorPool;

    /** @brief The descriptor set layout handle. */
    VkDescriptorSetLayout _descriptorSetLayout;

    /** @brief The descriptor set handle. */
    VkDescriptorSet _descriptorSet;
} CuContext;

/**
 * @brief Create info for the context.
 * @note For extensions, Cutl's defaults are always used. See them in `src/vk.h`.
 */
typedef struct CuContextCreateInfo_T {
    /**
     * @brief The number of instance extensions to use.
     * @note Default is 0.
     */
    uint32_t nInstanceExtensions;

    /**
     * @brief A pointer to the instance extensions to use. Must not be null if `nInstanceExtensions`
     * isn't 0.
     * @note Default is null.
     */
    const char* const* pInstanceExtensions;

    /**
     * @brief The number of device extensions to use.
     * @note Default is 0.
     */
    uint32_t nDeviceExtensions;

    /**
     * @brief A pointer to the device extensions to use. Must not be null if `nDeviceExtensions`
     * isn't 0.
     * @note Default is null.
     */
    const char* const* pDeviceExtensions;

    /**
     * @brief The application name. Can be null.
     * @note Default is null.
     */
    const char* appName;

    /**
     * @brief The application version.
     * @note Default is v0.0.0.
     */
    uint32_t appVersion;

    /**
     * @brief The number of sampled image descriptor slots.
     * @note Default is 16 (spec minimum).
     */
    uint32_t nSampledImageDescriptors;

    /**
     * @brief The number of sampler descriptor slots.
     * @note Default is 16 (spec minimum).
     */
    uint32_t nSamplerDescriptors;

    /**
     * @brief The number of storage image descriptor slots.
     * @note Default is 4 (spec minimum).
     */
    uint32_t nStorageImageDescriptors;
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

/** @brief Returns a pointer to the global GPU context. */
CuContext* cu_context_get();

/**
 * @brief Creates the context.
 * @param pCreateInfo A pointer to the create info. Must not be null. Use
 * `&CU_DEFAULT_CONTEXT_CREATE_INFO` for the defaults.
 * @note On failure, the context is terminated automatically.
 */
CU_NODISCARD_RESULT
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
 * @param pImage A pointer to the image to bind. Must not be null.
 * @param i The index of the descriptor to bind to. Must be less than
 * `CuContextCreateInfo::nSampledImageDescriptors`.
 * @note The image must be in `VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL` when used.
 */
void cu_context_bind_sampled_image(CuImage* pImage, uint32_t i);

/**
 * @brief Binds the sampler to the given sampler descriptor.
 * @param pSampler A pointer to the sampler to bind. Must not be null.
 * @param i The index of the descriptor to bind to. Must be less than
 * `CuContextCreateInfo::nSamplerDescriptors`.
 */
void cu_context_bind_sampler(CuSampler* pSampler, uint32_t i);

/**
 * @brief Binds the image to the given storage image descriptor.
 * @param pImage A pointer to the image to bind. Must not be null.
 * @param i The index of the descriptor to bind to. Must be less than
 * `CuContextCreateInfo::nStorageImageDescriptors`.
 * @note The image must be in `VK_IMAGE_LAYOUT_GENERAL` when used.
 */
void cu_context_bind_storage_image(CuImage* pImage, uint32_t i);

/**
 * @brief Returns whether the context is initialized at all and should be
 * terminated.
 */
static inline bool cu_context_is_initialized()
{
    return cu_context_get()->_instance != VK_NULL_HANDLE;
}

#ifdef __cplusplus
}
#endif
