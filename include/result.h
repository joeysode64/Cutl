#pragma once

#include <stdint.h>

/** @brief A result. */
typedef enum : uint32_t {
    CU_SUCCESS = 0, /**< A success. */

    CU_ERROR_OUT_OF_RAM, /**< Out of RAM (allocation failed). */

    CU_ERROR_FILE_IO, /**< File I/O error. */

    CU_ERROR_TIMEOUT, /**< Operation hit a timeout. */

    CU_ERROR_OUT_OF_VRAM, /**< Out of VRAM (device memory). */

    CU_ERROR_UNSUPPORTED, /**< Some Vulkan requirements for Cutl are not met by the system. */

    CU_ERROR_NO_DEVICE, /**< No suitable graphics device was found. */

    CU_ERROR_GLFW_INIT, /**< GLFW failed to initialize. */

    CU_ERROR_UNKNOWN, /**< An unhandled error. Sorry! */
} CuResult;

/**
 * @brief Returns whether a result is a success.
 *
 * @param r The result to query.
 *
 * @return Whether the result is a success.
 */
#define cu_is_success(r) ((r) == CU_SUCCESS)
