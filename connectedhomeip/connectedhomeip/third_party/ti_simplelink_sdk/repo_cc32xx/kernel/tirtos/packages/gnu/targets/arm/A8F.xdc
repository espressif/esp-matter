/*
 *  Copyright 2021 by Texas Instruments Incorporated.
 *
 */

/*
 * Copyright (c) 2012-2020 Texas Instruments Incorporated - http://www.ti.com
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
import xdc.bld.ITarget2;

/*!
 *  ======== A8F.xdc ========
 *  Embedded Cortex A8, little endian, hard-float, bare metal target
 *
 *  This module defines an embedded bare metal target on Cortex A8. The target
 *  generates code compatible with the "v7A" architecture.
 *
 */
metaonly module A8F inherits gnu.targets.arm.ITarget {
    override readonly config string name                = "A8F";
    override readonly config string suffix              = "a8fg";
    override readonly config string isa                 = "v7A";
    override readonly config xdc.bld.ITarget.Model model= {
        endian: "little",
        shortEnums: true
    };

    override readonly config Bool alignDirectiveSupported = true;

    override readonly config string rts = "gnu.targets.arm.rtsv7A";
    override config string platform     = "ti.platforms.evmAM3359";

    override config string GCCTARG = "arm-none-eabi";

    override readonly config String stdInclude = "gnu/targets/arm/std.h";

    override readonly config ITarget2.Command cc = {
        cmd: "$(rootDir)/bin/$(GCCTARG)-gcc -c -MD -MF $@.dep",
        opts: "-mcpu=cortex-a8 -mfpu=neon -mfloat-abi=hard -mabi=aapcs -g"
    };

    readonly config ITarget2.Command ccBin = {
        cmd: "bin/arm-none-eabi-gcc -c -MD -MF $@.dep",
        opts: "-mcpu=cortex-a8 -mfpu=neon -mfloat-abi=hard -mabi=aapcs -g"
    };

    override config ITarget2.Options ccOpts = {
        prefix: "-Wunused -Wunknown-pragmas -ffunction-sections -fdata-sections ",
        suffix: "-Dfar= -D__DYNAMIC_REENT__ "
    };

    /*!
     *  ======== ccConfigOpts ========
     *  User configurable compiler options for the generated config C file.
     */
    override config ITarget2.Options ccConfigOpts = {
        prefix: "$(ccOpts.prefix)",
        suffix: "$(ccOpts.suffix)"
    };

    override readonly config ITarget2.Command asm = {
        cmd: "$(rootDir)/bin/$(GCCTARG)-gcc -c -x assembler-with-cpp",
        opts: "-mcpu=cortex-a8 -mfpu=neon -mfloat-abi=hard"
    };

    readonly config ITarget2.Command asmBin = {
        cmd: "bin/arm-none-eabi-gcc -c -x assembler-with-cpp",
        opts: "-mcpu=cortex-a8 -mfpu=neon -mfloat-abi=hard"
    };

    override config ITarget2.Options lnkOpts = {
        prefix: "-mcpu=cortex-a8 -mfloat-abi=hard -mfpu=neon -nostartfiles -Wl,-static -Wl,--gc-sections",
        suffix: "-L$(packageBase)/libs/install-native/$(GCCTARG)/lib/thumb/v7-a/hard -Wl,--start-group -lgcc -lc -lm -Wl,--end-group -Wl,-Map=$(XDCCFGDIR)/$@.map"
    };

    readonly config ITarget2.Command arBin = {
        cmd: "bin/arm-none-eabi-ar ",
        opts: ""
    };

    /*!
     *  ======== bspLib ========
     *  bspLib is assigned the name of the BSP library. The specified library
     *  is included on the link line.
     *
     *  Possible values for this field are "nosys" and "rdimon" with the default
     *  being "nosys".
     */
    override config string bspLib = "nosys";

    /*!
     *  ======== includeOpts ========
     *  Additional user configurable target-specific include path options
     *
     *  This target uses newlib run-time.
     */
    override config string includeOpts = "-I$(packageBase)/libs/install-native/$(GCCTARG)/include";

    /*
     *  ======== profiles ========
     */
    override config xdc.bld.ITarget.OptionSet profiles[string] = [
        ["debug", {
            compileOpts: {
                copts: "-g",
                defs:  "-D_DEBUG_=1",
            },
            linkOpts: "-g",
        }],

        ["release", {
            compileOpts: {
                copts: " -O2 ",
            },
            linkOpts: " ",
        }],
    ];

    /*
     *  ======== compatibleSuffixes ========
     */
    override config String compatibleSuffixes[] = [];

    override readonly config xdc.bld.ITarget.StdTypes stdTypes = {
        t_IArg          : { size: 4, align: 4 },
        t_Char          : { size: 1, align: 1 },
        t_Double        : { size: 8, align: 8 },
        t_Float         : { size: 4, align: 4 },
        t_Fxn           : { size: 4, align: 4 },
        t_Int           : { size: 4, align: 4 },
        t_Int8          : { size: 1, align: 1 },
        t_Int16         : { size: 2, align: 2 },
        t_Int32         : { size: 4, align: 4 },
        t_Int64         : { size: 8, align: 8 },
        t_Long          : { size: 4, align: 4 },
        t_LDouble       : { size: 8, align: 8 },
        t_LLong         : { size: 8, align: 8 },
        t_Ptr           : { size: 4, align: 4 },
        t_Short         : { size: 2, align: 2 },
        t_Size          : { size: 4, align: 4 },
    };
}
/*
 *  @(#) gnu.targets.arm; 1, 0, 0,; 7-28-2021 06:57:31; /db/ztree/library/trees/xdctargets/xdctargets-w20/src/ xlibrary

 */

