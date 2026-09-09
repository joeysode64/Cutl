#include "cmd_buffer.h"

#include "pipeline.h"

#include <stdint.h>
#include <vulkan/vulkan.h>

/** @return The dereferenced command buffer. */
#define deref(p) (*(VkCommandBuffer*)p)

void cu_cmd_bind_graphics_pipeline(
    const CuCmdBuffer commandBuffer,
    const CuGraphicsPipeline graphicsPipeline)
{
    vkCmdBindPipeline(
        deref(commandBuffer), VK_PIPELINE_BIND_POINT_GRAPHICS, (VkPipeline)graphicsPipeline
    );
}

void cu_cmd_draw(
    const CuCmdBuffer commandBuffer,
    const uint32_t nVertices,
    const uint32_t iFirstVertex,
    const uint32_t nInstances,
    const uint32_t iFirstInstance)
{
    vkCmdDraw(deref(commandBuffer), nVertices, nInstances, iFirstVertex, iFirstInstance);
}

void cu_cmd_bind_compute_pipeline(
    const CuCmdBuffer commandBuffer,
    const CuComputePipeline computePipeline)
{
    vkCmdBindPipeline(
        deref(commandBuffer), VK_PIPELINE_BIND_POINT_COMPUTE, (VkPipeline)computePipeline
    );
}

void cu_cmd_dispatch(
    const CuCmdBuffer commandBuffer,
    const uint32_t x,
    const uint32_t y,
    const uint32_t z)
{
    vkCmdDispatch(deref(commandBuffer), x, y, z);
}

void cu_cmd_write_push_constants(
    CuCmdBuffer commandBuffer,
    const CuPipelineLayout pipelineLayout,
    const void* const p,
    const size_t z,
    const size_t o)
{
    vkCmdPushConstants(
        deref(commandBuffer), (VkPipelineLayout)pipelineLayout, VK_SHADER_STAGE_ALL, o, z, p
    );
}
