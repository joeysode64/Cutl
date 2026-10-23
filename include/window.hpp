#pragma once

#include "result.hpp"
#include "window.h"
#include "wrapper.hpp"

namespace cu {
    /** @ref CuWindow */
    class Window : public Wrapper<CuWindow> {
    public:
        /** @ref cu_window_destroy() */
        inline ~Window()
        {
            cu_window_destroy(this);
        }

        /** @brief Creates an uninitialized window. */
        constexpr Window()
            : Wrapper{}
        {
        }

        /** @ref cu_window_create() */
        inline Result init(const uint32_t w, const uint32_t h, const char* const title)
        {
            return cu_window_create(this, w, h, title);
        }

        /** @ref cu_window_update() */
        inline void update()
        {
            cu_window_update(this);
        }

        /** @ref cu_window_get_width() */
        inline uint32_t get_width() const
        {
            return cu_window_get_width(this);
        }

        /** @ref cu_window_get_height() */
        inline uint32_t get_height() const
        {
            return cu_window_get_height(this);
        }

        /** @ref cu_window_should_close() */
        inline bool should_close() const
        {
            return cu_window_should_close(this);
        }
    };
} // namespace cu
