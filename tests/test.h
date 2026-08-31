#pragma once

#include "result.h"

#include <stdio.h>
#include <stdlib.h>

/** @brief Fails the test if the expression does not return a success. */
#define query(e)                                                                                   \
    do {                                                                                           \
        const CuResult r = (e);                                                                    \
        if (!cu_is_success(r)) {                                                                   \
            fprintf(                                                                               \
                stderr,                                                                            \
                " Error running `%s` on line #%i: %i\n",                                           \
                #e,                                                                                \
                __LINE__,                                                                          \
                r                                                                                  \
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
