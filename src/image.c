#include "image.h"

#include "allocation.h"
#include "g_context.h"
#include "log.h"
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

    cu_try_catch(create_image(
        &pImage->_image,
        w,
        h,
        (VkFormat)format,
        (VkImageUsageFlags)usage));
    cu_log_create(pImage->_image);

    VkMemoryRequirements memoryRequirements = {};
    vkGetImageMemoryRequirements(gContext._device, pImage->_image, &memoryRequirements);
    cu_try_catch(mode_allocate(
        &pImage->_allocation,
        nullptr, mode,
        &memoryRequirements,
        VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT));

    cu_try_catch_vk(vkBindImageMemory(
        gContext._device,
        pImage->_image,
        pImage->_allocation._memory,
        pImage->_allocation._offset));

    cu_try_catch(create_image_view(
        &pImage->_imageView, pImage->_image, (VkFormat)format));
    cu_log_create(pImage->_imageView);

    return CU_SUCCESS;

FAIL:
    cu_log_fail(result);
    cu_image_destroy(pImage, mode);
    return result;
}

void cu_image_destroy(
    CuImage* const pImage,
    const CuAllocationMode mode)
{
    vkDestroyImageView(gContext._device, pImage->_imageView, nullptr);
    vkDestroyImage(gContext._device, pImage->_image, nullptr);
    mode_free(&pImage->_allocation, mode);
}

CuResult cu_sampler_create(
    CuSampler* const pSampler)
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
    return cu_vk_result(vkCreateSampler(
        gContext._device, &createInfo, nullptr, &pSampler->_sampler));
}

void cu_sampler_destroy(
    CuSampler* const pSampler)
{
    vkDestroySampler(gContext._device, pSampler->_sampler, nullptr);
}

CuResult cu_png_load(
    void* const pData,
    uint32_t* const pW,
    uint32_t* const pH,
    const char* const path,
    const png_uint_32 format)
{
    CuResult result = CU_SUCCESS;

    png_image image = {};
    image.version = PNG_IMAGE_VERSION;
    cu_assert_catch(
        png_image_begin_read_from_file(&image, path), cu_error(CU_ERROR_PNG_READ));

    if (format != 0) {
        image.format = format;
    }

    if (pW != nullptr) {
        *pW = image.width;
    }
    if (pH != nullptr) {
        *pH = image.height;
    }

    if (pData != nullptr) {
        cu_assert_catch(
            png_image_finish_read(&image, nullptr, pData, 0, nullptr),
            cu_error(CU_ERROR_PNG_READ));
    }

    png_image_free(&image);
    return CU_SUCCESS;

FAIL:
    cu_log_fail(result);
    png_image_free(&image);
    return result;
}
