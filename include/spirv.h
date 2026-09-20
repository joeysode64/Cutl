#pragma once

#include "result.h"

#include <stddef.h>
#include <stdint.h>

/// @brief A shader's source SPIR-V code.
typedef struct CuSpirv_T {
    /// @brief The number of words in the SPIR-V (length of `.data`).
    size_t n;

    /// @brief The SPIR-V code.
    uint32_t* data;
} CuSpirV;

/// @brief Reads the SPIR-V from a file into an object.
/// @param [out] pSpirV A pointer to the SPIR-V object..
/// @param path The source file path.
/// @return The result of the SPIR-V reading.
CuResult cu_spirv_read_from_file(
    CuSpirV* pSpirV,
    const char* path);

/// @brief Destroys the SPIR-V object.
/// @param pSpirV A pointer to the SPIR-V object to destroy.
void cu_spirv_destroy(
    CuSpirV* pSpirV);
