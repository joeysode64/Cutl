#pragma once

#include <stdint.h>

/** @brief A result type. */
typedef enum : uint32_t {
    /** @brief A success. The result's `.v` is irrelevant. */
    CU_RESULT_TYPE_SUCCESS = 0,

    /** @brief A Cutl error. The result's `.v` is the matching `CuError`. */
    CU_RESULT_TYPE_ERROR,

    /** @brief A C standard library error. Ther result's `.v` is `errno`. */
    CU_RESULT_TYPE_STD,

    /** @brief A Vulkan error. The result's `.v` is the `VkResult`. */
    CU_RESULT_TYPE_VULKAN,

    /** @brief A GLFW error. The result's `.v` is the value of `glfwGetError`. */
    CU_REUSLT_TYPE_GLFW,
} CuResultType;

/** @brief A Cutl error. */
typedef enum : int32_t {
    /** @brief A memory allocation failed. */
    CU_ERROR_BAD_ALLOC,

    /** @brief The system does not have a suitable graphics device. */
    CU_ERROR_NO_VALID_DEVICE,

    /** @brief Tried to allocate too large of data to an arena. */
    CU_ERROR_ARENA_TOO_SMALL,

    /** @brief Tried to allocate to an arena that didn't have enough room left. */
    CU_ERROR_ARENA_FULL,

    /** @brief Failed to read a PNG image. */
    CU_ERROR_PNG_READ,
} CuError;

/** @brief A Cutl result. */
typedef struct CuResult_T {
    /** @brief The result type part. */
    CuResultType t;

    /** @brief The result value part. */
    int32_t v;
} CuResult;

/**
 * @brief Returns whether the result is a success value.
 * @param r The result.
 */
#define cu_is_success(r) ((r).t == CU_RESULT_TYPE_SUCCESS)

/** @brief A successful `CuResult` */
constexpr CuResult CU_SUCCESS = {
    .t = CU_RESULT_TYPE_SUCCESS,
    .v = 0,
};

/** @brief A tag for a function that returns a `CuResult`. */
#define CU_NODISCARD_RESULT [[nodiscard("can fail and return an error")]]
