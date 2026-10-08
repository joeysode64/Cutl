#pragma once

#include <stdint.h>

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
         * @param xy The x and y value.
         */
        constexpr Vec2(
            const T xy)
            : x{xy}, y{xy}
        {

        }

        /**
         * @brief Creates a vector with the given x and y value.
         * @param x The x value.
         * @param y The y value.
         */
        constexpr Vec2(
            const T x,
            const T y)
            : x{x}, y{y}
        {

        }

        /**
         * @brief Returns the cross product of the two vectors.
         * @param rhs The other vector.
         * @return The cross product of the two vectors.
         */
        constexpr T dot(
            const Vec2<T> rhs) const
        {
            return (x * rhs.x) + (y * rhs.y);
        }

        /**
         * @brief Returns the vector's magnitude squared.
         * @return The vector's magnitude squared.
         */
        constexpr T mag_sqr() const
        {
            return (x * x) + (y * y);
        }

        template<typename U>
        explicit constexpr operator Vec2<U>() const
        {
            return Vec2<U>{static_cast<U>(x), static_cast<U>(y)};
        }

        constexpr Vec2<T> operator~() const
        {
            return Vec2{y, x};
        }

        constexpr Vec2<T> operator+(
            const Vec2<T> rhs) const
        {
            return Vec2{x + rhs.x, y + rhs.y};
        }

        constexpr Vec2<T> operator-(
            const Vec2<T> rhs) const
        {
            return Vec2{x - rhs.x, y - rhs.y};
        }

        constexpr Vec2<T> operator*(
            const Vec2<T> rhs) const
        {
            return Vec2{x * rhs.x, y * rhs.y};
        }

        constexpr Vec2<T> operator/(
            const Vec2<T> rhs) const
        {
            return Vec2{x / rhs.x, y / rhs.y};
        }

        constexpr Vec2<T> operator+(
            const T rhs) const
        {
            return Vec2{x + rhs, y + rhs};
        }

        constexpr Vec2<T> operator-(
            const T rhs) const
        {
            return Vec2{x - rhs, y - rhs};
        }

        constexpr Vec2<T> operator*(
            const T rhs) const
        {
            return Vec2{x * rhs, y * rhs};
        }

        constexpr Vec2<T> operator/(
            const T rhs) const
        {
            return Vec2{x / rhs, y / rhs};
        }

        constexpr Vec2<T>& operator+=(
            const Vec2<T> rhs)
        {
            x += rhs.x;
            y += rhs.y;
            return *this;
        }

        constexpr Vec2<T>& operator-=(
            const Vec2<T> rhs)
        {
            x -= rhs.x;
            y -= rhs.y;
            return *this;
        }

        constexpr Vec2<T>& operator*=(
            const Vec2<T> rhs)
        {
            x *= rhs.x;
            y *= rhs.y;
            return *this;
        }

        constexpr Vec2<T>& operator/=(
            const Vec2<T> rhs)
        {
            x /= rhs.x;
            y /= rhs.y;
            return *this;
        }

        constexpr Vec2<T>& operator+=(
            const T rhs)
        {
            x += rhs;
            y += rhs;
            return *this;
        }

        constexpr Vec2<T>& operator-=(
            const T rhs)
        {
            x -= rhs;
            y -= rhs;
            return *this;
        }

        constexpr Vec2<T>& operator*=(
            const T rhs)
        {
            x *= rhs;
            y *= rhs;
            return *this;
        }

        constexpr Vec2<T>& operator/=(
            const T rhs)
        {
            x /= rhs;
            y /= rhs;
            return *this;
        }
    };

    using Vec2F = Vec2<float>;

    using Vec2I = Vec2<int32_t>;

    using Vec2U = Vec2<uint32_t>;
}
