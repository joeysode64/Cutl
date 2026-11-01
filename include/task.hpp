#pragma once

#include "command_buffer.hpp"
#include "result.hpp"
#include "task.h"
#include "wrapper.hpp"

#include <stdint.h>
#include <vulkan/vulkan.h>

namespace cu {
    /** @ref CuTask */
    class Task : public Wrapper<CuTask>, public CommandBufferWrapper {
    public:
        using Wrapper<CuTask>::get;

        /** @ref cu_task_destroy() */
        inline ~Task()
        {
            cu_task_destroy(this);
        }

        inline Task()
            : Wrapper{}, CommandBufferWrapper{}
        {
        }

        /** @ref cu_task_create() */
        inline Result init()
        {
            const Result result = cu_task_create(this);
            _v = _commandBuffer;
            return result;
        }

        /** @ref cu_task_begin() */
        inline Result begin()
        {
            return cu_task_begin(this);
        }

        /** @ref cu_task_submit() */
        inline Result submit()
        {
            return cu_task_submit(this);
        }

        /** @ref cu_task_is_running() */
        inline bool is_running()
        {
            return cu_task_is_running(this);
        }

        /** @ref cu_task_await() */
        inline void await(const uint64_t timeout = UINT64_MAX)
        {
            return cu_task_await(this, timeout);
        }
    };
} // namespace cu
