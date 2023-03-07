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

/*!
 *  ======== M3.xdc ========
 *  Cortex M3 little endian thumb2 (ELF)
 */
metaonly module M3 inherits ti.targets.arm.elf.IM3 {
    override readonly config string name        = "M3";
    override readonly config string suffix      = "em3";
    override readonly config string rts         = "ti.targets.arm.rtsarm";

    override readonly config xdc.bld.ITarget2.Command cc = {
        cmd:  "armcl -c",
        opts: "--endian=little -mv7M3 --abi=eabi"
    };

    override readonly config xdc.bld.ITarget2.Command asm = {
        cmd:  "armcl -c",
        opts: "--endian=little -mv7M3 --abi=eabi"
    };
}
/*
 *  @(#) ti.targets.arm.elf; 1, 0, 0,; 7-28-2021 06:57:41; /db/ztree/library/trees/xdctargets/xdctargets-w20/src/ xlibrary

 */

