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
var coffUtils = xdc.loadCapsule("ti/targets/linkUtils.xs");

function genElfSections(prog)
{
    var res = "";

    /* the section map is constructed from the target and program info */
    var sectMap = prog.getSectMap();

    if (sectMap['xdc.meta'] == undefined) {
        sectMap['xdc.meta'] = new prog.SectionSpec(sectMap[".const"]);
        sectMap['xdc.meta'].type = "COPY";
    }

    if ("xdc.runtime" in xdc.om && xdc.om['xdc.runtime.Text'].isLoaded != true
        && sectMap['xdc.noload'] == undefined) {
        var xv = Packages.xdc.services.intern.gen.Glob.vers().substr(9, 3);
        if (xv  < "A58" || xv > "a00") {
            sectMap['xdc.noload'] = new prog.SectionSpec(sectMap[".text"]);
            sectMap['xdc.noload'].type = "COPY";
        }
    }

    if (sectMap['.args'] != undefined) {
        sectMap['.args'].loadAlign = 4;
        sectMap['.args'].runAlign = 4;
    }

    /* if the target version is 0, this is an *old* compiler/linker that
     * probably does not support splitting; we may need a more precise
     * target-specific heuristic.
     */
    var targetVers = prog.build.target.version.split(',')[2] - 0;
    var useSplit = targetVers > 0;
    var bssGroupDone = false;
    var exclude = ("sectionsExclude" in prog)
                    ? new RegExp(prog.sectionsExclude): null;
    for (var sn in sectMap) {
        if (exclude != null && sn.match(exclude) != null) {
            /* don't generate this section; the user will take care of this */
            continue;   
        }
        if (sn == ".bss" || sn == ".rodata" || sn == ".neardata") {
            if (!bssGroupDone) {
                var allocation = coffUtils.sectLine(sectMap, sn, ">");
                allocation = allocation.replace(sn, "GROUP");
                res += "    " + allocation + "\n    {\n";
                res += "        " + ".bss:\n";
                res += "        " + ".neardata:\n";
                res += "        " + ".rodata:\n";
                res += "    }\n";
                bssGroupDone = true;

                /* Check if .bss, .rodata, and .neardata go to the same
                 * segment. If some of them are not defined, just assume
                 * they are allocated in the same way as one of the defined
                 * section to make the check simpler.
                 */
                if (sectMap[".neardata"] != null) {
                    var nearAlloc =
                        coffUtils.sectLine(sectMap, ".neardata", ">");
                    nearAlloc = nearAlloc.replace(".neardata", "");
                }
                else {
                    var nearAlloc = coffUtils.sectLine(sectMap, sn, ">");
                    nearAlloc = nearAlloc.replace(sn, "");
                }
                if (sectMap[".bss"] != null) {
                    var bssAlloc = coffUtils.sectLine(sectMap, ".bss", ">");
                    bssAlloc = bssAlloc.replace(".bss", "");
                }
                else {
                    var bssAlloc = coffUtils.sectLine(sectMap, sn, ">");
                    bssAlloc = bssAlloc.replace(sn, "");
                }
                if (sectMap[".rodata"] != null) {
                    var roAlloc = coffUtils.sectLine(sectMap, ".rodata", ">");
                    roAlloc = roAlloc.replace(".rodata", "");
                }
                else {
                    var roAlloc = coffUtils.sectLine(sectMap, sn, ">");
                    roAlloc = roAlloc.replace(sn, "");
                }
                if(!(bssAlloc == nearAlloc && bssAlloc == roAlloc)) {
                    throw new Error("Allocations for sections '.bss', '.rodata'"
                        + " and '.neardata' must be same.");
                }
            }
        }
        else if (prog.build.target.splitMap[sn] != true) {
            res += "    " + coffUtils.sectLine(sectMap, sn, ">") + "\n";
        }
        else if (sn == ".args") {
            res += "    " + coffUtils.sectLine(sectMap, sn, ">") + ", fill = 0" 
                + " {_argsize = " + utils.toHex(prog.argSize) + "; }\n";
        }
        else {
            res += "    "
                + coffUtils.sectLine(sectMap, sn, useSplit ? ">>" : ">") + "\n";
        }
    }

    /* filter content using Program.sectionsTemplate */
    if ("sectionsTemplate" in prog && prog.sectionsTemplate != null) {
        var tplt = xdc.loadTemplate(prog.sectionsTemplate);
        var bout = new java.io.ByteArrayOutputStream();
        var out = new java.io.PrintStream(bout);
        tplt.genStream(out, prog, [sectMap, res]);
        res = bout.toString();
    }

    return (res);
}
/*
 *  @(#) ti.targets.elf; 1, 0, 0,; 7-28-2021 06:57:45; /db/ztree/library/trees/xdctargets/xdctargets-w20/src/ xlibrary

 */

