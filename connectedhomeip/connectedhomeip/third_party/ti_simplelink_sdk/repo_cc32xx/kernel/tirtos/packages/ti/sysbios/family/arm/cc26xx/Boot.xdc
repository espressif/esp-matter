/*
 * Copyright (c) 2014-2016, Texas Instruments Incorporated
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
 *  ======== Boot.xdc ========
 */

package ti.sysbios.family.arm.cc26xx;

/*!
 *  ======== Boot ========
 *  CC26xx Boot Support.
 *
 */
@DirectCall

@Template("./Boot.xdt")
module Boot
{
    /*!
     *  Trim device flag; determines if SetupTrimDevice() is called at boot,
     *  default is true.
     *
     *  Set to false to disable the Boot module from trimming the device.
     *  The device *must* be trimmed for proper operation. If the Boot module
     *  doesn't do this, the application must explicitly call SetupTrimDevice().
     */
    metaonly config Bool trimDevice = true;

    /*!
     *  Include a default customer configuration (CCFG) structure, default is
     *  false.
     *
     *  Set to true to link in the default CCFG data structure in driverlib.
     */
    metaonly config Bool customerConfig = false;

   /*!
     *  @_nodoc
     *
     *  Which PG version of driverlib to compile/link with. 
     */
    metaonly config UInt driverlibVersion = 2;

   /*!
     *  @_nodoc
     *
     *  Should getLibs() serve up local copy of driverlib?
     */
    metaonly config Bool provideDriverlib = false;

    /*!
     *  ======== getBootReason ========
     *  Gets the reason for the most recent boot of the CPU.
     *
     *  @b(returns)     The reset reason, as reported by the
     *                  SysCtrlResetSourceGet() DriverLib API.
     */
    UInt32 getBootReason();

internal:

};
