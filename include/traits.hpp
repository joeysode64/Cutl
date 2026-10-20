#pragma once

namespace cu {
    template<typename T, typename U>
    inline constexpr bool vSameAs = false;

    template<typename T>
    inline constexpr bool vSameAs<T, T> = true;

    template<typename T, typename U>
    concept SameAs = vSameAs<T, U> && vSameAs<U, T>;

    template<typename T, typename U>
    concept ConvertibleTo = requires(T n) {
        static_cast<U>(n);
    };
}
