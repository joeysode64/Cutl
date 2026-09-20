#pragma once

#include <stdint.h>

/// @brief A version.
typedef struct CuVersion_T {
    /// @brief The version major.
    uint16_t major;

    /// @brief The version minor.
    uint16_t minor;

    /// @brief The version patch.
    uint16_t patch;
} CuVersion;
