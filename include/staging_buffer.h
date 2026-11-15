#pragma once

#include "allocation.h"
#include "result.h"

#include <stddef.h>
#include <vulkan/vulkan.h>

/** @brief A staging buffer. */
typedef struct CuStagingBuffer_T {
    /** @brief A pointer to the buffer's mapped memory. */
    void* _pData;

    /** @brief The buffer handle. */
    VkBuffer _buffer;

    /** @brief The buffer memory allocation. */
    CuAllocation _allocation;
} CuStagingBuffer;

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Creates a staging buffer.
 * @param[out] pStagingBuffer A pointer to the staging buffer. Must not be null.
 * @param z The size of the staging buffer in bytes. Must not be 0.
 * @param mode The allocation mode to allocate the buffer's memory with.
 */
CU_NODISCARD_RESULT
CuResult cu_staging_buffer_create(CuStagingBuffer* pStagingBuffer, size_t z, CuAllocationMode mode);

/**
 * @brief Destroys the staging buffer.
 * @param[in out] pStagingBuffer A pointer to the staging buffer. Must not be null.
 * @param mode The allocation mode the buffer was created with.
 */
void cu_staging_buffer_destroy(CuStagingBuffer* pStagingBuffer, CuAllocationMode mode);

/**
 * @brief Copies data into the staging buffer.
 * @param[in out] pStagingBuffer A pointer to the staging buffer. Must not be null.
 * @param p A pointer to the data to copy from. Must not be null.
 * @param z The size of the data to copy in bytes. `o + z` must not exceed the buffer's size.
 * @param o The offset into the buffer to write the data to in bytes.
 */
void cu_staging_buffer_copy(CuStagingBuffer* pStagingBuffer, const void* p, size_t z, size_t o);

#ifdef __cplusplus
}
#endif
