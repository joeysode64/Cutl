#pragma once

#include "result.h"

#include <stdint.h>

namespace cu {
    /** @brief A result type. */
    enum class ResultType : uint32_t {
        /** @brief A success. The result's `.v` is irrelevant. */
        Success = 0,

        /** @brief A Cutl error. The result's `.v` is the matching `CuError`. */
        Cu,

        /** @brief A C standard library error. Ther result's `.v` is `errno`. */
        Std,

        /** @brief A Vulkan error. The result's `.v` is the `VkResult`. */
        Vulkan,

        /** @brief A GLFW error. The result's `.v` is the value of `glfwGetError`. */
        Glfw,
    };

    /** @brief A Cutl error. */
    enum class Error : int32_t {
        /** @brief A memory allocation failed. */
        BadAlloc,

        /** @brief The system does not have a suitable graphics device. */
        NoValidDevice,

        /** @brief Tried to allocate too large of data to an arena. */
        ArenaTooSmall,

        /** @brief Tried to allocate to an arena that didn't have enough room left. */
        ArenaFull,
    };

    /** @brief A Cutl result. */
    class Result {
    private:
        /** @brief The result type. */
        ResultType t;

        /** @brief The result value. */
        int32_t v;

    public:
        /**
         * @brief Creates a result with the given result type and value.
         * @param t The result type.
         * @param v The result value. Its meaning depends on `t`.
         */
        constexpr Result(const ResultType t, const int32_t v)
            : t{t}, v{v}
        {
        }

        /**
         * @brief Creates a result from the C-style result.
         * @param r The C-style result to convert.
         */
        constexpr Result(const CuResult r)
            : t{r.t}, v{r.v}
        {
        }

        /** @brief Returns the result type. */
        constexpr ResultType type() const
        {
            return t;
        }

        /** @brief Returns the result value. */
        constexpr int32_t value() const
        {
            return v;
        }

        /** @brief Returns whether the result is a success. */
        constexpr bool is_success() const
        {
            return t == ResultType::Success;
        }

        /** @brief Converts to a C-style result. */
        constexpr operator CuResult()
        {
            return CuResult{.t = static_cast<CuResultType>(t), .v = v};
        }
    };

    /** @brief A successful result. */
    constexpr Result SUCCESS{ResultType::Success, 0};
} // namespace cu
