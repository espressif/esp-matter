/* 
 *  Copyright (c) 2013 Texas Instruments and others.
 *  All rights reserved. This program and the accompanying materials
 *  are made available under the terms of the Eclipse Public License v1.0
 *  which accompanies this distribution, and is available at
 *  http://www.eclipse.org/legal/epl-v10.html
 *
 *  Contributors:
 *      Texas Instruments - initial implementation
 *
 * */

function getLibs (prog)
{
    var suffix = prog.build.target.suffix;

    if (suffix == null) {
        return (null);
    }

    var libs = "lib/" + this.profile + "/boot.a" + suffix;
    if (prog.targetModules().length > 0 && this.Settings.bootOnly == false) {
        libs = "lib/" + this.profile + "/" + this.$name + ".a" + suffix + ";"
            + libs;
    }

    /* Vector table is conditionally included */
    var VectorTable = xdc.module('iar.targets.arm.rts.VectorTable');
    if (VectorTable.getVectorTableLib) {
        libs += ";lib/" + this.profile + "/iar_vectortable.a" + suffix;
    }

    return (libs);
}

/*
 *  ======== Package.getSects ========
 */
function getSects()
{
    /* Add the linker command conditionally */
    var VectorTable = xdc.module('iar.targets.arm.rts.VectorTable');
    if (VectorTable.getVectorTableLib) {
        return ("iar/targets/arm/rts/linkcmd.xdt");
    }
    else {
        return ("");
    }
}
/*
 *  @(#) iar.targets.arm.rts; 1, 0, 0,; 7-28-2021 07:05:45; /db/ztree/library/trees/xdctargets/xdctargets-w20/src/ xlibrary

 */

