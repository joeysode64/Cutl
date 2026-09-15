#include "dynamic_buffer.h"

#include "allocation.h"
#include "allocation_fns.h"
#include "buffer.h"
#include "g_context.h"
#include "result.h"
#include "vk.h"

#include <stdint.h>
#include <stddef.h>
#include <vulkan/vulkan.h>

CuResult cu_dynamic_buffer_create(
    CuDynamicBuffer* const pDynamicBuffer,
    const size_t n,
    const size_t z,
    const CuBufferUsage usage,
    const CuAllocationMode mode)
{
    CuResult result = CU_ERROR_UNKNOWN;

    cu_try_catch(create_buffer(
        &pDynamicBuffer->_buffer,
        &pDynamicBuffer->_allocation,
        &pDynamicBuffer->_p,
        n * z,
        usage | VK_BUFFER_USAGE_SHADER_DEVICE_ADDRESS_BIT,
        mode,
        VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT,
        0));
    pDynamicBuffer->_address = get_buffer_device_address(pDynamicBuffer->_buffer);
    pDynamicBuffer->_n = n;
    pDynamicBuffer->_z = z;

    return CU_SUCCESS;

FAIL:
    cu_dynamic_buffer_destroy(pDynamicBuffer, mode);
    return result;
}

void cu_dynamic_buffer_destroy(
    CuDynamicBuffer* const pDynamicBuffer,
    const CuAllocationMode mode)
{
    vkDestroyBuffer(gContext.device, pDynamicBuffer->_buffer, nullptr);
    mode_free(&pDynamicBuffer->_allocation, mode);
}

void cu_dynamic_buffer_get_data_frame(
    CuDynamicBuffer* const pDynamicBuffer,
    void** const ppData,
    CuDeviceAddress* const pDeviceAddress,
    const size_t i)
{
    const size_t offset = pDynamicBuffer->_z * i;
    *ppData = (uint8_t*)pDynamicBuffer->_p + offset;
    *pDeviceAddress = pDynamicBuffer->_address + offset;
}
