#include "cutl.h"
#include "test.h"

#include <math.h>
#include <string.h>

/** @brief How much the sweep advances per frame. */
constexpr float SWEEP_STEP = 0.02F;

/** @brief The half width of the sweeping triangle's base. */
constexpr float SWEEP_HALF_WIDTH = 0.4F;

/** @brief The stencil value marking the area covered by the sweeping triangle. */
constexpr uint32_t MASK_VALUE = 1;

/** @brief The number of vertices per triangle. */
constexpr uint32_t N_TRI_VERTICES = 3;

/** @brief The push constants. */
typedef struct {
    float color[4]; /**< The draw's color. */

    VkDeviceAddress vertices; /**< The vertex buffer address. */
} PushConstants;

/**
 * @brief Makes a stencil state that compares against the mask value.
 * @param compareOp The stencil comparison.
 * @param passOp What to do with the stencil value when the comparison passes.
 * @return The stencil state.
 */
static VkStencilOpState stencil_state(const VkCompareOp compareOp, const VkStencilOp passOp)
{
    return (VkStencilOpState){
        .failOp = VK_STENCIL_OP_KEEP,
        .passOp = passOp,
        .depthFailOp = VK_STENCIL_OP_KEEP,
        .compareOp = compareOp,
        .compareMask = 0xFF,
        .writeMask = 0xFF,
        .reference = MASK_VALUE,
    };
}

