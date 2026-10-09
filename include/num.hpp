#pragma once

#include <math.h>

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

    inline constexpr float sqrt(
        const float x)
    {
        return ::sqrtf(x);
    }

    inline constexpr double sqrt(
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
