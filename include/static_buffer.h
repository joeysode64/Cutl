#pragma once

#include "allocation.h"
#include "result.h"

#include <stddef.h>
#include <vulkan/vulkan.h>

/** @brief A static buffer. */
typedef struct CuStaticBuffer_T {
    VkBuffer _buffer; /**< The buffer handle. */

    VkDeviceAddress _address; /**< The buffer's device address. */

    CuAllocation _allocation; /**< The buffe's allocation. */
} CuStaticBuffer;

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Creates a static buffer.
 * @param [out] pStaticBuffer A pointer to the static buffer.
 * @param z The size of the buffer.
 * @param usage A bitmap of the buffer usage flags. The flags `VK_BUFFER_USAGE_TRANSFER_DST_BIT`
 *     and `VK_BUFFER_USAGE_DEVICE_ADDRESS_BIT` are set automatically.
 * @param mode The allocation mode.
 */
CuResult cu_static_buffer_create(
    CuStaticBuffer* pStaticBuffer,
    size_t z,
    VkBufferUsageFlags usage,
    CuAllocationMode mode);

/**
 * @brief Destroys the static buffer.
 * @param [in, out] pStaticBuffer A pointer to the static buffer.
 * @param mode The allocation mode.
 */
void cu_static_buffer_destroy(
    CuStaticBuffer* pStaticBuffer,
    CuAllocationMode mode);

#ifdef __cplusplus
}
#endif
