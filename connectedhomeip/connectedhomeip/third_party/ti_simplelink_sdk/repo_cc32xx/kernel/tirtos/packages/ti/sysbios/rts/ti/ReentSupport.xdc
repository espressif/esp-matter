/*
 * Copyright (c) 2017 Texas Instruments Incorporated - http://www.ti.com
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
 *  ======== ReentSupport.xdc ========
 */

package ti.sysbios.rts.ti;

import xdc.runtime.Assert;
import ti.sysbios.knl.Task;

/*!
 *  ======== ReentSupport ========
 *  Provide thread local storage for POSIX implementation
 */

@Template ("./ReentSupport.xdt")

module ReentSupport
{
    /*!
     *  ======== A_badThreadType ========
     *  Cannot make a call to C library from interrupt context
     *
     *  @_nodoc
     */
    config Assert.Id A_badThreadType = {
        msg: "A_badThreadType: Cannot call a C runtime library API from a Hwi or Swi thread."
    };

    /*
     *  ======== getReent ========
     *  Return a pointer to the current thread's errno location.
     *
     *  @b(returns) Address of the errno location in TLS.
     */
    Int *getReent();

internal:   /* not for client use */

    /* -------- hook functions -------- */

    /*
     *  ======== taskRegHook ========
     *  Registration function for the module's hook
     *
     *  @param(id) The ID of the hook assigned to this module.
     */
    Void taskRegHook(Int id);

    /*
     *  ======== getTlsAddr ========
     *  Return address of thread-local storage buffer
     *
     *  This function is generated. It's implementation differs depending
     *  if SYS/BIOS is in ROM or not.
     */
    Int *getTlsAddr();

    /* -------- Internal Module Types -------- */

    struct Module_State {
        Int     taskHId;   /* task hook context ID for this module */
    };
}
