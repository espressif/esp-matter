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
 *  ======== UIAUserCtx.c ========
 */

#include <xdc/std.h>
#include "package/internal/UIAUserCtx.xdc.h"

/*
 * User-defined Context Instrumentation
 *
 * Module-scope variables are used to trace
 * user-specified context changes and to allow
 * Host-side tooling to enable / disable
 * event logging based on a user-specified context value
 */


/*
 * ======== isLoggingEnabled ========
 * returns true if the new context matches the value to enable logging with.
 */
Bool UIAUserCtx_isLoggingEnabled(UInt newUserCtx)
{
    return (newUserCtx == UIAUserCtx_module->mEnableOnValue);
}

/*
 * ======== setOldValue =========
 * sets UIAUserCtx_module->mLastValue to the new value and returns the old value before it was updated.
 */
UInt UIAUserCtx_setOldValue(UInt newValue)
{
    UInt retValue = UIAUserCtx_module->mLastValue;
    if (UIAUserCtx_module->mLastValue != newValue)
        UIAUserCtx_module->mLastValue = newValue;
    return(retValue);
}

/*
 * ======== getCtxId ========
 * Get the ID for the current channel
 */
UInt UIAUserCtx_getCtxId()
{
    return(UIAUserCtx_module->mLastValue);
}

/*
 * ======== getEnableOnValue ========
 * Get the EnableOnValue
 */
UInt UIAUserCtx_getEnableOnValue()
{
    return(UIAUserCtx_module->mEnableOnValue);
}

/*
 * ======== setEnableOnValue ========
 * set the EnableOnValue
 */
Void UIAUserCtx_setEnableOnValue(UInt value)
{
    UIAUserCtx_module->mEnableOnValue = value;
}
