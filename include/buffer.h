#pragma once

#include <stdint.h>

/**
 * @brief Returns the buffer's buffer handle.
 * @param[in] pBuffer A pointer to the buffer.
 */
#define cu_buffer_get(pBuffer) ((pBuffer)->_buffer)

/**
 * @brief Returns the buffer's address.
 * @param[in] pBuffer A pointer to the buffer.
 */
#define cu_buffer_get_address(pBuffer) ((pBuffer)->_address)

/**
 * @brief Returns the a pointer to the buffer's mapped memory.
 * @param[in] pBuffer A pointer to the buffer.
 */
#define cu_buffer_get_mapped_memory(pBuffer) ((pBuffer)->_pData)
