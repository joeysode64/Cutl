#pragma once

#include "result.h"

#include <stdint.h>

/** @brief A window. */
typedef struct {
    struct GLFWwindow* _handle; /**< The GLFW window handle. */

    uint32_t _w; /**< The window's cached framebuffer width. */

    uint32_t _h; /**< The window's cached framebuffer height. */

    bool _shouldClose; /**< Whether the window should close. */
} CuWindow;

/** 
 * @brief Creates the window.
 *
 * @param [out] pWindow A pointer to the window.
 * @param w The window's width.
 * @param h The window's height.
 * @param title The window's title.
 *
 * @return The result of the window's creation.
 */
CuResult cu_window_create(
    CuWindow* pWindow,
    uint32_t w,
    uint32_t h,
    const char* title);

/**
 * @brief Destroys the window.
 *
 * @param [in, out] pWindow A pointer to the window.
 *
 * @note Passing a null pointer is a safe no-op.
 */
void cu_window_destroy(
    CuWindow* pWindow);

/**
 * @brief Updates the window.
 *
 * @param [in, out] pWindow A pointer to the window.
 */
void cu_window_update(
    CuWindow* pWindow);

/**
 * @brief Returns the window's framebuffer width.
 *
 * @param [in] pWindow A pointer to the window.
 *
 * @return The window's width.
 */
uint32_t cu_window_get_width(
    const CuWindow* pWindow);

/**
 * @brief Returns the window's framebuffer height.
 *
 * @param [in] pWindow A pointer to the window.
 *
 * @return The window's height.
 */
uint32_t cu_window_get_height(
    const CuWindow* pWindow);

/**
 * @brief Returns whether the window should close.
 *
 * @param [in] pWindow A pointer to the window.
 *
 * @return Whether the window should close.
 */
bool cu_window_should_close(
    const CuWindow* pWindow);
