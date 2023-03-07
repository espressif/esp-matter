/*
 * Copyright (c) 2019 Texas Instruments Incorporated - http://www.ti.com
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * *  Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *
 * *  Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * *  Neither the name of Texas Instruments Incorporated nor the names of
 *    its contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 * THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 * OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 * OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

/*
 *  ======== Bench.h ========
 *  Benchmark instrumentation
 *
 *  To enable the bench macros, you must define the following symbol
 *  to one of the following values before including this header file.
 *  Otherwise, all bench  statements will be removed.
 *
 *  Bench_DEVICE = 0x1001
 */

#ifndef ti_utils_runtime_Bench__include
#define ti_utils_runtime_Bench__include

#include <stddef.h>
#include <stdbool.h>
#include <stdint.h>

#if defined(__cplusplus)
extern "C" {
#endif

#define Bench_ARM       0x1001

#if defined(__GNUC__)
#ifndef __STATIC_INLINE
#define __STATIC_INLINE static inline
#endif
#elif defined(__clang__) && defined(__ti__)
#define __STATIC_INLINE static __inline
#endif

/*
 *  ======== Bench_Instance ========
 */
typedef struct Bench_Instance {
    uint32_t   *next;           /* next record to write */
    uint32_t   *buffer;         /* beginning of buffer */
    uint32_t   *limit;          /* end of buffer */
    bool        proxy;          /* data generated on remote code */
    char       *label;          /* determines computation sequence */
} Bench_Instance;

/*
 *  ======== Bench_Handle ========
 */
typedef Bench_Instance *Bench_Handle;

/*
 *  ======== Bench_State ========
 */
typedef struct Bench_State {
    int         numEntries;
    uint32_t    timestampHz;
} Bench_State;

/*
 *  ======== Bench_count ========
 *  The number of instances (i.e. mark points)
 */
extern int Bench_count;

/*
 *  ======== Bench_state ========
 *  Module state object
 */
extern Bench_State Bench_state;


#if defined(Bench_DEVICE)

/*
 *  ======== Bench_decl ========
 *  Declare bench instance handle
 */
#define Bench_decl(hndl) extern Bench_Handle hndl

#if Bench_DEVICE == Bench_ARM

/* Core Debug Module - Debug Exception and Monitor Control Register */
#define CoreDebug_DEMCR ((volatile uint32_t *)0xE000EDFC)

/* Data Watchpoint and Trace module */
#define DWT_CTRL        ((volatile uint32_t *)0xE0001000)
#define DWT_CYCCNT      ((volatile uint32_t *)0xE0001004)

/*
 *  ======== Bench_init ========
 *  Start the DWT cycle counter running
 */
__attribute__((always_inline)) __STATIC_INLINE
void Bench_init(void)
{
    /* enable the Data Watchpoint and Trace (DWT) module */
    *CoreDebug_DEMCR |= (1 << 24);

    /* ensure the counter is stopped */
    *DWT_CTRL = 0;

    /* reset the cycle counter */
    *DWT_CYCCNT = 0;
}

/*
 *  ======== Bench_start ========
 *  Start the DWT cycle counter running
 */
__attribute__((always_inline)) __STATIC_INLINE
void Bench_start(void)
{
    *DWT_CTRL = 1;
}

/*
 *  ======== Bench_stop ========
 *  Stop the DWT cycle counter running
 */
__attribute__((always_inline)) __STATIC_INLINE
void Bench_stop(void)
{
    *DWT_CTRL = 0;
}

/*
 *  ======== Bench_logTS ========
 *  Latch the current cycle counter register value into the buffer
 */
__attribute__((always_inline)) __STATIC_INLINE
void Bench_logTS(Bench_Handle hndl)
{
    if ((hndl->next != NULL) && (hndl->next < hndl->limit)) {
        *(hndl->next++) = *DWT_CYCCNT;
    }
}

/*
 *  ======== Bench_reset ========
 *  Reset a bench instance
 */
__attribute__((always_inline)) __STATIC_INLINE
void Bench_reset(Bench_Handle hndl)
{
    if (hndl->next != NULL) {
        hndl->next = hndl->buffer;
    }
}

#if 0
__attribute__((always_inline)) __STATIC_INLINE
void Bench_logTS(Bench_Handle hndl)
{
    if ((hndl->next != NULL) && (hndl->next < hndl->limit)) {
        *(hndl->next++) = SYSRTC->STAT.UTIME;
    }
}
#endif

#if 0
#include <ti/sysbios/family/arm/v8m/TimestampProvider.h>

__attribute__((always_inline)) __STATIC_INLINE
void Bench_logTS(Bench_Handle hndl)
{
    if ((hndl != NULL) && (hndl->next < hndl->limit)) {
//      *(hndl->next++) = *(uint32_t *)0xE000E018;
        *(hndl->next++) = TimestampProvider_get32();
    }
}
#endif

/*
 *  ======== Bench_disable ========
 *  Disable the given bench instance
 */
__attribute__((always_inline)) __STATIC_INLINE
uint32_t *Bench_disable(Bench_Handle hndl)
{
    uint32_t *next = hndl->next;

    hndl->next = NULL;
    return (next);
}

/*
 *  ======== Bench_restore ========
 *  Re-enable the given bench instance
 */
__attribute__((always_inline)) __STATIC_INLINE
void Bench_restore(Bench_Handle hndl, uint32_t *next)
{
    hndl->next = next;
}

#else
#error Unsupported Bench_DEVICE
#endif

#else

/* remove all module content from the compilation unit */
#define Bench_decl(hndl)
#define Bench_init()
#define Bench_logTS(hndl)
#define Bench_start()
#define Bench_stop()
#define Bench_disable(hndl)
#define Bench_restore(hndl, key)

#endif

#if defined(__cplusplus)
}
#endif

#endif /* ti_utils_runtime_Bench__include */
