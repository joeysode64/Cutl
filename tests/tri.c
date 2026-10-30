#include "cutl.h"
#include "test.h"

int main()
{
    query(cu_context_init(&CU_DEFAULT_CONTEXT_CREATE_INFO));

    CuWindow window = {};
    query(cu_window_create(&window, WIN_W, WIN_H, "Initialization Test"));

    CuRenderer renderer = {};
    query(cu_renderer_create(&renderer, &CU_DEFAULT_RENDERER_CREATE_INFO, &window));

    CuPipelineLayout pipelineLayout = {};
    query(cu_pipeline_layout_create(&pipelineLayout, &CU_DEFAULT_PIPELINE_LAYOUT_CREATE_INFO));

    CuSpirV vert = {};
    query(cu_spirv_read_from_file(&vert, "tests/shaders/tri.vert.spv"));

    CuSpirV frag = {};
    query(cu_spirv_read_from_file(&frag, "tests/shaders/tri.frag.spv"));

    CuGraphicsPipeline graphicsPipeline = {};
    const CuGraphicsPipelineCreateInfo graphicsPipelineCreateInfo = {
        .pVert = vert.data,
        .nVert = vert.n,
        .pFrag = frag.data,
        .nFrag = frag.n,
        .pFormats = (VkFormat[]){cu_renderer_get_format(&renderer)},
        .nFormats = 1,
    };
    query(cu_graphics_pipeline_create(
        &graphicsPipeline, &graphicsPipelineCreateInfo, pipelineLayout));

    while (!cu_window_should_close(&window)) {
        cu_window_update(&window);

        VkCommandBuffer commandBuffer = VK_NULL_HANDLE;
        query(cu_renderer_begin_frame(&renderer, &commandBuffer));
        if (commandBuffer == VK_NULL_HANDLE) {
            continue;
        }

        cu_renderer_start_rendering(&renderer, commandBuffer);
        cu_cmd_bind_graphics_pipeline(commandBuffer, graphicsPipeline);
        cu_cmd_draw(commandBuffer, 3, 0, 1, 0);
        cu_renderer_end_rendering(&renderer, commandBuffer);

        query(cu_renderer_submit_frame(&renderer));
    }

    cu_context_wait_for_idle();
    cu_graphics_pipeline_destroy(graphicsPipeline);
    cu_pipeline_layout_destroy(pipelineLayout);
    cu_renderer_destroy(&renderer);
    cu_window_destroy(&window);
    cu_context_terminate();

    success();
}
