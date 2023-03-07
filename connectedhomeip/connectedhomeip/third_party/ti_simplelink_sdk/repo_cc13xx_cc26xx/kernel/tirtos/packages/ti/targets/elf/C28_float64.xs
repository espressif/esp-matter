/* 
 *  Copyright (c) 2020 Texas Instruments and others.
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
 *  ======== C28_float64.xs ========
 */

/*
 *  ======== C28_float64.genConstCustom ========
 */
function genConstCustom(names)
{
    var sb = new java.lang.StringBuilder();
    for (var i = 0; i < names.length; i++) {
        sb.append('#pragma DATA_SECTION(' + names[i] + ', \".const:' + names[i]
            + '\");\n');
        sb.append('asm(\"\t.sect \\\".const:' + names[i] + '\\\"\");\n');
        sb.append('asm(\"\t.clink \");\n');

        /* fix for the alchemy problem with renaming sections */
        sb.append('asm(\"\t.sect \\\"[0].const:' + names[i] + '\\\"\");\n');
        sb.append('asm(\"\t.clink \");\n');
        sb.append('asm(\"\t.sect \\\"[1].const:' + names[i] + '\\\"\");\n');
        sb.append('asm(\"\t.clink \");\n\n');
    }
    return (sb.toString() + "");
}
/*
 *  @(#) ti.targets.elf; 1, 0, 0,; 7-28-2021 06:57:45; /db/ztree/library/trees/xdctargets/xdctargets-w20/src/ xlibrary

 */

