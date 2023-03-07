/*
 * Copyright (c) 2022, Arm Limited and Contributors. All rights reserved.
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef COMMON_DEBUG_H
#define COMMON_DEBUG_H

#include <stdio.h>

#if ((defined(__ARM_ARCH_6M__)) || (defined(__ARM_ARCH_7M__)) || (defined(__ARM_ARCH_7EM__)) \
     || (defined(__ARM_ARCH_8M_BASE__)) || (defined(__ARM_ARCH_8M_MAIN__)) || (defined(__ARM_ARCH_8_1M_MAIN__)))

#define COMMON_CORTEX_M_ARCH 1
static void __disable_irq(void)
{
    size_t result;
    __asm volatile("mrs %0, primask" : "=r"(result));
    __asm volatile("cpsid i");
}

#else

static void __disable_irq(void)
{
    /* Do nothing on non-Arm platforms */
}

#endif

#ifndef DEBUG_PRINT
#if COMMON_CORTEX_M_ARCH
/* No printing currently available on Cortex-M */
#define DEBUG_PRINT(...)
#else
#define DEBUG_PRINT(...) fprintf(stderr, __VA_ARGS__);
#endif
#endif

#define COMMON_ASSERT(e, x)                                                                   \
    do {                                                                                      \
        if (!(e)) {                                                                           \
            (void)("LCOV_EXCL_START");                                                        \
            __disable_irq();                                                                  \
            DEBUG_PRINT("Assertion \"%s\" failed at line %d in %s\n", x, __LINE__, __FILE__); \
            for (;;)                                                                          \
                ;                                                                             \
            (void)("LCOV_EXCL_STOP");                                                         \
        }                                                                                     \
    } while (0)
#endif // COMMON_DEBUG_H
