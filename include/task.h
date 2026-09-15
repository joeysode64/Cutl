#pragma once

#include "inner.h"
#include "result.h"

#include <stdint.h>

/// @brief A non-graphical GPU task.
/// @note Thsi type can be used as a command buffer.
typedef struct CuTask_T {
    /// @brief The command buffer handle.
    cu_vk_dispatch_t(CommandBuffer) _commandBuffer;

    /// @brief The "task finished" fence handle.
    cu_vk_t(Fence) _taskFinished;
} CuTask;

/// @brief Creates a task.
/// @param [out] pTask A pointer to the task.
/// @return The result of creating the task.
CuResult cu_task_create(
    CuTask* pTask);

/// @brief Destroys ta task.
/// @param [in, out] pTask A pointer to the task.
void cu_task_destroy(
    CuTask* pTask);

/// @brief Begins the task for command insertion.
/// @param [in, out] pTask A pointer to the task.
/// @return The result of beginning the task.
CuResult cu_task_begin(
    CuTask* pTask);

/// @brief Ends the task's command insertions and submits it.
/// @param [in, out] pTask A pointer to the task.
/// @return The result of ending and submitting the task.
CuResult cu_task_submit(
    CuTask* pTask);

/// @brief Returns whether the task is currently running.
/// @param [in] pTask A pointer to the task.
/// @return Whether the task is currently running.
bool cu_task_is_running(
    CuTask* pTask);

/// @brief Waits for the task to finish running, if it is.
/// @param [in] pTask A pointer to the task.
/// @param timeout The maximum number of nanoseconds to wait.
void cu_task_await(
    CuTask* pTask,
    uint64_t timeout);
