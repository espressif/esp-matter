/*
 *  Copyright 2021 by Texas Instruments Incorporated.
 *
 */

/*
 * Copyright (c) 2020 Texas Instruments Incorporated
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

/*!
 *  @_nodoc
 *  ======== C28_float64 ========
 *  TI C28 large model little endian with floating point support (fpu64) (ELF)
 */
metaonly module C28_float64 inherits ti.targets.ITarget {
    override readonly config string name                = "C28_float64";
    override readonly config string suffix              = "e28FP64";
    override readonly config string isa                 = "28FP64";
    override readonly config xdc.bld.ITarget.Model model = {
        dataModel: "large",
        endian: "little"
    };
    override readonly config xdc.bld.ITarget.Module base = ti.targets.C28_float;

    override readonly config string rts                 = "ti.targets.rts2800";

    /*
     *  ======== compatibleSuffixes ========
     */
    override config String compatibleSuffixes[] = ["e28L"];

    final override readonly config Bool alignDirectiveSupported = false;

    /*!
     *  ======== cc ========
     *  Define compiler executable
     *
     *  Options:
     *  @p(dlist)
     *      -`-c`
     *          no linking
     *      -`-v28`
     *          compile for c28x.
     *      -`-ml`
     *          use large memory model
     *      -`-DLARGE_MODEL=1`
     *          required to use va_arg in large model
     *      -`--float_support=fpu64`
     *          Enable FPU
     */
    override readonly config xdc.bld.ITarget2.Command cc = {
        cmd: "cl2000 -c",
        opts: "-v28 -DLARGE_MODEL=1 -ml --float_support=fpu64 --abi=eabi"
    };

    /*!
     *  ======== asm ========
     *  Define assembler executable
     *
     *  Options:
     *  @p(dlist)
     *      -`-c`
     *          no linking
     *      -`-v28`
     *          compile for c28x.
     *      -`-ml`
     *          use large memory model
     *      -`-DLARGE_MODEL=1`
     *          because compiler and BIOS are broken; asembler
     *          defines __LARGE_MODEL but BIOS uses LARGE_MODEL
     *      -`--float_support=fpu64`
     *          Enable FPU
     */
    override readonly config xdc.bld.ITarget2.Command asm = {
        cmd: "cl2000 -c",
        opts: "-v28 -ml -DLARGE_MODEL=1 --float_support=fpu64 --abi=eabi"
    };

    /*!
     *  ======== lnk ========
     *  Define linker executable
     */
    override readonly config xdc.bld.ITarget2.Command lnk = {
        cmd: "cl2000",
        opts: "--abi=eabi -z"
    };

    /*!
     *  ======== lnkOpts ========
     *  Linker options
     *
     *  Options:
     *  @p(dlist)
     *      -`-q`
     *          Quite run
     *      -`-u`
     *          Place unresolved external symbol into symbol table
     *      -`-c`
     *          ROM autoinitialization model
     *      -`-m`
     *          create a map file
     *      -`-l`
     *          archive library file as linker input
     */
    override config xdc.bld.ITarget2.Options lnkOpts = {
        prefix: "-q -u _c_int00",
        suffix: "-w -c -m $(XDCCFGDIR)/$@.map -l $(rootDir)/lib/libc.a"
    };

    override readonly config xdc.bld.ITarget2.Command vers =
        ti.targets.C28_float.vers;

    override readonly config xdc.bld.ITarget2.Command ar =
        ti.targets.C28_float.ar;

    final override readonly config string sectMap[string] = [
        [".text", "code"],
        [".switch", "data"],
        [".data", "data"],
        [".rodata", "data"],
        [".neardata", "data"],
        [".fardata", "data"],
        [".far", "data"],
        [".cinit", "data"],
        [".bss", "data"],
        [".const", "data"],
        [".stack", "stack"],
        [".sysmem", "data"],
        [".init_array", "data"],
        [".args", "data"],
        [".bss:.cio", "data"],
        [".c28xabi.exidx", "data"],
        [".c28xabi.extab", "data"],
    ];

    final override readonly config Bool splitMap[string] =
        ti.targets.C28_float.splitMap;

    override readonly config xdc.bld.ITarget.StdTypes stdTypes = {
        t_IArg          : { size: 2, align: 2 },
        t_Char          : { size: 1, align: 1 },
        t_Double        : { size: 4, align: 4 },
        t_Float         : { size: 2, align: 2 },
        t_Fxn           : { size: 2, align: 2 }, /* far */
        t_Int           : { size: 1, align: 1 },
        t_Int8          : { size: 1, align: 1 },
        t_Int16         : { size: 1, align: 1 },
        t_Int32         : { size: 2, align: 2 },
        t_Int64         : { size: 4, align: 2 },
        t_Long          : { size: 2, align: 2 },
        t_LDouble       : { size: 2, align: 2 },
        t_LLong         : { size: 4, align: 2 },
        t_Ptr           : { size: 2, align: 2 },
        t_Short         : { size: 1, align: 1 },
        t_Size          : { size: 2, align: 2 },
    };

    override config String includeOpts = "-I$(rootDir)/include";

    override config String stdInclude = "ti/targets/elf/std.h";

    override config String binaryParser = "xdc.targets.omf.Elf";

    override readonly config Int bitsPerChar =
        ti.targets.C28_float.bitsPerChar;
}
/*
 *  @(#) ti.targets.elf; 1, 0, 0,; 7-28-2021 06:57:45; /db/ztree/library/trees/xdctargets/xdctargets-w20/src/ xlibrary

 */

