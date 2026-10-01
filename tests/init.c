#include "context.h"
#include "cutl.h"
#include "renderer.h"
#include "test.h"

int main()
{
    query(cu_context_init(&CU_DEFAULT_CONTEXT_CREATE_INFO));

    CuRenderer renderer = {};
    query(cu_renderer_create(&renderer, &CU_DEFAULT_RENDERER_CREATE_INFO, nullptr));

    cu_context_wait_for_idle();
    cu_renderer_destroy(&renderer);
    cu_context_terminate();

    success();
}
