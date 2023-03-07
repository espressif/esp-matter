/*
 * Copyright (c) 2022, Arm Limited and Contributors. All rights reserved.
 * SPDX-License-Identifier: Apache-2.0
 */
#ifndef ARCH_CC_H_
#define ARCH_CC_H_

#include "cmsis_os2.h"
#include "lwip/debug.h"

#include <inttypes.h>
#include <stdio.h>
#include <stdlib.h>

// Add "extern C" guard for CMSIS file which allows C++ compiling
#if __cplusplus
extern "C" {
#endif
#include "cmsis_compiler.h"
#if __cplusplus
}
#endif

#if (!defined(__CC_ARM)) && (!defined(__ICCARM__)) && (!defined(__ARMCC_VERSION))
#define LWIP_TIMEVAL_PRIVATE 0
#include <sys/time.h>
#endif

#if defined(__ICCARM__)
#define PACK_STRUCT_STRUCT __packed
#else
#define PACK_STRUCT_STRUCT __attribute__((packed))
#endif

#ifndef BYTE_ORDER
#define BYTE_ORDER LITTLE_ENDIAN
#endif /* BYTE_ORDER */

/* Define generic types used in lwIP */
typedef uint8_t u8_t;
typedef int8_t s8_t;
typedef uint16_t u16_t;
typedef int16_t s16_t;
typedef uint32_t u32_t;
typedef int32_t s32_t;

typedef uintptr_t mem_ptr_t;
typedef s32_t sys_prot_t;

/* Define (sn)printf formatters for these lwIP types */
#define X8_F  "02" PRIx8
#define U16_F PRIu16
#define S16_F PRId16
#define X16_F PRIx16
#define U32_F PRIu32
#define S32_F PRId32
#define X32_F PRIx32
#define SZT_F PRIuPTR

#ifndef DEBUG_PRINT
#define DEBUG_PRINT(...)
#else
int DEBUG_PRINT(const char *fmt, ...);
#endif

#ifndef LWIP_PLATFORM_DIAG
#define LWIP_PLATFORM_DIAG(x) \
    do {                      \
        DEBUG_PRINT x;        \
    } while (0)
#endif

#ifndef LWIP_PLATFORM_ASSERT
#define LWIP_PLATFORM_ASSERT(x)                                                           \
    do {                                                                                  \
        __disable_irq();                                                                  \
        DEBUG_PRINT("Assertion \"%s\" failed at line %d in %s\n", x, __LINE__, __FILE__); \
        while (1) {                                                                       \
        }                                                                                 \
    } while (0)
#endif

#ifndef LWIP_ERROR
#define LWIP_ERROR(message, expression, handler)                                                    \
    do {                                                                                            \
        if (!(expression)) {                                                                        \
            DEBUG_PRINT("Assertion \"%s\" failed at line %d in %s\n", message, __LINE__, __FILE__); \
            handler;                                                                                \
        }                                                                                           \
    } while (0)
#endif

#define LWIP_RAND() ((u32_t)rand())

#endif /* ARCH_CC_H_ */
