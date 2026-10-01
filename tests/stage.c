#include "cutl.h"
#include "pipeline.h"
#include "test.h"

#include <stdlib.h>
#include <time.h>

#define N_TRIANGLES 100
#define N_VERTICES (3 * N_TRIANGLES)

typedef struct {
    float x;
    float y;
    float _pad[2];
    float r;
    float g;
    float b;
    float a;
} Vertex;

static float rand_f(
    float a,
    float b)
{
    return a + ((b - a) * ((float)rand() / (float)RAND_MAX));
}

static Vertex rand_v()
{
    return (Vertex){
        .x = rand_f(-1.0F, 1.0F),
        .y = rand_f(-1.0F, 1.0F),
        .r = rand_f(0.1F, 1.0F),
        .g = rand_f(0.1F, 1.0F),
        .b = rand_f(0.1F, 1.0F),
        .a = 1.0F,
    };
}

int main()
{
    srand((unsigned)time(nullptr));

    query(cu_context_init(&CU_DEFAULT_CONTEXT_CREATE_INFO));

    CuWindow window = {};
    query(cu_window_create(&window, 800, 450, "Staging Buffer Test"));

    CuRenderer renderer = {};
    query(cu_renderer_create(&renderer, &CU_DEFAULT_RENDERER_CREATE_INFO, &window));

    const CuPipelineLayoutCreateInfo pipelineLayoutCreateInfo = {
        .pushConstantSize = sizeof(CuBufferAddress),
    };
    CuPipelineLayout pipelineLayout = {};
    query(cu_pipeline_layout_create(&pipelineLayout, &pipelineLayoutCreateInfo));

    CuSpirV vert = {};
    query(cu_spirv_read_from_file(&vert, "tests/shaders/stage.vert.spv"));

    CuSpirV frag = {};
    query(cu_spirv_read_from_file(&frag, "tests/shaders/stage.frag.spv"));

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

    // Generate triangles.
    Vertex vertices[N_VERTICES] = {};
    Vertex prev[3] = { rand_v(), rand_v(), rand_v() };
    for (size_t i = 0; i < N_TRIANGLES; i++) {
        vertices[(3 * i) + 0] = prev[0];
        vertices[(3 * i) + 1] = prev[1];
        vertices[(3 * i) + 2] = prev[2];
        prev[0] = prev[1];
        prev[1] = prev[2];
        prev[2] = rand_v();
    }

    CuStaticBuffer vertexBuffer = {};
    query(cu_static_buffer_create(
        &vertexBuffer, sizeof(vertices), CU_BUFFER_STORAGE_BUFFER, CU_DEDICATED_ALLOCATOR_MODE));

    CuStagingBuffer stagingBuffer = {};
    query(cu_staging_buffer_create(&stagingBuffer, sizeof(vertices), CU_DEDICATED_ALLOCATOR_MODE));
    cu_staging_buffer_copy(&stagingBuffer, vertices, sizeof(vertices), 0);

    // Upload to static buffer.
    CuTask task = {};
    query(cu_task_create(&task));
    query(cu_task_begin(&task));
    cu_cmd_buffer_copy(
        &task,
        cu_buffer_get(&vertexBuffer),
        0,
        cu_buffer_get(&stagingBuffer),
        0,
        sizeof(vertices));
    query(cu_task_submit(&task));
    cu_task_await(&task, UINT64_MAX);
    cu_task_destroy(&task);
    cu_staging_buffer_destroy(&stagingBuffer, CU_DEDICATED_ALLOCATOR_MODE);

    const CuBufferAddress address = cu_buffer_get_address(&vertexBuffer);

    while (!cu_window_should_close(&window)) {
        cu_window_update(&window);

        CuFrame* pFrame = nullptr;
        cu_renderer_begin_frame(&renderer, &pFrame);

        cu_frame_begin_render(pFrame, &renderer);
        cu_cmd_bind_graphics_pipeline(pFrame, graphicsPipeline);
        cu_cmd_write_push_constants(pFrame, pipelineLayout, &address, sizeof(address), 0);
        cu_cmd_draw(pFrame, N_VERTICES, 0, 1, 0);
        cu_frame_end_render(pFrame, &renderer);

        cu_renderer_submit_frame(&renderer, pFrame);
    }

    cu_context_wait_for_idle();
    cu_static_buffer_destroy(&vertexBuffer, CU_DEDICATED_ALLOCATOR_MODE);
    cu_graphics_pipeline_destroy(graphicsPipeline);
    cu_pipeline_layout_destroy(pipelineLayout);
    cu_renderer_destroy(&renderer);
    cu_window_destroy(&window);
    cu_context_terminate();

    success();
}
