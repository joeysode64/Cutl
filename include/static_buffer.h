#pragma once

#include "allocation.h"
#include "result.h"

#include <stddef.h>
#include <vulkan/vulkan.h>

/** @brief A static buffer. */
typedef struct CuStaticBuffer_T {
    /** @brief The buffer handle. */
    VkBuffer _buffer;

    /** @brief The buffer's device address. */
    VkDeviceAddress _address;

    /** @brief The buffer memory allocation. */
    CuAllocation _allocation;
} CuStaticBuffer;

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Creates a static buffer.
 * @param[out] pStaticBuffer A pointer to the static buffer. Must not be null.
 * @param z The size of the buffer in bytes. Must not be 0.
 * @param usage A bitmap of the buffer usage flags. The flags
 * `VK_BUFFER_USAGE_TRANSFER_DST_BIT` and
 * `VK_BUFFER_USAGE_SHADER_DEVICE_ADDRESS_BIT` are set automatically.
 * @param mode The allocation mode to allocate the buffer's memory with.
 */
CU_NODISCARD_RESULT
CuResult cu_static_buffer_create(
    CuStaticBuffer* pStaticBuffer,
    size_t z,
    VkBufferUsageFlags usage,
    CuAllocationMode mode);

/**
 * @brief Destroys the static buffer.
 * @param[in out] pStaticBuffer A pointer to the static buffer. Must not be null.
 * @param mode The allocation mode the buffer was created with.
 */
void cu_static_buffer_destroy(CuStaticBuffer* pStaticBuffer, CuAllocationMode mode);

#ifdef __cplusplus
}
#endif
