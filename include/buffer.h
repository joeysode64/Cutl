#pragma once

#include <stdint.h>

/**
 * @brief Returns the buffer's buffer handle.
 * @param [in] pBuffer A pointer to the buffer.
 * @return The buffer's handle.
 */
#define cu_buffer_get(pBuffer) ((pBuffer)->_buffer)

/**
 * @brief Returns the buffer's address.
 * @param [in] pBuffer A pointer to the buffer.
 * @return The buffer's address.
 */
#define cu_buffer_get_address(pBuffer) ((pBuffer)->_address)
