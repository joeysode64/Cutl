#pragma once

#include "result.hpp"
#include "task.h"
#include "wrapper.hpp"

#include <stdint.h>

namespace cu {
    /**
    * @brief A non-graphical GPU task.
    * @note This type can be used as a command buffer.
    */
    class Task : public Wrapper<CuTask> {
    public:
        inline ~Task() {
            cu_task_destroy(&me);
        }

        inline Task()
            : Wrapper{}
        {

        }

        /** @ref cu_task_create */
        static inline Result create(Task& task) {
            return cu_task_create(&task.me);
        }

        /** @ref cu_task_begin() */
        inline Result begin() {
            return cu_task_begin(&me);
        }

        /** @ref cu_task_submit() */
        inline Result submit() {
            return cu_task_submit(&me);
        }

        /** @ref cu_task_is_running() */
        inline bool is_running() {
            return cu_task_is_running(&me);
        }

        /** @ref cu_task_await */
        inline void await(uint64_t timeout = UINT64_MAX) {
            return cu_task_await(&me, timeout);
        }

        inline operator VkCommandBuffer() const {
            return me._commandBuffer;
        }
    };
}
