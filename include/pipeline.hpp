#pragma once

#include "pipeline.h"
#include "result.hpp"
#include "spirv.h"
#include "wrapper.hpp"

#include <stddef.h>
#include <stdint.h>
#include <vulkan/vulkan.h>

namespace cu {
    /** @ref CuPipelineLayout */
    class PipelineLayout : public CWrapper<CuPipelineLayout> {
    private:
        PipelineLayout(const PipelineLayout&) = delete;

        PipelineLayout& operator=(const PipelineLayout&) = delete;

    public:
        /** @ref CuPipelineLayoutCreateInfo */
        using CreateInfo = CuPipelineLayoutCreateInfo;

        /** @ref CU_DEFAULT_PIPELINE_LAYOUT_CREATE_INFO */
        static constexpr CreateInfo DEFAULT_CREATE_INFO = CU_DEFAULT_PIPELINE_LAYOUT_CREATE_INFO;

        /** @brief A pipeline layout builder. */
        struct Builder : public PipelineLayout::CreateInfo {
            /** @brief Creates a builder with the default create info. */
            constexpr Builder()
                : PipelineLayout::CreateInfo{PipelineLayout::DEFAULT_CREATE_INFO}
            {
            }

            /**
             * @brief Creates a builder with the given create info.
             * @param createInfo The create info.
             */
            constexpr Builder(const PipelineLayout::CreateInfo& createInfo)
                : PipelineLayout::CreateInfo{createInfo}
            {
            }

            /** @ref CuPipelineLayoutCreateInfo::pushConstantSize */
            constexpr Builder& push_constant_size(const uint32_t x)
            {
                pushConstantSize = x;
                return *this;
            }

            /**
             * @brief Builds/creates the pipeline layout.
             * @param [out] pipelineLayout The pipeline layout.
             * @return The result of creating the pipeline layout.
             */
            inline Result build(PipelineLayout& pipelineLayout) const
            {
                return pipelineLayout.init(*this);
            }
        };

        /** @ref cu_pipeline_layout_destroy() */
        inline ~PipelineLayout()
        {
            cu_pipeline_layout_destroy(_v);
        }

        /** @brief Creates an uninitialized pipeline layout. */
        constexpr PipelineLayout()
            : CWrapper{}
        {
        }

        /** @ref cu_pipeline_layout_create() */
        inline Result init(const CreateInfo& createInfo = DEFAULT_CREATE_INFO)
        {
            return cu_pipeline_layout_create(&_v, &createInfo);
        }
    };

    /** @ref CuComputePipeline */
    class ComputePipeline : public CWrapper<CuComputePipeline> {
    private:
        ComputePipeline(const ComputePipeline&) = delete;

        ComputePipeline& operator=(const ComputePipeline&) = delete;

    public:
        /** @ref cu_compute_pipeline_destroy() */
        inline ~ComputePipeline()
        {
            cu_compute_pipeline_destroy(_v);
        }

        /** @brief Creates an uninitialized compute pipeline. */
        constexpr ComputePipeline()
            : CWrapper{}
        {
        }

        /** @ref cu_compute_pipeline_create() */
        inline Result init(
            const CuPipelineLayout pipelineLayout,
            const uint32_t* const pCode,
            const size_t n)
        {
            return cu_compute_pipeline_create(&_v, pipelineLayout, pCode, n);
        }

        /**
         * @brief Creates the compute pipeline from a SPIR-V file.
         * @param pipelineLayout The pipeline layout to use.
         * @param path The compute shader's SPIR-V file path.
         * @return The result of the compute pipeline's creation.
         */
        inline Result init(const CuPipelineLayout pipelineLayout, const char* const path)
        {
            CuSpirV spirv{};
            const Result readResult = cu_spirv_read_from_file(&spirv, path);
            if (!readResult.is_success()) {
                return readResult;
            }

            const Result result = init(pipelineLayout, spirv.data, spirv.n);
            cu_spirv_destroy(&spirv);
            return result;
        }
    };

    /** @ref CuGraphicsPipeline */
    class GraphicsPipeline : public CWrapper<CuGraphicsPipeline> {
    private:
        GraphicsPipeline(const GraphicsPipeline&) = delete;

        GraphicsPipeline& operator=(const GraphicsPipeline&) = delete;

    public:
        /** @ref CuGraphicsPipelineCreateInfo */
        using CreateInfo = CuGraphicsPipelineCreateInfo;

