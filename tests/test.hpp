#pragma once

#include "context.hpp"
#include "result.hpp"

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

/** @brief Fails the test if the expression does not return a success. */
#define query(e)                                                                                   \
    do {                                                                                           \
        const cu::Result _r = (e);                                                                 \
        if (!_r.is_success()) {                                                                    \
            fprintf(                                                                               \
                stderr,                                                                            \
                " " ANSI_RED "Failed:" ANSI_RESET " running `" #e "` on line #%i: %u:%i\n",        \
                __LINE__,                                                                          \
                static_cast<uint32_t>(_r.type()),                                                  \
                _r.value()                                                                         \
            );                                                                                     \
            exit(EXIT_FAILURE);                                                                    \
        }                                                                                          \
    } while (false)

/** @brief Prints the message and fails the test if the expression is not true. */
#define expect(e, ...)                                                                             \
    if (!(e)) {                                                                                    \
        fprintf(stderr, __VA_ARGS__);                                                              \
        exit(EXIT_FAILURE);                                                                        \
    }

/** @brief Fails the test and prints the message. */
#define fail(...)                                                                                  \
    fprintf(stderr, __VA_ARGS__);                                                                  \
    exit(EXIT_FAILURE);                                                                            \

/**
 * @brief Prints a success message and exits with a success code.
 * @note Must be called from the main function.
 */
#define success()                                                                                  \
    atexit([] { fprintf(stderr, " Test passed!\n"); });                                            \
    return EXIT_SUCCESS;
