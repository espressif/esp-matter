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
 *  ======== ICore.xdc ========
 *
 *
 */
import xdc.runtime.Error;

/*!
 *  ======== ICore ========
 *  @_nodoc
 *  Interface for Core module
 */

@DirectCall
interface ICore
{
    /*!
     *  ======== numCores ========
     *  number of Cores in SMP environment
     */
    config UInt numCores;
        
    /*!
     *  ======== getId ========
     *  return the current core id
     */
    UInt getId();

    /*!
     *  ======== interruptCore ========
     *  Cause an interrupt on a particular core.
     */
    Void interruptCore(UInt coreId);

    /*!
     *  @_nodoc
     *  ======== lock ========
     *  acquire Inter-core lock and locally disable interrupts.
     *
     *  @b(returns)     opaque key for use by Hwi_restore() or Core_hwiRestore()
     */
    IArg lock();

    /*!
     *  @_nodoc
     *  ======== unlock ========
     *  release Inter-core lock.
     */
    Void unlock();

    /*!
     *  @_nodoc
     *  ======== hwiDisable ========
     *  Locally disable interrupts.
     *
     *  Core_hwiDisable locally disables hardware interrupts on the current
     *  Core and returns an opaque key indicating whether interrupts were enabled
     *  or disabled on entry to Core_hwiDisable().
     *
     *  The actual value of the key is target/device specific and is meant
     *  to be passed to Hwi_restore() or Core_hwiRestore().
     *
     *  @b(returns)     opaque key for use by Hwi_restore() or Core_hwiRestore()
     */
    UInt hwiDisable();

    /*!
     *  ======== hwiEnable ========
     *  Locally enable interrupts.
     *
     *  Core_hwiEnable locally enables hardware interrupts on the current Core
     *  and returns an opaque key indicating whether interrupts were enabled or
     *  disabled on entry to Core_hwiEnable().
     *
     *  The actual value of the key is target/device specific and is meant
     *  to be passed to Hwi_restore() or Core_hwiRestore().
     *
     *  @b(returns)     opaque key for use by Hwi_restore() or Core_hwiRestore()
     */
    UInt hwiEnable();

    /*!
     *  @_nodoc
     *  ======== hwiRestore ========
     *  Locally restore interrupts.
     *
     *  Core_hwiRestore locally restores interrupts on the current Core to the 
     *  state determined by the key argument provided by a previous invocation 
     *  of Hwi_disable() or Core_hwiDisable().
     *
     *  @param(key)     enable/disable state to restore
     */
    Void hwiRestore(UInt key);

}

