#pragma once

#include "result.h"
#include "util.h"
#include "vk.h"

#include <stdint.h>
#include <stdlib.h>
#include <vulkan/vulkan.h>

/// @brief A type for Vulkan enumeration functions.
#define Enumerator(t) struct { uint32_t n; t* p; }

/// @brief Enumerate a Vulkan enumeration function.
/// @param e The enumerator.
/// @param r The value to return if the allocation fails.
/// @param f The enumeration function.
/// @param _ The arguments.
#define enumerate(e, r, f, ...)                                                                    \
    f(__VA_ARGS__, &e.n, nullptr);                                                                 \
    e.p = calloc(e.n, sizeof(e.p[0]));                                                             \
    if (e.p == nullptr) {                                                                          \
        return r;                                                                                  \
    }                                                                                              \
    f(__VA_ARGS__, &e.n, e.p);

/// @brief Enumerate a Vulkan enumeration function.
/// @param e The enumerator.
/// @param f The enumeration function.
/// @param _ The arguments.
/// @note Returns the first `VkResult` that's an error, if any.
#define enumerate_vk(e, f, ...)                                                                    \
    vk_try(f(__VA_ARGS__, &e.n, nullptr));                                                         \
    e.p = calloc(e.n, sizeof(e.p[0]));                                                             \
    if (e.p == nullptr) {                                                                          \
        return VK_ERROR_OUT_OF_HOST_MEMORY;                                                        \
    }                                                                                              \
    vk_try(f(__VA_ARGS__, &e.n, e.p));

/// @brief Enumerate a Vulkan enumeration function.
/// @param e The enumerator.
/// @param f The enumeration function.
/// @param _ The arguments.
/// @note Returns the first `CuResult` that's an error, if any.
#define enumerate_cu(e, f, ...)                                                                    \
    cu_try_vk(f(__VA_ARGS__, &e.n, nullptr));                                                      \
    e.p = calloc(e.n, sizeof(e.p[0]));                                                             \
    if (e.p == nullptr) {                                                                          \
        return CU_ERROR_OUT_OF_RAM;                                                                \
    }                                                                                              \
    cu_try_vk(f(__VA_ARGS__, &e.n, e.p));

/// @brief An attribute for an enumerator to automatically free it.
#define ENUMERATOR_AUTO_FREE __attribute__((cleanup(enumerator_auto_free_)))

/// @brief The `AUTO_FREE` callback.
static inline void enumerator_auto_free_(
    void* pEnumerator)
{
    auto e = *(Enumerator(void)*)pEnumerator;
    free(e.p);
}
