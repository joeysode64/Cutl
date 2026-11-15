#pragma once

#include <stdint.h>
#include <vulkan/vulkan.h>

/** @brief A version. */
typedef struct CuVersion_T {
    /** @brief The version major. */
    uint16_t major;

    /** @brief The version minor. */
    uint16_t minor;

    /** @brief The version patch. */
    uint16_t patch;

    /** @brief The version tweak. */
    uint16_t tweak;
} CuVersion;

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Returns the version as a `uint32_t` Vulkan-style version.
 * @param[in] pVersion A pointer to the version. Must not be null.
 * @note This does not use the version's tweak value.
 */
static inline uint32_t cu_version_vk_version(const CuVersion* pVersion)
{
    return VK_MAKE_API_VERSION(0, pVersion->major, pVersion->minor, pVersion->patch);
}

#ifdef __cplusplus
}
#endif
