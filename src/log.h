#pragma once

#include <stdio.h>

#if defined(CU_DEBUGGING)
    #define cu_log(fmt, ...) fprintf(stderr, fmt "\n" __VA_OPT__(,) __VA_ARGS__)
#else
    #define cu_log(fmt, ...)
#endif

/**
 * @brief Logs the creation of a Vulkan object.
 * @param x The Vulkan object's handle.
 */
#define cu_log_create(x) cu_log("Created `" #x "`: %p", (void*)(x))

/**
 * @brief Logs the destruction of a Vulkan object.
 * @param x The Vulkan object's handle.
 */
#define cu_log_destroy(x) cu_log("Destroyed `" #x "`: %p", (void*)(x))

/**
 * @brief Logs a failed operation.
 * @param msg A message describing what failed.
 * @param r The result.
 */
#define cu_log_fail(msg, r) cu_log(msg ": %u:%i", (r).t, (r).v)
