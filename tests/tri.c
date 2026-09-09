#include "cutl.h"
#include "test.h"

int main() {
    query(cu_context_init(nullptr));

    CuWindow window = {};
    query(cu_window_create(&window, 800, 450, "Initialization Test"));

    CuRenderer renderer = {};
    query(cu_renderer_create(&renderer, nullptr, &window));

    CuPipelineLayout pipelineLayout = {};
    query(cu_pipeline_layout_create(&pipelineLayout, nullptr));

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
        .pFormats = (CuFormat[]){ cu_renderer_get_format(&renderer) },
        .nFormats = 1,
    };
    query(cu_graphics_pipeline_create(&graphicsPipeline, &graphicsPipelineCreateInfo, pipelineLayout));

    while (!cu_window_should_close(&window)) {
        cu_window_update(&window);

        CuFrame* pFrame = nullptr;
        cu_renderer_begin_frame(&renderer, &pFrame);

        cu_frame_begin_render(pFrame, &renderer);
        cu_cmd_bind_graphics_pipeline(pFrame, graphicsPipeline);
        cu_cmd_draw(pFrame, 3, 0, 1, 0);
        cu_frame_end_render(pFrame, &renderer);

        cu_renderer_submit_frame(&renderer, pFrame);
    }

    cu_renderer_destroy(&renderer);
    cu_window_destroy(&window);
    cu_context_terminate();

    success();
}
