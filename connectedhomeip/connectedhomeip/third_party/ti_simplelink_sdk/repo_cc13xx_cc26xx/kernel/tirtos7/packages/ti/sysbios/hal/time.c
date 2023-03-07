/*
 * Copyright (c) 2014-2020, Texas Instruments Incorporated
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
 * ====== time.c ========
 */

#if Seconds_generateTimeFunction_D

#if defined(__ti__) && !defined(__clang__)

#pragma FUNC_EXT_CALLED(time);

#define ATTRIBUTE

#elif defined(__IAR_SYSTEMS_ICC__)

#define ATTRIBUTE

#else

#define ATTRIBUTE __attribute__ ((used))

#endif

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#include <ti/sysbios/hal/Seconds.h>

#include <ti/sysbios/runtime/Types.h>

#include <time.h>

#if defined(__clang__) && defined(__TI_TIME_USES_64) && __TI_TIME_USES_64

_CODE_ACCESS __time64_t __time64(__time64_t *_timer)
{
    uint64_t t;
    Seconds_Time ts;

    Seconds_getTime(&ts);

    t = ((uint64_t)ts.secsHi << 32) + ts.secs;
    if (_timer != NULL) {
        *_timer = t;
    }

    return (t);
}

#else
/*
 *  ======== time ========
 */
time_t ATTRIBUTE time(time_t *tout)
{
    uint32_t t;

    /* Seconds_get() returns number of seconds since Jan 1, 1970 00:00:00 GMT. */
    t = Seconds_get();

#if defined(__ti__)
    /*
     *  TI time() function returns seconds since 1900, so add the number
     *  of seconds from 1900 to 1970 (2208988800).
     */
    t += 2208988800;
#endif

    if (tout) {
        *tout = t;
    }

    return (t);
}
#endif

#endif /* Seconds_generateTimeFunction_D */