int main()
{
    query(cu_context_init(&CU_DEFAULT_CONTEXT_CREATE_INFO));

    CuWindow window = {};
    query(cu_window_create(&window, WIN_W, WIN_H, "Stencil Test"));

    CuRendererCreateInfo rendererCreateInfo = CU_DEFAULT_RENDERER_CREATE_INFO;
    rendererCreateInfo.depthStencilFormat = VK_FORMAT_S8_UINT;
    rendererCreateInfo.enableStencil = true;

    CuRenderer renderer = {};
    query(cu_renderer_create(&renderer, &rendererCreateInfo, &window));

    const CuPipelineLayoutCreateInfo pipelineLayoutCreateInfo = {
        .pushConstantSize = sizeof(PushConstants),
    };
    CuPipelineLayout pipelineLayout = {};
    query(cu_pipeline_layout_create(&pipelineLayout, &pipelineLayoutCreateInfo));

    CuSpirV vert = {};
    query(cu_spirv_read_from_file(&vert, "tests/shaders/stencil.vert.spv"));

    CuSpirV frag = {};
    query(cu_spirv_read_from_file(&frag, "tests/shaders/stencil.frag.spv"));

    CuGraphicsPipelineCreateInfo graphicsPipelineCreateInfo = CU_DEFAULT_GRAPHICS_PIPELINE_CREATE_INFO;
    graphicsPipelineCreateInfo.pVert = vert.data;
    graphicsPipelineCreateInfo.nVert = vert.n;
    graphicsPipelineCreateInfo.pFrag = frag.data;
    graphicsPipelineCreateInfo.nFrag = frag.n;
    graphicsPipelineCreateInfo.pFormats = (VkFormat[]){cu_renderer_get_format(&renderer)};
    graphicsPipelineCreateInfo.nFormats = 1;
    graphicsPipelineCreateInfo.depthStencilFormat = rendererCreateInfo.depthStencilFormat;
    graphicsPipelineCreateInfo.enableStencil = true;

    // Marks the sweeping triangle's area in the stencil buffer.
    graphicsPipelineCreateInfo.stencilFront = stencil_state(VK_COMPARE_OP_ALWAYS, VK_STENCIL_OP_REPLACE);
    graphicsPipelineCreateInfo.stencilBack = graphicsPipelineCreateInfo.stencilFront;
    CuGraphicsPipeline maskPipeline = {};
    query(cu_graphics_pipeline_create(&maskPipeline, &graphicsPipelineCreateInfo, pipelineLayout));

    // Draws only outside the marked area.
    graphicsPipelineCreateInfo.stencilFront = stencil_state(VK_COMPARE_OP_NOT_EQUAL, VK_STENCIL_OP_KEEP);
    graphicsPipelineCreateInfo.stencilBack = graphicsPipelineCreateInfo.stencilFront;
    CuGraphicsPipeline outsidePipeline = {};
    query(cu_graphics_pipeline_create(
        &outsidePipeline, &graphicsPipelineCreateInfo, pipelineLayout));

    // Draws only inside the marked area.
    graphicsPipelineCreateInfo.stencilFront = stencil_state(VK_COMPARE_OP_EQUAL, VK_STENCIL_OP_KEEP);
    graphicsPipelineCreateInfo.stencilBack = graphicsPipelineCreateInfo.stencilFront;
    CuGraphicsPipeline insidePipeline = {};
    query(cu_graphics_pipeline_create(
        &insidePipeline, &graphicsPipelineCreateInfo, pipelineLayout));

    // Holds the sweeping triangle, a fullscreen triangle, and the hidden triangle, in that order.
    CuDynamicBuffer vertexBuffer = {};
    query(cu_dynamic_buffer_create(
        &vertexBuffer,
        rendererCreateInfo.maxFramesInFlight,
        3 * N_TRI_VERTICES * sizeof(vec2),
        VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
        CU_DEDICATED_ALLOCATOR_MODE));
    float angle = 0.0F;

    while (!cu_window_should_close(&window)) {
        cu_window_update(&window);

        void* pData = nullptr;
        VkDeviceAddress address = 0;
        cu_dynamic_buffer_get_data_frame(
            &vertexBuffer, &pData, &address, cu_renderer_get_frame_index(&renderer));

        const float x = 1.5F * sinf(angle);
        const vec2 vertices[3 * N_TRI_VERTICES] = {
            // Sweeping triangle.
            {x - SWEEP_HALF_WIDTH, -1.0F},
            {x + SWEEP_HALF_WIDTH, -1.0F},
            {x, 1.0F},
            // Fullscreen triangle.
            {-1.0F, -1.0F},
            {3.0F, -1.0F},
            {-1.0F, 3.0F},
            // Hidden triangle (top left, top right, bottom middle).
            {-1.0F, 1.0F},
            {1.0F, 1.0F},
            {0.0F, -1.0F},
        };
        memcpy(pData, vertices, sizeof(vertices));
        angle += SWEEP_STEP;

        VkCommandBuffer commandBuffer = VK_NULL_HANDLE;
        query(cu_renderer_begin_frame(&renderer, &commandBuffer));
        if (commandBuffer == VK_NULL_HANDLE) {
            continue;
        }

        // Fully transparent, so it only writes the stencil.
        const PushConstants mask = {.color = {0.0F, 0.0F, 0.0F, 0.0F}, .vertices = address};
        const PushConstants red = {.color = {1.0F, 0.0F, 0.0F, 1.0F}, .vertices = address};
        const PushConstants blue = {.color = {0.0F, 0.0F, 1.0F, 1.0F}, .vertices = address};

        cu_renderer_start_rendering(&renderer, commandBuffer);

        cu_cmd_bind_graphics_pipeline(commandBuffer, maskPipeline);
        cu_cmd_write_push_constants(commandBuffer, pipelineLayout, &mask, sizeof(mask), 0);
        cu_cmd_draw(commandBuffer, N_TRI_VERTICES, 0, 1, 0);

        cu_cmd_bind_graphics_pipeline(commandBuffer, outsidePipeline);
        cu_cmd_write_push_constants(commandBuffer, pipelineLayout, &red, sizeof(red), 0);
        cu_cmd_draw(commandBuffer, N_TRI_VERTICES, N_TRI_VERTICES, 1, 0);

        cu_cmd_bind_graphics_pipeline(commandBuffer, insidePipeline);
        cu_cmd_write_push_constants(commandBuffer, pipelineLayout, &blue, sizeof(blue), 0);
        cu_cmd_draw(commandBuffer, N_TRI_VERTICES, 2 * N_TRI_VERTICES, 1, 0);

        cu_renderer_end_rendering(&renderer, commandBuffer);

        query(cu_renderer_submit_frame(&renderer));
    }

    cu_context_wait_for_idle();
    cu_dynamic_buffer_destroy(&vertexBuffer, CU_DEDICATED_ALLOCATOR_MODE);
    cu_graphics_pipeline_destroy(insidePipeline);
    cu_graphics_pipeline_destroy(outsidePipeline);
    cu_graphics_pipeline_destroy(maskPipeline);
    cu_spirv_destroy(&frag);
    cu_spirv_destroy(&vert);
    cu_pipeline_layout_destroy(pipelineLayout);
    cu_renderer_destroy(&renderer);
    cu_window_destroy(&window);
    cu_context_terminate();

    success();
}
