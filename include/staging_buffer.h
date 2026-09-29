#pragma once

#include "allocation.h"
#include "def.h"
#include "result.h"

#include <stddef.h>

/** @brief A staging buffer. */
typedef struct CuStagingBuffer_T {
    void* _pData; /**< A pointer to the buffer's mapped memory. */

    cu_vk_t(Buffer) _buffer; /**< The buffer handle. */

    CuAllocation _allocation; /**< The buffer memory allocation. */
} CuStagingBuffer;

/**
 * @brief Creates a staging buffer.
 * @param [out] pStagingBuffer A pointer to the staging buffer.
 * @param z The size of the staging buffer's data.
 * @param mode The allocation mode.
 * @return The result of creating the staging buffer.
 */
CuResult cu_staging_buffer_create(
    CuStagingBuffer* pStagingBuffer,
    size_t z,
    CuAllocationMode mode);

/**
 * @brief Destroys the staging buffer.
 * @param [in, out] pStagingBuffer A pointer to the staging buffer.
 * @param mode The allocation mode.
 */
void cu_staging_buffer_destroy(
    CuStagingBuffer* pStagingBuffer,
    CuAllocationMode mode);

/**
 * @brief Copies data into the staging buffer.
 * @param [in, out] pStagingBuffer A pointer to the staging buffer.
 * @param p A pointer to the data to copy from.
 * @param z The size of the data to copy.
 * @param o The offset to write the data to.
 */
void cu_staging_buffer_copy(
    CuStagingBuffer* pStagingBuffer,
    const void* p,
    size_t z,
    size_t o);
