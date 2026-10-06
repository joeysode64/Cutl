#pragma once

#include "result.h"

#include <stdio.h>
#include <stdlib.h>

/** @brief Fails the test if the expression does not return a success. */
#define query(e)                                                                                   \
    do {                                                                                           \
        const cu::Result _r = (e);                                                                 \
        if (!_r.is_success()) {                                                                    \
            fprintf(                                                                               \
                stderr,                                                                            \
                " Error running `%s` on line #%i: %u:%i\n",                                        \
                #e,                                                                                \
                __LINE__,                                                                          \
                _r.type(),                                                                         \
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
    fprintf(stderr, " Test passed!\n");                                                            \
    return EXIT_SUCCESS;
