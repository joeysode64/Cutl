#pragma once

#include "context.h"
#include "result.hpp"
#include "version.hpp"

#include <stdint.h>

namespace cu
{
    /** @brief A Vulkan GPU context. */
    class Context {
    public:
        /** @brief Create info for a GPU context. */
        using CreateInfo = CuContextCreateInfo;

        /** @brief The default context create info. */
        static constexpr CreateInfo DEFAULT_CREATE_INFO = CU_DEFAULT_CONTEXT_CREATE_INFO;

        /** @brief A guard for safe context deconstruction with RAII. */
        class Guard {
            Guard(const Guard&) = delete;

            Guard(Guard&&) = delete;

            Guard& operator=(const Guard&) = delete;

            Guard& operator=(Guard&&) = delete;
        
        public:
            inline ~Guard() {
                Context::terminate();
            }

            constexpr Guard() = default;
        };

        /** @brief A context builder. */
        struct Builder {
            /** @brief The context create info. */
            Context::CreateInfo createInfo{Context::DEFAULT_CREATE_INFO};

            /** @brief Creates a builder with the default create info. */
            constexpr Builder() = default;

            /**
             * @brief Creates a builder with the given create info.
             * @param createInfo The create info.
             */
            constexpr Builder(const Context::CreateInfo& createInfo)
                : createInfo{createInfo}
            {

            }

            constexpr Builder& extensions(const CuExtensionFlags mExtensions) {
                createInfo.mExtensions = mExtensions;
                return *this;
            }

            constexpr Builder& app_name(const char* const appName) {
                createInfo.appName = appName;
                return *this;
            }

            constexpr Builder& app_version(const uint32_t appVersion) {
                createInfo.appVersion = appVersion;
                return *this;
            }

            constexpr Builder& app_version(const Version version) {
                createInfo.appVersion = version.vk_version();
                return *this;
            }

            constexpr Builder& n_sampled_image_descriptors(const uint32_t n) {
                createInfo.nSampledImageDescriptors = n;
                return *this;
            }

            constexpr Builder& n_sampler_descriptors(const uint32_t n) {
                createInfo.nSamplerDescriptors = n;
                return *this;
            }

            constexpr Builder& n_storage_image_descriptors(const uint32_t n) {
                createInfo.nStorageImageDescriptors = n;
                return *this;
            }

            /**
             * @brief Initializes the context with the builder.
             * @return The result of the context's initialization.
             */
            inline Result build() const {
                return Context::init(createInfo);
            }
        };

        /**
         * @brief Returns the global GPU context.
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
