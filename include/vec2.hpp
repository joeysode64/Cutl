#pragma once

namespace cu
{
    /** @brief A 2-dimensional vector. */
    template<typename T>
    struct Vec2 {
        T x; /**< The x factor. */

        T y; /**< The y factor. */

        /** @brief Creates a vector with an x and y of 0. */
        constexpr Vec2()
            : x{}, y{}
        {

        }

        /**
         * @brief Creates a vector with the given x and y value.
         *
         * @param xy The x and y value.
         */
        constexpr Vec2(T xy)
            : x{xy}, y{xy}
        {

        }

        /**
         * @brief Creates a vector with the given x and y value.
         *
         * @param x The x value.
         * @param y The y value.
         */
        constexpr Vec2(T x, T y)
            : x{x}, y{y}
        {
            
        }
    };
}
