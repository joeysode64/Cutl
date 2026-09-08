#include "pipeline.h"

#include "g_context.h"
#include "result.h"
#include "vk.h"

#include <stdint.h>
#include <vulkan/vulkan.h>

CuResult cu_pipeline_layout_create(
    CuPipelineLayout* const pPipelineLayout,
    const CuPipelineLayoutCreateInfo* pCreateInfo)
{
    if (pCreateInfo == nullptr) {
        pCreateInfo = &CU_DEFAULT_PIPELINE_LAYOUT_CREATE_INFO;
    }

    const VkPipelineLayoutCreateInfo createInfo = {
        .sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO,
        .pNext = nullptr,
        .flags = 0,
        .setLayoutCount = 0,
        .pSetLayouts = nullptr,
        .pushConstantRangeCount = pCreateInfo->pushConstantSize > 0 ? 1 : 0,
        .pPushConstantRanges =
            (VkPushConstantRange[]){
                {
                    .offset = 0,
                    .size = pCreateInfo->pushConstantSize,
                    .stageFlags = VK_SHADER_STAGE_ALL,
                },
            },
    };
    return vk_result_to_cu_result(vkCreatePipelineLayout(
        gContext.device, &createInfo, nullptr, (VkPipelineLayout*)pPipelineLayout
    ));
}

void cu_pipeline_layout_destroy(
    const CuPipelineLayout pipelineLayout)
{
    vkDestroyPipelineLayout(gContext.device, (VkPipelineLayout)pipelineLayout, nullptr);
}

CuResult cu_compute_pipeline_create(
    CuGraphicsPipeline* const pComputePipeline,
    CuPipelineLayout pipelineLayout,
    const uint32_t* const pCode,
    const size_t n)
{
    const VkShaderModuleCreateInfo shaderModuleCreateInfo = {
        .sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO,
        .pNext = nullptr,
        .flags = 0,
        .codeSize = n,
        .pCode = pCode,
    };
    const VkComputePipelineCreateInfo createInfo = {
        .sType = VK_STRUCTURE_TYPE_COMPUTE_PIPELINE_CREATE_INFO,
        .pNext = nullptr,
        .flags = 0,
        .stage =
            {
                .sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,
                .pNext = &shaderModuleCreateInfo,
                .flags = 0,
                .stage = VK_SHADER_STAGE_COMPUTE_BIT,
                .module = VK_NULL_HANDLE,
                .pName = "main",
                .pSpecializationInfo = nullptr,
            },
        .layout = (VkPipelineLayout)pipelineLayout,
        .basePipelineHandle = VK_NULL_HANDLE,
        .basePipelineIndex = 0,
    };
    return vk_result_to_cu_result(vkCreateComputePipelines(
        gContext.device, VK_NULL_HANDLE, 1, &createInfo, nullptr, (VkPipeline*)pComputePipeline
    ));
}

void cu_compute_pipeline_destroy(
    CuComputePipeline computePipeline)
{
    vkDestroyPipeline(gContext.device, (VkPipeline)computePipeline, nullptr);
}

