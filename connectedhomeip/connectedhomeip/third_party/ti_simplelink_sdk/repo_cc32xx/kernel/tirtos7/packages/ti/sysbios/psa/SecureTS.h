/*
 * Copyright (c) 2019-2020 Texas Instruments Incorporated - http://www.ti.com
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
 *  ======== SecureTS.h ========
 */

#ifndef ti_spe_SecureTS__include
#define ti_spe_SecureTS__include

#include <stddef.h>
#include <stdint.h>

#if defined(__GNUC__)
#ifndef __STATIC_INLINE
#define __STATIC_INLINE static inline
#endif
#elif defined(__clang__) && defined(__ti__)
#define __STATIC_INLINE static __inline
#endif

#if defined (__cplusplus)
extern "C" {
#endif

#define HWREG(x)                                                              \
        (*((volatile unsigned long *)(x)))

#define AON_RTC_BASE            0x40092000 // AON_RTC

// Second Counter Value, Integer Part
#define AON_RTC_O_SEC                                               0x00000008

// Second Counter Value, Fractional Part
#define AON_RTC_O_SUBSEC                                            0x0000000C


/*
 * ======== AONRTCCurrent64BitValueGet ========
 * Implementation copied from driverlib to avoid secure image
 * dependency on driverlib.
 */

__STATIC_INLINE uint64_t
AONRTCCurrent64BitValueGet( void )
{
    union {
        uint64_t  returnValue       ;
        uint32_t  secAndSubSec[ 2 ] ;
    } currentRtc                    ;
    uint32_t      ui32SecondSecRead ;

    // Reading SEC both before and after SUBSEC in order to detect if SEC incremented while reading SUBSEC
    // If SEC incremented, we can't be sure which SEC the SUBSEC belongs to, so repeating the sequence then.
    do {
        currentRtc.secAndSubSec[ 1 ] = HWREG( AON_RTC_BASE + AON_RTC_O_SEC    );
        currentRtc.secAndSubSec[ 0 ] = HWREG( AON_RTC_BASE + AON_RTC_O_SUBSEC );
        ui32SecondSecRead            = HWREG( AON_RTC_BASE + AON_RTC_O_SEC    );
    } while ( currentRtc.secAndSubSec[ 1 ] != ui32SecondSecRead );

    return ( currentRtc.returnValue );
}

/*
 *  ======== SecureTS_get32 ========
 *  returns the LS 32 bits of the 48 bit SEC + SUBSEC registers
 *
 *  This API is available to both non-secure and secure code
 */
__attribute__ ((always_inline)) __STATIC_INLINE
uint32_t SecureTS_get32()
{
    return (AONRTCCurrent64BitValueGet() >> 16);
}

/*
 *  ======== SecureTS_get ========
 *  returns a 48 bit coherent concatenation of the current
 *  SEC and SUBSEC register values from the SYSTEM RTC
 *
 *  This API is available to both non-secure and secure code
 */
__attribute__ ((always_inline)) __STATIC_INLINE
uint64_t SecureTS_get()
{
    return (AONRTCCurrent64BitValueGet() >> 16);
}

#if defined (__cplusplus)
}
#endif

#endif /* ti_spe_SecureTS__include */

