/* 
 *  Copyright (c) 2008-2015 Texas Instruments Incorporated
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
 *  ======== IM4.xdc ========
 *  Cortex M4 little endian thumb2 target
 */
metaonly interface IM4 inherits ti.targets.arm.elf.IArm {
    override readonly config string isa         = "v7M4";

    override config string platform   = "ti.platforms.tiva:TM4C1294NCPDT:1";

    override readonly config xdc.bld.ITarget.Model model = {
        endian: "little",
        codeModel: "thumb2",
        shortEnums: true
    };

    override readonly config xdc.bld.ITarget2.Command lnk = {
        cmd:  "armcl",
        opts: "--silicon_version=7M4 -z --strict_compatibility=on"
    };
}
/*
 *  @(#) ti.targets.arm.elf; 1, 0, 0,; 7-28-2021 06:57:41; /db/ztree/library/trees/xdctargets/xdctargets-w20/src/ xlibrary

 */

