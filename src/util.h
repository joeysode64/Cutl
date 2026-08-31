#pragma once

#include "result.h"
#include <stdlib.h>

#if defined(__APPLE__)

#define ON_APPLE true
#define ON_LINUX false
#define ON_WINDOWS false

#elif defined(__linux__)

#define ON_APPLE false
#define ON_LINUX true
#define ON_WINDOWS false

#elif defined(_WIN32)

#define ON_APPLE false
#define ON_LINUX false
#define ON_WINDOWS true

#else

#error "Unsupported OS"

#endif

/** @brief Returns the length of the array. */
#define arr_len(a) (sizeof(a) / (sizeof(a[0])))

/** @brief Returns whether the two values have any matching 1-bits. */
#define ones_overlap(a, b) (((a) & (b)) != 0)

/** @brief Returns whether the first value contains all the bits of the second value. */
#define ones_match(a, b) (((a) & (b)) == (b))

/** @brief Tries to allocate to the elements and returns `CU_ERROR_BAD_ALLOC` if it fails. */
#define allocate_n(p, n)                                                                           \
    p = calloc(n, sizeof(*p));                                                                     \
    if (p == nullptr) {                                                                            \
        return CU_ERROR_OUT_OF_RAM;                                                                \
    }

/** @brief Tries to allocate to the pointer and returns `CU_ERROR_BAD_ALLOC` if it fails. */
#define allocate(p) allocate_n(p, 1)

/**
 * @brief Queries the result and stores it in the local `CuResult result` variable. If it is not a
 * success value, jumps to `FAIL`.
 */
#define cu_try(e)                                                                                  \
    result = (e);                                                                                  \
    if (!cu_is_success(result)) {                                                                  \
        goto FAIL;                                                                                 \
    }

/** @brief An attribute to automatically call `free` on a variable. */
#define AUTO_FREE __attribute__((cleanup(auto_free_cb)))

/** @brief The `AUTO_FREE` callback. */
static inline void auto_free_cb(void* pp) {
    void* const p = *(void**)pp;
    free(p);
}
