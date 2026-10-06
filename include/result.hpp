#pragma once

#include "result.h"

#include <stdint.h>

namespace cu
{
    /** @brief A result type. */
    enum class ResultType : uint32_t {
        Success = 0, /**< A success. The result's `.v` is irrelevant. */

        Cu, /**< A Cutl error. The result's `.v` is the matching `CuError`. */

        Std, /**< A C standard library error. Ther result's `.v` is `errno`. */

        Vulkan, /**< A Vulkan error. The result's `.v` is the `VkResult`. */

        Glfw, /**< A GLFW error. The result's `.v` is the value of `glfwGetError`. */
    };

    /** @brief A Cutl error. */
    enum class Error : int32_t {
        BadAlloc, /**< A memory allocation failed. */

        NoValidDevice, /**< The system does not have a suitable graphics device. */

        ArenaTooSmall, /**< Tried to allocate too large of data to an arena. */

        ArenaFull, /**< Tried to allocate to an arena that didn't have enough room left. */
    };

    /** @brief A Cutl result. */
    class Result {
    private:
        ResultType t; /**< The result type. */

        int32_t v; /**< The result value. */
    
    public:
        /** @brief Creates a result with the given result type and value. */
        constexpr Result(
            const ResultType t,
            const int32_t v)
            : t{t}, v{v}
        {

        }

        /** @brief Creates a result from the C-style result. */
        constexpr Result(
            const CuResult r)
            : t{r.t}, v{r.v}
        {

        }

        /**
         * @brief Returns the result type.
         * @return The result type.
         */
        constexpr ResultType type() const
        {
            return t;
        }

        /**
         * @brief Returns the result value.
         * @return The result value.
         */
        constexpr int32_t value() const
        {
            return v;
        }

        /**
         * @brief Returns whether the result is a success.
         * @return Whether the result is a success.
         */
        constexpr bool is_success() const
        {
            return t == ResultType::Success;
        }

        /**
         * @brief Converts to a C-style result.
         * @return The result as a C-style result.
         */
        constexpr operator CuResult()
        {
            return CuResult{
                .t = static_cast<CuResultType>(t),
                .v = v
            };
        }
    };

    /** @brief A successful result. */
    constexpr Result SUCCESS{ResultType::Success, 0};
}
