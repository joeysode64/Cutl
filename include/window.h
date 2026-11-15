#pragma once

#include "result.h"

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <stdint.h>

/** @brief A window. */
typedef struct CuWindow_T {
    /** @brief The GLFW window handle. */
    GLFWwindow* _handle;

    /** @brief The window's cached framebuffer width. */
    uint32_t _w;

    /** @brief The window's cached framebuffer height. */
    uint32_t _h;

    /** @brief Whether the window should close. */
    bool _shouldClose;
} CuWindow;

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Creates the window.
 * @param[out] pWindow A pointer to the window. Must not be null.
 * @param w The window's width in screen coordinates. Must not be 0.
 * @param h The window's height in screen coordinates. Must not be 0.
 * @param title The window's title, UTF-8 encoded. Must not be null.
 * @note The context must be initialized first.
 */
CU_NODISCARD_RESULT
CuResult cu_window_create(CuWindow* pWindow, uint32_t w, uint32_t h, const char* title);

/**
 * @brief Destroys the window.
 * @param[in out] pWindow A pointer to the window. Ignored if null.
 */
void cu_window_destroy(CuWindow* pWindow);

/**
 * @brief Polls the window's events and updates its cached state.
 * @param[in out] pWindow A pointer to the window. Must not be null.
 */
void cu_window_update(CuWindow* pWindow);

/**
 * @brief Returns the window's framebuffer width.
 * @param[in] pWindow A pointer to the window. Must not be null.
 */
static inline uint32_t cu_window_get_width(const CuWindow* pWindow)
{
    return pWindow->_w;
}

/**
 * @brief Returns the window's framebuffer height.
 * @param[in] pWindow A pointer to the window. Must not be null.
 */
static inline uint32_t cu_window_get_height(const CuWindow* pWindow)
{
    return pWindow->_h;
}

/**
 * @brief Returns whether the window should close.
 * @param[in] pWindow A pointer to the window. Must not be null.
 */
static inline bool cu_window_should_close(const CuWindow* pWindow)
{
    return pWindow->_shouldClose;
}

#ifdef __cplusplus
}
#endif
