#include "static_buffer.h"

#include "allocation.h"
#include "buffer.h"
#include "g_context.h"
#include "result.h"
#include "util.h"
#include "vk.h"

#include <stddef.h>
#include <vulkan/vulkan.h>

CuResult cu_static_buffer_create(
    CuStaticBuffer* const pStaticBuffer,
    const size_t z,
    const CuBufferUsage usage,
    const CuAllocationMode mode)
{
    CuResult result = CU_SUCCESS;

    cu_try_catch(create_buffer(
        &pStaticBuffer->_buffer,
        &pStaticBuffer->_allocation,
        nullptr,
        z,
        usage | VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_SHADER_DEVICE_ADDRESS_BIT,
        mode,
        VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
        0));
    pStaticBuffer->_address = get_buffer_device_address(pStaticBuffer->_buffer);

    return CU_SUCCESS;

FAIL:
    cu_static_buffer_destroy(pStaticBuffer, mode);
    return result;
}

void cu_static_buffer_destroy(
    CuStaticBuffer* const pStaticBuffer,
    const CuAllocationMode mode)
{
    mode_free(&pStaticBuffer->_allocation, mode);
    vkDestroyBuffer(gContext._device, pStaticBuffer->_buffer, nullptr);
}
