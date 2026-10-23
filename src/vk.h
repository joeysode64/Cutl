#pragma once

#include "allocation.h"
#include "frame.h"
#include "result.h"
#include "util.h"

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <stddef.h>
#include <stdint.h>
#include <vulkan/vulkan.h>

/** @brief Queries the Vulkan result and returns it as a `CuResult` if it's not
 * `VK_SUCCESS`. */
#define vk_try(e)                                                                                  \
    do {                                                                                           \
        const VkResult _result = (e);                                                              \
        if (_result != VK_SUCCESS) {                                                               \
            cu_log_expect(e, _result);                                                             \
            return _result;                                                                        \
        }                                                                                          \
    } while (false)

/** @brief Queries the Vulkan result and returns it if it's not `VK_SUCCESS`. */
#define cu_try_vk(e)                                                                               \
    do {                                                                                           \
        const VkResult _result = (e);                                                              \
        if (_result != VK_SUCCESS) {                                                               \
            cu_log_expect(e, _result);                                                             \
            return cu_vk_error(_result);                                                           \
        }                                                                                          \
    } while (false)

/**
 * @brief Queries the result and stores it in the local `VkResult result`
 * variable. If it is not `VK_SUCCESS`, jumps to `FAIL`.
 */
#define vk_try_catch(e)                                                                            \
    result = (e);                                                                                  \
    if (result != VK_SUCCESS) {                                                                    \
        cu_log_expect(e, result);                                                                  \
        goto FAIL;                                                                                 \
    }

/**
 * @brief Queries the Vulkan result and stores it in the local `CuResult result`
 * variable. If it is not a success value, jumps to `FAIL`.
 */
#define cu_try_catch_vk(e)                                                                         \
    do {                                                                                           \
        const VkResult _result = (e);                                                              \
        result = cu_vk_result(_result);                                                            \
        if (!cu_is_success(result)) {                                                              \
            cu_log_expect(e, _result);                                                             \
            goto FAIL;                                                                             \
        }                                                                                          \
    } while (false)

/**
 * @brief Tries to allocate to the elements and returns
 * `VK_ERROR_OUT_OF_HOST_MEMORY` if it fails.
 */
#define vk_allocate_n(p, n)                                                                        \
    p = (typeof(*(p))*)calloc(n, sizeof(*(p)));                                                    \
    if ((p) == nullptr) {                                                                          \
        return VK_ERROR_OUT_OF_HOST_MEMORY;                                                        \
    }

/**
 * @brief Tries to allocate to the pointer and returns
 * `VK_ERROR_OUT_OF_HOST_MEMORY` if it fails.
 */
#define vk_allocate(p) allocate_n(p, 1)

/** @brief The timeout time. */
constexpr uint64_t TIMEOUT_NANOS = 2'000'000'000;

/** @brief The instance extensions. */
static const char* const INSTANCE_EXTENSIONS[] = {
    "VK_KHR_surface",
    "VK_EXT_headless_surface",
#if ON_APPLE
    "VK_KHR_portability_enumeration",
    "VK_EXT_metal_surface",
#elif ON_LINUX
    "VK_KHR_xcb_surface", "VK_KHR_xlib_surface",     "VK_KHR_wayland_surface",
#elif ON_WINDOWS
    "VK_KHR_win32_surface",
#endif
#if defined(CU_DEBUGGING)
    "VK_EXT_debug_utils",
#endif
};

/** @brief The number of instance extensions. */
constexpr uint32_t N_INSTANCE_EXTENSIONS = arr_len(INSTANCE_EXTENSIONS);

/** @brief The device extensions. */
static const char* const DEVICE_EXTENSIONS[] = {
    "VK_KHR_swapchain",
#if ON_APPLE
    "VK_KHR_portability_subset",
#endif
};

/**@ brief The number of device extensions. */
constexpr uint32_t N_DEVICE_EXTENSIONS = arr_len(DEVICE_EXTENSIONS);

/**
 * @brief Creates the surface.
 * @param [out] pSurface A pointer to the surface handle.
 * @param window The window the surface is for.
 * @return The result of creating the surface.
 */
CuResult create_surface(VkSurfaceKHR* pSurface, GLFWwindow* window);

/**
 * @brief Creates the headless surface.
 * @param [out] pSurface A pointer to the surface handle.
 * @return The result of creating the surface.
 */
CuResult create_headless_surface(VkSurfaceKHR* pSurface);

/** @brief Creates an image. */
CuResult create_image(
    VkImage* pImage,
    uint32_t w,
    uint32_t h,
    VkFormat format,
    VkImageUsageFlags usage);

/** @brief Creates an image view. */
CuResult create_image_view(VkImageView* pImageView, VkImage image, VkFormat format);

/**
 * @brief Allocates the command buffers.
 * @param [out] pCommandBuffers A pointer to the command buffers.
 * @param nCommandBuffers The number of command buffers.
 * @return The result of allocating the command buffers.
 */
CuResult allocate_command_buffers(VkCommandBuffer* pCommandBuffers, size_t nCommandBuffers);

/** @brief Creates a semaphore. */
CuResult create_semaphore(VkSemaphore* pSemaphore, VkSemaphoreType type, uint64_t x);

/**
 * @brief Allocates the memory.
 * @param [out] pMemory A pointer to the memory handle.
 * @param device The device handle.
 * @param size The size of the allocation.
 * @param i The index of the memory type.
 * @return The result of allocationg the memory.
 */
CuResult allocate_memory(VkDeviceMemory* pMemory, VkDevice device, uint64_t size, uint32_t i);

/**
 * @brief Creates the buffer and its allication.
 * @param [out] pBuffer A pointer to the buffer.
 * @param [out] pAllocation A pointer to the allocation.
 * @param [out] ppData A pointer to the mapped memory (ignored if null).
 * @param z The size of the buffer.
 * @param usage A bitmap of the buffer usage flags.
 * @param mode The allocation mode.
 * @param mProperties A bitmap of the memory properties.
 * @return The result of the buffer's creation.
 */
CuResult create_buffer(
    VkBuffer* pBuffer,
    CuAllocation* pAllocation,
    void** ppData,
    size_t z,
    VkBufferUsageFlags usage,
    CuAllocationMode mode,
    VkMemoryPropertyFlags mProperties);

/**
 * @brief Returns the buffer's device address.
 * @param buffer The buffer to get the device address of.
 * @return The buffer's device address.
 */
VkDeviceAddress get_buffer_device_address(VkBuffer buffer);

/** @brief Creates N frames. */
CuResult create_frames(CuFrame* pFrames, size_t nFrames);

/** @brief Destroys N frames. */
void destroy_frames(CuFrame* pFrames, size_t nFrames);
