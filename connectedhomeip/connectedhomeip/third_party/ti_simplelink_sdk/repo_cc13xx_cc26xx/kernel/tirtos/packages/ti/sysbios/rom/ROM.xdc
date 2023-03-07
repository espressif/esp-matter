/*
 * Copyright (c) 2015-2020, Texas Instruments Incorporated
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

package ti.sysbios.rom;

/*!
 *  ======== ROM.xdc ========
 *  ROM selection and support module.
 *
 *  @a(CC26xx Device Users)
 *
 *  To build a TI-RTOS application utilizing the ROM in a CC26xx device,
 *  add these lines to your configuration file:
 *
 *  @p(code)
 *       var ROM = xdc.useModule('ti.sysbios.rom.ROM');
 *       ROM.romName = ROM.CC2650;
 *  @p
 *
 *  @a(IAR Workbench Users of CC26xx Devices)
 *
 *  IAR Workbench users should use the CC2650.icf file provided in the
 *  the following directory relative to the BIOS installation:
 *  @p(code)
 *     ti/sysbios/rom/cortexm/cc26xx/golden/CC26xx/CC2650.icf
 *  @p
 *
 *  If for some reason you must use your own .icf file, then you are
 *  required to copy the contiguous block of 131 lines at the end of the
 *  above referenced CC2650.icf file starting with:
 *  @p(code)
 *    place at address mem:0x0000058c {readonly section .const_xdc_runtime_Error_policy__C};
 *    place at address mem:0x00000538 {readonly section .const_xdc_runtime_IModule_Interface__BASE__C};
 *    ...
 *  @p
 *  into your .icf file. Failing to do this will result in your application
 *  getting stuck in a forever loop at startup.
 *
 *  @p(html)
 *  <br/>
 *  <br/>
 *  @p
 *
 *  @a(CC13xx Device Users)
 *
 *  To build a TI-RTOS application utilizing the ROM in a CC13xx device,
 *  add these lines to your configuration file:
 *
 *  @p(code)
 *       var ROM = xdc.useModule('ti.sysbios.rom.ROM');
 *       ROM.romName = ROM.CC1350;
 *  @p
 *
 *  @a(IAR Workbench Users of CC13xx Devices)
 *
 *  IAR Workbench users should use the CC1350.icf file provided in the
 *  the following directory relative to the BIOS installation:
 *  @p(code)
 *     ti/sysbios/rom/cortexm/cc13xx/golden/CC13xx/CC1350.icf
 *  @p
 *
 *  If for some reason you must use your own .icf file, then you are
 *  required to copy the contiguous block of 131 lines at the end of the
 *  above referenced CC1350.icf file starting with:
 *  @p(code)
 *    place at address mem:0x00001490 {readonly section .const_xdc_runtime_Error_policy__C};
 *    place at address mem:0x0000143c {readonly section .const_xdc_runtime_IModule_Interface__BASE__C};
 *    ...
 *  @p
 *  into your .icf file. Failing to do this will result in your application
 *  getting stuck in a forever loop at startup.
 */

@Template('./ROM.xdt')
metaonly module ROM
{
    enum RomName {
        NO_ROM,         /*! Default for NOT building against a ROM */
        CC2650,         /*! Use for all CC26xx devices */
        CC2640R2F,      /*! Use for all CC26xx R2 devices */
        CC1350,         /*! Use for all CC13xx devices */
        F28004x,        /*! Use for all F28004x class devices */
        CC26X2,         /*! Use for all CC26x2 agama devices, OAD supported */
        CC26X2V2,       /*! Use for all CC26x2R2 agama pg2 devices, OAD supported */
        CC13X2,         /*! Use for all CC13x2 agama devices */
        CC13X2V2,       /*! Use for all CC13x2R2 agama pg2 devices, OAD supported */
        CC26X2_NO_OAD,  /*! Use for all CC26x2 agama devices, OAD not supported. */
        CC1352,         /* Use for all CC13x2 agama devices */
        CC2652          /* Use for all CC26x2 agama devices, OAD supported */
    };

    /*!
     *  The name of the device being used.
     *  Currently only 'ROM.CC2650' and
     *  'ROM.CC1350' are supported.
     */
    metaonly config RomName romName = NO_ROM;

    /*!  @_nodoc */
    metaonly config String configDefs[];

    /*!
     *  @_nodoc
     *  Defines an array of func pointers to allow
     *  indirect access to functions accessed from within ROM code
     *  but NOT to be included in the ROM.
     *
     *  When the ROM is built, functions labels added to the excludedFuncs
     *  are -D defined as derefenced function calls.
     *
     *  When an application is built, the API's corresponding function ptr
     *  is populated with the named function addresses.
     */
    metaonly config String excludedFuncs[];

    /*!  @_nodoc */
    metaonly config String generatedFuncs[];

    /*!  @_nodoc */
    metaonly config String otherLibs[];

    /*!  @_nodoc */
    metaonly config String otherLibIncludes[];

    /*!  @_nodoc */
    metaonly config String otherLibFuncs[];

    /*!  @_nodoc */
    metaonly config Bool makePatch = false;

    /*!  @_nodoc */
    metaonly config Bool groupFuncPtrs = false;

    metaonly config Ptr constStructAddr = null;
    metaonly config Ptr dataStructAddr = null;
    metaonly config Ptr externFuncStructAddr = null;

    metaonly struct ExFunc {
        Bool generated;
        String type;
        String name;
        String args;
    };
	
    metaonly config ExFunc excludeFuncs[];
    metaonly config String excludeIncludes[];

    /*!
     *  @_nodoc
     *  ======== getOtherLibs ========
     *  Get the list of "-l lib" 
     *  to link rom with.
     */
    metaonly String getOtherLibs();

    /*!
     *  @_nodoc
     *  ======== getLibDefs ========
     *  Get the compiler -D mappings of the
     *  library funcs into function ptrs
     */
    metaonly String getOtherLibDefs();

    /*!
     *  @_nodoc
     *  ======== getExternDefs ========
     *  Get the compiler -D mappings of the
     *  extern funcs into function ptrs
     */
    metaonly String getExternDefs();

    /*!
     *  @_nodoc
     *  ======== getGeneratedDefs ========
     *  Get the compiler -D mappings of the
     *  generated funcs into function ptrs
     */
    metaonly String getGeneratedDefs();

    /*!
     *  @_nodoc
     *  ======== getConfigDefs ========
     *  Get the list of module configuration -D's
     */
    metaonly String getConfigDefs();

    /*!
     *  @_nodoc
     *  ======== getExterns ========
     *  Get the list of externally referenced symbols
     */
    metaonly String getExterns();


    /*!
     *  @_nodoc
     *  ======== makeExternsFile ========
     *  Creates a file of name 'fileName' that includes the list of funcs
     *  defined in the excludedFuncs[] array.
     */
    function makeExternsFile(fileName);
}
