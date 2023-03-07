/*
 * Copyright (c) 2013, Texas Instruments Incorporated
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
 * */

/*
 *  ======== UIAFrameCtx.c ========
 */

#include <xdc/std.h>
#include "package/internal/UIAFrameCtx.xdc.h"

/*
 * Frame Context Instrumentation
 *
 * Module-scope variables are used to trace
 * frame context changes and to allow
 * Host-side tooling to enable / disable
 * event logging based on a user-specified frame number / frame ID
 */


/*
 * ======== isLoggingEnabled ========
 * returns true if the new context matches the value to enable logging with.
 */
Bool UIAThreadCtx_isLoggingEnabled(UInt newFrameId)
{
    return (newFrameId == UIAFrameCtx_module->mEnableOnValue);
}

/*
 * ======== setOldValue =========
 * sets UIAFrameCtx_module->mLastValue to the new value and returns the old value before it was updated.
 */
UInt UIAFrameCtx_setOldValue(UInt newValue)
{
    UInt retValue = UIAFrameCtx_module->mLastValue;
    if (UIAFrameCtx_module->mLastValue != newValue)
        UIAFrameCtx_module->mLastValue = newValue;
    return(retValue);
}

/*
 * ======== getCtxId ========
 * Get the ID for the current channel
 */
UInt UIAFrameCtx_getCtxId()
{
    return(UIAFrameCtx_module->mLastValue);
}

/*
 * ======== getEnableOnValue ========
 * Get the EnableOnValue
 */
UInt UIAFrameCtx_getEnableOnValue()
{
    return (UIAFrameCtx_module->mEnableOnValue);
}

/*
 * ======== setEnableOnValue ========
 * set the EnableOnValue
 */
Void UIAFrameCtx_setEnableOnValue(UInt value)
{
    UIAFrameCtx_module->mEnableOnValue = value;
}
