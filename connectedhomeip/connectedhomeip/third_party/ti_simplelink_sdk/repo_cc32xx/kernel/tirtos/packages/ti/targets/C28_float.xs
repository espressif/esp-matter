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
 *  ======== C28_float.xs ========
 *
 *
 */

/*
 *  ======== C28_float.genConstCustom ========
 */
function genConstCustom(names)
{
    var sb = new java.lang.StringBuilder();
    for (var i = 0; i < names.length; i++) {
        sb.append('#pragma DATA_SECTION(' + names[i] + ', \".econst:' + names[i]
            + '\");\n');
        sb.append('asm(\"\t.sect \\\".econst:' + names[i] + '\\\"\");\n');
        sb.append('asm(\"\t.clink \");\n');

        /* fix for the alchemy problem with renaming sections */
        sb.append('asm(\"\t.sect \\\"[0].econst:' + names[i] + '\\\"\");\n');
        sb.append('asm(\"\t.clink \");\n');
        sb.append('asm(\"\t.sect \\\"[1].econst:' + names[i] + '\\\"\");\n');
        sb.append('asm(\"\t.clink \");\n\n');
    }
    return (sb.toString() + "");
}
/*
 *  @(#) ti.targets; 1, 0, 3,; 7-28-2021 06:57:38; /db/ztree/library/trees/xdctargets/xdctargets-w20/src/ xlibrary

 */

