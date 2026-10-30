#include "cutl.h"
#include "test.h"

#include <math.h>
#include <string.h>

int main()
{
    query(cu_context_init(&CU_DEFAULT_CONTEXT_CREATE_INFO));

    CuWindow window = {};
    query(cu_window_create(&window, WIN_W, WIN_H, "Dynamic Buffer Test"));

    CuRenderer renderer = {};
    query(cu_renderer_create(&renderer, &CU_DEFAULT_RENDERER_CREATE_INFO, &window));

    const CuPipelineLayoutCreateInfo pipelineLayoutCreateInfo = {
        .pushConstantSize = sizeof(VkDeviceAddress),
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
        .pFormats = (VkFormat[]){cu_renderer_get_format(&renderer)},
        .nFormats = 1,
    };
    query(cu_graphics_pipeline_create(
        &graphicsPipeline, &graphicsPipelineCreateInfo, pipelineLayout));

    CuDynamicBuffer vertexBuffer = {};
    query(cu_dynamic_buffer_create(
        &vertexBuffer, CU_DEFAULT_RENDERER_CREATE_INFO.maxFramesInFlight, 3 * sizeof(vec2),
        VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, CU_DEDICATED_ALLOCATOR_MODE));
    float angle = 0.0F;

    while (!cu_window_should_close(&window)) {
        cu_window_update(&window);

        void* pData = nullptr;
        VkDeviceAddress address = 0;
        cu_dynamic_buffer_get_data_frame(
            &vertexBuffer, &pData, &address, cu_renderer_get_frame_index(&renderer));

        const vec2 vertices[3] = {
            {-1.0F, -1.0F},
            {1.0F, -1.0F},
            {cosf(angle), sinf(angle)},
        };
        memcpy(pData, vertices, sizeof(vertices));
        angle += 0.02F;

        VkCommandBuffer commandBuffer = VK_NULL_HANDLE;
        query(cu_renderer_begin_frame(&renderer, &commandBuffer));
        if (commandBuffer == VK_NULL_HANDLE) {
            continue;
        }

        cu_renderer_start_rendering(&renderer, commandBuffer);
        cu_cmd_bind_graphics_pipeline(commandBuffer, graphicsPipeline);
        cu_cmd_write_push_constants(commandBuffer, pipelineLayout, &address, sizeof(address), 0);
        cu_cmd_draw(commandBuffer, 3, 0, 1, 0);
        cu_renderer_end_rendering(&renderer, commandBuffer);

        query(cu_renderer_submit_frame(&renderer));
    }

    cu_context_wait_for_idle();
    cu_dynamic_buffer_destroy(&vertexBuffer, CU_DEDICATED_ALLOCATOR_MODE);
    cu_graphics_pipeline_destroy(graphicsPipeline);
    cu_pipeline_layout_destroy(pipelineLayout);
    cu_renderer_destroy(&renderer);
    cu_window_destroy(&window);
    cu_context_terminate();

    success();
}
