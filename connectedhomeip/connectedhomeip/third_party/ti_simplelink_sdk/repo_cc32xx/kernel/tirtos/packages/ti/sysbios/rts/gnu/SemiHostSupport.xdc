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
 */
/*
 *  ======== SemiHostSupport.xdc ========
 */

package ti.sysbios.rts.gnu;

/*!
 *  ======== SemiHostSupport ========
 *  This module does the required setup for supporting Semi-Hosted SYS/BIOS
 *  applications for all Cortex-A and Cortex-M GNU targets.
 *
 *  For Cortex-A targets, this module generates a SVC_Handler() function and
 *  registers it as the default SWI/SVC handler.
 *
 *  This module also registers a startup last function
 *  (see {@link xdc.runtime.Startup#lastFxns Startup.lastFxns}) that initializes
 *  the file handles for all Cortex-A and Cortex-M GNU targets.
 *
 *  Adding Semi-Hosting support to a SYS/BIOS application requires linking
 *  with a semi-hosting gnu library called "librdimon" and including this
 *  module in the config script.
 *
 *  Here's an example of including this module in the *.cfg file.
 *  @p(code)
 *      var SemiHost = xdc.useModule('ti.sysbios.rts.gnu.SemiHostSupport');
 *  @p
 *
 *  The following examples illustrate how to link with the Semi-Hosting library
 *  in the different build flows supported by SYS/BIOS:
 *
 *  - When using package.bld build flow, the following code needs to be added to
 *    the config.bld file:
 *
 *  @p(code)
 *      var gccArmTargets = xdc.loadPackage('gnu.targets.arm');
 *
 *      // For Cortex-A8 target
 *      gccArmTargets.A8F.bspLib = "rdimon";
 *
 *      // For Cortex-A9 target
 *      gccArmTargets.A9F.bspLib = "rdimon";
 *
 *      // For Cortex-A15 target
 *      gccArmTargets.A15F.bspLib = "rdimon";
 *
 *      // For Cortex-M3 target
 *      gccArmTargets.M3.bspLib = "rdimon";
 *
 *      // For Cortex-M4 target
 *      gccArmTargets.M4.bspLib = "rdimon";
 *
 *      // For Cortex-M4F target
 *      gccArmTargets.M4F.bspLib = "rdimon";
 *  @p
 *
 *  - When using configuro or building a CCS project, the user needs to add
 *    the following link options to link with the librdimon.a library.
 *
 *  @p(code)
 *      -Wl,--start-group -lrdimon -Wl,--end-group
 *  @p
 *
 *  @a(Note)
 *  If the SemiHostSupport module is used without linking with the librdimon
 *  library, the application build will generate a linker error. The linker
 *  complaints about a missing "initialise_monitor_handles" symbol which is
 *  defined in librdimon library. Please do not include this module if you
 *  are not linking with librdimon library to avoid any linker errors.
 */

@Template ("./SemiHostSupport.xdt") /* generate SVC_Handler() function */

module SemiHostSupport
{

internal:

    /*!
     *  ======== startup ========
     *  startup function to initialize semi-hosting file handles early
     *  during climb-up
     */
    Void startup();

}
