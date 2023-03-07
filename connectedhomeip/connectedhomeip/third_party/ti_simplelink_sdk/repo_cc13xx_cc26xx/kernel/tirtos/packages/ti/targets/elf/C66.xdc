/* 
 *  Copyright (c) 2010 Texas Instruments and others.
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
 *  ======== C66.xdc ========
 *
 */

/*!
 *  ======== C66 ========
 *  TI C66 little endian (ELF)
 */
metaonly module C66 inherits ITarget {
    override readonly config string name                 = "C66";       
    override readonly config string suffix               = "e66";
    override readonly config string isa                  = "66";
    override readonly config xdc.bld.ITarget.Model model = {
        endian: "little",
        shortEnums: false
    };
    override readonly config xdc.bld.ITarget.Module base = ti.targets.elf.C64P;

    /*
     *  ======== compatibleSuffixes ========
     */
    override config String compatibleSuffixes[] = ["e674", "e64P"];

    override config string platform     = "ti.platforms.simTCI6616";

    override readonly config xdc.bld.ITarget2.Command cc = {
        cmd: "cl6x -c",
        opts: "-mv6600 --abi=eabi"
    };

    override readonly config xdc.bld.ITarget2.Command asm = {
        cmd: "cl6x -c",
        opts: "-mv6600 --abi=eabi"
    };

    override readonly config Int bitsPerChar =
        ti.targets.elf.C64P.bitsPerChar;
}
/*
 *  @(#) ti.targets.elf; 1, 0, 0,; 7-28-2021 06:57:45; /db/ztree/library/trees/xdctargets/xdctargets-w20/src/ xlibrary

 */

