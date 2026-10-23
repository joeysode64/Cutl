#pragma once

#include "allocation.h"
#include "result.h"

#include <png.h>
#include <stdint.h>
#include <vulkan/vulkan.h>

/** @brief An image. */
typedef struct CuImage_T {
    VkImage _image; /**< The image handle. */

    VkImageView _imageView; /**< The image view handle. */

    CuAllocation _allocation; /**< The allocation. */
} CuImage;

/** @brief An image sampler. */
typedef struct CuSampler_T {
    VkSampler _sampler; /**< The sampler handle. */
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
    VkFormat format,
    VkImageUsageFlags usage,
    CuAllocationMode mode);

/**
 * @brief Destroys the image.
 * @param [in, out] pImage The image.
 * @param mode The allocation mode.
 */
void cu_image_destroy(CuImage* pImage, CuAllocationMode mode);

/**
 * @brief Creates the sampler.
 * @param [out] pSampler A pointer to the sampler.
 * @return The result of creating the sampler.
 */
CuResult cu_sampler_create(CuSampler* pSampler);

/**
 * @brief Destroys the sampler.
 * @param [in, out] pSampler A pointer to the sampler.
 */
void cu_sampler_destroy(CuSampler* pSampler);

/**
 * @brief Loads the PNG image from the file.
 * @param [out] pData A pointer to the buffer to write the image's data to.
 * Ignored if null.
 * @param [out] pW A pointer to the image's width. Ignored if null.
 * @param [out] pH A pointer to the image's height. Ignored if null.
 * @param path The path to the PNG image file.
 * @param format The `PNG_FORMAT_*` to convert the image to. Ignored if 0.
 * @return The result of reading the image.
 * @note The buffer must hold `w * h` pixels of the format. Call with a null
 * `pData` first to get the size.
 */
CuResult cu_png_load(void* pData, uint32_t* pW, uint32_t* pH, const char* path, png_uint_32 format);

#ifdef __cplusplus
}
#endif
