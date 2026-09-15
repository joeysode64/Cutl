#include "task.h"

#include "g_context.h"
#include "result.h"
#include "vk.h"

#include <limits.h>
#include <stdint.h>
#include <vulkan/vulkan.h>
#include <vulkan/vulkan_core.h>

/// @brief Creates a fence.
/// @param [out] pFence The fence to create.
/// @return The result of creating the fence.
static VkResult create_fence(
    VkFence* pFence);

CuResult cu_task_create(
    CuTask* const pTask)
{
    CuResult result = CU_ERROR_UNKNOWN;

    cu_try_catch_vk(allocate_command_buffers(&pTask->_commandBuffer, 1));
    cu_try_catch_vk(create_fence(&pTask->_taskFinished));

    return CU_SUCCESS;

FAIL:
    cu_task_destroy(pTask);
    return result;
}

void cu_task_destroy(
    CuTask* const pTask)
{
    cu_task_await(pTask, UINT_MAX);
    vkDestroyFence(gContext.device, pTask->_taskFinished, nullptr);
    vkFreeCommandBuffers(gContext.device, gContext.commandPool, 1, &pTask->_commandBuffer);
}

CuResult cu_task_begin(
    CuTask* const pTask)
{
    const VkCommandBufferBeginInfo beginInfo = {
        .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,
        .pNext = nullptr,
        .flags = 0,
        .pInheritanceInfo = nullptr,
    };
    cu_try_vk(vkBeginCommandBuffer(pTask->_commandBuffer, &beginInfo));

    return CU_SUCCESS;
}

CuResult cu_task_submit(
    CuTask* const pTask)
{
    cu_try_vk(vkEndCommandBuffer(pTask->_commandBuffer));

    cu_try_vk(vkResetFences(gContext.device, 1, &pTask->_taskFinished));

    const VkSubmitInfo submitInfo = {
        .sType = VK_STRUCTURE_TYPE_SUBMIT_INFO,
        .pNext = nullptr,
        .waitSemaphoreCount = 0,
        .pWaitSemaphores = nullptr,
        .pWaitDstStageMask = nullptr,
        .commandBufferCount = 1,
        .pCommandBuffers = &pTask->_commandBuffer,
        .signalSemaphoreCount = 0,
        .pSignalSemaphores = nullptr,
    };
    cu_try_vk(vkQueueSubmit(gContext.queue, 1, &submitInfo, pTask->_taskFinished));

    return CU_SUCCESS;
}

bool cu_task_is_running(
    CuTask* const pTask)
{
    return vkGetFenceStatus(gContext.device, pTask->_taskFinished) == VK_NOT_READY;
}

void cu_task_await(
    CuTask* pTask,
    uint64_t timeout)
{
    vkWaitForFences(gContext.device, 1, &pTask->_taskFinished, VK_TRUE, timeout);
}

VkResult create_fence(
    VkFence* const pFence)
{
    const VkFenceCreateInfo createInfo = {
        .sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO,
        .pNext = nullptr,
        .flags = VK_FENCE_CREATE_SIGNALED_BIT,
    };
    return vkCreateFence(gContext.device, &createInfo, nullptr, pFence);
}
