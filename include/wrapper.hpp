#pragma once

namespace cu {
    /** @brief A wrapper for a Cutl C structure. */
    template <typename T> class Wrapper : protected T {
    public:
        /** @brief Creates the value initialized to zero. */
        constexpr Wrapper()
            : T{}
        {
        }

        /**
         * @brief Returns the internal value.
         * @return The internal value.
         */
        constexpr T& get()
        {
            return *this;
        }

        /**
         * @brief Returns the internal value.
         * @return The internal value.
         */
        constexpr const T& get() const
        {
            return *this;
        }
    };

    /** @brief A wrapper for an opaque type. */
    template <typename T> class CWrapper {
    protected:
        T _v; /**< The wrapped value. */

    public:
        /**
         * @brief Returns the internal value.
         * @return The internal value.
         */
        constexpr T& get()
        {
            return _v;
        }

        /**
         * @brief Returns the internal value.
         * @return The internal value.
         */
        constexpr const T& get() const
        {
            return _v;
        }

        operator T() const
        {
            return _v;
        }
    };
} // namespace cu
