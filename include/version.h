#pragma once

#include <stdint.h>
#include <vulkan/vulkan.h>

/** @brief A version. */
typedef struct CuVersion_T {
    uint16_t major; /**< The version major. */

    uint16_t minor; /**< The version minor */

    uint16_t patch; /**< The version patch. */

    uint16_t tweak; /**< The version tweak. */
} CuVersion;

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Returns the version as a `uint32_t` Vulkan-style version.
 * @param [in] pVersion A pointer to the version.
 * @return The version as a `uint32_t` Vulkan-style version.
 * @note This does not use the version's tweak value.
 */
static inline uint32_t cu_version_vk_version(const CuVersion* pVersion) {
    return VK_MAKE_API_VERSION(0, pVersion->major, pVersion->minor, pVersion->patch);
}

#ifdef __cplusplus
}
#endif
