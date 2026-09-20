#pragma once

#include <stdint.h>

/// @brief A result.
typedef enum : uint32_t {
    /// @brief A success.
    CU_SUCCESS = 0,

    /// @brief Out of RAM (allocation failed).
    CU_ERROR_OUT_OF_RAM,

    /// @brief File I/O error.
    CU_ERROR_FILE_IO,

    /// @brief Operation hit a timeout.
    CU_ERROR_TIMEOUT,

    /// @brief Out of VRAM (device memory).
    CU_ERROR_OUT_OF_VRAM,

    /// @brief Asked for an allocation too large for its allocator.
    CU_ERROR_ALLOCATION_TOO_LARGE,

    /// @brief Tried to allocate to an arena allocator that doesn't have enough room.
    CU_ERROR_ARENA_FULL,

    /// @brief Some Vulkan requirements for Cutl are not met by the system.
    CU_ERROR_UNSUPPORTED,

    /// @brief No suitable graphics device was found.
    CU_ERROR_NO_DEVICE,

    /// @brief GLFW failed to initialize.
    CU_ERROR_GLFW_INIT,

    /// @brief An unhandled error. Sorry!
    CU_ERROR_UNKNOWN,
} CuResult;

/// @brief Returns whether a result is a success.
/// @param r The result to query.
/// @return Whether the result is a success.
#define cu_is_success(r) ((r) == CU_SUCCESS)
