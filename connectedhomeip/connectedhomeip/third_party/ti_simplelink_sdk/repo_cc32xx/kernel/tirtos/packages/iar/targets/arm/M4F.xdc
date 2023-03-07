/* 
 *  Copyright (c) 2013-2015 Texas Instruments and others.
 *  All rights reserved. This program and the accompanying materials
 *  are made available under the terms of the Eclipse Public License v1.0
 *  which accompanies this distribution, and is available at
 *  http://www.eclipse.org/legal/epl-v10.html
 *
 *  Contributors:
 *      Texas Instruments - initial implementation
 *
 * */
/*!
 *  ======== M4F ========
 *  IAR M4F target
 */
metaonly module M4F inherits ITarget
{
    override readonly config String name = "M4F";

    override readonly config String suffix = "rm4f";

    override readonly config String isa  = "v7M4";

    override config string platform  = "ti.platforms.tiva:TM4C1294NCPDT:1";

    /*!
     *  ======== cc ========
     *  The command used to compile C/C++ source files into object files
     */
    override readonly config Command cc = {
        cmd: "iccarm",
        opts: "--aeabi --cpu=Cortex-M4F --diag_suppress=Pa050,Go005 --endian=little -e --fpu=VFPv4_sp --thumb"
    };

    /*!
     *  ======== asm ========
     *  The command used to assembles assembly source files into object files
     */
    override readonly config Command asm = {
        cmd: "iasmarm",
        opts: "--cpu Cortex-M4F --endian little --fpu VFPv4_sp "
    };

    /*!
     *  ======== lnk ========
     *  The command used to link executables.
     */
    override readonly config Command lnk = {
        cmd: "ilinkarm",
        opts: "--cpu=Cortex-M4F"
    };
}
/*
 *  @(#) iar.targets.arm; 1, 0, 0,; 7-28-2021 06:57:36; /db/ztree/library/trees/xdctargets/xdctargets-w20/src/ xlibrary

 */

