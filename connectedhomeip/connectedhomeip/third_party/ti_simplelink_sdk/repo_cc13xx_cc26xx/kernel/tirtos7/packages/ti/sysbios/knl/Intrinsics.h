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

/*
 * ======== Intrinsics.h ========
 */

#ifndef ti_sysbios_knl_Intrinsics__include
#define ti_sysbios_knl_Intrinsics__include

#include <ti/sysbios/runtime/Types.h>

#define ti_sysbios_knl_Intrinsics_long_names
#include "Intrinsics_defs.h"

#ifdef __cplusplus
extern "C" {
#endif

#if (defined(__ARM_ARCH) && ((__ARM_ARCH == 6) && (__ARM_ARCH_PROFILE == 'M'))) || \
    (defined(__CORE__) && (__CORE__ == __ARM6M__)) || \
    defined(__ARM_ARCH_6M__)
/*
 *  ======== Intrinsics_maxbit ========
 */
static inline unsigned int Intrinsics_maxbit(unsigned int bits)
{
    unsigned int maxbit = 0;

    if (bits >> 16) {
        bits >>= 16;
        maxbit += 16;
    }

    if (bits >> 8) {
        bits >>= 8;
        maxbit += 8;
    }

    if (bits >> 4) {
        bits >>= 4;
        maxbit += 4;
    }

    if (bits >> 2) {
        bits >>= 2;
        maxbit += 2;
    }

    if (bits >> 1) {
        maxbit += 1;
    }

    return (maxbit);
}

#elif defined(__ti__) && !defined(__clang__)
extern int _norm(int val);
static inline unsigned int Intrinsics_maxbit(unsigned int bits)
{
    return ((unsigned int)(int)(31 - _norm((int)(bits))));
}
#else
static inline unsigned int Intrinsics_maxbit(unsigned int bits)
{
    unsigned int retVal;

#if defined(__IAR_SYSTEMS_ICC__)
    asm volatile (
#else
    __asm__ __volatile__ (
#endif
            "clz %0, %1\n\t"
            "rsb %0, %0, #31"
            : "=r" (retVal)
            : "r" (bits)
            : "cc"
            );
    return retVal;
}
#endif

#ifdef __cplusplus
}
#endif

#endif /* ti_sysbios_knl_Intrinsics__include */

#undef ti_sysbios_knl_Intrinsics_long_names
#include "Intrinsics_defs.h"
