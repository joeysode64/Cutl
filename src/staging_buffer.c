#include "staging_buffer.h"

#include "allocation.h"
#include "g_context.h"
#include "result.h"
#include "util.h"
#include "vk.h"

#include <stddef.h>
#include <stdint.h>
#include <string.h>
#include <vulkan/vulkan.h>

CuResult cu_staging_buffer_create(
    CuStagingBuffer* const pStagingBuffer,
    const size_t z,
    const CuAllocationMode mode)
{
    CuResult result = CU_SUCCESS;

    cu_try_catch(create_buffer(
        &pStagingBuffer->_buffer,
        &pStagingBuffer->_allocation,
        &pStagingBuffer->_pData,
        z,
        VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
        mode,
        VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
        0));

    return CU_SUCCESS;

FAIL:
    cu_staging_buffer_destroy(pStagingBuffer, mode);
    return result;
}

void cu_staging_buffer_destroy(
    CuStagingBuffer* const pStagingBuffer,
    const CuAllocationMode mode)
{
    mode_free(&pStagingBuffer->_allocation, mode);
    vkDestroyBuffer(gContext._device, pStagingBuffer->_buffer, nullptr);
}

void cu_staging_buffer_copy(
    CuStagingBuffer* const pStagingBuffer,
    const void* const p,
    const size_t z,
    const size_t o)
{
    memcpy((uint8_t*)pStagingBuffer->_pData + o, p, z);
}
