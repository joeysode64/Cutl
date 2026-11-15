#pragma once

#include "num.hpp"
#include "vec2.hpp"

namespace cu {
    /**
     * @brief A rectangle.
     * @note A rectangle with a negative width or height is considered invalid.
     */
    template <Num T> struct Rect {
        /** @brief The rectangle's left bound. */
        T x;

        /** @brief The rectangle's bottom bound. */
        T y;

        /** @brief The rectangle's width. */
        T w;

        /** @brief The rectangle's height. */
        T h;

        /**
         * @brief Creates a square with the given x and y position and side length.
         * @param x The x position of the left bound.
         * @param y The y position of the bottom bound.
         * @param s The side length. Must not be negative.
         */
        constexpr Rect(const T x, const T y, const T s)
            : x{x}, y{y}, w{s}, h{s}
        {
        }

        /**
         * @brief Creates a rectangle with the given x and y position and dimensions.
         * @param x The x position of the left bound.
         * @param y The y position of the bottom bound.
         * @param w The width. Must not be negative.
         * @param h The height. Must not be negative.
         */
        constexpr Rect(const T x, const T y, const T w, const T h)
            : x{x}, y{y}, w{w}, h{h}
        {
        }

        /** @brief Returns the rectangle's position. */
        constexpr Vec2<T> pos() const
        {
            return Vec2{x, y};
        }

        /** @brief Returns the rectangle's dimensions. */
        constexpr Vec2<T> dimensions() const
        {
            return Vec2{w, h};
        }

        /** @brief Returns the rectangle's area. */
        constexpr T area() const
        {
            return w * h;
        }

        /** @brief Returns the rectangle's perimeter. */
        constexpr T perimeter() const
        {
            return 2 * (w + h);
        }

        /** @brief Returns the rectangle's diagonal length squared. */
        constexpr T diag_sqr() const
        {
            return dimensions().mag_sqr();
        }

        /** @brief Returns the rectangle's diagonal length. */
        constexpr T diag() const
            requires Float<T>
        {
            return dimensions().mag();
        }
    };
} // namespace cu
