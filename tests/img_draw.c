#include "cutl.h"
#include "test.h"

#include <math.h>
#include <string.h>

#define W (WIN_W / 4)
#define H (WIN_H / 4)

typedef struct {
    vec4 color;
    VkDeviceAddress vertices;
} TrianglePushConstants;

/// @return The color for the triangle at the given angle.
static vec4 get_color(float angle)
{
    const float phase = angle * 1.3F;
    return (vec4){
        .x = 0.5F + (0.5F * cosf(phase)),
        .y = 0.5F + (0.5F * cosf(phase + 2.0943951F)),
        .z = 0.5F + (0.5F * cosf(phase + 4.1887902F)),
        .w = 1.0F,
    };
}

int main()
{
    query(cu_context_init(&CU_DEFAULT_CONTEXT_CREATE_INFO));

    CuWindow window = {};
    query(cu_window_create(&window, WIN_W, WIN_H, "Image Draw Test"));

    CuRenderer renderer = {};
    query(cu_renderer_create(&renderer, &CU_DEFAULT_RENDERER_CREATE_INFO, &window));

    CuImage image = {};
    query(cu_image_create(
        &image, W, H, VK_FORMAT_R8G8B8A8_UNORM,
        VK_IMAGE_USAGE_SAMPLED_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT |
            VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT,
        CU_DEDICATED_ALLOCATOR_MODE));
    cu_context_bind_sampled_image(&image, 0);

    CuSampler sampler = {};
    query(cu_sampler_create(&sampler));
    cu_context_bind_sampler(&sampler, 0);

    CuTask task = {};
    query(cu_task_create(&task));
    query(cu_task_begin(&task));
    cu_cmd_image_barrier(
        &task, &image, VK_PIPELINE_STAGE_2_NONE, VK_ACCESS_2_NONE, VK_PIPELINE_STAGE_2_CLEAR_BIT,
        VK_ACCESS_2_TRANSFER_WRITE_BIT, VK_IMAGE_LAYOUT_UNDEFINED,
        VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);
    cu_cmd_image_fill(&task, &image, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, CU_RGBA_BLACK);
    cu_cmd_image_barrier(
        &task, &image, VK_PIPELINE_STAGE_2_CLEAR_BIT, VK_ACCESS_2_TRANSFER_WRITE_BIT,
        VK_PIPELINE_STAGE_2_FRAGMENT_SHADER_BIT, VK_ACCESS_2_SHADER_READ_BIT,
        VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
    query(cu_task_submit(&task));
    cu_task_await(&task, UINT64_MAX);
    cu_task_destroy(&task);

    // The pipeline that draws the triangle to the image.
    const CuPipelineLayoutCreateInfo trianglePipelineLayoutCreateInfo = {
        .pushConstantSize = sizeof(TrianglePushConstants),
    };
    CuPipelineLayout trianglePipelineLayout = {};
    query(cu_pipeline_layout_create(&trianglePipelineLayout, &trianglePipelineLayoutCreateInfo));
    CuSpirV triangleVert = {};
    query(cu_spirv_read_from_file(&triangleVert, "tests/shaders/img_tri.vert.spv"));
    CuSpirV triangleFrag = {};
    query(cu_spirv_read_from_file(&triangleFrag, "tests/shaders/img_tri.frag.spv"));
    CuGraphicsPipeline trianglePipeline = {};
    const CuGraphicsPipelineCreateInfo trianglePipelineCreateInfo = {
        .pVert = triangleVert.data,
        .nVert = triangleVert.n,
        .pFrag = triangleFrag.data,
        .nFrag = triangleFrag.n,
        .pFormats = (VkFormat[]){VK_FORMAT_R8G8B8A8_UNORM},
        .nFormats = 1,
    };
    query(cu_graphics_pipeline_create(
        &trianglePipeline, &trianglePipelineCreateInfo, trianglePipelineLayout));
    cu_spirv_destroy(&triangleVert);
    cu_spirv_destroy(&triangleFrag);

    // The pipeline that draws the image to the screen.
    CuPipelineLayout screenPipelineLayout = {};
    query(
        cu_pipeline_layout_create(&screenPipelineLayout, &CU_DEFAULT_PIPELINE_LAYOUT_CREATE_INFO));
    CuSpirV screenVert = {};
    query(cu_spirv_read_from_file(&screenVert, "tests/shaders/img_tex.vert.spv"));
    CuSpirV screenFrag = {};
    query(cu_spirv_read_from_file(&screenFrag, "tests/shaders/img_tex.frag.spv"));
    CuGraphicsPipeline screenPipeline = {};
    const CuGraphicsPipelineCreateInfo screenPipelineCreateInfo = {
        .pVert = screenVert.data,
        .nVert = screenVert.n,
        .pFrag = screenFrag.data,
        .nFrag = screenFrag.n,
        .pFormats = (VkFormat[]){cu_renderer_get_format(&renderer)},
        .nFormats = 1,
    };
    query(cu_graphics_pipeline_create(
        &screenPipeline, &screenPipelineCreateInfo, screenPipelineLayout));
    cu_spirv_destroy(&screenVert);
    cu_spirv_destroy(&screenFrag);

    CuDynamicBuffer vertexBuffer = {};
    query(cu_dynamic_buffer_create(
        &vertexBuffer, CU_DEFAULT_RENDERER_CREATE_INFO.maxFramesInFlight, 3 * sizeof(vec2),
        VK_BUFFER_USAGE_STORAGE_BUFFER_BIT, CU_DEDICATED_ALLOCATOR_MODE));
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
        const TrianglePushConstants pushConstants = {
            .color = get_color(angle),
            .vertices = address,
        };
        angle += 0.02F;

        CuFrame* pFrame = nullptr;
        query(cu_renderer_begin_frame(&renderer, &pFrame));
        if (pFrame == nullptr) {
            continue;
        }

        cu_cmd_image_barrier(
            pFrame, &image, VK_PIPELINE_STAGE_2_FRAGMENT_SHADER_BIT, VK_ACCESS_2_SHADER_READ_BIT,
            VK_PIPELINE_STAGE_2_COLOR_ATTACHMENT_OUTPUT_BIT, VK_PIPELINE_STAGE_2_ALL_GRAPHICS_BIT,
            VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL);
        cu_frame_begin_image_render(pFrame, &image, W, H);
        cu_cmd_bind_graphics_pipeline(pFrame, trianglePipeline);
        cu_cmd_write_push_constants(
            pFrame, trianglePipelineLayout, &pushConstants, sizeof(pushConstants), 0);
        cu_cmd_draw(pFrame, 3, 0, 1, 0);
        cu_frame_end_image_render(pFrame, &image, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);

        cu_frame_begin_render(pFrame, &renderer);
        cu_cmd_bind_graphics_pipeline(pFrame, screenPipeline);
        cu_cmd_bind_descriptors(pFrame, screenPipelineLayout);
        cu_cmd_draw(pFrame, 3, 0, 1, 0);
        cu_frame_end_render(pFrame, &renderer);

        query(cu_renderer_submit_frame(&renderer, pFrame));
    }

    cu_context_wait_for_idle();
    cu_dynamic_buffer_destroy(&vertexBuffer, CU_DEDICATED_ALLOCATOR_MODE);
    cu_graphics_pipeline_destroy(screenPipeline);
    cu_pipeline_layout_destroy(screenPipelineLayout);
    cu_graphics_pipeline_destroy(trianglePipeline);
    cu_pipeline_layout_destroy(trianglePipelineLayout);
    cu_sampler_destroy(&sampler);
    cu_image_destroy(&image, CU_DEDICATED_ALLOCATOR_MODE);
    cu_renderer_destroy(&renderer);
    cu_window_destroy(&window);
    cu_context_terminate();

    success();
}
