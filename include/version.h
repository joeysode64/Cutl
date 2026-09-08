#pragma once

#include <stdint.h>

/** @brief A version. */
typedef struct CuVersion_T {
    uint16_t major; /**< The version major. */

    uint16_t minor; /**< The version minor. */

    uint16_t patch; /**< The version patch. */
} CuVersion;
