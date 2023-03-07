/*
 *  Copyright 2021 by Texas Instruments Incorporated.
 *
 */

/*
 * Copyright (c) 2013-2020 Texas Instruments Incorporated
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
 *  ======== ITarget ========
 *  IAR ARM extension to the base `xdc.bld.ITarget3` interface.
 */
@TargetHeader("xdc/bld/stddefs.xdt")
metaonly interface ITarget inherits xdc.bld.ITarget3 {

    override readonly config String rts = "iar.targets.arm.rts";

    override config string stdInclude = "iar/targets/arm/std.h";

    /*
     *  ======== binDir ========
     *  Code gen tools bin directory path
     */
    config string binDir = "$(rootDir)/bin/";

    /*!
     *  ======== model ========
     *  Little endian, thumb2 model
     */
    override readonly config xdc.bld.ITarget.Model model = {
        endian: "little",
        codeModel: "thumb2",
        shortEnums: true
    };

    /*!
     *  ======== ccOpts ========
     *  User configurable compiler options.
     */
    override config Options ccOpts = {
        prefix: "--silent",
        suffix: ""
    };

    /*!
     *  ======== asmOpts ========
     *  User configurable assembler options.
     */
    override config Options asmOpts = {
        prefix: "-S",
        suffix: ""
    };

    /*!
     *  ======== ar ========
     *  The command used to create an archive
     */
    override readonly config Command ar = {
        cmd: "iarchive",
        opts: ""
    };

    /*!
     *  ======== arOpts ========
     *  User configurable archiver options.
     */
    override config Options arOpts = {
        prefix: "--silent",
        suffix: ""
    };

    /*!
     *  ======== lnkOpts ========
     *  User configurable linker options.
     */
    override config xdc.bld.ITarget2.Options lnkOpts = {
        prefix: "--silent",
        suffix: "--map $(XDCCFGDIR)/$@.map  --redirect _Printf=_PrintfSmall --redirect _Scanf=_ScanfSmall ",
    };

    /*!
     *  ======== vers ========
     *  The command used to get the tool-chain to return a version number.
     */
    readonly config Command vers = {
        cmd: "iccarm",
        opts: "-v"
    };

    /*!
     *  ======== extension ========
     *  The IAR assembly file extension recognised by this target.
     */
    override config Extension extensions[string] = [
        [".asm",  {suf: ".asm", typ: "asm"}],
        [".c",    {suf: ".c",   typ: "c"  }],
        [".cpp",  {suf: ".cpp", typ: "cpp"}],
        [".cxx",  {suf: ".cxx", typ: "cpp"}],
        [".C",    {suf: ".C",   typ: "cpp"}],
        [".cc",   {suf: ".cc",  typ: "cpp"}],
        [".s",    {suf: ".s",   typ: "asm"}],
        [".sv6M", {suf: ".sv6M",typ: "asm"}],
        [".sv7M", {suf: ".sv7M",typ: "asm"}],
        [".sv8M", {suf: ".sv8M",typ: "asm"}],
    ];

    /*!
     *  ======== includeOpts ========
     *  Additional user configurable target-specific include path options
     */
    override config String includeOpts = "";

    /*!
     *  ======== cmdPrefix ========
     *  Prefix to put in front of each command
     *
     *  This string is put in front of every Command before being passed to
     *  the shell for execution.  This string can be used to run the compiler
     *  in emulation environments.
     *
     *  LC_ALL=C must be set for code gen to run on SUSE
     */
    config String cmdPrefix = "LC_ALL=C ";

    /*!
     *  ======== alignDirectiveSupported ========
     *  The compiler supports an align directive.
     */
    override readonly config Bool alignDirectiveSupported = true;

    /*!
     *  ======== stdTypes  ========
     *  Size and alignment for standard base types
     */
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
        t_Long          : { size: 4, align: 4 },
        t_LDouble       : { size: 8, align: 8 },
        t_LLong         : { size: 8, align: 8 },
        t_Ptr           : { size: 4, align: 4 },
        t_Short         : { size: 2, align: 2 },
        t_Size          : { size: 4, align: 4 },
        t_Int64         : { size: 8, align: 8 },
    };

    /*!
     *  ======== profiles ========
     *  Standard options profiles.
     */
    override config xdc.bld.ITarget.OptionSet profiles[string] = [
        ["debug", {
            compileOpts: {
                copts: "--debug --dlib_config $(rootDir)/inc/c/DLib_Config_Normal.h",
            },
            linkOpts: "--semihosting=iar_breakpoint",
        }],
        ["release", {
            compileOpts: {
                copts: "--debug -Ohs --dlib_config $(rootDir)/inc/c/DLib_Config_Normal.h",
            },
            linkOpts: "--semihosting=iar_breakpoint",
        }],
        ["debug_full", {
            compileOpts: {
                copts: "--debug --dlib_config $(rootDir)/inc/c/DLib_Config_Full.h",
            },
            linkOpts: "--semihosting=iar_breakpoint",
        }],
        ["release_full", {
            compileOpts: {
                copts: "--debug -Ohs --dlib_config $(rootDir)/inc/c/DLib_Config_Full.h",
            },
            linkOpts: "--semihosting=iar_breakpoint",
        }],
    ];

    override config String binaryParser = "xdc.targets.omf.Elf";
}
/*
 *  @(#) iar.targets.arm; 1, 0, 0,; 7-28-2021 06:57:36; /db/ztree/library/trees/xdctargets/xdctargets-w20/src/ xlibrary

 */

