#pragma once

#include "num.hpp"
#include "vec2.hpp"

namespace cu {
    /**
     * @brief A rectangle.
     * @note A rectangle with a negative width or height is considered invalid.
     */
    template <Num T> struct Rect {
        T x; /**< The rectangle's left bound. */

        T y; /**< The rectangle's bottom bound. */

        T w; /**< The rectangle's width. */

        T h; /**< The rectangle's height. */

        /**
         * @brief Creates a rectangle with the given x and y position an side length.
         * @param x The x position.
         * @param y The y position.
         * @param s The side length.
         */
        constexpr Rect(const T x, const T y, const T s)
            : x{x}, y{y}, w{s}, h{s}
        {
        }

        /**
         * @brief Creates a rectangle with the given x and y position and dimensions.
         * @param x The x position.
         * @param y The y position.
         * @param w The width.
         * @param h The height.
         */
        constexpr Rect(const T x, const T y, const T w, const T h)
            : x{x}, y{y}, w{w}, h{h}
        {
        }

        /**
         * @brief Returns the rectangle's position.
         * @return The rectangle's position.
         */
        constexpr Vec2<T> pos() const
        {
            return Vec2{x, y};
        }

        /**
         * @brief Returns the rectangle's dimensions.
         * @return The rectangle's dimensions.
         */
        constexpr Vec2<T> dimensions() const
        {
            return Vec2{w, h};
        }

        /**
         * @brief Returns the rectangle's area.
         * @return The rectangle's area.
         */
        constexpr T area() const
        {
            return w * h;
        }

        /**
         * @brief Returns the rectangle's perimeter.
         * @return The rectangle's perimeter.
         */
        constexpr T perimeter() const
        {
            return 2 * (w + h);
        }

        /**
         * @brief Returns the rectangle's diagonal length squared.
         * @return The rectangle's diagonal length squared.
         */
        constexpr T diag_sqr() const
        {
            return dimensions().mag_sqr();
        }

        /**
         * @brief Returns the rectangle's diagonal length.
         * @return The rectangle's diagonal length.
         */
        constexpr T diag() const
            requires Float<T>
        {
            return dimensions().mag();
        }
    };
} // namespace cu
