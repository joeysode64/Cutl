#pragma once

#include "allocation.h"
#include "buffer.h"
#include "def.h"
#include "result.h"

#include <stddef.h>

/** @brief A static buffer. */
typedef struct CuStaticBuffer_T {
    cu_vk_t(Buffer) _buffer; /**< The buffer handle. */

    CuBufferAddress _address; /**< The buffer's device address. */

    CuAllocation _allocation; /**< The buffe's allocation. */
} CuStaticBuffer;

/**
 * @brief Creates a static buffer.
 * @param [out] pStaticBuffer A pointer to the static buffer.
 * @param z The size of the buffer.
 * @param usage The buffer usage flags.
 * @param mode The allocation mode.
 */
CuResult cu_static_buffer_create(
    CuStaticBuffer* pStaticBuffer,
    size_t z,
    CuBufferUsage usage,
    CuAllocationMode mode);

/**
 * @brief Destroys the static buffer.
 * @param [in, out] pStaticBuffer A pointer to the static buffer.
 * @param mode The allocation mode.
 */
void cu_static_buffer_destroy(
    CuStaticBuffer* pStaticBuffer,
    CuAllocationMode mode);
