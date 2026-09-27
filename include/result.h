#pragma once

#include <stdint.h>

/** @brief A result type. */
typedef enum : uint32_t {
    CU_RESULT_TYPE_SUCCESS = 0, /**< A success. The result's `.v` is irrelevant. */

    CU_RESULT_TYPE_ERROR, /**< A Cutl error. The result's `.v` is the matching `CuError`. */

    CU_RESULT_TYPE_STD, /**< A C standard library error. Ther result's `.v` is `errno`. */

    CU_RESULT_TYPE_VULKAN, /**< A Vulkan error. The result's `.v` is the `VkResult`. */

    CU_REUSLT_TYPE_GLFW, /**< A GLFW error. The result's `.v` is the value of `glfwGetError`. */
} CuResultType;

/** @brief A Cutl error. */
typedef enum : int32_t {
    CU_ERROR_BAD_ALLOC, /**< A memory allocation failed. */

    CU_ERROR_NO_VALID_DEVICE, /**< The system does not have a suitable graphics device. */

    CU_ERROR_ARENA_TOO_SMALL, /**< Tried to allocate too large of data to an arena. */

    CU_ERROR_ARENA_FULL, /**< Tried to allocate to an arena that didn't have enough room left. */
} CuError;

/** @brief A Cutl result. */
typedef struct CuResult_T {
    CuResultType t; /**< The result type part. */

    int32_t v; /**< The result value part. */
} CuResult;

/**
 * @brief Returns whether the result is a success value.
 * @param r The result.
 * @return Whether the result is a success value.
 */
#define cu_is_success(r) ((r).t == CU_RESULT_TYPE_SUCCESS)

/** @brief A successful `CuResult` */
constexpr CuResult CU_SUCCESS = {
    .t = CU_RESULT_TYPE_SUCCESS,
    .v = 0,
};
