#pragma once

#include "renderer.h"
#include "result.hpp"
#include "window.hpp"
#include "wrapper.hpp"

#include <vulkan/vulkan.h>

namespace cu
{
    /** @brief A Vulkan renderer. */
    class Renderer : public Wrapper<CuRenderer> {
    public:
        /** @brief Create info for a renderer. */
        using CreateInfo = CuRendererCreateInfo;

        /** @brief The default renderer create info. */
        static constexpr CreateInfo DEFAULT_CREATE_INFO = CU_DEFAULT_RENDERER_CREATE_INFO;

        /** @brief A context builder. */
        struct Builder {
            /** @brief The context create info. */
            Renderer::CreateInfo createInfo{Renderer::DEFAULT_CREATE_INFO};

            /** @brief Creates a builder with the default create info. */
            constexpr Builder() = default;

            /**
             * @brief Creates a builder with the given create info.
             * @param createInfo The create info.
             */
            constexpr Builder(const Renderer::CreateInfo& createInfo)
                : createInfo{createInfo}
            {

            }

            constexpr Builder& max_frames_in_flight(const uint32_t n) {
                createInfo.maxFramesInFlight = n;
                return *this;
            }

            constexpr Builder& min_swapchain_images(const uint32_t n) {
                createInfo.minSwapchainImages = n;
                return *this;
            }

            /**
             * @brief Builds/creates the renderer.
             * @param [out] renderer The renderer.
             * @return The result of creating the renderer.
             */
            inline Result build(Renderer& renderer) const {
                return Renderer::create(renderer, createInfo);
            }

            /**
             * @brief Builds/creates the renderer.
             * @param [out] renderer The renderer.
             * @param window The target window.
             * @return The result of creating the renderer.
             */
            inline Result build(Renderer& renderer, const Window& window) const {
                return Renderer::create(renderer, window, createInfo);
            }
        };

        inline ~Renderer()
        {
            cu_renderer_destroy(&me);
        }

        /** @brief Creates an uninitialized renderer. */
        constexpr Renderer()
            : Wrapper{}
        {

        }

        /**
         * @brief Creates the renderer.
         * @param [out] renderer The renderer to create.
         * @param createInfo The create info. Ignore for default.
         * @return The result of creating the renderer.
         */
        static inline Result create(
            Renderer& renderer,
            const CreateInfo& createInfo = DEFAULT_CREATE_INFO)
        {
            return cu_renderer_create(&renderer.me, &createInfo, nullptr);
        }

        /**
         * @brief Creates the renderer.
         * @param [out] renderer The renderer to create.
         * @param window The target window.
         * @param createInfo The create info. Ignore for default.
         * @return The result of creating the renderer.
         */
        static inline Result create(
            Renderer& renderer,
            const Window& window,
            const CreateInfo& createInfo = DEFAULT_CREATE_INFO)
        {
            return cu_renderer_create(&renderer.me, &createInfo, &window.get());
        }

        /**
         * @brief Begins the next frame.
         * @param [out] pFrame A pointer to the frame.
         * @return The result of beginning the frame.
         */
        inline Result begin_frame(
            CuFrame*& pFrame)
        {
            return cu_renderer_begin_frame(&me, &pFrame);
        }

        /**
         * @brief Submits the frame.
         * @param pFrame A pointer to the frame.
         * @return The result of submitting the frame.
         * @warning The frame becomes unusable after being submitted.
         */
        inline Result submit_frame(
            CuFrame* pFrame)
        {
            return cu_renderer_submit_frame(&me, pFrame);
        }

        /**
         * @brief Returns the renderer's target image format.
         * @return The renderer's target image format.
         */
        inline VkFormat get_format() const
        {
            return cu_renderer_get_format(&me);
        }

        /**
         * @brief Returns the renderer's frame index.
         * @return The renderer's current frame index.
         */
        inline uint32_t get_frame_index() const
        {
            return cu_renderer_get_frame_index(&me);
        }
    };
}
