#pragma once

#include "allocation.h"
#include "def.h"
#include "format.h"
#include "result.h"

#include <stdint.h>

/**
 * @brief Image usage flag bitmap bits.
 * @note Interchangable with `VkImageUsageFlagBits`.
 */
typedef enum : int32_t {
    CU_IMAGE_USAGE_TRANSFER_SRC_BIT = 1 << 0,

    CU_IMAGE_USAGE_TRANSFER_DST_BIT = 1 << 1,

    CU_IMAGE_USAGE_SAMPLED_BIT = 1 << 2,

    CU_IMAGE_USAGE_STORAGE_BIT = 1 << 3,

    CU_IMAGE_USAGE_COLOR_ATTACHMENT_BIT = 1 << 4,
} CuImageUsageFlagBits;

/**
 * @brief Image usage flags.
 * @note Interchangable with `VkImageUsageFlags`.
 */
typedef uint32_t CuImageUsageFlags;

/**
 * @brief An image layout.
 * @note Interchangable with `VK_IMAGE_LAYOUT`.
 */
typedef enum : int32_t {
    CU_IMAGE_LAYOUT_UNDEFINED = 0, /**< Undefined. */

    CU_IMAGE_LAYOUT_GENERAL = 1, /**< Used for anything. */

    CU_IMAGE_LAYOUT_COLOR_ATTACHMENT = 2, /**< Used as a color attachment. */

    CU_IMAGE_LAYOUT_SHADER_READ_ONLY = 5, /**< Used for sampling. */

    CU_IMAGE_LAYOUT_TRANSFER_SRC = 6, /**< Used as a transfer source. */

    CU_IMAGE_LAYOUT_TRANSFER_DST = 7, /**< Used as a transfer destination (e.g. filling). */

    CU_IMAGE_LAYOUT_PRESENT_SRC = 1000001002, /**< Presented to a surface. */
} CuImageLayout;

/** @brief An image. */
typedef struct CuImage_T {
    cu_vk_t(Image) _image; /**< The image handle. */

    cu_vk_t(ImageView) _imageView; /**< The image view handle. */

    CuAllocation _allocation; /**< The allocation. */
} CuImage;

/** @brief An image sampler. */
typedef struct CuSampler_T {
    cu_vk_t(Sampler) _sampler; /**< The sampler handle. */
} CuSampler;


#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Creates the image.
 * @param [out] pImage A pointer to the image.
 * @param w The image's width.
 * @param h The image's height.
 * @param mode The allocation mode.
 * @return The result of creating the image.
 */
CuResult cu_image_create(
    CuImage* pImage,
    uint32_t w,
    uint32_t h,
    CuFormat format,
    CuImageUsageFlags usage,
    CuAllocationMode mode);

/**
 * @brief Destroys the image.
 * @param [in, out] pImage The image.
 * @param mode The allocation mode.
 */
void cu_image_destroy(
    CuImage* pImage,
    CuAllocationMode mode);

/**
 * @brief Creates the sampler.
 * @param [out] pSampler A pointer to the sampler.
 * @return The result of creating the sampler.
 */
CuResult cu_sampler_create(
    CuSampler* pSampler);

/**
 * @brief Destroys the sampler.
 * @param [in, out] pSampler A pointer to the sampler.
 */
void cu_sampler_destroy(
    CuSampler* pSampler);

#ifdef __cplusplus
}
#endif
