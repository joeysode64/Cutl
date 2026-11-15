#pragma once

#include "result.h"

#include <stdint.h>
#include <vulkan/vulkan.h>

/**
 * @brief A non-graphical GPU task.
 * @note This type can be used as a command buffer.
 */
typedef struct CuTask_T {
    /** @brief The command buffer handle. */
    VkCommandBuffer _commandBuffer;

    /** @brief The "task finished" fence handle. */
    VkFence _taskFinished;
} CuTask;

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Creates a task.
 * @param[out] pTask A pointer to the task. Must not be null.
 */
CU_NODISCARD_RESULT
CuResult cu_task_create(CuTask* pTask);

/**
 * @brief Destroys a task.
 * @param[in out] pTask A pointer to the task. Must not be null.
 * @note Waits for the task to finish running first.
 */
void cu_task_destroy(CuTask* pTask);

/**
 * @brief Begins the task for command insertion.
 * @param[in] pTask A pointer to the task. Must not be null or currently running.
 */
CU_NODISCARD_RESULT
CuResult cu_task_begin(const CuTask* pTask);

/**
 * @brief Ends the task's command insertions and submits it.
 * @param[in] pTask A pointer to the task. Must not be null and must have been begun.
 */
CU_NODISCARD_RESULT
CuResult cu_task_submit(const CuTask* pTask);

/**
 * @brief Returns whether the task is currently running.
 * @param[in] pTask A pointer to the task. Must not be null.
 */
bool cu_task_is_running(const CuTask* pTask);

/**
 * @brief Waits for the task to finish running, if it is.
 * @param[in] pTask A pointer to the task. Must not be null.
 * @param timeout The maximum number of nanoseconds to wait. Use `UINT64_MAX` to wait
 * indefinitely.
 */
void cu_task_await(const CuTask* pTask, uint64_t timeout);

/**
 * @brief Returns the task's command buffer.
 * @param[in] pTask A pointer to the task. Must not be null.
 * @warning The command buffer is only open between `cu_task_begin` and `cu_task_submit`.
 */
inline static VkCommandBuffer cu_task_get_command_buffer(const CuTask* const pTask)
{
    return pTask->_commandBuffer;
}

#ifdef __cplusplus
}
#endif
