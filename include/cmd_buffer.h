#pragma once

#include "pipeline.h"

#include <stdint.h>

/**
 * @brief A command buffer.
 *
 * @note This is any type that can be dereferenced as a `VkCommandBuffer.
 */
typedef void* CuCmdBuffer;

/**
 * @brief Binds the graphics pipeline.
 *
 * @param [in, out] commandBuffer The command buffer.
 * @param graphicsPipeline The graphics pipeline.
 */
void cu_cmd_bind_graphics_pipeline(
    CuCmdBuffer commandBuffer,
    CuGraphicsPipeline graphicsPipeline);

/**
 * @brief Runs a draw call.
 *
 * @param [in, out] commandBuffer The command buffer.
 * @param nVertices The number of vertices.
 * @param iFirstVertex The index of the first vertex.
 * @param nInstances The number of instances.
 * @param iFirstInstance The index of the first instance.
 */
void cu_cmd_draw(
    CuCmdBuffer commandBuffer,
    uint32_t nVertices,
    uint32_t iFirstVertex,
    uint32_t nInstances,
    uint32_t iFirstInstance);

/**
 * @brief Binds the compute pipeline.
 *
 * @param [in, out] commandBuffer The command buffer.
 * @param computePipeline The graphics pipeline.
 */
void cu_cmd_bind_compute_pipeline(
    CuCmdBuffer commandBuffer,
    CuComputePipeline computePipeline);

/**
 * @brief Dispatches compute workgroups.
 *
 * @param [in, out] commandBuffer The command buffer.
 * @param x The number of local workgroups in X.
 * @param y The number of local workgroups in Y.
 * @param z The number of local workgroups in Z.
 */
void cu_cmd_dispatch(
    CuCmdBuffer commandBuffer,
    uint32_t x,
    uint32_t y,
    uint32_t z);

/**
 * @brief Sets the push constants at the given range.
 *
 * @param [in, out] commandBuffer The command buffer.
 * @param pipelineLayout The pipeline layout.
 * @param p A pointer to the push constant data to write.
 * @param z The size of the push constant data.
 * @param o The offset to write the push constants to.
 */
void cu_cmd_write_push_constants(
    CuCmdBuffer commandBuffer,
    CuPipelineLayout pipelineLayout,
    const void* p,
    size_t z,
    size_t o);
