#pragma once

#include "version.h"
#include <stdint.h>
#include <vulkan/vulkan.h>

namespace cu {
    /** @brief A version. */
    struct Version {
        /** @brief The version major. */
        uint16_t major;

        /** @brief The version minor. */
        uint16_t minor;

        /** @brief The version patch. */
        uint16_t patch;

        /** @brief The version tweak. */
        uint16_t tweak;

        /** @brief Creates a version with a major, minor, patch, and tweak of 0. */
        constexpr Version()
            : major{}, minor{}, patch{}, tweak{}
        {
        }

        /**
         * @brief Creates a version with the given major, minor, patch, and tweak.
         * @param major The version major.
         * @param minor The version minor. Defaults to 0.
         * @param patch The version patch. Defaults to 0.
         * @param tweak The version tweak. Defaults to 0.
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
         * @note This does not use the version's tweak value.
         */
        constexpr uint32_t vk_version() const
        {
            return VK_MAKE_API_VERSION(0, major, minor, patch);
        }

        /** @brief Converts to a C-style version. */
        constexpr operator CuVersion() const
        {
            return CuVersion{
                .major = major,
                .minor = minor,
                .patch = patch,
                .tweak = tweak,
            };
        }
    };
} // namespace cu