        /** @ref CU_DEFAULT_GRAPHICS_PIPELINE_CREATE_INFO */
        static constexpr CreateInfo DEFAULT_CREATE_INFO = CU_DEFAULT_GRAPHICS_PIPELINE_CREATE_INFO;

        /** @brief A graphics pipeline builder. */
        struct Builder : public GraphicsPipeline::CreateInfo {
            /** @brief Creates a builder with the default create info. */
            constexpr Builder()
                : GraphicsPipeline::CreateInfo{GraphicsPipeline::DEFAULT_CREATE_INFO}
            {
            }

            /**
             * @brief Creates a builder with the given create info.
             * @param createInfo The create info.
             */
            constexpr Builder(const GraphicsPipeline::CreateInfo& createInfo)
                : GraphicsPipeline::CreateInfo{createInfo}
            {
            }

            /** @ref CuGraphicsPipelineCreateInfo::rasterizationSamples */
            constexpr Builder& rasterization_samples(const VkSampleCountFlags x)
            {
                rasterizationSamples = x;
                return *this;
            }

            /** @ref CuGraphicsPipelineCreateInfo::depthTestEnable */
            constexpr Builder& depth_test_enable(const bool x)
            {
                depthTestEnable = x;
                return *this;
            }

            /** @ref CuGraphicsPipelineCreateInfo::depthWriteEnable */
            constexpr Builder& depth_write_enable(const bool x)
            {
                depthWriteEnable = x;
                return *this;
            }

            /** @ref CuGraphicsPipelineCreateInfo::depthCompareOp */
            constexpr Builder& depth_compare_op(const VkCompareOp x)
            {
                depthCompareOp = x;
                return *this;
            }

            /** @ref CuGraphicsPipelineCreateInfo::minDepthBounds */
            constexpr Builder& min_depth_bounds(const float x)
            {
                minDepthBounds = x;
                return *this;
            }

            /** @ref CuGraphicsPipelineCreateInfo::maxDepthBounds */
            constexpr Builder& max_depth_bounds(const float x)
            {
                maxDepthBounds = x;
                return *this;
            }

            /**
             * @brief Builds/creates the graphics pipeline.
             * @param [out] graphicsPipeline The graphics pipeline.
             * @param pipelineLayout The pipeline layout to use.
             * @param vertPath The vertex shader's SPIR-V file path.
             * @param fragPath The fragment shader's SPIR-V file path.
             * @param pFormats A pointer to the formats the pipeline must handle.
             * @param nFormats The number of formats the pipeline must handle.
             * @return The result of creating the graphics pipeline.
             */
            inline Result build(
                GraphicsPipeline& graphicsPipeline,
                const CuPipelineLayout pipelineLayout,
                const char* const vertPath,
                const char* const fragPath,
                const VkFormat* const pFormats,
                const size_t nFormats) const
            {
                CuSpirV vert{};
                const Result vertResult = cu_spirv_read_from_file(&vert, vertPath);
                if (!vertResult.is_success()) {
                    return vertResult;
                }

                CuSpirV frag{};
                const Result fragResult = cu_spirv_read_from_file(&frag, fragPath);
                if (!fragResult.is_success()) {
                    cu_spirv_destroy(&vert);
                    return fragResult;
                }

                GraphicsPipeline::CreateInfo createInfo = *this;
                createInfo.pVert = vert.data;
                createInfo.nVert = vert.n;
                createInfo.pFrag = frag.data;
                createInfo.nFrag = frag.n;
                createInfo.pFormats = pFormats;
                createInfo.nFormats = nFormats;

                const Result result = graphicsPipeline.init(pipelineLayout, createInfo);
                cu_spirv_destroy(&frag);
                cu_spirv_destroy(&vert);
                return result;
            }
        };

        /** @ref cu_graphics_pipeline_destroy() */
        inline ~GraphicsPipeline()
        {
            cu_graphics_pipeline_destroy(_v);
        }

        /** @brief Creates an uninitialized graphics pipeline. */
        constexpr GraphicsPipeline()
            : CWrapper{}
        {
        }

        /** @ref cu_graphics_pipeline_create() */
        inline Result init(const CuPipelineLayout pipelineLayout, const CreateInfo& createInfo)
        {
            return cu_graphics_pipeline_create(&_v, &createInfo, pipelineLayout);
        }
    };
} // namespace cu
