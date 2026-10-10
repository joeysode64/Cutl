#pragma once

#include "def.h"
#include "result.h"
#include "version.h"

#include <stdint.h>

/// @brief Bits for Vulkan extensions usable by Cutl.
typedef enum : uint64_t {
    CU_EXTENSION_SWAPCHAIN = 1 << 0, /**< "VK_KHR_swapchain". */

    CU_EXTENSION_HEADLESS_SURFACE = 1 << 1, /**< "VK_KHR_headless_surface". */
} CuExtensionFlagBits;

/// @brief A bitmask of `CuExtensionFlagBits`.
typedef uint64_t CuExtensionFlags;

/// @brief A bitmask of all Vulkan extensions usable by Cutl.
constexpr CuExtensionFlags CU_EXTENSIONS_ALL = UINT64_MAX;

/// @brief A Vulkan context.
/// @note Only one global instance is meant to exist.
typedef struct CuContext_T {
    /// @brief The Vulkan instance handle.
    cu_vk_dispatch_t(Instance) _instance;

    /// @brief The (optional) debug messenger handle.
    cu_vk_t(DebugUtilsMessengerEXT) _debugMessenger;

    /// @brief The physical device handle.
    cu_vk_dispatch_t(PhysicalDevice) _physicalDevice;

    /// @brief The physical device memory info.
    CuPhysicalDeviceMemoryInfo _memoryInfo;

    /// @brief The logical device handle.
    cu_vk_dispatch_t(Device) _device;

    /// @brief The device queue handle.
    cu_vk_dispatch_t(Queue) _queue;

    /// @brief The command pool handle.
    cu_vk_t(CommandPool) _commandPool;

    /// @brief The index of the queue family.
    uint32_t _iQueueFamily;

    /// @brief The descriptor pool handle.
    cu_vk_t(DescriptorPool) _descriptorPool;

    /// @brief The descriptor set layout handle.
    cu_vk_t(DescriptorSetLayout) _descriptorSetLayout;

    /// @brief The descriptor set handle.
    cu_vk_t(DescriptorSet) _descriptorSet;

    /// @brief A bitmap of the enabled extensions.
    CuExtensionFlags mEnabledExtensions;

    /// @brief Whether the context is fully initialized.
    bool _isInitialized;
} CuContext;

/// @brief Create info for the context.
typedef struct CuContextCreateInfo_T {
    /// @brief A bitmask of the required extensions. Default is all.
    CuExtensionFlags mRequiredExtensions;

    /// @brief A bitmask of the preferred but non-required extensions. Default is none.
    CuExtensionFlags mPreferredExtensions;

    /// @brief The application name. Can be null. Default is null.
    const char* appName;

    /// @brief The application version. Default is v0.0.0.
    CuVersion appVersion;

    /// @brief The number of sampled image descriptors. Default is 16 (spec minimum).
    uint32_t nSampledImageDescriptors;

    /// @brief The number of descriptor samplers to. Default is 16 (spec minimum).
    uint32_t nSamplerDescriptors;

    /// @brief the number of storage image descriptors. Default is 4 (spec minimum).
    uint32_t nStorageImageDescriptors;

    /// @brief Whether to enable validation layers.
    bool enableValidation;
} CuContextCreateInfo;

/// @brief The default context create info.
constexpr CuContextCreateInfo CU_DEFAULT_CONTEXT_CREATE_INFO = {
    .mRequiredExtensions = CU_EXTENSIONS_ALL,
    .mPreferredExtensions = 0,
    .appName = nullptr,
    .appVersion = {
        .major = 0,
        .minor = 0,
        .patch = 0,
        .tweak = 0,
    },
    .nSampledImageDescriptors = 16,
    .nSamplerDescriptors = 16,
    .nStorageImageDescriptors = 4,
    .enableValidation =
#if defined(NDEBUG)
        false,
#else
        true,
#endif
};

#ifdef __cplusplus
extern "C" {
#endif

/// @brief Returns a pointer to the global GPU context.
/// @return A pointer to the global GPU Context.
CuContext* cu_context_get();

/// @brief Creates the context
/// @param pCreateInfo A pointer to the create info.
/// @return The result of creating the context.
CuResult cu_context_init(
    const CuContextCreateInfo* pCreateInfo);

/// @brief Terminates the context.
/// @warning All of the Cutl GPU library relies on the context. Call this only after cleanup.
void cu_context_terminate();

/// @brief Waits for the context to idle.
void cu_context_wait_for_idle();

/// @brief Returns a bitmask of the enabled extensions.
/// @return A bitmask of the enabled extensions.
CuExtensionFlags cu_context_get_enabled_extensions();

#ifdef __cplusplus
}
#endif
