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
 *  ======== UIAAppCtx.c ========
 */

#include <xdc/std.h>
#include "package/internal/UIAAppCtx.xdc.h"

/*
 * Application Context Instrumentation
 *
 * Module-scope variables are used to trace
 * application context changes and to allow
 * Host-side tooling to enable / disable
 * event logging based on a user-specified application ID
 */


/*
 * ======== isLoggingEnabled ========
 * returns true if the new context matches the value to enable logging with.
 */
Bool UIAAppCtx_isLoggingEnabled(UInt newAppId)
{
    return (newAppId == UIAAppCtx_module->mEnableOnValue);
}

/*
 * ======== setOldValue =========
 * sets UIAAppCtx_module->mLastValue to the new value and returns the old value before it was updated.
 */
UInt UIAAppCtx_setOldValue(UInt newValue)
{
    UInt retValue = UIAAppCtx_module->mLastValue;
    if (UIAAppCtx_module->mLastValue != newValue)
        UIAAppCtx_module->mLastValue = newValue;
    return(retValue);
}

/*
 * ======== getCtxId ========
 * Get the CtxID
 */
UInt UIAAppCtx_getCtxId()
{
    return (UIAAppCtx_module->mLastValue);
}

/*
 * ======== getEnableOnValue ========
 * Get the EnableOnValue
 */
UInt UIAAppCtx_getEnableOnValue()
{
    return (UIAAppCtx_module->mEnableOnValue);
}

/*
 * ======== setEnableOnValue ========
 * set the EnableOnValue
 */
Void UIAAppCtx_setEnableOnValue(UInt value)
{
    UIAAppCtx_module->mEnableOnValue = value;
}
