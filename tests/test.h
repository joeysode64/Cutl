#pragma once

#include "result.h"

#include <stdio.h>
#include <stdlib.h>

/** @brief ANSI escape code for red text. */
#define ANSI_RED "\x1b[31m"

/** @brief ANSI escape code for green text. */
#define ANSI_GREEN "\x1b[32m"

/** @brief ANSI escape code to reset. */
#define ANSI_RESET "\x1b[0m"

/** @brief The window width. */
#define WIN_W 800

/** @brief The test window height. */
#define WIN_H 450

/**
 * @brief Fails the test and prints the message.
 * @param msg The error message to print.
 * @warning Must be called from the main function.
 */
#define fail(msg, ...)                                                                             \
    fprintf(stderr, " " ANSI_RED "Failure:" ANSI_RESET " " msg __VA_OPT__(,) __VA_ARGS__);         \
    return EXIT_FAILURE;

/** @brief The success messgae. */
#define SUCCESS_MSG " " ANSI_GREEN "Test passed!" ANSI_RESET

#if defined(__cplusplus)
    /**
    * @brief Prints a success message and returns a success code.
    * @warning Must be called from the main function.
    */
    #define success()                                                                              \
        atexit([]{ fprintf(stderr, SUCCESS_MSG "\n"); });                                          \
        return EXIT_SUCCESS;
#else
    /**
    * @brief Prints a success message and returns a success code.
    * @warning Must be called from the main function.
    */
    #define success()                                                                              \
        fprintf(stderr, SUCCESS_MSG "\n");                                                         \
        return EXIT_SUCCESS;
#endif

/**
 * @brief Queries the expression and fails if it returns an error.
 * @param e The expression to query.
 * @warning Must be called from the main function.
 */
#define query(e)                                                                                   \
    do {                                                                                           \
        const CuResult _r = (e);                                                                   \
        if (!cu_is_success(_r)) {                                                                  \
            fail(                                                                                  \
                "Error running `" #e "` on line #%i: %u:%i\n",                                     \
                __LINE__,                                                                          \
                _r.t,                                                                              \
                _r.v                                                                               \
            );                                                                                     \
        }                                                                                          \
    } while (false)

/** 
 * @brief Prints the message and fails the test if the expression is not true.
 * @param c The condition to query.
 * @param msg The message to print.
 * @warning Must be called from the main function.
 */
#define expect(c, msg, ...)                                                                        \
    if (!(c)) {                                                                                    \
        fail(msg __VA_OPT__(,) __VA_ARGS__);                                                       \
    }
