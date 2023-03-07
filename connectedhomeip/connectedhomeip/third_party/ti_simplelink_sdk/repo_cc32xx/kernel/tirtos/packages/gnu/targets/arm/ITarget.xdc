/*
 *  Copyright 2021 by Texas Instruments Incorporated.
 *
 */

/*
 * Copyright (c) 2016-2019 Texas Instruments Incorporated - http://www.ti.com
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
 *  ======== gnu.targets.arm.ITarget ========
 *  Interface to GCC compatible Arm compilers
 */
@TargetHeader("xdc/bld/stddefs.xdt")
metaonly interface ITarget inherits xdc.bld.ITarget3 {

    /*!
     *  ======== GCCVERS ========
     *  Version number of the GCC compiler; e.g., "3.2".
     *
     *  This string can be supplied by the user, otherwise it is obtained
     *  by running "gcc -dumpversion".
     */
    config string GCCVERS = null;

    /*!
     *  ======== BINVERS ========
     *  Version number of binutils used with the compiler; e.g., "2.19".
     *
     *  This string can be supplied by the user, otherwise it is obtained
     *  by running "ld -v".
     */
    config string BINVERS = null;

    /*!
     *  ======== GCCTARG ========
     *  The name of the platform executing programs produced by this target
     *
     *  This string can be supplied by the user, otherwise is is obtained
     *  from the compiler and follows the GNU standard format
     *  (<cpu>-<manufacturer>-<os> or <cpu>-<manufacturer>-<kernel>-<os>);
     *  e.g., "arm-none-eabi" or "x86_64-unknown-linux-gnu".
     *
     *  When building a GCC compiler, there are three different execution
     *  platforms to consider: the platform used to "build" the compiler, the
     *  "host" platform that runs the compiler, and the "target" platform
     *  that runs the executables produced by the compiler. All three
     *  platforms are identified using a
     *  {@link http://sources.redhat.com/autobook/autobook/autobook_17.html configuration name}
     *  defined by GNU Autotools.  `GCCTARG` is the name of the "target"
     *  platform.
     */
    config string GCCTARG = null;

    /*!
     *  ======== LONGNAME ========
     *  @_nodoc
     *  The "long name" of the gcc compiler
     *
     *  This name is used (in conjunction with rootDir) to find the compiler
     *  and linker for this target. The format of `LONGNAME` is always
     *  "/bin/<machine>-gcc". For majority of the targets, the default value
     *  for `LONGNAME` does not ever need to be changed. But, there are
     *  targets where the different but compatible compilers may have
     *  different `LONGNAME` parameters. For such targets and compilers,
     *  `LONGNAME` can be set in `config.bld`.
     *
     *  @a(Example)
     *  If a version 2010q1 of the CodeSourcery GNU toolchain for Arm is
     *  installed in C:/CodeSourcery/arm-2010q1, the following settings in
     *  `config.bld` configure `gnu.targets.arm.GCArmv6` target to use that
     *  toolchain:
     *  @p(code)
     *  var GCArmv6 = xdc.module("gnu.targets.arm.GCArmv6");
     *  GCArmv6.rootDir = "C:/CodeSourcery/arm-2010q1";
     *  GCArmv6.LONGNAME = "bin/arm-none-linux-gnueabi-gcc";
     *  @p
     *
     */
    config string LONGNAME = null;

    /*!
     *  ======== remoteHost ========
     *  Remote host used to run compiler, linker, and archiver tools
     *
     *  If `remoteHost` is `null` (or `undefined`), the configured compiler
     *  is run locally; otherwise, `remoteHost` is taken to be the host name
     *  of the machine that that should be used to run the specified compiler.
     *
     *  All target commands are prefixed with a command that uses `rsh` to run
     *  the commands on the specified host.  Thus, in order to use this
     *  setting, the remote machine must be support `rsh` and the user must
     *  have permission to run commands from the local machine on the remote
     *  host named `remoteHost`.  This usually involves adding a line to the
     *  user's `~/.rhosts` file on the remote machine of the form:
     *  @p(code)
     *      local-machine-name user-name
     *  @p
     *  where `local-machine-name` is the name of the local machine and
     * `user-name` is the user's login name on the local machine.
     */
    config string remoteHost;

    /*!
     *  ======== ar ========
     *  The command used to create an archive
     */
    override readonly config xdc.bld.ITarget2.Command ar = {
        cmd: "$(rootDir)/bin/$(GCCTARG)-ar",
        opts: "cr"
    };

    /*!
     *  ======== lnk ========
     *  The command used to link executables.
     */
    override readonly config xdc.bld.ITarget2.Command lnk = {
        cmd: "$(rootDir)/bin/$(GCCTARG)-gcc",
        opts: ""
    };

    /*!
     *  ======== cc ========
     *  The command used to compile C/C++ source files into object files
     */
    override readonly config xdc.bld.ITarget2.Command cc = {
        cmd: "$(rootDir)/bin/$(GCCTARG)-gcc -c -MD -MF $@.dep",
        opts: ""
    };

    /*!
     *  ======== asm ========
     *  The command used to assembles assembly source files into object files
     */
    override readonly config xdc.bld.ITarget2.Command asm = {
        cmd: "$(rootDir)/bin/$(GCCTARG)-gcc -c -x assembler",
        opts: ""
    };

    /*
     *  ======== profiles ========
     *
     *  -fdata-sections is removed from "release" because of
     *  https://bugs.launchpad.net/gcc-linaro/+bug/1329080. It applies only to
     *  Linux-on-Arm targets. The bare metal targets specify -fdata-sections
     *  in their ccOpts, so this change doesn't affect them.
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
                copts: "-O2 -ffunction-sections",
            },
            linkOpts: "-Wl,--gc-sections",
        }],

        ["profile", {
            compileOpts: {
                copts: "-g -pg",
            },
            linkOpts: "-pg"     /* can't use -static here */
        }],

        ["coverage", {
            compileOpts: {
                copts: "-fprofile-arcs -ftest-coverage",
            },
            linkOpts: "-fprofile-arcs -ftest-coverage",
        }],
    ];

    /*!
     *  ======== includeOpts ========
     *  Additional user configurable target-specific include path options
     */
    override config string includeOpts = "";

    override config String binaryParser = "xdc.targets.omf.Elf";

    /*!
     *  ======== bspLib ========
     *  bspLib is assigned the name of the BSP library. The specified library
     *  is included on the link line.
     */
    config string bspLib = null;

    /*
     *  ======== initVers ========
     *  The function that detects the version of the Gnu compiler.
     */
    String initVers();

    /*!
     *  ======== asmName ========
     *  The function that converts a C name into an assembly name
     */
    String asmName(String CName);
}
/*
 *  @(#) gnu.targets.arm; 1, 0, 0,; 7-28-2021 06:57:31; /db/ztree/library/trees/xdctargets/xdctargets-w20/src/ xlibrary

 */

