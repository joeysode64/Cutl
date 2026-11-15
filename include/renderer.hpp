#pragma once

#include "command_buffer.hpp"
#include "renderer.h"
#include "result.hpp"
#include "window.hpp"
#include "wrapper.hpp"

#include <stdint.h>
#include <vulkan/vulkan.h>

namespace cu {
    /** @ref CuRenderer */
    class Renderer : public Wrapper<CuRenderer> {
    public:
        /** @ref CuRendererConfig */
        using Config = CuRendererConfig;

        /** @ref CuRendererCreateInfo */
        using CreateInfo = CuRendererCreateInfo;

        /** @ref CU_DEFAULT_RENDERER_CREATE_INFO */
        static constexpr CreateInfo DEFAULT_CREATE_INFO = CU_DEFAULT_RENDERER_CREATE_INFO;

        /** @brief A context builder. */
        struct Builder : Renderer::CreateInfo {
            /** @brief Creates a builder with the default create info. */
            constexpr Builder()
                : Renderer::CreateInfo{Renderer::DEFAULT_CREATE_INFO}
            {
            }

            /**
             * @brief Creates a builder with the given create info.
             * @param createInfo The create info.
             */
            constexpr Builder(const Renderer::CreateInfo& createInfo)
                : Renderer::CreateInfo{createInfo}
            {
            }

            /** @ref CuRendererCreateInfo::maxFramesInFlight */
            constexpr Builder& max_frames_in_flight(const uint32_t n)
            {
                maxFramesInFlight = n;
                return *this;
            }

            /** @ref CuRendererCreateInfo::minSwapchainImages */
            constexpr Builder& min_swapchain_images(const uint32_t n)
            {
                minSwapchainImages = n;
                return *this;
            }

            /** @ref CuRendererCreateInfo::preferredPresentMode */
            constexpr Builder& preferred_present_mode(const VkPresentModeKHR x)
            {
                preferredPresentMode = x;
                return *this;
            }

            /** @ref CuRendererCreateInfo::nMsaaSamples */
            constexpr Builder& n_msaa_samples(const uint32_t n)
            {
                nMsaaSamples = n;
                return *this;
            }

            /** @ref CuRendererCreateInfo::enableDepth */
            constexpr Builder& enable_depth(const bool x)
            {
                enableDepth = x;
                return *this;
            }

            /**
             * @brief Builds/creates the renderer.
             * @param[out] renderer The renderer to create. Must be uninitialized.
             */
            CU_NODISCARD_RESULT
            inline Result build(Renderer& renderer) const
            {
                return renderer.init(*this);
            }

            /**
             * @brief Builds/creates the renderer.
             * @param[out] renderer The renderer to create. Must be uninitialized.
             * @param window The target window. Must outlive the renderer.
             */
            CU_NODISCARD_RESULT
            inline Result build(Renderer& renderer, const Window& window) const
            {
                return renderer.init(window, *this);
            }
        };

        /** @ref cu_renderer_destroy() */
        inline ~Renderer()
        {
            cu_renderer_destroy(this);
        }

        /** @brief Creates an uninitialized renderer. */
        constexpr Renderer()
            : Wrapper{}
        {
        }

        /** @ref cu_renderer_create() */
        CU_NODISCARD_RESULT
        inline Result init(const CreateInfo& createInfo = DEFAULT_CREATE_INFO)
        {
            return cu_renderer_create(this, &createInfo, nullptr);
        }

        /** @ref cu_renderer_create() */
        CU_NODISCARD_RESULT
        inline Result init(const Window& window, const CreateInfo& createInfo = DEFAULT_CREATE_INFO)
        {
            return cu_renderer_create(this, &createInfo, &window.get());
        }

        /** @ref cu_renderer_recreate_swapchain() */
        CU_NODISCARD_RESULT
        inline Result recreate_swapchain()
        {
            return cu_renderer_recreate_swapchain(this);
        }

        /** @ref cu_renderer_begin_frame() */
        CU_NODISCARD_RESULT
        inline Result begin_frame(CommandBuffer& commandBuffer)
        {
            return cu_renderer_begin_frame(this, &commandBuffer.get());
        }

        /** @ref cu_renderer_submit_frame() */
        CU_NODISCARD_RESULT
        inline Result submit_frame()
        {
            return cu_renderer_submit_frame(this);
        }

        /** @ref cu_renderer_start_rendering() */
        inline void start_rendering(const CommandBuffer commandBuffer)
        {
            return cu_renderer_start_rendering(this, commandBuffer);
        }

        /** @ref cu_renderer_end_rendering() */
        inline void end_rendering(const CommandBuffer commandBuffer)
        {
            return cu_renderer_end_rendering(this, commandBuffer);
        }

        /** @ref cu_renderer_get_config() */
        inline Config& get_config()
        {
            return _config;
        }

        /** @ref cu_renderer_get_config() */
        inline const Config& get_config() const
        {
            return _config;
        }

        /** @ref cu_renderer_get_format() */
        inline VkFormat get_format() const
        {
            return cu_renderer_get_format(this);
        }

        /** @ref cu_renderer_get_frame_index() */
        inline uint32_t get_frame_index() const
        {
            return cu_renderer_get_frame_index(this);
        }
    };
} // namespace cu
