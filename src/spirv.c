#include "spirv.h"
#include "result.h"

#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

CuResult cu_spirv_read_from_file(
    CuSpirV* pSpirV,
    const char* path)
{
    CuResult result = CU_SUCCESS;

    uint32_t* data = nullptr;

    FILE* f = fopen(path, "rb");
    if (f == nullptr) {
        result = CU_ERROR_FILE_IO;
        goto FAIL;
    }

    fseek(f, 0, SEEK_END);
    const size_t size = ftell(f);
    if (size % sizeof(uint32_t) != 0) {
        result = CU_ERROR_FILE_IO;
        goto FAIL;
    }
    fseek(f, 0, SEEK_SET);

    const size_t n = size / sizeof(uint32_t);
    data = calloc(n, sizeof(uint32_t));
    if (data == nullptr) {
        result = CU_ERROR_FILE_IO;
        goto FAIL;
    }

    if (fread(data, sizeof(uint32_t), n, f) != n) {
        result = CU_ERROR_FILE_IO;
        goto FAIL;
    }

    fclose(f);

    pSpirV->n = n;
    pSpirV->data = data;
    return CU_SUCCESS;

FAIL:
    fclose(f);
    free(data);
    return result;
}

void cu_spirv_destroy(
    CuSpirV* pSpirV)
{
    free(pSpirV->data);
}
