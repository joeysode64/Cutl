#include "window.h"

#include "result.h"
#include "util.h"

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <assert.h>
#include <stdint.h>

CuResult cu_window_create(
    CuWindow* const pWindow,
    const uint32_t w,
    const uint32_t h,
    const char* const title)
{
    assert(pWindow != nullptr);

    CuResult result = CU_ERROR_UNKNOWN;

    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    pWindow->_handle = glfwCreateWindow((int)w, (int)h, title, nullptr, nullptr);
    cu_assert_catch(pWindow != nullptr, CU_ERROR_UNKNOWN);

    cu_window_update(pWindow);

    return CU_SUCCESS;

FAIL:
    cu_window_destroy(pWindow);
    return result;
}

void cu_window_destroy(
    CuWindow* const pWindow)
{
    if (pWindow == nullptr) {
        return;
    }

    glfwDestroyWindow(pWindow->_handle);
}

void cu_window_update(
    CuWindow* const pWindow)
{
    assert(pWindow != nullptr);

    glfwPollEvents();

    int w = 0;
    int h = 0;
    glfwGetFramebufferSize(pWindow->_handle, &w, &h);
    pWindow->_w = (uint32_t)w;
    pWindow->_h = (uint32_t)h;

    pWindow->_shouldClose = glfwWindowShouldClose(pWindow->_handle);
}

uint32_t cu_window_get_width(
    const CuWindow* const pWindow)
{
    assert(pWindow != nullptr);

    return pWindow->_w;
}

uint32_t cu_window_get_height(
    const CuWindow* const pWindow)
{
    assert(pWindow != nullptr);

    return pWindow->_h;
}

bool cu_window_should_close(
    const CuWindow* const pWindow)
{
    assert(pWindow != nullptr);

    return pWindow->_shouldClose;
}
