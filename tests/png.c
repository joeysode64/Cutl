#define CU_INCLUDE_MATH
#include "cutl.h"
#include "test.h"

#include <stdlib.h>

#define PNG_PATH "tests/assets/emotion.png"

/** @return The scale that fits the image inside the window while keeping its
 * aspect ratio. */
static vec2 get_scale(const CuWindow* pWindow, uint32_t w, uint32_t h)
{
    const float imageAspect = (float)w / (float)h;
    const float windowAspect =
        (float)cu_window_get_width(pWindow) / (float)cu_window_get_height(pWindow);
    if (imageAspect > windowAspect) {
        return (vec2){.x = 1.0F, .y = windowAspect / imageAspect};
    } else {
        return (vec2){.x = imageAspect / windowAspect, .y = 1.0F};
    }
}

int main()
{
    query(cu_context_init(&CU_DEFAULT_CONTEXT_CREATE_INFO));

    CuWindow window = {};
    query(cu_window_create(&window, WIN_W, WIN_H, "PNG Test"));

    CuRenderer renderer = {};
    query(cu_renderer_create(&renderer, &CU_DEFAULT_RENDERER_CREATE_INFO, &window));

    // Get the PNG's size then its data.
    uint32_t w = 0;
    uint32_t h = 0;
    query(cu_png_load(nullptr, &w, &h, PNG_PATH, PNG_FORMAT_RGBA));
    const size_t zPixels = (size_t)w * h * 4;
    void* pPixels = malloc(zPixels);
    expect(pPixels != nullptr, "Failed to allocate the pixels.\n");
    query(cu_png_load(pPixels, nullptr, nullptr, PNG_PATH, PNG_FORMAT_RGBA));

    CuImage image = {};
    query(cu_image_create(
        &image, w, h, VK_FORMAT_R8G8B8A8_SRGB,
        VK_IMAGE_USAGE_SAMPLED_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT, CU_DEDICATED_ALLOCATOR_MODE));
    cu_context_bind_sampled_image(&image, 0);

    CuSampler sampler = {};
    query(cu_sampler_create(&sampler));
    cu_context_bind_sampler(&sampler, 0);

    CuStagingBuffer stagingBuffer = {};
    query(cu_staging_buffer_create(&stagingBuffer, zPixels, CU_DEDICATED_ALLOCATOR_MODE));
    cu_staging_buffer_copy(&stagingBuffer, pPixels, zPixels, 0);
    free(pPixels);

    // Upload the pixels to the image.
    CuTask task = {};
    query(cu_task_create(&task));
    query(cu_task_begin(&task));
    cu_cmd_image_barrier(
        &task, &image, VK_PIPELINE_STAGE_2_NONE, VK_ACCESS_2_NONE, VK_PIPELINE_STAGE_2_COPY_BIT,
        VK_ACCESS_2_TRANSFER_WRITE_BIT, VK_IMAGE_LAYOUT_UNDEFINED,
        VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);
    cu_cmd_image_copy_from_buffer(
        &task, &image, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, w, h, cu_buffer_get(&stagingBuffer),
        0);
    cu_cmd_image_barrier(
        &task, &image, VK_PIPELINE_STAGE_2_COPY_BIT, VK_ACCESS_2_TRANSFER_WRITE_BIT,
        VK_PIPELINE_STAGE_2_FRAGMENT_SHADER_BIT, VK_ACCESS_2_SHADER_READ_BIT,
        VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
    query(cu_task_submit(&task));
    cu_task_await(&task, UINT64_MAX);
    cu_task_destroy(&task);
    cu_staging_buffer_destroy(&stagingBuffer, CU_DEDICATED_ALLOCATOR_MODE);

    // The pipeline that draws the image to the screen.
    const CuPipelineLayoutCreateInfo pipelineLayoutCreateInfo = {
        .pushConstantSize = sizeof(vec2),
    };
    CuPipelineLayout pipelineLayout = {};
    query(cu_pipeline_layout_create(&pipelineLayout, &pipelineLayoutCreateInfo));
    CuSpirV vert = {};
    query(cu_spirv_read_from_file(&vert, "tests/shaders/png.vert.spv"));
    CuSpirV frag = {};
    query(cu_spirv_read_from_file(&frag, "tests/shaders/img_tex.frag.spv"));
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
    cu_spirv_destroy(&vert);
    cu_spirv_destroy(&frag);

    while (!cu_window_should_close(&window)) {
        cu_window_update(&window);

        const vec2 scale = get_scale(&window, w, h);

        CuFrame* pFrame = nullptr;
        query(cu_renderer_begin_frame(&renderer, &pFrame));
        if (pFrame == nullptr) {
            continue;
        }

        cu_frame_begin_render(pFrame, &renderer);
        cu_cmd_bind_graphics_pipeline(pFrame, graphicsPipeline);
        cu_cmd_bind_descriptors(pFrame, pipelineLayout);
        cu_cmd_write_push_constants(pFrame, pipelineLayout, &scale, sizeof(scale), 0);
        cu_cmd_draw(pFrame, 6, 0, 1, 0);
        cu_frame_end_render(pFrame, &renderer);

        query(cu_renderer_submit_frame(&renderer, pFrame));
    }

    cu_context_wait_for_idle();
    cu_graphics_pipeline_destroy(graphicsPipeline);
    cu_pipeline_layout_destroy(pipelineLayout);
    cu_sampler_destroy(&sampler);
    cu_image_destroy(&image, CU_DEDICATED_ALLOCATOR_MODE);
    cu_renderer_destroy(&renderer);
    cu_window_destroy(&window);
    cu_context_terminate();

    success();
}
