/* 
 *  Copyright (c) 2008 Texas Instruments and others.
 *  All rights reserved. This program and the accompanying materials
 *  are made available under the terms of the Eclipse Public License v1.0
 *  which accompanies this distribution, and is available at
 *  http://www.eclipse.org/legal/epl-v10.html
 *
 *  Contributors:
 *      Texas Instruments - initial implementation
 *
 * */
/*
 *  ======== C64P.xdc ========
 */

/*!
 *  @_nodoc
 *  ======== C64P ========
 *  TI C64P little endian (ELF)
 */
metaonly module C64P inherits ITarget {
    override readonly config string name                 = "C64P";
    override readonly config string suffix               = "e64P";
    override readonly config string isa                  = "64P";
    override readonly config xdc.bld.ITarget.Model model = {
        endian: "little",
        shortEnums: false
    };
    override readonly config xdc.bld.ITarget.Module base = ti.targets.C64P;

    /*
     *  ======== compatibleSuffixes ========
     */
    override config String compatibleSuffixes[] = ["", "e64", "e62"];

    override config string platform     = "ti.platforms.sim6xxx:TMS320CDM420";

    override readonly config xdc.bld.ITarget2.Command cc = {
        cmd: "cl6x -c",
        opts: "-mv64p --abi=eabi"
    };

    override readonly config xdc.bld.ITarget2.Command asm = {
        cmd: "cl6x -c",
        opts: "-mv64P --abi=eabi"
    };

    override readonly config Int bitsPerChar =
        ti.targets.C64P.bitsPerChar;
}
/*
 *  @(#) ti.targets.elf; 1, 0, 0,; 7-28-2021 06:57:45; /db/ztree/library/trees/xdctargets/xdctargets-w20/src/ xlibrary

 */

