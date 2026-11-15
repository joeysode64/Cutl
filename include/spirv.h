#pragma once

#include "result.h"

#include <stddef.h>
#include <stdint.h>

/** @brief A shader's source SPIR-V code. */
typedef struct CuSpirv_T {
    /** @brief The number of words in the SPIR-V (length of `.data`). */
    size_t n;

    /** @brief A pointer to the SPIR-V code. */
    uint32_t* data;
} CuSpirV;

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Reads the SPIR-V from a file into an object.
 * @param[out] pSpirV A pointer to the SPIR-V object. Must not be null.
 * @param path The path to the SPIR-V file. Must not be null.
 * @note The SPIR-V object must be destroyed with `cu_spirv_destroy`.
 */
CU_NODISCARD_RESULT
CuResult cu_spirv_read_from_file(CuSpirV* pSpirV, const char* path);

/**
 * @brief Destroys the SPIR-V object.
 * @param[in out] pSpirV A pointer to the SPIR-V object. Must not be null.
 */
void cu_spirv_destroy(CuSpirV* pSpirV);

#ifdef __cplusplus
}
#endif
