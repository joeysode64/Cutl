#pragma once

#include <stdio.h>

/** @brief ANSI escape code for red text. */
#define ANSI_RED "\x1b[31m"

/** @brief ANSI escape code for green text. */
#define ANSI_GREEN "\x1b[32m"

/** @brief ANSI escape code for yellow text. */
#define ANSI_YELLOW "\x1b[33m"

/** @brief ANSI escape code for blue text. */
#define ANSI_BLUE "\x1b[34m"

/** @brief ANSI escape code for gray text. */
#define ANSI_GRAY "\x1b[90m"

/** @brief ANSI escape code to reset. */
#define ANSI_RESET "\x1b[0m"

#if CU_DEBUGGING
    #define dbg_log(fmt, ...) fprintf(stderr, fmt "\n" __VA_OPT__(, ) __VA_ARGS__)
#else
    #if CU_DEBUGGING
        #define CU_DEBUGGING 0
    #endif
    #define dbg_log(fmt, ...)
#endif

/**
 * @brief Logs a note.
 * @param msg The note.
 */
#define dbg_log_note(msg, ...) dbg_log(ANSI_BLUE "Note:" ANSI_RESET " " msg, __VA_ARGS__)

/**
 * @brief Logs the creation of a Vulkan object.
 * @param x The Vulkan object's handle.
 */
#define dbg_log_create(x) dbg_log(ANSI_GREEN "Created" ANSI_RESET " `" #x "`: %p", (void*)(x))

/**
 * @brief Logs the destruction of a Vulkan object.
 * @param x The Vulkan object's handle.
 */
#define dbg_log_destroy(x) dbg_log(ANSI_GRAY "Destroyed" ANSI_RESET " `" #x "`: %p", (void*)(x))

/**
 * @brief Logs a failed operation.
 * @param r The result.
 */
#define dbg_log_fail(r)                                                                            \
    dbg_log(ANSI_RED "Error:" ANSI_RESET " `%s` failed (%u:%i)", __func__, (r).t, (r).v)

/**
 * @brief Logs an unexpected value.
 * @param e The expression.
 * @param v The value it returned.
 */
#define dbg_log_expect(e, v)                                                                       \
    dbg_log(ANSI_RED "Error:" ANSI_RESET " `" #e "` returned an unexpected value: %i", (v))

/**
 * @brief Logs a warning.
 * @param msg The message.
 */
#define dbg_log_warning(msg) dbg_log(ANSI_YELLOW "Warning:" ANSI_RESET " " msg)
