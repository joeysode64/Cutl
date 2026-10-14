#pragma once

#include "buffer.h"
#include "color.h"
#include "image.h"
#include "pipeline.h"

#include <stddef.h>
#include <stdint.h>

/// @brief A command buffer.
/// @note This is any type that can be dereferenced as a `VkCommandBuffer`.
typedef void* CuCmdBuffer;

/**
 * @brief Pipeline stage bitmap flags.
 * @note The values match their `VkPipelineStageFlagBits2` counterparts.
 */
typedef enum : uint64_t {
    CU_PIPELINE_STAGE_NONE = 0, /**< No stage. */

    CU_PIPELINE_STAGE_DRAW_INDIRECT = 0x00000002, /**< Indirect command consumption. */

    CU_PIPELINE_STAGE_VERTEX_INPUT = 0x00000004, /**< Vertex and index buffer consumption. */

    CU_PIPELINE_STAGE_VERTEX_SHADER = 0x00000008, /**< The vertex shader. */

    CU_PIPELINE_STAGE_FRAGMENT_SHADER = 0x00000080, /**< The fragment shader. */

    CU_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS = 0x00000100, /**< Early depth and stencil tests. */

    CU_PIPELINE_STAGE_LATE_FRAGMENT_TESTS = 0x00000200, /**< Late depth and stencil tests. */

    CU_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT = 0x00000400, /**< Color attachment output. */

    CU_PIPELINE_STAGE_COMPUTE_SHADER = 0x00000800, /**< The compute shader. */

    CU_PIPELINE_STAGE_ALL_TRANSFER = 0x00001000, /**< All copy, blit, resolve and clear commands. */

    CU_PIPELINE_STAGE_HOST = 0x00004000, /**< Host reads and writes of device memory. */

    CU_PIPELINE_STAGE_ALL_GRAPHICS = 0x00008000, /**< All graphics pipeline stages. */

    CU_PIPELINE_STAGE_ALL_COMMANDS = 0x00010000, /**< All stages supported by the queue. */

    CU_PIPELINE_STAGE_COPY = 0x100000000, /**< Copy commands. */

    CU_PIPELINE_STAGE_RESOLVE = 0x200000000, /**< Resolve commands. */

    CU_PIPELINE_STAGE_BLIT = 0x400000000, /**< Blit commands. */

    CU_PIPELINE_STAGE_CLEAR = 0x800000000, /**< Clear commands. */
} CuPipelineStageFlagBits;

/// @brief A bitmask of `CuPipelineStageFlagBits`.
typedef uint64_t CuPipelineStageFlags;

/**
 * @brief Memory access bitmap flags.
 * @note The values match their `VkAccessFlagBits2` counterparts.
 */
typedef enum : uint64_t {
    CU_ACCESS_NONE = 0, /**< No access. */

    CU_ACCESS_INDIRECT_COMMAND_READ = 0x00000001, /**< Read as indirect command data. */

    CU_ACCESS_INDEX_READ = 0x00000002, /**< Read as an index buffer. */

    CU_ACCESS_VERTEX_ATTRIBUTE_READ = 0x00000004, /**< Read as a vertex buffer. */

    CU_ACCESS_UNIFORM_READ = 0x00000008, /**< Read as a uniform buffer. */

    CU_ACCESS_SHADER_READ = 0x00000020, /**< Read in a shader. */

    CU_ACCESS_SHADER_WRITE = 0x00000040, /**< Written to in a shader. */

    CU_ACCESS_COLOR_ATTACHMENT_READ = 0x00000080, /**< Read as a color attachment. */

    CU_ACCESS_COLOR_ATTACHMENT_WRITE = 0x00000100, /**< Written to as a color attachment. */

    CU_ACCESS_DEPTH_STENCIL_READ = 0x00000200, /**< Read as a depth/stencil attachment. */

    CU_ACCESS_DEPTH_STENCIL_WRITE = 0x00000400, /**< Written to as a depth/stencil attachment. */

    CU_ACCESS_TRANSFER_READ = 0x00000800, /**< Read in a transfer command. */

    CU_ACCESS_TRANSFER_WRITE = 0x00001000, /**< Written to in a transfer command. */

    CU_ACCESS_HOST_READ = 0x00002000, /**< Read by the host. */

    CU_ACCESS_HOST_WRITE = 0x00004000, /**< Written to by the host. */

    CU_ACCESS_MEMORY_READ = 0x00008000, /**< Any type of read. */

    CU_ACCESS_MEMORY_WRITE = 0x00010000, /**< Any type of write. */
} CuAccessFlagBits;

