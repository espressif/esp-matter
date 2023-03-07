/*
 * Copyright 2018,2019 NXP
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef FSL_SSS_TYPES_H
#define FSL_SSS_TYPES_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#ifdef __STDC__
#include <unistd.h>
#endif

#ifndef FALSE
#define FALSE false
#endif

#ifndef TRUE
#define TRUE true
#endif

#ifndef ARRAY_SIZE
#define ARRAY_SIZE(array) (sizeof(array) / (sizeof(array[0])))
#endif

#define assert_static(e)                    \
    {                                       \
        char assert_static__[(e) ? 1 : -1]; \
        assert_static__;                    \
    }

/** Compile time assert */
#define SSS_ASSERT(condition) assert_static(condition)

/*! @brief Compile time sizeof() check */
#define SSCP_BUILD_ASSURE(condition, msg) assert_static(condition)

/* snprintf definition for MSVisualC */
#ifdef _MSC_VER
#define SNPRINTF _snprintf
#define STRNICMP _strnicmp
#else /* _MSC_VER*/
#define SNPRINTF snprintf
#define STRNICMP strncasecmp
#endif /*_MSC_VER*/

#if defined(USE_RTOS) && USE_RTOS == 1
// #include "FreeRTOS.h"
extern void vPortFree(void *pv);
extern void *pvPortMalloc(size_t xWantedSize);

void *pvPortCalloc(size_t num, size_t size); /*Calloc for Heap3/Heap4.*/

#ifndef SSS_MALLOC
#define SSS_MALLOC pvPortMalloc
#endif // SSS_MALLOC

#ifndef SSS_FREE
#define SSS_FREE vPortFree
#endif // SSS_FREE

#ifndef SSS_CALLOC
#define SSS_CALLOC pvPortCalloc
#endif // SSS_CALLOC

#else // !USE_RTOS

#include <stdlib.h>

#ifndef SSS_MALLOC
#define SSS_MALLOC malloc
#endif // SSS_MALLOC

#ifndef SSS_FREE
#define SSS_FREE free
#endif // SSS_FREE

#ifndef SSS_CALLOC
#define SSS_CALLOC calloc
#endif // SSS_CALLOC

#endif // USE_RTOS

#endif /* FSL_SSS_TYPES_H */
