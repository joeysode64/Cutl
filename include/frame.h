#pragma once

#include "inner.h"

/** @brief A renderer frame. */
typedef struct CuFrame_T {
    cu_vk_dispatch_t(CommandBuffer) _commandBuffer; /**< The command buffer handle. */

    cu_vk_t(Semaphore) _imageAvailable; /**< The "image available" semaphore handle. */
} CuFrame;
