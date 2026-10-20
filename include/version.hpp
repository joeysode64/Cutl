#pragma once

#include "version.h"
#include <stdint.h>
#include <vulkan/vulkan.h>

namespace cu
{
    /** @brief A version. */
    struct Version {
        uint16_t major; /**< The version major. */

        uint16_t minor; /**< The version minor. */

        uint16_t patch; /**< The version patch. */

        uint16_t tweak; /**< The version tweak. */

        /** @brief Creates a version with a major minor patch and tweak version of 0. */
        constexpr Version()
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
        constexpr Version(
            const uint16_t major,
            const uint16_t minor = {},
            const uint16_t patch = {},
            const uint16_t tweak = {})
            : major{major}, minor{minor}, patch{patch}, tweak{tweak}
        {

        }

        /**
        * @brief Returns the version as a `uint32_t` Vulkan-style version.
        * @return The version as a `uint32_t` Vulkan-style version.
        * @note This does not use the version's tweak value.
        */
        constexpr uint32_t vk_version() const {
            return VK_MAKE_API_VERSION(0, major, minor, patch);
        }

        constexpr operator CuVersion() const {
            return CuVersion{
                .major = major,
                .minor = minor,
                .patch = patch,
                .tweak = tweak,
            };
        }
    };
}
