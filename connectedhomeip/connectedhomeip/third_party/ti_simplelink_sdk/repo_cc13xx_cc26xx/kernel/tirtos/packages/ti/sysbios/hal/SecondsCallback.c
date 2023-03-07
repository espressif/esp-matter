/*
 * Copyright (c) 2014-2019, Texas Instruments Incorporated
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
 *  ======== SecondsCallback.c ========
 */

#include <xdc/std.h>

#include "package/internal/SecondsCallback.xdc.h"

/*
 *  ======== SecondsCallback_get ========
 */
UInt32 SecondsCallback_get(Void)
{
    return (SecondsCallback_getFxn());
}

/*
 *  ======== SecondsCallback_getTime ========
 */
UInt32 SecondsCallback_getTime(SecondsCallback_Time *ts)
{
    return (SecondsCallback_getTimeFxn(ts));
}

/*
 *  ======== SecondsCallback_set ========
 */
Void SecondsCallback_set(UInt32 seconds)
{
    SecondsCallback_setFxn(seconds);
}

/*
 *  ======== SecondsCallback_setTime ========
 */
UInt32 SecondsCallback_setTime(SecondsCallback_Time *ts)
{
    return (SecondsCallback_setTimeFxn(ts));
}

/*
 *  ======== ti_sysbios_hal_SecondsCallback_defaultGet ========
 */
UInt32 ti_sysbios_hal_SecondsCallback_defaultGet(Void)
{
    return (0);
}

/*
 *  ======== ti_sysbios_hal_SecondsCallback_defaultGetTime ========
 */
UInt32 ti_sysbios_hal_SecondsCallback_defaultGetTime(SecondsCallback_Time *ts)
{
    return (0);
}

/*
 *  ======== ti_sysbios_hal_SecondsCallback_defaultSet ========
 */
Void ti_sysbios_hal_SecondsCallback_defaultSet(UInt32 seconds)
{
}

/*
 *  ======== ti_sysbios_hal_SecondsCallback_defaultSetTime ========
 */
UInt32 ti_sysbios_hal_SecondsCallback_defaultSetTime(SecondsCallback_Time *ts)
{
    return (0);
}
