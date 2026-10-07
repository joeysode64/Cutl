#pragma once

#include "result.hpp"
#include "window.h"

namespace cu
{
    /** @brief A window. */
    class Window {
    private:
        CuWindow me; /**< The window. */

    public:
        inline ~Window()
        {
            cu_window_destroy(&me);
        }

        /** @brief Creates an uninitialized window. */
        constexpr Window()
            : me{}
        {

        }

        /**
         * @brief Creates the window.
         * @param [out] window The window to create.
         * @param w The window's width.
         * @param h The window's height.
         * @param title The window's title.
         * @return The result of creating the window.
         */
        static inline Result create(
            Window& window,
            uint32_t w,
            uint32_t h,
            const char* title)
        {
            return cu_window_create(&window.me, w, h, title);
        }

        /**
         * @brief Returns the window.
         * @return The window.
         */
        constexpr CuWindow& get() {
            return me;
        }

        /**
         * @brief Returns the window.
         * @return The window.
         */
        constexpr const CuWindow& get() const {
            return me;
        }

        /** @brief Updates the window. */
        inline void update()
        {
            cu_window_update(&me);
        }

        /**
         * @brief Returns the window's framebuffer width.
         * @return The window's width.
         */
        inline uint32_t width() const
        {
            return cu_window_get_width(&me);
        }

        /**
         * @brief Returns the window's framebuffer height.
         * @return The window's height.
         */
        inline uint32_t height() const
        {
            return cu_window_get_height(&me);
        }

        /**
         * @brief Returns whether the window should close.
         * @return Whether the window should close.
         */
        inline bool should_close() const
        {
            return cu_window_should_close(&me);
        }
    };
}
