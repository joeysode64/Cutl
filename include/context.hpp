#pragma once

#include "context.h"
#include "result.hpp"

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
         */
        static inline Result init(
            const CreateInfo& createInfo = DEFAULT_CREATE_INFO)
        {
            return cu_context_init(&createInfo);
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
