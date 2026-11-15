#pragma once

#include "result.h"

#include <stddef.h>
#include <stdint.h>
#include <vulkan/vulkan.h>

/** @brief A pipeline layout. */
typedef VkPipelineLayout CuPipelineLayout;

/** @brief Create info for a pipeline layout. */
typedef struct CuPipelineLayoutCreateInfo_T {
    /**
     * @brief The size of the push constant data in bytes. Must be a multiple of 4.
     * @note Default is 0 (no push constants).
     */
    uint32_t pushConstantSize;
} CuPipelineLayoutCreateInfo;

/** @brief The default pipeline layout create info. */
constexpr CuPipelineLayoutCreateInfo CU_DEFAULT_PIPELINE_LAYOUT_CREATE_INFO = {
    .pushConstantSize = 0,
};

/** @brief A compute pipeline. */
typedef VkPipeline CuComputePipeline;

/** @brief A graphics pipeline. */
typedef VkPipeline CuGraphicsPipeline;

/** @brief Create info for a graphics pipeline. */
typedef struct {
    /** @brief A pointer to the vertex shader SPIR-V code. Must not be null. */
    const uint32_t* pVert;

    /** @brief The number of SPIR-V words in the vertex shader code. */
    size_t nVert;

    /** @brief A pointer to the fragment shader SPIR-V code. Must not be null. */
    const uint32_t* pFrag;

    /** @brief The number of SPIR-V words in the fragment shader code. */
    size_t nFrag;

    /** @brief A pointer to the color attachment formats the pipeline must handle. */
    const VkFormat* pFormats;

    /** @brief The number of color attachment formats the pipeline must handle. */
    size_t nFormats;

    /**
     * @brief The rasterization samples value.
     * @note Default is 1 (no MSAA).
     */
    VkSampleCountFlags rasterizationSamples;

    /**
     * @brief Whether to enable depth testing.
     * @note Default is false.
     */
    bool depthTestEnable;

    /**
     * @brief Whether to enable depth writes.
     * @note Default is false.
     */
    bool depthWriteEnable;

    /**
     * @brief The depth comparison.
     * @note Default is `VK_COMPARE_OP_NEVER`.
     */
    VkCompareOp depthCompareOp;

    /**
     * @brief The minimum depth bound.
     * @note Default is 0.0.
     */
    float minDepthBounds;

    /**
     * @brief The maximum depth bound.
     * @note Default is 1.0.
     */
    float maxDepthBounds;
} CuGraphicsPipelineCreateInfo;

/**
 * @brief The default graphics pipeline create info.
 * @warning This cannot be used on its own as it doesn't define any shader code or formats.
 */
constexpr CuGraphicsPipelineCreateInfo CU_DEFAULT_GRAPHICS_PIPELINE_CREATE_INFO = {
    .pVert = nullptr,
    .nVert = 0,
    .pFrag = nullptr,
    .nFrag = 0,
    .pFormats = nullptr,
    .nFormats = 0,
    .rasterizationSamples = 1,
    .depthTestEnable = false,
    .depthWriteEnable = false,
    .depthCompareOp = VK_COMPARE_OP_NEVER,
    .minDepthBounds = 0.0F,
    .maxDepthBounds = 1.0F,
};

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Creates the pipeline layout.
 * @param[out] pPipelineLayout A pointer to the pipeline layout. Must not be null.
 * @param pCreateInfo A pointer to the create info. Must not be null.
 */
CU_NODISCARD_RESULT
CuResult cu_pipeline_layout_create(
    CuPipelineLayout* pPipelineLayout,
    const CuPipelineLayoutCreateInfo* pCreateInfo);

/**
 * @brief Destroys the pipeline layout.
 * @param pipelineLayout The pipeline layout to destroy. Ignored if `VK_NULL_HANDLE`.
 */
void cu_pipeline_layout_destroy(CuPipelineLayout pipelineLayout);

/**
 * @brief Creates the compute pipeline.
 * @param[out] pComputePipeline A pointer to the compute pipeline. Must not be null.
 * @param pipelineLayout The pipeline layout to use.
 * @param pCode A pointer to the compute shader SPIR-V code. Must not be null.
 * @param n The number of SPIR-V words in the compute shader code.
 */
CU_NODISCARD_RESULT
CuResult cu_compute_pipeline_create(
    CuComputePipeline* pComputePipeline,
    CuPipelineLayout pipelineLayout,
    const uint32_t* pCode,
    size_t n);

/**
 * @brief Destroys the compute pipeline.
 * @param computePipeline The compute pipeline to destroy. Ignored if `VK_NULL_HANDLE`.
 */
void cu_compute_pipeline_destroy(CuComputePipeline computePipeline);

/**
 * @brief Creates the graphics pipeline.
 * @param[out] pGraphicsPipeline A pointer to the graphics pipeline. Must not be null.
 * @param pCreateInfo A pointer to the create info. Must not be null.
 * @param pipelineLayout The pipeline layout to use.
 */
CU_NODISCARD_RESULT
CuResult cu_graphics_pipeline_create(
    CuGraphicsPipeline* pGraphicsPipeline,
    const CuGraphicsPipelineCreateInfo* pCreateInfo,
    CuPipelineLayout pipelineLayout);

/**
 * @brief Destroys the graphics pipeline.
 * @param graphicsPipeline The graphics pipeline to destroy. Ignored if `VK_NULL_HANDLE`.
 */
void cu_graphics_pipeline_destroy(CuGraphicsPipeline graphicsPipeline);

#ifdef __cplusplus
}
#endif
