#pragma once

#include "context.h"
#include "result.hpp"

#include <cstdlib>
#include <stdlib.h>

namespace cu
{
    /** @brief A Vulkan GPU context. */
    class Context {
    public:
        /** @brief Create info for a GPU context. */
        using CreateInfo = CuContextCreateInfo;

        /** @brief The default context create info. */
        static constexpr CreateInfo DEFAULT_CREATE_INFO = CU_DEFAULT_CONTEXT_CREATE_INFO;

        /**
         * @brief Returns the global GPU context.
         *
         * @return The global GPU context.
         */
        static inline CuContext& get()
        {
            return *cu_context_get();
        }

        /**
         * @brief Initializes the context.
         * @param createInfo The create info to use (default is `Context::DEFAULT_CREATE_INFO`).
         * @return The result of creating the context.
         * @note On success, `terminate()` is registered to run when the program exits.
         */
        static inline Result init(
            const CreateInfo& createInfo = DEFAULT_CREATE_INFO)
        {
            const Result result = cu_context_init(&createInfo);
            if (!result.is_success()) {
                return result;
            }

            // Run cleanup at exit time.
            static bool registered = false;
            if (!registered) {
                atexit(&cu_context_terminate);
                registered = true;
            }

            return SUCCESS;
        }

        /** @brief Destroys the context. */
        static inline void terminate()
        {
            return cu_context_terminate();
        }

        /** @brief Waits for the context to idle. */
        static inline void await()
        {
            return cu_context_wait_for_idle();
        }
    };
}
