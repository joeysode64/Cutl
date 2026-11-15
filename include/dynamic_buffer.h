#pragma once

#include "allocation.h"
#include "result.h"

#include <stddef.h>
#include <vulkan/vulkan.h>

/** @brief A dynamic buffer. */
typedef struct CuDynamicbuffer_T {
    /** @brief The buffer's device address. */
    VkDeviceAddress _address;

    /** @brief The buffer memory allocation. */
    CuAllocation _allocation;

    /** @brief The buffer handle. */
    VkBuffer _buffer;

    /** @brief The mapped memory pointer. */
    void* _p;

    /** @brief The number of data frames. */
    size_t _n;

    /** @brief The size of each data frame. */
    size_t _z;
} CuDynamicBuffer;

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Creates the dynamic buffer.
 * @param[out] pDynamicBuffer A pointer to the dynamic buffer. Must not be null.
 * @param n The number of data frames to have. Must not be 0.
 * @param z The size of each data frame in bytes. Must not be 0.
 * @param usage The buffer usage flags.
 * @param mode The allocation mode to allocate the buffer's memory with.
 */
CU_NODISCARD_RESULT
CuResult cu_dynamic_buffer_create(
    CuDynamicBuffer* pDynamicBuffer,
    size_t n,
    size_t z,
    VkBufferUsageFlags usage,
    CuAllocationMode mode);

/**
 * @brief Destroys the dynamic buffer.
 * @param[in out] pDynamicBuffer A pointer to the dynamic buffer. Must not be null.
 * @param mode The allocation mode the buffer was created with.
 */
void cu_dynamic_buffer_destroy(CuDynamicBuffer* pDynamicBuffer, CuAllocationMode mode);

/**
 * @brief Retrieves the data frame's data pointer and device address.
 * @param[in] pDynamicBuffer A pointer to the dynamic buffer. Must not be null.
 * @param[out] ppData A pointer to the data pointer. Must not be null.
 * @param[out] pDeviceAddress A pointer to the device address. Must not be null.
 * @param i The index of the data frame to get. Must be less than the number of data frames.
 */
void cu_dynamic_buffer_get_data_frame(
    CuDynamicBuffer* pDynamicBuffer,
    void** ppData,
    VkDeviceAddress* pDeviceAddress,
    size_t i);

#ifdef __cplusplus
}
#endif
