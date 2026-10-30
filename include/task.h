#pragma once

#include "result.h"

#include <stdint.h>
#include <vulkan/vulkan.h>

/**
 * @brief A non-graphical GPU task.
 * @note This type can be used as a command buffer.
 */
typedef struct CuTask_T {
    VkCommandBuffer _commandBuffer; /**< The command buffer handle. */

    VkFence _taskFinished; /**< The "task finished" fence handle. */
} CuTask;

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Creates a task.
 * @param [out] pTask A pointer to the task.
 * @return The result of creating the task.
 */
CuResult cu_task_create(CuTask* pTask);

/**
 * @brief Destroys ta task.
 * @param [in, out] pTask A pointer to the task.
 */
void cu_task_destroy(CuTask* pTask);

/**
 * @brief Begins the task for command insertion.
 * @param [in] pTask A pointer to the task.
 * @return The result of beginning the task.
 */
CuResult cu_task_begin(const CuTask* pTask);

/**
 * @brief Ends the task's command insertions and submits it.
 * @param [in] pTask A pointer to the task.
 * @return The result of ending and submitting the task.
 */
CuResult cu_task_submit(const CuTask* pTask);

/**
 * @brief Returns whether the task is currently running.
 * @param [in] pTask A pointer to the task.
 * @return Whether the task is currently running.
 */
bool cu_task_is_running(const CuTask* pTask);

/**
 * @brief Waits for the task to finish running, if it is.
 * @param [in] pTask A pointer to the task.
 * @param timeout The maximum number of nanoseconds to wait.
 */
void cu_task_await(const CuTask* pTask, uint64_t timeout);

/**
 * @brief Returns the task's command buffer.
 * @param [in] pTask A pointer to the task.
 * @return The task's command buffer.
 * @warning The command buffer is only open between `cu_task_begin` and `cu_task_submit`.
 */
inline static VkCommandBuffer cu_task_get_command_buffer(const CuTask* const pTask)
{
    return pTask->_commandBuffer;
}

#ifdef __cplusplus
}
#endif
