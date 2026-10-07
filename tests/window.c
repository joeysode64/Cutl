#include "cutl.h"
#include "test.h"

int main()
{
    query(cu_context_init(&CU_DEFAULT_CONTEXT_CREATE_INFO));

    CuWindow window = {};
    query(cu_window_create(&window, 800, 450, "Window Test"));

    CuRenderer renderer = {};
    query(cu_renderer_create(&renderer, &CU_DEFAULT_RENDERER_CREATE_INFO, &window));

    while (!cu_window_should_close(&window)) {
        cu_window_update(&window);
    }

    cu_context_wait_for_idle();
    cu_renderer_destroy(&renderer);
    cu_window_destroy(&window);
    cu_context_terminate();

    success();
}
