#include "cutl.hpp"
#include "test.h"

using namespace cu;

int main()
{
    query(Context::init());
    const auto contextGuard = Context::Guard{};

    Window window{};
    query(window.init(WIN_W, WIN_H, "MSAA Test C++"));

    Renderer renderer{};
    query(Renderer::Builder{}.n_msaa_samples(4).build(renderer, window));

    PipelineLayout pipelineLayout{};
    query(pipelineLayout.init());

    const VkFormat format = renderer.get_format();
    GraphicsPipeline graphicsPipeline{};
    query(
        GraphicsPipeline::Builder{}.build(
            graphicsPipeline,
            pipelineLayout,
            "tests/shaders/tri.vert.spv",
            "tests/shaders/tri.frag.spv",
            &format,
            1));

    while (!window.should_close()) {
        window.update();

        CommandBuffer commandBuffer{VK_NULL_HANDLE};
        query(renderer.begin_frame(commandBuffer));
        if (commandBuffer.get() == VK_NULL_HANDLE) {
            continue;
        }

        renderer.start_rendering(commandBuffer);
        commandBuffer.cmd_bind_graphics_pipeline(graphicsPipeline);
        commandBuffer.cmd_draw(3, 0, 1, 0);
        renderer.end_rendering(commandBuffer);

        query(renderer.submit_frame());
    }

    Context::await();

    success();
}
