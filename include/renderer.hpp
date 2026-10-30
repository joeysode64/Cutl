#pragma once

#include "renderer.h"
#include "result.hpp"
#include "window.hpp"
#include "wrapper.hpp"

#include <vulkan/vulkan.h>

namespace cu {
    /** @ref CuRenderer */
    class Renderer : public Wrapper<CuRenderer> {
    public:
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

            /**
             * @brief Builds/creates the renderer.
             * @param [out] renderer The renderer.
             * @return The result of creating the renderer.
             */
            inline Result build(Renderer& renderer) const
            {
                return renderer.init(*this);
            }

            /**
             * @brief Builds/creates the renderer.
             * @param [out] renderer The renderer.
             * @param window The target window.
             * @return The result of creating the renderer.
             */
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
        inline Result init(const CreateInfo& createInfo = DEFAULT_CREATE_INFO)
        {
            return cu_renderer_create(this, &createInfo, nullptr);
        }

        /** @ref cu_renderer_create() */
        inline Result init(const Window& window, const CreateInfo& createInfo = DEFAULT_CREATE_INFO)
        {
            return cu_renderer_create(this, &createInfo, &window.get());
        }

        /** @ref cu_renderer_begin_frame() */
        inline Result begin_frame(VkCommandBuffer& commandBuffer)
        {
            return cu_renderer_begin_frame(this, &commandBuffer);
        }

        /** @ref cu_renderer_submit_frame() */
        inline Result submit_frame()
        {
            return cu_renderer_submit_frame(this);
        }

        /** @ref cu_renderer_start_rendering() */
        inline void start_rendering(const VkCommandBuffer commandBuffer)
        {
            return cu_renderer_start_rendering(this, commandBuffer);
        }

        /** @ref cu_renderer_end_rendering() */
        inline void end_rendering(const VkCommandBuffer commandBuffer)
        {
            return cu_renderer_end_rendering(this, commandBuffer);
        }

        /** @ref cu_renderer_get_formats() */
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
