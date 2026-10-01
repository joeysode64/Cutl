#pragma once

#include <stdint.h>

namespace cu
{
    /** @brief A version. */
    template<typename T = uint16_t>
    struct Version {
        T major; /**< The version major. */

        T minor; /**< The version minor. */

        T patch; /**< The version patch. */

        T tweak; /**< The version tweak. */

        /** @brief Creates a version with a major minor patch and tweak version of 0. */
        constexpr inline Version()
            : major{}, minor{}, patch{}, tweak{}
        {

        }

        /** 
         * @brief Creates a version with the given major minor and patch.
         * @param major The version major.
         * @param minor The version minor (ignore for 0).
         * @param patch The version patch (ignore for 0).
         * @param tweak The version tweak (ignore for 0).
         */
        constexpr inline Version(
            const T major,
            const T minor = {},
            const T patch = {},
            const T tweak = {})
            : major{major}, minor{minor}, patch{patch}, tweak{tweak}
        {

        }
    };
}
