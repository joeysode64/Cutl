#pragma once

#include "info.h"
#include "version.hpp"

#include <stdint.h>

namespace cu
{
    /** @brief The Cutl version major. */
    constexpr uint16_t VERSION_MAJOR = CU_VERSION_MAJOR;

    /** @brief The Cutl version minor. */
    constexpr uint16_t VERSION_MINOR = CU_VERSION_MINOR;

    /** @brief The Cutl version patch. */
    constexpr uint16_t VERSION_PATCH = CU_VERSION_PATCH;

    /** @brief The Cutl version tweak. */
    constexpr uint16_t VERSION_TWEAK = CU_VERSION_TWEAK;

    /** @brief The Cutl version. */
    constexpr Version VERSION{
        VERSION_MAJOR,
        VERSION_MINOR,
        VERSION_PATCH,
        VERSION_TWEAK,
    };
}
