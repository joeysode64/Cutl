# Cutl

Cutl is a thin Vulkan engine that provides ease of creation, usage, and destruction of Vulkan objects.

It is implemented in C but has a C++ wrapper.

It is currently pre-1.0.

## Requirements

- Vulkan 1.4+ drivers
- Vulkan SDK
- GLFW
- CMake
- C23

## Usage

Cutl uses a global Vulkan context that contains important structures such as the instance and logical device.

Everything else (windows, renderers, buffers, etc.) depends on the context and is created using `cu_T_create` and `cu_T_destroy`.

Render loops are done via starting a frame which gives you a command buffer to fill and submit.

All Vulkan internals are accessible to use yourself if needed.

## Options

You can initialize CMake with `-DCUTL_DEBUGGING=ON` for debug logging of everything under the hood.

## Examples

Drawing a triangle:
```c
#include "cutl.h"

int main() {
    cu_context_init(&CU_DEFAULT_CONTEXT_CREATE_INFO);

    CuWindow window = {};
    cu_window_create(&window, 800, 450, "Cutl Example");

    CuRenderer renderer = {};
    cu_renderer_create(&renderer, &CU_DEFAULT_RENDERER_CREATE_INFO, &window);

    CuPipelineLayout pipelineLayout = {};
    cu_pipeline_layout_create(&pipelineLayout, &CU_DEFAULT_PIPELINE_LAYOUT_CREATE_INFO);

    // Load the SPIR-V and create the graphics pipeline.
    CuSpirV vert = {};
    cu_spirv_read_from_file(&vert, "tests/shaders/tri.vert.spv");
    CuSpirV frag = {};
    cu_spirv_read_from_file(&frag, "tests/shaders/tri.frag.spv");
    CuGraphicsPipeline graphicsPipeline = {};
    const CuGraphicsPipelineCreateInfo graphicsPipelineCreateInfo = {
        .pVert = vert.data,
        .nVert = vert.n,
        .pFrag = frag.data,
        .nFrag = frag.n,
        .pFormats = (VkFormat[]){ cu_renderer_get_format(&renderer) },
        .nFormats = 1,
    };
    cu_graphics_pipeline_create(&graphicsPipeline, &graphicsPipelineCreateInfo, pipelineLayout);

    cu_spirv_destroy(&vert);
    cu_spirv_destroy(&frag);

    while (!cu_window_should_close(&window)) {
        cu_window_update(&window);

        // Get the command buffer for the next frame.
        VkCommandBuffer commandBuffer = VK_NULL_HANDLE;
        cu_renderer_begin_frame(&renderer, &commandBuffer);

        // Will be set to null if the swapchain is resizing.
        if (commandBuffer == VK_NULL_HANDLE) {
            continue;
        }

        // Begin rendering and draw the triangle.
        cu_renderer_start_rendering(&renderer, commandBuffer);
        cu_cmd_bind_graphics_pipeline(commandBuffer, graphicsPipeline);
        cu_cmd_draw(commandBuffer, 3, 0, 1, 0);

        // Finish rendering and submit.
        cu_renderer_end_rendering(&renderer, commandBuffer);
        cu_renderer_submit_frame(&renderer);
    }

    // Cleanup.
    cu_context_wait_for_idle();
    cu_graphics_pipeline_destroy(graphicsPipeline);
    cu_pipeline_layout_destroy(pipelineLayout);
    cu_renderer_destroy(&renderer);
    cu_window_destroy(&window);
    cu_context_terminate();

    return 0;
}
```

Simple example programs are in [`tests`](tests).

## Version 1.0 Roadmap

Before version 1.0:
- ~~Expose Vulkan and remove remapped `enum`s~~
- ~~Standardize formatting~~
- ~~Add image loading/saving (`libpng`)~~
- ~~Add swapchain recreation~~
- Add the ability for users to use their own physical device and swapchain format/present mode judge functions
- ~~Add the ability to include custom extensions~~
- Add template shaders
- Finish C++ wrapper

After version 1.0:
- Ditch GLFW for custom multi-platform window handler
- Keyboard and mouse input
- Audio