CuResult cu_graphics_pipeline_create(
    CuGraphicsPipeline* const pGrahicsPipeline,
    const CuGraphicsPipelineCreateInfo* const pCreateInfo,
    const CuPipelineLayout pipelineLayout)
{
    const char* shaderMainFnName = "main";
    const VkShaderModuleCreateInfo vertexShaderModuleCreateInfo = {
        .sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO,
        .pNext = nullptr,
        .flags = 0,
        .codeSize = pCreateInfo->nVert * sizeof(uint32_t),
        .pCode = pCreateInfo->pVert,
    };
    const VkShaderModuleCreateInfo fragmentShaderModuleCreateInfo = {
        .sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO,
        .pNext = nullptr,
        .flags = 0,
        .codeSize = pCreateInfo->nFrag * sizeof(uint32_t),
        .pCode = pCreateInfo->pFrag,
    };
    const VkPipelineShaderStageCreateInfo shaderStageCreateInfos[] = {
        {
            .sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,
            .pNext = &vertexShaderModuleCreateInfo,
            .flags = 0,
            .stage = VK_SHADER_STAGE_VERTEX_BIT,
            .module = VK_NULL_HANDLE,
            .pName = shaderMainFnName,
            .pSpecializationInfo = nullptr,
        },
        {
            .sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,
            .pNext = &fragmentShaderModuleCreateInfo,
            .flags = 0,
            .stage = VK_SHADER_STAGE_FRAGMENT_BIT,
            .module = VK_NULL_HANDLE,
            .pName = shaderMainFnName,
            .pSpecializationInfo = nullptr,
        },
    };
    const VkPipelineVertexInputStateCreateInfo vertexInputStateCreateInfo = {
        .sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO,
        .pNext = nullptr,
        .flags = 0,
        .vertexBindingDescriptionCount = 0,
        .pVertexBindingDescriptions = nullptr,
        .vertexAttributeDescriptionCount = 0,
        .pVertexAttributeDescriptions = nullptr,
    };
    const VkPipelineInputAssemblyStateCreateInfo inputAssemblyCreateInfo = {
        .sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO,
        .pNext = nullptr,
        .flags = 0,
        .topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST,
        .primitiveRestartEnable = VK_FALSE,
    };
    const VkViewport viewport = {
        .x = 0.0F,
        .y = 0.0F,
        .width = 0.0F,
        .height = 0.0F,
        .minDepth = 0.0F,
        .maxDepth = 0.0F,
    };
    const VkRect2D scissor = {
        .offset =
            {
                .x = 0,
                .y = 0,
            },
        .extent =
            {
                .width = 0,
                .height = 0,
            },
    };
    const VkPipelineViewportStateCreateInfo viewportStateCreateInfo = {
        .sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO,
        .pNext = nullptr,
        .flags = 0,
        .viewportCount = 1,
        .pViewports = &viewport,
        .scissorCount = 1,
        .pScissors = &scissor,
    };
    const VkPipelineRasterizationStateCreateInfo rasterizationStateCreateInfo = {
        .sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO,
        .pNext = nullptr,
        .flags = 0,
        .depthClampEnable = VK_FALSE,
        .rasterizerDiscardEnable = VK_FALSE,
        .polygonMode = VK_POLYGON_MODE_FILL,
        .cullMode = VK_CULL_MODE_NONE,
        .frontFace = VK_FRONT_FACE_CLOCKWISE,
        .depthBiasEnable = VK_FALSE,
        .depthBiasConstantFactor = 0.0F,
        .depthBiasClamp = 0.0F,
        .depthBiasSlopeFactor = 0.0F,
        .lineWidth = 1.0F,
    };
    const VkPipelineMultisampleStateCreateInfo multisampleStateCreateInfo = {
        .sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO,
        .pNext = nullptr,
        .flags = 0,
        .rasterizationSamples = VK_SAMPLE_COUNT_1_BIT,
        .sampleShadingEnable = VK_FALSE,
        .minSampleShading = 1.0F,
        .pSampleMask = nullptr,
        .alphaToCoverageEnable = VK_FALSE,
        .alphaToOneEnable = VK_FALSE,
    };
    const VkPipelineDepthStencilStateCreateInfo depthStencilStateCreateInfo = {
        .sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO,
        .pNext = nullptr,
        .flags = 0,
        .depthTestEnable = VK_FALSE,
        .depthWriteEnable = VK_FALSE,
        .depthCompareOp = VK_COMPARE_OP_NEVER,
        .depthBoundsTestEnable = VK_FALSE,
        .stencilTestEnable = VK_FALSE,
        .front = {},
        .back = {},
        .minDepthBounds = 0.0F,
        .maxDepthBounds = 1.0F,
    };
    const VkColorComponentFlags colorWriteMask = VK_COLOR_COMPONENT_R_BIT |
        VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
    const VkPipelineColorBlendAttachmentState colorBlendAttachmentState = {
        .blendEnable = VK_TRUE,
        .srcColorBlendFactor = VK_BLEND_FACTOR_SRC_ALPHA,
        .dstColorBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA,
        .colorBlendOp = VK_BLEND_OP_ADD,
        .srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE,
        .dstAlphaBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA,
        .alphaBlendOp = VK_BLEND_OP_ADD,
        .colorWriteMask = colorWriteMask,
    };
    const VkPipelineColorBlendStateCreateInfo colorBlendStateCreateInfo = {
        .sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO,
        .pNext = nullptr,
        .flags = 0,
        .logicOpEnable = VK_FALSE,
        .logicOp = VK_LOGIC_OP_COPY,
        .attachmentCount = 1,
        .pAttachments = &colorBlendAttachmentState,
        .blendConstants = {0.0F, 0.0F, 0.0F, 0.0F},
    };
    const VkPipelineRenderingCreateInfo renderingCreateInfo = {
        .sType = VK_STRUCTURE_TYPE_PIPELINE_RENDERING_CREATE_INFO,
        .pNext = nullptr,
        .viewMask = 0,
        .colorAttachmentCount = pCreateInfo->nFormats,
        .pColorAttachmentFormats = (VkFormat*)pCreateInfo->pFormats,
        .depthAttachmentFormat = VK_FORMAT_UNDEFINED,
        .stencilAttachmentFormat = VK_FORMAT_UNDEFINED,
    };
    const VkDynamicState dynamicStates[] = {
        VK_DYNAMIC_STATE_VIEWPORT,
        VK_DYNAMIC_STATE_SCISSOR,
    };
    const VkPipelineDynamicStateCreateInfo dynamicStateCreateInfo = {
        .sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO,
        .pNext = nullptr,
        .flags = 0,
        .dynamicStateCount = sizeof(dynamicStates) / sizeof(dynamicStates[0]),
        .pDynamicStates = dynamicStates,
    };
    const VkGraphicsPipelineCreateInfo createInfo = {
        .sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO,
        .pNext = &renderingCreateInfo,
        .flags = 0,
        .stageCount = sizeof(shaderStageCreateInfos) / sizeof(shaderStageCreateInfos[0]),
        .pStages = shaderStageCreateInfos,
        .pVertexInputState = &vertexInputStateCreateInfo,
        .pInputAssemblyState = &inputAssemblyCreateInfo,
        .pTessellationState = nullptr,
        .pViewportState = &viewportStateCreateInfo,
        .pRasterizationState = &rasterizationStateCreateInfo,
        .pMultisampleState = &multisampleStateCreateInfo,
        .pDepthStencilState = &depthStencilStateCreateInfo,
        .pColorBlendState = &colorBlendStateCreateInfo,
        .pDynamicState = &dynamicStateCreateInfo,
        .layout = (VkPipelineLayout)pipelineLayout,
        .renderPass = VK_NULL_HANDLE,
        .subpass = 0,
        .basePipelineHandle = VK_NULL_HANDLE,
        .basePipelineIndex = -1,
    };
    return vk_result_to_cu_result(vkCreateGraphicsPipelines(
        gContext.device, VK_NULL_HANDLE, 1, &createInfo, NULL, (VkPipeline*)pGrahicsPipeline
    ));
}

void cu_graphics_pipeline_destroy(
    const CuGraphicsPipeline graphicsPipeline)
{
    vkDestroyPipeline(gContext.device, (VkPipeline)graphicsPipeline, nullptr);
}
