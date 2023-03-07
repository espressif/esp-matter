/*
 * Copyright (c) 2015, Texas Instruments Incorporated
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
 *  ======== CoreNull.c ========
 *  Null ICore implementation for default BIOS CoreDelegate
 */

#include <xdc/std.h>
#include <ti/sysbios/hal/Hwi.h>
#include "package/internal/CoreNull.xdc.h"

/*
 *  ======== CoreNull_lock ========
 */
IArg CoreNull_lock()
{
    return (CoreNull_hwiDisable());
}

/*
 *  ======== CoreNull_unlock ========
 */
Void CoreNull_unlock()
{
}

/*
 *  ======== CoreNull_getId ========
 */
UInt CoreNull_getId()
{
    return (0);
}

/*
 *  ======== CoreNull_interruptCore ========
 */
Void CoreNull_interruptCore(UInt coreId)
{
}

/*
 *  ======== CoreNull_hwiDisable ========
 */
UInt CoreNull_hwiDisable()
{
    return Hwi_disable();
}

/*
 *  ======== CoreNull_hwiEnable ========
 */
UInt CoreNull_hwiEnable()
{
    return Hwi_enable();
}

/*
 *  ======== CoreNull_hwiRestore ========
 */
Void CoreNull_hwiRestore(UInt key)
{
    Hwi_restore(key);
}
