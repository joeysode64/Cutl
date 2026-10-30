#pragma once

#include <stdint.h>

/** @brief A 2D vector of 32-bit floats. */
typedef struct {
    float x;
    float y;
} CuVec2F;

/** @brief A 2D vector of 32-bit signed integers. */
typedef struct {
    int32_t x;
    int32_t y;
} CuVec2I;

/** @brief A 2D vector of 32-bit unsigned integers. */
typedef struct {
    uint32_t x;
    uint32_t y;
} CuVec2U;

typedef CuVec2F CuVec2;

/** @brief A 3D vector of 32-bit floats. */
typedef struct {
    float x;
    float y;
    float z;
} CuVec3F;

/** @brief A 3D vector of 32-bit signed integers. */
typedef struct {
    int32_t x;
    int32_t y;
    int32_t z;
} CuVec3I;

/** @brief A 3D vector of 32-bit unsigned integers. */
typedef struct {
    uint32_t x;
    uint32_t y;
    uint32_t z;
} CuVec3U;

typedef CuVec3F CuVec3;

/** @brief A 4D vector of 32-bit floats. */
typedef struct {
    float x;
    float y;
    float z;
    float w;
} CuVec4F;

/** @brief A 4D vector of 32-bit signed integers. */
typedef struct {
    int32_t x;
    int32_t y;
    int32_t z;
    int32_t w;
} CuVec4I;

/** @brief A 4D vector of 32-bit unsigned integers. */
typedef struct {
    uint32_t x;
    uint32_t y;
    uint32_t z;
    uint32_t w;
} CuVec4U;

typedef CuVec4F CuVec4;

#if !defined(CU_NO_GLSL_DEF)
typedef CuVec2F vec2f;
typedef CuVec2I vec2i;
typedef CuVec2U vec2u;
typedef CuVec2 vec2;

typedef CuVec3F vec3f;
typedef CuVec3I vec3i;
typedef CuVec3U vec3u;
typedef CuVec3 vec3;

typedef CuVec4F vec4f;
typedef CuVec4I vec4i;
typedef CuVec4U vec4u;
typedef CuVec4 vec4;
#endif
