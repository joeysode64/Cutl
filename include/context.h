#pragma once

#include "result.h"
#include "version.h"

#include <stdint.h>

/** @brief Create info for the context. */
typedef struct {
    const char* appName; /**< The application name. Can be null. Default is null. */

    CuVersion appVersion; /**< The application version. Default is v0.0.0. */
} CuContextCreateInfo;

/** @brief The default context create info. */
constexpr CuContextCreateInfo CU_DEFAULT_CONTEXT_CREATE_INFO = {
    .appName = nullptr,
    .appVersion = {
        .major = 0,
        .minor = 0,
        .patch = 0,
    },
};

/**
 * @brief Creates the context
 *
 * @param pCreateInfo A pointer to the create info, or `nullptr` for defaults.
 * 
 * @return The result of creating the context.
 */
CuResult cu_context_init(const CuContextCreateInfo* pCreateInfo);

/**
 * @brief Terminates the context.
 *
 * @warning All of the Cutl GPU library relies on the context. Call this only after cleanup.
 */
void cu_context_terminate();

/** @brief Waits for the context to idle. */
void cu_context_wait_for_idle();
