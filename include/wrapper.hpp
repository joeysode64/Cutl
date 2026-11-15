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

        /** @brief Returns the internal value. */
        constexpr T& get()
        {
            return *this;
        }

        /** @brief Returns the internal value. */
        constexpr const T& get() const
        {
            return *this;
        }
    };

    /** @brief A wrapper for an opaque type. */
    template <typename T> class CWrapper {
    protected:
        /** @brief The wrapped value. */
        T _v;

    public:
        /** @brief Returns the internal value. */
        constexpr T& get()
        {
            return _v;
        }

        /** @brief Returns the internal value. */
        constexpr const T& get() const
        {
            return _v;
        }

        /** @brief Converts to the internal value. */
        operator T() const
        {
            return _v;
        }
    };
} // namespace cu
