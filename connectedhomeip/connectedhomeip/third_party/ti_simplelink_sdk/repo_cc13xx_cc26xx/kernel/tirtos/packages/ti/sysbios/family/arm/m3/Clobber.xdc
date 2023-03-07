/*
 * Copyright (c) 2013-2014, Texas Instruments Incorporated
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
 *  ======== Clobber.xdc ========
 *
 *
 */

package ti.sysbios.family.arm.m3;

/*!
 *  @_nodoc
 *  ======== Clobber ========
 *  Clobber module used for SYS/BIOS 6 testing of the interrupt dispatcher.
 *
 *  This module consists of three functions which are used in unison to verify
 *  that the interrupt dispatcher is properly saving and restoring the scratch
 *  registers.
 *
 *  All clobber tests include 4 key steps:
 *    1. Trash the registers with Clobber_trashRegs. This function takes a
 *       (random) value, then assigns consecutive values to all of the
 *       scratch registers (saved by parent) (r0-r3, r12).
 *    2. Post an interrupt using Clobber_postIntr. This will invoke the
 *       dispatcher, which will save all of the scratch register values.
 *    3. The interrupt may simply return, or it may schedule a Task or a Swi,
 *       but at some point trashRegs is called again with a different value
 *       to make sure that all of the scratch registers have been modified and
 *       need to be restored.
 *    4. Once postIntr returns, checkRegs is called to verify that all of the
 *       scratch registers have been restored.
 *
 *  The Clobber module handles registers a4 (argument 1) and b3 (return
 *  address) differently than the others since these can't be trashed.
 *  Registers a4 and b3 are instead handled inside of postIntr, where they are
 *  trashed before the interrupt flag is set, then checked and restored
 *  afterwards. A module config param, postIntrCheck, is used to report whether
 *  the registers were correctly restored. Returning the result to a local
 *  variable would use another scratch register, so the module config is used
 *  instead.
 *
 *  In Clobber tests, it needs to be verified (by looking at the assembly) that
 *  the code which calls trash, post, and check is not using (and thus saving and
 *  restoring) any of the scratch registers except a4 and b3.
 *
 *  Clobber tests must check both the return value of checkRegs and the value of
 *  postIntrCheck to ensure that no registers were corrupted.
 */
@DirectCall
module Clobber
{
     // -------- Module Parameters --------

    /*! Number of registers broken in postIntr testing. */
    config UInt postIntrCheck = 0;

    /*!
     *  ======== trashRegs ========
     *  Trashes scratch registers
     *
     *  @param(value)        Value to use in trashing. Usually random number.
     */
    Void trashRegs(UInt value);

    /*!
     *  ======== postIntr ========
     *
     *  @param(intNum)        Interrupt number to post.
     */
    Void postIntr(UInt intNum);

    /*!
     *  ======== checkRegs ========
     *  Checks the scratch registers
     *  and returns the number of registers which have been
     *  corrupted.
     *
     *  @param(value)       Value to check against, same value passed to
     *                      trashRegs
     *  @a(returns)         Number of corrupted registers
     */
    UInt checkRegs(UInt value);

}
