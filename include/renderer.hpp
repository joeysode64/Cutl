#pragma once

#include "renderer.h"
#include "result.hpp"
#include "window.h"

namespace cu
{
    /** @brief A Vulkan renderer. */
    class Renderer {
    private:
        CuRenderer me; /**< The renderer. */
    
    public:
        /** @brief Create info for a renderer. */
        using CreateInfo = CuRendererCreateInfo;

        /** @brief The default renderer create info. */
        static constexpr CreateInfo DEFAULT_CREATE_INFO = CU_DEFAULT_RENDERER_CREATE_INFO;

        inline ~Renderer()
        {
            cu_renderer_destroy(&me);
        }

        /** @brief Creates an uninitialized renderer. */
        constexpr Renderer()
            : me{}
        {

        }

        /**
         * @brief Creates the renderer.
         * @param [out] renderer The renderer to create.
         * @param pWindow A pointer to the window.
         * @param createInfo The create info. Ignore for default.
         * @return The result of creating the renderer.
         */
        static inline Result create(
            Renderer& renderer,
            const CuWindow* pWindow,
            const CreateInfo& createInfo = DEFAULT_CREATE_INFO)
        {
            return cu_renderer_create(&renderer.me, &createInfo, pWindow);
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
        inline CuFormat format() const
        {
            return cu_renderer_get_format(&me);
        }

        /**
         * @brief Returns the renderer's frame index.
         * @return The renderer's current frame index.
         */
        inline uint32_t frame_index() const
        {
            return cu_renderer_get_frame_index(&me);
        }
    };
}
