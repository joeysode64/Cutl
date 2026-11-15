#pragma once

#include "allocation.h"
#include "result.h"

#include <png.h>
#include <stdint.h>
#include <vulkan/vulkan.h>

/** @brief An image. */
typedef struct CuImage_T {
    /** @brief The image handle. */
    VkImage _image;

    /** @brief The image view handle. */
    VkImageView _imageView;

    /** @brief The image's memory allocation. */
    CuAllocation _allocation;
} CuImage;

/** @brief An image sampler. */
typedef struct CuSampler_T {
    /** @brief The sampler handle. */
    VkSampler _sampler;
} CuSampler;

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Creates the image.
 * @param[out] pImage A pointer to the image. Must not be null.
 * @param w The image's width in pixels. Must not be 0.
 * @param h The image's height in pixels. Must not be 0.
 * @param format The image's format.
 * @param usage A bitmap of how the image will be used. Must not be 0.
 * @param mode The allocation mode to allocate the image's memory with.
 */
CU_NODISCARD_RESULT
CuResult cu_image_create(
    CuImage* pImage,
    uint32_t w,
    uint32_t h,
    VkFormat format,
    VkImageUsageFlags usage,
    CuAllocationMode mode);

/**
 * @brief Destroys the image.
 * @param[in out] pImage A pointer to the image. Must not be null.
 * @param mode The allocation mode the image was created with.
 */
void cu_image_destroy(CuImage* pImage, CuAllocationMode mode);

/**
 * @brief Starts rendering to the image.
 * @param[in] pImage A pointer to the image. Must not be null.
 * @param commandBuffer The command buffer to record to. Must be in the recording state.
 * @param w The width of the area to render to in pixels.
 * @param h The height of the area to render to in pixels.
 * @note The image's layout must be `VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL`.
 */
void cu_image_start_rendering(
    const CuImage* pImage,
    VkCommandBuffer commandBuffer,
    uint32_t w,
    uint32_t h);

/**
 * @brief Ends the frame's current image render.
 * @param[in] pImage A pointer to the image. Must not be null.
 * @param commandBuffer The command buffer to record to. Must be rendering to the image.
 * @param newLayout The layout to leave the image in.
 */
void cu_image_end_rendering(
    const CuImage* pImage,
    VkCommandBuffer commandBuffer,
    VkImageLayout newLayout);

/**
 * @brief Creates the sampler.
 * @param[out] pSampler A pointer to the sampler. Must not be null.
 */
CU_NODISCARD_RESULT
CuResult cu_sampler_create(CuSampler* pSampler);

/**
 * @brief Destroys the sampler.
 * @param[in out] pSampler A pointer to the sampler. Must not be null.
 */
void cu_sampler_destroy(CuSampler* pSampler);

/**
 * @brief Loads the PNG image from the file.
 * @param[out] ppData A pointer to the allocated image data. Ignored if null.
 * @param[out] pW A pointer to the image's width. Ignored if null.
 * @param[out] pH A pointer to the image's height. Ignored if null.
 * @param path The path to the PNG image file. Must not be null.
 * @param format The `PNG_FORMAT_*` to convert the image to. Ignored if 0.
 * @warning If `ppData` is set, it must be freed yourself with `free`.
 */
CU_NODISCARD_RESULT
CuResult cu_png_load(
    void** ppData,
    uint32_t* pW,
    uint32_t* pH,
    const char* path,
    png_uint_32 format);

#ifdef __cplusplus
}
#endif
