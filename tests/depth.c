#include "cutl.h"
#include "test.h"

#include <math.h>
#include <stdint.h>

/** @brief How much the oscillation advances per frame. */
constexpr float ANGLE_STEP = 0.05F;

/** @brief The push constants. */
typedef struct {
    float color[4]; /**< The triangle's color. */

    float z; /**< The triangle's depth. */

    float scale; /**< The triangle's scale. */
} PushConstants;

int main()
{
    query(cu_context_init(&CU_DEFAULT_CONTEXT_CREATE_INFO));

    CuWindow window = {};
    query(cu_window_create(&window, WIN_W, WIN_H, "Depth Test"));

    CuRendererCreateInfo rendererCreateInfo = CU_DEFAULT_RENDERER_CREATE_INFO;
    rendererCreateInfo.enableDepth = true;

    CuRenderer renderer = {};
    query(cu_renderer_create(&renderer, &rendererCreateInfo, &window));

    const CuPipelineLayoutCreateInfo pipelineLayoutCreateInfo = {
        .pushConstantSize = sizeof(PushConstants),
    };
    CuPipelineLayout pipelineLayout = {};
    query(cu_pipeline_layout_create(&pipelineLayout, &pipelineLayoutCreateInfo));

    CuSpirV vert = {};
    query(cu_spirv_read_from_file(&vert, "tests/shaders/depth.vert.spv"));

    CuSpirV frag = {};
    query(cu_spirv_read_from_file(&frag, "tests/shaders/depth.frag.spv"));

    CuGraphicsPipelineCreateInfo graphicsPipelineCreateInfo = {
        .pVert = vert.data,
        .nVert = vert.n,
        .pFrag = frag.data,
        .nFrag = frag.n,
        .pFormats = (VkFormat[]){ cu_renderer_get_format(&renderer) },
        .nFormats = 1,
        .rasterizationSamples = 1,
        .depthTestEnable = true,
        .depthWriteEnable = true,
        .depthCompareOp = VK_COMPARE_OP_LESS,
    };
    CuGraphicsPipeline graphicsPipeline = {};
    query(cu_graphics_pipeline_create(
        &graphicsPipeline, &graphicsPipelineCreateInfo, pipelineLayout));

    float angle = 0.0F;

    while (!cu_window_should_close(&window)) {
        cu_window_update(&window);

        VkCommandBuffer commandBuffer = VK_NULL_HANDLE;
        query(cu_renderer_begin_frame(&renderer, &commandBuffer));
        if (commandBuffer == VK_NULL_HANDLE) {
            continue;
        }

        const PushConstants largeTriangle = {
            .color = {1.0F, 0.5F, 0.25F, 1.0F},
            .z = 0.5F + (0.4F * sinf(angle)),
            .scale = 1.0F,
        };
        const PushConstants smallTriangle = {
            .color = {0.25F, 0.75F, 1.0F, 1.0F},
            .z = 0.5F - (0.4F * sinf(angle)),
            .scale = 0.5F,
        };
        angle += ANGLE_STEP;

        cu_renderer_start_rendering(&renderer, commandBuffer);
        cu_cmd_bind_graphics_pipeline(commandBuffer, graphicsPipeline);

        cu_cmd_write_push_constants(
            commandBuffer, pipelineLayout, &largeTriangle, sizeof(largeTriangle), 0);
        cu_cmd_draw(commandBuffer, 3, 0, 1, 0);

        cu_cmd_write_push_constants(
            commandBuffer, pipelineLayout, &smallTriangle, sizeof(smallTriangle), 0);
        cu_cmd_draw(commandBuffer, 3, 0, 1, 0);

        cu_renderer_end_rendering(&renderer, commandBuffer);

        query(cu_renderer_submit_frame(&renderer));
    }

    cu_context_wait_for_idle();
    cu_graphics_pipeline_destroy(graphicsPipeline);
    cu_spirv_destroy(&frag);
    cu_spirv_destroy(&vert);
    cu_pipeline_layout_destroy(pipelineLayout);
    cu_renderer_destroy(&renderer);
    cu_window_destroy(&window);
    cu_context_terminate();

    success();
}
