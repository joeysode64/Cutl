#pragma once

#include "def.h"
#include "result.h"

#include <stddef.h>
#include <stdint.h>

/// @brief A pipeline layout.
typedef cu_vk_t(PipelineLayout) CuPipelineLayout;

/// @brief Create info for a pipeline layout.
typedef struct CuPipelineLayoutCreateInfo_T {
    /// @brief The size of the push constant data. Default is 0.
    uint32_t pushConstantSize;
} CuPipelineLayoutCreateInfo;

/// @brief The default pipeline layout create info.
static const CuPipelineLayoutCreateInfo CU_DEFAULT_PIPELINE_LAYOUT_CREATE_INFO = {
    .pushConstantSize = 0,
};

/// @brief A compute pipeline.
typedef cu_vk_t(Pipeline) CuComputePipeline;


/// @brief A graphics pipeline.
typedef cu_vk_t(Pipeline) CuGraphicsPipeline;

/// @brief Create info for a graphics pipeline.
typedef struct {
    /// @brief A pointer to the vertex shader SPIR-V code..
    const uint32_t* pVert;

    /// @brief The number of SPIR-V words in the vertex shader code.
    size_t nVert;

    /// @brief A pointer to the fragment shader SPIR-V code.
    const uint32_t* pFrag;

    /// @brief The number of SPIR-V words in the fragment shader code.
    size_t nFrag;

    /// @brief A pointer to the formats the pipeline must handle.
    const CuFormat* pFormats;

    /// @brief The number of formats the pipeline must handle.
    size_t nFormats;
} CuGraphicsPipelineCreateInfo;

#ifdef __cplusplus
extern "C" {
#endif

/// @brief Creates the pipeline layout.
/// @param [out] pPipelineLayout A pointer to the pipeline layout.
/// @param pCreateInfo A pointer to the create info.
/// @return The result of the pipeline layout's creation.
CuResult cu_pipeline_layout_create(
    CuPipelineLayout* pPipelineLayout,
    const CuPipelineLayoutCreateInfo* pCreateInfo);

/// @brief Destroys the pipeline layout.
/// @param pipelineLayout The pipeline layout.
void cu_pipeline_layout_destroy(
    CuPipelineLayout pipelineLayout);

/// @brief Creates the compute pipeline.
/// @param [out] pComputePipeline A pointer to the graphics pipeline.
/// @param pipelineLayout The pipeline layout to use.
/// @param pCode A pointer to the compute SPIR-V code.
/// @param n The number of SPIR-V words in the compute shader code.
/// @return The result of the compute pipeline's creation.
CuResult cu_compute_pipeline_create(
    CuComputePipeline* pComputePipeline,
    CuPipelineLayout pipelineLayout,
    const uint32_t* pCode,
    size_t n);

/// @brief Destroys the compute pipeline.
/// @param computePipeline The compute pipeline to destroy.
void cu_compute_pipeline_destroy(
    CuComputePipeline computePipeline);

/// @brief Creates the graphics pipeline.
/// @param [out] pGraphicsPipeline A pointer to the graphics pipeline to create.
/// @param pCreateInfo A pointer to the create info.
/// @param pipelineLayout The pipeline layout to use.
/// @return The result of the graphics pipeline's creation.
CuResult cu_graphics_pipeline_create(
    CuGraphicsPipeline* pGrahicsPipeline,
    const CuGraphicsPipelineCreateInfo* pCreateInfo,
    CuPipelineLayout pipelineLayout);

/// @brief Destroys the graphics pipeline.
/// @param graphicsPipeline The graphics pipeline to destroy.
void cu_graphics_pipeline_destroy(
    CuGraphicsPipeline graphicsPipeline);

#ifdef __cplusplus
}
#endif
