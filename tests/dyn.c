#include "cutl.h"
#include "test.h"

#include <math.h>
#include <string.h>

typedef struct {
    float x;
    float y;
} Vertex;

int main() {
    query(cu_context_init(nullptr));

    CuWindow window = {};
    query(cu_window_create(&window, 800, 450, "Dynamic Buffer Test"));

    CuRenderer renderer = {};
    query(cu_renderer_create(&renderer, nullptr, &window));

    const CuPipelineLayoutCreateInfo pipelineLayoutCreateInfo = {
        .pushConstantSize = sizeof(CuDeviceAddress),
    };
    CuPipelineLayout pipelineLayout = {};
    query(cu_pipeline_layout_create(&pipelineLayout, &pipelineLayoutCreateInfo));

    CuSpirV vert = {};
    query(cu_spirv_read_from_file(&vert, "tests/shaders/dyn.vert.spv"));

    CuSpirV frag = {};
    query(cu_spirv_read_from_file(&frag, "tests/shaders/dyn.frag.spv"));

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

    CuDynamicBuffer vertexBuffer = {};
    query(cu_dynamic_buffer_create(
        &vertexBuffer,
        CU_DEFAULT_RENDERER_CREATE_INFO.maxFramesInFlight,
        3 * sizeof(Vertex),
        CU_BUFFER_VERTEX_BUFFER,
        CU_DEDICATED_ALLOCATOR_MODE));
    float angle = 0.0F;

    while (!cu_window_should_close(&window)) {
        cu_window_update(&window);

        void* pData = nullptr;
        CuDeviceAddress address = 0;
        cu_dynamic_buffer_get_data_frame(
            &vertexBuffer, &pData, &address, cu_renderer_get_frame_index(&renderer));

        const Vertex vertices[3] = {
            { -1.0F, -1.0F },
            { 1.0F, -1.0F },
            { cosf(angle), sinf(angle) },
        };
        memcpy(pData, vertices, sizeof(vertices));
        angle += 0.02F;

        CuFrame* pFrame = nullptr;
        cu_renderer_begin_frame(&renderer, &pFrame);

        cu_frame_begin_render(pFrame, &renderer);
        cu_cmd_bind_graphics_pipeline(pFrame, graphicsPipeline);
        cu_cmd_write_push_constants(pFrame, pipelineLayout, &address, sizeof(address), 0);
        cu_cmd_draw(pFrame, 3, 0, 1, 0);
        cu_frame_end_render(pFrame, &renderer);

        cu_renderer_submit_frame(&renderer, pFrame);
    }

    cu_context_wait_for_idle();
    cu_dynamic_buffer_destroy(&vertexBuffer, CU_DEDICATED_ALLOCATOR_MODE);

    cu_renderer_destroy(&renderer);
    cu_window_destroy(&window);
    cu_context_terminate();

    success();
}
