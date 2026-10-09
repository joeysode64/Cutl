#pragma once

#include "num.hpp"
#include "vec2.hpp"

#include <stdint.h>

namespace cu
{
    /** @brief A 2-dimensional vector. */
    template<Num T>
    struct Vec3 {
        T x; /**< The x factor. */

        T y; /**< The y factor. */

        T z; /**< The z factor. */

        /** @brief Creates a vector with an x and y of 0. */
        constexpr Vec3()
            : x{}, y{}, z{}
        {

        }

        /**
         * @brief Creates a vector with the given x y and z value.
         * @param xyz The x y and z value.
         */
        constexpr Vec3(
            const T xyz)
            : x{xyz}, y{xyz}, z{xyz}
        {

        }

        /**
         * @brief Creates a vector with the given x y and z value.
         * @param x The x value.
         * @param y The y value.
         * @param z The z value.
         */
        constexpr Vec3(
            const T x,
            const T y,
            const T z)
            : x{x}, y{y}, z{z}
        {

        }

        /**
         * @brief Creates a vector with the given x y and z value.
         * @param xy The x and y values.
         * @param z The z value. Ignore for 0.
         */
        constexpr Vec3(
            const Vec2<T> xy,
            const T z = {})
            : x{xy.x}, y{xy.y}, z{z}
        {

        }

        /**
         * @brief Creates a vector with the given x y and z value.
         * @param x The x value.
         * @param yz The y and z value.
         */
        constexpr Vec3(
            const T x,
            const Vec2<T> yz)
            : x{x}, y{yz.y}, z{yz.z}
        {

        }

        /**
         * @brief Returns the cross product of the two vectors.
         * @param rhs The other vector.
         * @return The cross product of the two vectors.
         */
        constexpr T dot(
            const Vec3<T> rhs) const 
        {
            return (x * rhs.x) + (y * rhs.y) + (z * rhs.z);
        }

        /**
         * @brief Returns the distance between the vectors squared.
         * @param v The other vector.
         * @return The distance between the vectors squared.
         */
        constexpr T dst_sqr(
            const Vec3<T> v) const
        {
            const T dx = safe_diff(v.x, x);
            const T dy = safe_diff(v.y, y);
            const T dz = safe_diff(v.z, z);
            return (dx * dx) + (dy * dy) + (dz * dz);
        }

        /**
         * @brief Returns the distance between the vectors.
         * @param v The other vector.
         * @return The distance between the vectors.
         */
        constexpr T dst(
            const Vec3<T> v) const
            requires Float<T>
        {
            return sqrt(dst_sqr(v));
        }

        /**
         * @brief Returns the vector's magnitude squared.
         * @return The vector's magnitude squared.
         */
        constexpr T mag_sqr() const
        {
            return (x * x) + (y * y) + (z * z);
        }

        /**
         * @brief Returns the vector's magnitude.
         * @return The vector's magnitude.
         */
        constexpr T mag() const
            requires Float<T>
        {
            return sqrt(mag_sqr());
        }

        /**
         * @brief Returns the vector normalized.
         * @return The vector normalized.
         */
        constexpr Vec3<T> normalized() const
            requires Float<T>
        {
            return *this / mag();
        }

        /** @brief Normalizes the vector. */
        constexpr void normalize()
            requires Float<T>
        {
            *this /= mag();
        } 

        /**
         * @brief Returns the vector converted to the given type.
         * @return The vector converted to the given type.
         */
        template<Num U>
        constexpr Vec3<U> as() const
        {
            Vec3<U>{static_cast<U>(x), static_cast<U>(y), static_cast<U>(z)};
        }

        template<typename U>
        explicit constexpr operator Vec3<U>() const
        {
            return Vec3<U>{static_cast<U>(x), static_cast<U>(y), static_cast<U>(z)};
        }

        constexpr Vec3<T> operator+(
            const Vec3<T> rhs) const
        {
            return Vec3{x + rhs.x, y + rhs.y, z + rhs.z};
        }

