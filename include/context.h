#pragma once

#include "def.h"
#include "def.h"
#include "result.h"
#include "version.h"

#include <stdint.h>

/// @brief A Vulkan context.
/// @note Only one global instance is meant to exist.
typedef struct CuContext_T {
    /// @brief The Vulkan instance handle.
    cu_vk_dispatch_t(Instance) _instance;

    /// @brief The physical device info.
    cu_vk_dispatch_t(PhysicalDevice) _physicalDeviceInfo;

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

    /// @brief Whether the context is fully initialized.
    bool _isInitialized;
} CuContext;

/// @brief Create info for the context.
typedef struct CuContextCreateInfo_T {
    /// @brief The application name. Can be null. Default is null.
    const char* appName;

    /// @brief The application version. Default is v0.0.0.
    CuVersion appVersion;
} CuContextCreateInfo;

/// @brief The default context create info.
constexpr CuContextCreateInfo CU_DEFAULT_CONTEXT_CREATE_INFO = {
    .appName = nullptr,
    .appVersion = {
        .major = 0,
        .minor = 0,
        .patch = 0,
    },
};

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
