#pragma once

#include "result.hpp"
#include "task.h"
#include "wrapper.hpp"

#include <stdint.h>

namespace cu {
    /** @ref CuTask */
    class Task : public Wrapper<CuTask> {
    public:
        /** @ref cu_task_destroy() */
        inline ~Task() {
            cu_task_destroy(this);
        }

        inline Task()
            : Wrapper{}
        {

        }

        /** @ref cu_task_create() */
        inline Result init() {
            return cu_task_create(this);
        }

        /** @ref cu_task_begin() */
        inline Result begin() {
            return cu_task_begin(this);
        }

        /** @ref cu_task_submit() */
        inline Result submit() {
            return cu_task_submit(this);
        }

        /** @ref cu_task_is_running() */
        inline bool is_running() {
            return cu_task_is_running(this);
        }

        /** @ref cu_task_await() */
        inline void await(const uint64_t timeout = UINT64_MAX) {
            return cu_task_await(this, timeout);
        }

        inline operator VkCommandBuffer() const {
            return _commandBuffer;
        }
    };
}
