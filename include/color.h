#pragma once

#include <stdint.h>

/** @brief An RGBA color. */
typedef struct {
    uint8_t r; /**< The red value. */

    uint8_t g; /**< The green value. */

    uint8_t b; /**< The blue value. */

    uint8_t a; /**< The alpha value. */
} CuRGBA;

/** @brief A black RGBA value. */
constexpr CuRGBA CU_RGBA_BLACK = { .r = 0x00, .g = 0x00, .b = 0x00, .a = 0xFF };

/** @brief A white RGBA value. */
constexpr CuRGBA CU_RGBA_WHITE = { .r = 0xFF, .g = 0xFF, .b = 0xFF, .a = 0xFF };