/// @brief A bitmask of `CuAccessFlagBits`.
typedef uint64_t CuAccessFlags;

#ifdef __cplusplus
extern "C" {
#endif

/// @brief Binds the graphics pipeline.
/// @param [in, out] commandBuffer The command buffer.
/// @param graphicsPipeline The graphics pipeline.
void cu_cmd_bind_graphics_pipeline(
    CuCmdBuffer commandBuffer,
    CuGraphicsPipeline graphicsPipeline);

/// @brief Runs a draw call.
/// @param [in, out] commandBuffer The command buffer.
/// @param nVertices The number of vertices.
/// @param iFirstVertex The index of the first vertex.
/// @param nInstances The number of instances.
/// @param iFirstInstance The index of the first instance.
void cu_cmd_draw(
    CuCmdBuffer commandBuffer,
    uint32_t nVertices,
    uint32_t iFirstVertex,
    uint32_t nInstances,
    uint32_t iFirstInstance);

/// @brief Binds the compute pipeline.
/// @param [in, out] commandBuffer The command buffer.
/// @param computePipeline The graphics pipeline.
void cu_cmd_bind_compute_pipeline(
    CuCmdBuffer commandBuffer,
    CuComputePipeline computePipeline);

/// @brief Dispatches compute workgroups.
/// @param [in, out] commandBuffer The command buffer.
/// @param x The number of local workgroups in X.
/// @param y The number of local workgroups in Y.
/// @param z The number of local workgroups in Z.
void cu_cmd_dispatch(
    CuCmdBuffer commandBuffer,
    uint32_t x,
    uint32_t y,
    uint32_t z);

/// @brief Sets the push constants at the given range.
/// @param [in, out] commandBuffer The command buffer.
/// @param pipelineLayout The pipeline layout.
/// @param p A pointer to the push constant data to write.
/// @param z The size of the push constant data.
/// @param o The offset to write the push constants to.
void cu_cmd_write_push_constants(
    CuCmdBuffer commandBuffer,
    CuPipelineLayout pipelineLayout,
    const void* p,
    size_t z,
    size_t o);

/**
 * @brief Copies data from one buffer to another.
 * @param [in, out] commandBuffer The command buffer.
 * @param dst The destination buffer.
 * @param oDst The write offset.
 * @param src The source buffer.
 * @param oSrc The read offset.
 * @param z The size of the data to copy.
 */
void cu_cmd_buffer_copy(
    CuCmdBuffer commandBuffer,
    CuBuffer dst,
    size_t oDst,
    CuBuffer src,
    size_t oSrc,
    size_t z);

/**
 * @brief Transitions the image from one layout to another.
 * @param [in, out] commandBuffer The command buffer.
 * @param [in] pImage A pointer to the image.
 * @param srcStageMask The pipeline stages that last used the image.
 * @param srcAccessMask How the image was last accessed.
 * @param dstStageMask The pipeline stages that will use the image next.
 * @param dstAccessMask How the image will be accessed next.
 * @param oldLayout The image's current layout.
 * @param newLayout The layout to transition the image to.
 */
void cu_cmd_image_barrier(
    CuCmdBuffer commandBuffer,
    const CuImage* pImage,
    CuPipelineStageFlags srcStageMask,
    CuAccessFlags srcAccessMask,
    CuPipelineStageFlags dstStageMask,
    CuAccessFlags dstAccessMask,
    CuImageLayout oldLayout,
    CuImageLayout newLayout);

/**
 * @brief Fills the image with a color.
 * @param [in, out] commandBuffer The command buffer.
 * @param [in] pImage A pointer to the image.
 * @param layout The image's current layout.
 * @param color The color to fill the image with.
 */
void cu_cmd_image_fill(
    CuCmdBuffer commandBuffer,
    const CuImage* pImage,
    CuImageLayout layout,
    CuRGBA color);

/**
 * @brief Binds the context's image and sampler descriptors for use in shaders.
 * @param [in, out] commandBuffer The command buffer.
 * @param pipelineLayout The pipeline layout.
 * @note Sampled images are at binding 0, samplers at 1 and storage images at 2 of set 0.
 */
void cu_cmd_bind_descriptors(
    CuCmdBuffer commandBuffer,
    CuPipelineLayout pipelineLayout);

#ifdef __cplusplus
}
#endif
