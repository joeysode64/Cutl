#pragma once

#include <math.h>
#include <stdint.h>

namespace cu
{
    template<typename T, typename U>
    inline constexpr bool vIsSame = false;

    template<typename T>
    inline constexpr bool vIsSame<T, T> = true;

    template<typename T, typename U>
    concept SameAs = vIsSame<T, U> && vIsSame<U, T>;

    template<typename T, typename U>
    concept ConvertibleTo = requires(T n) {
        static_cast<U>(n);
    };

    /** @brief A concept for numeric/arithmetic types. */
    template<typename T>
    concept Num = requires(T x, T y) {
        { x + y } -> ConvertibleTo<T>;
        { x - y } -> ConvertibleTo<T>;
        { x * y } -> ConvertibleTo<T>;
        { x / y } -> ConvertibleTo<T>;
    };

    template<Num T>
    constexpr T safe_diff(
        const T x,
        const T y)
    {
        return x - y;
    }

    template<>
    constexpr uint8_t safe_diff<uint8_t>(
        const uint8_t x,
        const uint8_t y)
    {
        return (x >= y) ? (x - y) : (y - x);
    }

    template<>
    constexpr uint16_t safe_diff<uint16_t>(
        const uint16_t x,
        const uint16_t y)
    {
        return (x >= y) ? (x - y) : (y - x);
    }

    template<>
    constexpr uint32_t safe_diff<uint32_t>(
        const uint32_t x,
        const uint32_t y)
    {
        return (x >= y) ? (x - y) : (y - x);
    }

    template<>
    constexpr uint64_t safe_diff<uint64_t>(
        const uint64_t x,
        const uint64_t y)
    {
        return (x >= y) ? (x - y) : (y - x);
    }

    constexpr float sqrt(
        const float x)
    {
        return ::sqrtf(x);
    }

    constexpr double sqrt(
        const double x)
    {
        return ::sqrt(x);
    }

    /** @brief A concept for floats. */
    template<typename T>
    concept Float = Num<T> && requires(T n) {
        { sqrt(n) } -> SameAs<T>;
    };
}
