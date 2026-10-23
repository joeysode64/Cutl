#pragma once

#include "context.h"
#include "result.hpp"
#include "version.hpp"

#include <stdint.h>

namespace cu {
    /** @ref CuContext */
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
            inline ~Guard()
            {
                Context::terminate();
            }

            constexpr Guard() = default;
        };

        /** @brief A context builder. */
        struct Builder : public Context::CreateInfo {
            /** @brief Creates a builder with the default create info. */
            constexpr Builder()
                : Context::CreateInfo{Context::DEFAULT_CREATE_INFO}
            {
            }

            /**
             * @brief Creates a builder with the given create info.
             * @param createInfo The create info.
             */
            constexpr Builder(const Context::CreateInfo& createInfo)
                : Context::CreateInfo{createInfo}
            {
            }

            /** @ref CuContextCreateInfo::appName */
            constexpr Builder& app_name(const char* const x)
            {
                appName = x;
                return *this;
            }

            /** @ref CuContextCreateInfo::appVersion */
            constexpr Builder& app_version(const uint32_t x)
            {
                appVersion = x;
                return *this;
            }

            /** @ref CuContextCreateInfo::appVersion */
            constexpr Builder& app_version(const Version x)
            {
                appVersion = x.vk_version();
                return *this;
            }

            /** @ref CuContextCreateInfo::nSampledImageDescriptors */
            constexpr Builder& n_sampled_image_descriptors(const uint32_t x)
            {
                nSampledImageDescriptors = x;
                return *this;
            }

            /** @ref CuContextCreateInfo::nSamplerDescriptors */
            constexpr Builder& n_sampler_descriptors(const uint32_t x)
            {
                nSamplerDescriptors = x;
                return *this;
            }

            /** @ref CuContextCreateInfo::nStorageImageDescriptors */
            constexpr Builder& n_storage_image_descriptors(const uint32_t x)
            {
                nStorageImageDescriptors = x;
                return *this;
            }

            /**
             * @brief Initializes the context with the builder.
             * @return The result of the context's initialization.
             */
            inline Result build() const
            {
                return Context::init(*this);
            }
        };

        /** @ref cu_context_get() */
        static inline CuContext& get()
        {
            return *cu_context_get();
        }

        /** @ref cu_context_init()  */
        static inline Result init(const CreateInfo& createInfo = DEFAULT_CREATE_INFO)
        {
            return cu_context_init(&createInfo);
        }

        /** @ref cu_context_terminate() */
        static inline void terminate()
        {
            return cu_context_terminate();
        }

        /** @ref cu_context_wait_for_idle() */
        static inline void await()
        {
            return cu_context_wait_for_idle();
        }
    };
} // namespace cu
