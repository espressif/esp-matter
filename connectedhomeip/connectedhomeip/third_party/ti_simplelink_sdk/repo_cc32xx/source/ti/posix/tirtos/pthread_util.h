/*
 * Copyright (c) 2017-2021 Texas Instruments Incorporated - http://www.ti.com
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
 *  ======== pthread_util.h ========
 */

#ifndef ti_posix_ccs_pthread_util__include
#define ti_posix_ccs_pthread_util__include

#include <stdint.h>

#include "time.h"

/*  CLOCK_REALTIME represents the realtime clock for the system. For this
 *  clock, clock_gettime() returns the time since the beginning of the
 *  Epoch. SYS/BIOS implements clock_gettime() and clock_settime() with
 *  the ti.sysbios.hal.Seconds module for CLOCK_REALTIME.
 *
 *  For SYS/BIOS, CLOCK_MONOTONIC represents the ti.sysbios.knl.Clock
 *  system clock.
 *
 *  clock_gettime(CLOCK_MONOTONIC) will keep track of Clock tick rollovers
 *  to ensure that it is monotonic. The tick count is 32 bits, so at a
 *  1 msec tick rate, the count rolls over every 4294967296 / 1000 seconds,
 *  or about every 50 days.
 *
 *  To detect rollover, we'll save the current tick count in clock_gettime()
 *  to prevTicks. If the current tick count is less than prevTicks, the
 *  tick count has rolled over and we'll increment rolloverCount.
 *  If clock_gettime() is not called sufficiently often (e.g. every 49 days
 *  for a 1 msec tick rate), the tick count could rollover twice without
 *  being detected. Although clock_gettime(CLOCK_MONOTONIC) would still
 *  detect one rollover, the time would be off (by 50 days for a 1 msec
 *  tick rate). To prevent this from happening, we use a Clock object
 *  with a timeout of 0xFFFFFFFF ticks, and a timeout function that
 *  checks for rollover. This prevents the application from having to
 *  call clock_gettime() sufficiently often to keep an accurate time.
 *
 *  Each time the tick count rolls over, there will be a remainder of ticks
 *  that don't make up a whole second. We need to take this into account
 *  in clock_gettime(). The following definitions are for that purpose.
 */

/*
 *  The maximum number of ticks before the tick count rolls over. We use
 *  0xFFFFFFFF instead of 0x100000000 to avoid 64-bit math.
 */
#define MAX_TICKS 0xFFFFFFFF
#define TICKS_PER_SEC (1000000 / Clock_tickPeriod)

/* The integral number of seconds in a period of MAX_TICKS */
#define MAX_SECONDS (MAX_TICKS / TICKS_PER_SEC)

/* The total number of system ticks in MAX_SECONDS seconds */
#define MAX_SECONDS_TICKS (MAX_SECONDS * TICKS_PER_SEC)

/*
 *  MAX_TICKS - MAX_SECONDS_TICKS is the number of ticks left over that
 *  don't make up a whole second.  We add 1 to get the remaining number
 *  of ticks when the tick count wraps back to 0.  REM_TICKS could
 *  theoritically be equivalent to 1 second (when the tick period divides
 *  0x100000000 evenly), so it is not really a "remainder", since it ranges
 *  from 1 to TICKS_PER_SEC, instead of from 0 to TICKS_PER_SEC - 1.
 *  However, this will not affect the seconds calculation in clock_gettime(),
 *  so we can ignore this special case.
 */
#define REM_TICKS ((MAX_TICKS - MAX_SECONDS_TICKS) + 1)

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Compute a timeout in Clock ticks from the difference of
 *  an absolute time and the current time.
 * 
 * @param[in] clockId CLOCK_MONOTONIC or CLOCK_REALTIME
 * @param[in] abstime time for which the number of ticks should be calculated
 * @param[out] ticks number of ticks until \a abstime
 * @return Returns a status indicating success or failure
 * @retval 0 The call was successful
 * @retval -1 Invalid input arguments
 */
extern int _pthread_abstime2ticks(clockid_t clockId,
        const struct timespec *abstime, uint32_t *ticks);

#ifdef __cplusplus
}
#endif

#endif /* ti_posix_ccs_pthread_util__include */
