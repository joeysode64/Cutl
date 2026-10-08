#pragma once

#include <stdint.h>

/** @brief A 2D vector of 32-bit floats. */
typedef struct {
    float x;
    float y;
} vec2f;

/** @brief A 2D vector of 32-bit signed integers. */
typedef struct {
    int32_t x;
    int32_t y;
} vec2i;

/** @brief A 2D vector of 32-bit unsigned integers. */
typedef struct {
    uint32_t x;
    uint32_t y;
} vec2u;

typedef vec2f vec2;

/** @brief A 3D vector of 32-bit floats. */
typedef struct {
    float x;
    float y;
    float z;
} vec3f;

/** @brief A 3D vector of 32-bit signed integers. */
typedef struct {
    int32_t x;
    int32_t y;
    int32_t z;
} vec3i;

/** @brief A 3D vector of 32-bit unsigned integers. */
typedef struct {
    uint32_t x;
    uint32_t y;
    uint32_t z;
} vec3u;

typedef vec3f vec3;

/** @brief A 4D vector of 32-bit floats. */
typedef struct {
    float x;
    float y;
    float z;
    float w;
} vec4f;

/** @brief A 4D vector of 32-bit signed integers. */
typedef struct {
    int32_t x;
    int32_t y;
    int32_t z;
    int32_t w;
} vec4i;

/** @brief A 4D vector of 32-bit unsigned integers. */
typedef struct {
    uint32_t x;
    uint32_t y;
    uint32_t z;
    uint32_t w;
} vec4u;

typedef vec4f vec4;
