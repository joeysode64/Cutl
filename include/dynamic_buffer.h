#pragma once

#include "allocation.h"
#include "result.h"

#include <stddef.h>
#include <vulkan/vulkan.h>

/** @brief A dynamic buffer. */
typedef struct CuDynamicbuffer_T {
    VkDeviceAddress _address; /**< The buffer's device address. */

    CuAllocation _allocation; /**< The buffer memory allocation. */

    VkBuffer _buffer; /**< The buffer handle. */

    void* _p; /**< The mapped memory pointer. */

    size_t _n; /**< The number of data frames. */

    size_t _z; /**< The size of each data frame. */
} CuDynamicBuffer;

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Creates the dynamic buffer.
 * @param [out] pDynamicBuffer A pointer to the dynamic buffer.
 * @param n The number of data frames to have.
 * @param z The size of each data frame.
 * @param usage The buffer usage flags.
 * @param mode The allocation mode.
 * @return The result of the dynamic buffer's creation.
 */
CuResult cu_dynamic_buffer_create(
    CuDynamicBuffer* pDynamicBuffer,
    size_t n,
    size_t z,
    VkBufferUsageFlags usage,
    CuAllocationMode mode);

/**
 * @brief Destroys the dynamic buffer.
 * @param [in, out] pDynamicBuffer A pointer to the dynamic buffer.
 * @param mode The allication mode.
 */
void cu_dynamic_buffer_destroy(
    CuDynamicBuffer* pDynamicBuffer,
    CuAllocationMode mode);

/**
 * @brief Retrieves the data frame's data pointer and device address.
 * @param [in] pDynamicBuffer A pointer to the dynamic buffer.
 * @param [out] ppData A pointer to the data pointer.
 * @param [out] pDeviceAddress A pointer to the device address.
 * @param i The index of the data frame to get.
 */
void cu_dynamic_buffer_get_data_frame(
    CuDynamicBuffer* pDynamicBuffer,
    void** ppData,
    VkDeviceAddress* pDeviceAddress,
    size_t i);

#ifdef __cplusplus
}
#endif
