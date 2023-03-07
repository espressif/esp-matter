/*
 * Copyright (c) 2017, Texas Instruments Incorporated
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
 *  ======== CC26xx.xdc ========
 *  CC26xx device specific ROM support module.
 */

/*
 *
 *  These proxy bindings are fixed in ROM:
 *
 *   ti.sysbios.BIOS.RtsGateProxy is fixed and bound to GateMutex (non-issue for IAR and GNU)
 *   ti.sysbios.hal.Timer.TimerProxy is fixed and bound to m3.Timer.
 *   ti.sysbios.hal.Hwi.HwiProxy is fixed and bound to m3.Hwi.
 *   ti.sysbios.heaps.HeapMem.Module_GateProxy is fixed and bound to GateMutex
 *   ti.sysbios.knl.Clock.TimerProxy is fixed and bound to cc26xx.Timer.
 *   ti.sysbios.knl.Task.SupportProxy is fixed and bound to m3.TaskSupport.
 *   xdc.runtime.Memory_HeapProxy is fixed and bound to HeapMem.
 *   xdc.runtime.System_Module_GateProxy is fixed and bound to GateHwi
 *   xdc.runtime.Timestamp.SupportProxy is fixed and bound to cc26xx.Timestamp 

 *  These proxy bindings are configurable:
 *
 *   System.SupportProxy is configurable.
 *
 *  This module's C file (CC26xx.c) will not be included in the 
 *  string returned by Build.getCFiles() because the 
 *  ti.sysbios.rom.cortexm.cc26xx package is deliberately not
 *  included in the "Build.biosPackages[]" table.
 */

@Template('./CC26xx.xdt')
module CC26xx inherits ti.sysbios.interfaces.IRomDevice
{
    override metaonly config String libDir = null;
    override metaonly config String templateName = "CC26xx_rom_makefile.xdt";
    
    /*
     *  Set this constant to a magic value unique to the ROM.romName.
     */
    config UInt32 REVISION = 286529877;
    metaonly config UInt32 REVISION_WORD_ADDRESS = 0x1002B400;

    /*
     *  List of driverlib functions referenced by ROM.
     *  Re-implemented ROM.xdc version to support ROM'd driverlib
     *  API call syntax.
     */
    metaonly config String otherLibFuncs[];

    /*!
     *  ======== getLibDefs ========
     *  Get the compiler -D mappings of the
     *  driverlib library funcs into function ptrs
     */
    metaonly String getOtherLibDefs();

    /*!
     *  @_nodoc
     *  ======== validate ========
     *  Validate that the configuration options align with the
     *  settings that were used when buidling the ROM. This can be
     *  set to false to bypass the ROM validation warnings that are
     *  are raised with configuration options don't align with the 
     *  settings that were used when building the ROM.
     */
    metaonly config Bool validate = true;

internal:   /* not for client use */

    /*
     *  Controls if section pragmas are generated for the module states
     *  used by the ROM code. These should be generated only when the
     *  initial ROM image is created so that the module states can
     *  be placed into an identifiable memory region. 
     */
    metaonly config Bool genPragmas = false;
}
