#include "image.h"

#include "allocation.h"
#include "cmd_buffer.h"
#include "debug.h"
#include "g_context.h"
#include "result.h"
#include "util.h"
#include "vk.h"

#include <png.h>
#include <stdint.h>
#include <vulkan/vulkan.h>

CuResult cu_image_create(
    CuImage* const pImage,
    const uint32_t w,
    const uint32_t h,
    const VkFormat format,
    const VkImageUsageFlags usage,
    const CuAllocationMode mode)
{
    CuResult result = CU_SUCCESS;

    cu_try_catch(create_image(&pImage->_image, w, h, (VkFormat)format, (VkImageUsageFlags)usage));
    dbg_log_create(pImage->_image);

    VkMemoryRequirements memoryRequirements = {};
    vkGetImageMemoryRequirements(gContext._device, pImage->_image, &memoryRequirements);
    cu_try_catch(mode_allocate(
        &pImage->_allocation,
        nullptr,
        mode,
        &memoryRequirements,
        VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT));

    cu_try_catch_vk(vkBindImageMemory(
        gContext._device,
        pImage->_image,
        pImage->_allocation._memory,
        pImage->_allocation._offset));

    cu_try_catch(create_image_view(&pImage->_imageView, pImage->_image, (VkFormat)format));
    dbg_log_create(pImage->_imageView);

    return CU_SUCCESS;

FAIL:
    dbg_log_fail(result);
    cu_image_destroy(pImage, mode);
    return result;
}

void cu_image_destroy(CuImage* const pImage, const CuAllocationMode mode)
{
    vkDestroyImageView(gContext._device, pImage->_imageView, nullptr);
    vkDestroyImage(gContext._device, pImage->_image, nullptr);
    mode_free(&pImage->_allocation, mode);
}

void cu_image_start_rendering(
    const CuImage* const pImage,
    const VkCommandBuffer commandBuffer,
    const uint32_t w,
    const uint32_t h)
{
    // Set the viewport and scissor.
    const VkRect2D area = {
        .offset =
            {
                .x = 0,
                .y = 0,
            },
        .extent =
            {
                .width = w,
                .height = h,
            },
    };
    const VkViewport viewport = {
        .x = 0.0F,
        .y = (float)area.extent.height,
        .width = (float)area.extent.width,
        .height = -(float)area.extent.height,
        .minDepth = 0.0F,
        .maxDepth = 1.0F,
    };
    vkCmdSetViewport(commandBuffer, 0, 1, &viewport);
    vkCmdSetScissor(commandBuffer, 0, 1, &area);

    // Begin rendering.
    const VkRenderingAttachmentInfo colorAttachment = {
        .sType = VK_STRUCTURE_TYPE_RENDERING_ATTACHMENT_INFO,
        .pNext = nullptr,
        .imageView = pImage->_imageView,
        .imageLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
        .resolveMode = VK_RESOLVE_MODE_NONE,
        .resolveImageView = VK_NULL_HANDLE,
        .resolveImageLayout = VK_IMAGE_LAYOUT_UNDEFINED,
        .loadOp = VK_ATTACHMENT_LOAD_OP_LOAD,
        .storeOp = VK_ATTACHMENT_STORE_OP_STORE,
        .clearValue = {},
    };
    const VkRenderingInfo renderingInfo = {
        .sType = VK_STRUCTURE_TYPE_RENDERING_INFO,
        .pNext = nullptr,
        .flags = 0,
        .renderArea = area,
        .layerCount = 1,
        .viewMask = 0,
        .colorAttachmentCount = 1,
        .pColorAttachments = &colorAttachment,
        .pDepthAttachment = nullptr,
        .pStencilAttachment = nullptr,
    };
    vkCmdBeginRendering(commandBuffer, &renderingInfo);
}

void cu_image_end_rendering(
    const CuImage* const pImage,
    const VkCommandBuffer commandBuffer,
    const VkImageLayout newLayout)
{
    // End the render.
    vkCmdEndRendering(commandBuffer);

    // Convert the image for its next use.
    cu_cmd_image_barrier(
        commandBuffer,
        pImage->_image,
        VK_PIPELINE_STAGE_2_COLOR_ATTACHMENT_OUTPUT_BIT,
        VK_ACCESS_2_COLOR_ATTACHMENT_WRITE_BIT,
        VK_PIPELINE_STAGE_2_ALL_GRAPHICS_BIT,
        VK_ACCESS_2_MEMORY_READ_BIT | VK_ACCESS_2_MEMORY_WRITE_BIT,
        VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
        newLayout);
}

CuResult cu_sampler_create(CuSampler* const pSampler)
{
    const VkSamplerCreateInfo createInfo = {
        .sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO,
        .pNext = nullptr,
        .flags = 0,
        .magFilter = VK_FILTER_NEAREST,
        .minFilter = VK_FILTER_NEAREST,
        .mipmapMode = VK_SAMPLER_MIPMAP_MODE_NEAREST,
        .addressModeU = VK_SAMPLER_ADDRESS_MODE_REPEAT,
        .addressModeV = VK_SAMPLER_ADDRESS_MODE_REPEAT,
        .addressModeW = VK_SAMPLER_ADDRESS_MODE_REPEAT,
        .mipLodBias = 0,
        .anisotropyEnable = VK_FALSE,
        .maxAnisotropy = 0,
        .compareEnable = VK_FALSE,
        .compareOp = VK_COMPARE_OP_NEVER,
        .minLod = 0.0F,
        .maxLod = 0.0F,
        .borderColor = VK_BORDER_COLOR_FLOAT_TRANSPARENT_BLACK,
        .unnormalizedCoordinates = VK_FALSE,
    };
    return cu_vk_result(
        vkCreateSampler(gContext._device, &createInfo, nullptr, &pSampler->_sampler));
}

void cu_sampler_destroy(CuSampler* const pSampler)
{
    vkDestroySampler(gContext._device, pSampler->_sampler, nullptr);
}

CuResult cu_png_load(
    void** const ppData,
    uint32_t* const pW,
    uint32_t* const pH,
    const char* const path,
    const png_uint_32 format)
{
    CuResult result = CU_SUCCESS;

    png_image image = {};
    image.version = PNG_IMAGE_VERSION;
    cu_assert_catch(png_image_begin_read_from_file(&image, path), cu_error(CU_ERROR_PNG_READ));

    if (format != 0) {
        image.format = format;
    }

    if (pW != nullptr) {
        *pW = image.width;
    }
    if (pH != nullptr) {
        *pH = image.height;
    }

    if (ppData != nullptr) {
        allocate_catch_z(*ppData, PNG_IMAGE_SIZE(image));
        cu_assert_catch(
            png_image_finish_read(&image, nullptr, *ppData, 0, nullptr),
            cu_error(CU_ERROR_PNG_READ));
    }

    png_image_free(&image);
    return CU_SUCCESS;

FAIL:
    dbg_log_fail(result);
    png_image_free(&image);
    return result;
}
