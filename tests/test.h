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

/** @brief Fails the test if the expression does not return a success. */
#define query(e)                                                                                   \
    do {                                                                                           \
        const CuResult _r = (e);                                                                   \
        if (!cu_is_success(_r)) {                                                                  \
            fprintf(                                                                               \
                stderr,                                                                            \
                " " ANSI_RED "Failed:" ANSI_RESET " running `" #e "` on line #%i: %u:%i\n",        \
                __LINE__,                                                                          \
                _r.t,                                                                              \
                _r.v                                                                               \
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

/** @brief Prints a success message and exits with a success code. */
#define success()                                                                                  \
    fprintf(stderr, " Test passed!\n");                                                            \
    exit(EXIT_SUCCESS);
