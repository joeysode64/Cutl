#pragma once

#include "allocation.h"
#include "buffer.h"
#include "inner.h"
#include "result.h"

#include <stddef.h>

/// @brief A dynamic buffer.
typedef struct CuDynamicbuffer_T {
    /// @brief The buffer's device address.
    CuDeviceAddress _address;

    /// @brief The buffer memory allocation.
    CuAllocation _allocation;

    /// @brief The buffer handle.
    cu_vk_t(Buffer) _buffer;

    /// @brief The mapped memory pointer.
    void* _p;

    /// @brief The number of data frames.
    size_t _n;

    /// @brief The size of each data frame.
    size_t _z;
} CuDynamicBuffer;

/// @brief Creates the dynamic buffer.
/// @param [out] pDynamicBuffer A pointer to the dynamic buffer.
/// @param n The number of data frames to have.
/// @param z The size of each data frame.
/// @param usage The buffer usage flags.
/// @param mode The allocation mode.
/// @return The result of the dynamic buffer's creation.
CuResult cu_dynamic_buffer_create(
    CuDynamicBuffer* pDynamicBuffer,
    size_t n,
    size_t z,
    CuBufferUsage usage,
    CuAllocationMode mode);

/// @brief Destroys the dynamic buffer.
/// @param [in, out] pDynamicBuffer A pointer to the dynamic buffer.
/// @param mode The allication mode.
void cu_dynamic_buffer_destroy(
    CuDynamicBuffer* pDynamicBuffer,
    CuAllocationMode mode);

/// @brief Retrieves the data frame's data pointer and device address.
/// @param [in] pDynamicBuffer A pointer to the dynamic buffer.
/// @param [out] ppData A pointer to the data pointer.
/// @param [out] pDeviceAddress A pointer to the device address.
/// @param i The index of the data frame to get.
void cu_dynamic_buffer_get_data_frame(
    CuDynamicBuffer* pDynamicBuffer,
    void** ppData,
    CuDeviceAddress* pDeviceAddress,
    size_t i);
