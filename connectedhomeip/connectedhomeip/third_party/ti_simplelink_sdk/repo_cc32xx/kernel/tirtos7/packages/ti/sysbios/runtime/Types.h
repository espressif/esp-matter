/*
 * Copyright (c) 2020, Texas Instruments Incorporated - http://www.ti.com
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
 *
 */
/*!
 * @file ti/sysbios/runtime/Types.h
 * @brief Basic constants and types
 *
 * This module defines basic constants and types used throughout the
 * SYS/BIOS.
 */

/*
 * ======== Types.h ========
 */

#ifndef ti_sysbios_runtime_Types__include
#define ti_sysbios_runtime_Types__include

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

/* 
 * legacy TI tools pre-define "__TI_COMPILER_VERSION__"
 * TI clang tools pre-define "__ti_version__"
 * we should stop using __ti__, but until then ...
 */
#if defined (__TI_COMPILER_VERSION__) || defined(__ti_version__)
#ifndef __ti__
#define __ti__
#endif
#endif

/*! @cond NODOC */
/* BIOS 6.x compatibility, use -Dxdc_std__include to disable */
#include <xdc/std.h>

#define ti_sysbios_runtime_Types_long_names
#include "Types_defs.h"
/*! @endcond */

#ifdef __cplusplus
extern "C" {
#endif

/*!
 * @brief 64-bit timestamp struct
 *
 * Some platforms only support 32-bit timestamps.  In this case, the most
 * significant 32-bits are always set to 0.
 */
typedef struct {
    /*!
     * @brief most significant 32-bits of timestamp
     */
    uint32_t hi;
    /*!
     * @brief least significant 32-bits of timestamp
     */
    uint32_t lo;
} Types_Timestamp64;

/*!
 * @brief 64-bit frequency struct (in Hz)
 *
 * Some platforms only support 32-bit frequency.  In this case, the most
 * significant 32-bits are always set to 0.
 */
typedef struct {
    /*!
     * @brief most significant 32-bits of frequency
     */
    uint32_t hi;
    /*!
     * @brief least significant 32-bits of frequency
     */
    uint32_t lo;
} Types_FreqHz;

/*! @cond NODOC */
typedef int (*Types_Fxn)();

/* conversion macros (inline functions) */
static inline void * Types_iargToPtr(intptr_t a)
{
    return ((void *)a);
}

static inline void * Types_uargToPtr(uintptr_t a)
{
    return ((void *)a);
}

static inline Types_Fxn Types_iargToFxn(intptr_t a)
{
    return ((Types_Fxn)a);
}

static inline Types_Fxn Types_uargToFxn(uintptr_t a)
{
    return ((Types_Fxn)a);
}

/*
 * functions to efficiently convert a single precision float to an IArg
 * and vice-versa while maintaining client type safety
 *
 * Here the assumption is that sizeof(Float) <= sizeof(IArg);
 */
typedef union {
    float f;
    intptr_t  a;
} Types_FloatData;

static inline intptr_t Types_floatToArg(float f)
{
    Types_FloatData u;
    u.f = f;

    return (u.a);
}

static inline float Types_argToFloat(intptr_t a)
{
    Types_FloatData u;
    u.a = a;

    return (u.f);
}
/*! @endcond */

#ifdef __cplusplus
}
#endif

#endif /* ti_sysbios_runtime_Types__include */

/*! @cond NODOC */
#undef ti_sysbios_runtime_Types_long_names
#include "Types_defs.h"
/*! @endcond */