        constexpr Vec3<T> operator-(
            const Vec3<T> rhs) const
        {
            return Vec3{x - rhs.x, y - rhs.y, z - rhs.z};
        }

        constexpr Vec3<T> operator*(
            const Vec3<T> rhs) const
        {
            return Vec3{x * rhs.x, y * rhs.y, z * rhs.z};
        }

        constexpr Vec3<T> operator/(
            const Vec3<T> rhs) const
        {
            return Vec3{x / rhs.x, y / rhs.y, z / rhs.z};
        }

        constexpr Vec3<T> operator+(
            const Vec2<T> rhs) const
        {
            return Vec3{x + rhs.x, y + rhs.y, z};
        }

        constexpr Vec3<T> operator-(
            const Vec2<T> rhs) const
        {
            return Vec3{x - rhs.x, y - rhs.y, z};
        }

        constexpr Vec3<T> operator*(
            const Vec2<T> rhs) const
        {
            return Vec3{x * rhs.x, y * rhs.y, z};
        }

        constexpr Vec3<T> operator/(
            const Vec2<T> rhs) const
        {
            return Vec3{x / rhs.x, y / rhs.y, z};
        }

        constexpr Vec3<T> operator+(
            const T rhs) const
        {
            return Vec3{x + rhs, y + rhs, z + rhs};
        }

        constexpr Vec3<T> operator-(
            const T rhs) const
        {
            return Vec3{x - rhs, y - rhs, z - rhs};
        }

        constexpr Vec3<T> operator*(
            const T rhs) const
        {
            return Vec3{x * rhs, y * rhs, z * rhs};
        }

        constexpr Vec3<T> operator/(
            const T rhs) const
        {
            return Vec3{x / rhs, y / rhs, z / rhs};
        }

        constexpr Vec3<T>& operator+=(
            const Vec3<T> rhs)
        {
            x += rhs.x;
            y += rhs.y;
            z += rhs.z;
            return *this;
        }

        constexpr Vec3<T>& operator-=(
            const Vec3<T> rhs)
        {
            x -= rhs.x;
            y -= rhs.y;
            z -= rhs.z;
            return *this;
        }

        constexpr Vec3<T>& operator*=(
            const Vec3<T> rhs)
        {
            x *= rhs.x;
            y *= rhs.y;
            z *= rhs.z;
            return *this;
        }

        constexpr Vec3<T>& operator/=(
            const Vec3<T> rhs)
        {
            x /= rhs.x;
            y /= rhs.y;
            z /= rhs.z;
            return *this;
        }

        constexpr Vec3<T>& operator+=(
            const Vec2<T> rhs)
        {
            x += rhs.x;
            y += rhs.y;
            return *this;
        }

        constexpr Vec3<T>& operator-=(
            const Vec2<T> rhs)
        {
            x -= rhs.x;
            y -= rhs.y;
            return *this;
        }

        constexpr Vec3<T>& operator*=(
            const Vec2<T> rhs)
        {
            x *= rhs.x;
            y *= rhs.y;
            return *this;
        }

        constexpr Vec3<T>& operator/=(
            const Vec2<T> rhs)
        {
            x /= rhs.x;
            y /= rhs.y;
            return *this;
        }

        constexpr Vec3<T>& operator+=(
            const T rhs)
        {
            x += rhs;
            y += rhs;
            z += rhs;
            return *this;
        }

        constexpr Vec3<T>& operator-=(
            const T rhs)
        {
            x -= rhs;
            y -= rhs;
            z -= rhs;
            return *this;
        }

        constexpr Vec3<T>& operator*=(
            const T rhs)
        {
            x *= rhs;
            y *= rhs;
            z *= rhs;
            return *this;
        }

        constexpr Vec3<T>& operator/=(
            const T rhs)
        {
            x /= rhs;
            y /= rhs;
            z /= rhs;
            return *this;
        }
    };

    using Vec3F = Vec3<float>;

    using Vec3I = Vec3<int32_t>;

    using Vec3U = Vec3<uint32_t>;
}
