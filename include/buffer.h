#pragma once

#include <stdint.h>

/// @brief A command buffer.
/// @note This is any type that can be dereferenced as a `CuDeviceAddress` (`VkDeviceAddress`).
typedef void CuBuffer;

/// @brief A buffer's device address.
typedef uint64_t CuDeviceAddress;

/// @brief Gets a buffer's device address.
/// @param [in] pBuffer A pointer to the buffer.
/// @return The buffer's device address.
inline static CuDeviceAddress cu_buffer_get_address(
    CuBuffer* pBuffer)
{
    return *(CuDeviceAddress*)pBuffer;
}

/// @brief Buffer usage bitmap flags.
typedef enum : int32_t {
    /// @brief The buffer is used as a transfer source.
    CU_BUFFER_TRANSFER_SRC = 0x00000001,

    /// @brief The buffer is used as a transfer destination.
    CU_BUFFER_TRANSFER_DST = 0x00000002,

    /// @brief The buffer is used as a uniform texel buffer.
    CU_BUFFER_UNIFORM_TEXEL_BUFFER = 0x00000004,

    /// @brief The buffer is used as a storage texel buffer.
    CU_BUFFER_STORAGE_TEXEL_BUFFER = 0x00000008,

    /// @brief The buffer is used as a uniform buffer.
    CU_BUFFER_UNIFORM_BUFFER = 0x00000010,

    /// @brief The buffer is used as a storage buffer.
    CU_BUFFER_STORAGE_BUFFER = 0x00000020,

    /// @brief The buffer is used as an index buffer.
    CU_BUFFER_INDEX_BUFFER = 0x00000040,

    /// @brief The buffer is used as a vertex buffer.
    CU_BUFFER_VERTEX_BUFFER = 0x00000080,

    /// @brief The buffer is used as an indirect buffer.
    CU_BUFFER_INDIRECT_BUFFER = 0x00000100,

    /// @brief The buffer is used as a device address buffer.
    CU_BUFFER_SHADER_DEVICE_ADDRESS = 0x00020000,
} CuBufferUsage;
