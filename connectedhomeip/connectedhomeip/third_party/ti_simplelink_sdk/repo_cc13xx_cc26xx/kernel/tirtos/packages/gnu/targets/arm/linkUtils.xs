/*
 *  Copyright 2021 by Texas Instruments Incorporated.
 *
 */

/*
 * Copyright (c) 2019 Texas Instruments Incorporated - http://www.ti.com
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 *   Redistributions of source code must retain the above copyright
 *   notice, this list of conditions and the following disclaimer.
 *
 *   Redistributions in binary form must reproduce the above copyright
 *   notice, this list of conditions and the following disclaimer in the
 *   documentation and/or other materials provided with the
 *   distribution.
 *
 *   Neither the name of Texas Instruments Incorporated nor the names of
 *   its contributors may be used to endorse or promote products derived
 *   from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 * A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 * OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

/*
 *  ======== genContributions ========
 *
 *  This function invokes getSects() for all loaded packages, and then returns
 *  the combined output.
 */
function genContributions(args)
{
    var res = "";
    for (var i = 0; i < xdc.om.$packages.length; i++) {
        var pkg = xdc.om.xdc.IPackage.Module($om.$packages[i]);

        var sectFile = pkg.getSects();
        if (sectFile) {
        res += "\n/* Content from " + pkg.$name + " (" + sectFile + "): */\n";
            var tplt = xdc.loadTemplate(sectFile);
            var bout = new java.io.ByteArrayOutputStream();
            var out = new java.io.PrintStream(bout);
            tplt.genStream(out, pkg, args);
            res += bout.toString();
        }
    }
    return (res);
}

/*
 *  ======== genSections ========
 *
 *  This function generates section allocations.
 */
function genSections(prog)
{
    var res = "";

    /* the section map is constructed from the target and program info */
    var sectMap = prog.getSectMap();

    if (sectMap['xdc.meta'] == undefined) {
        sectMap['xdc.meta'] = new prog.SectionSpec();
        sectMap['xdc.meta'].loadSegment = prog.platform.codeMemory;
        sectMap['xdc.meta'].type = "COPY";
    }

    /* 'xdc.noload' section is not allocated or loaded, if
     * xdc.runtime.Text.isLoaded is 'false'.
     */
    if ("xdc.runtime" in xdc.om && xdc.om['xdc.runtime.Text'].isLoaded != true
        && sectMap['xdc.noload'] == undefined) {
        var xv = Packages.xdc.services.intern.gen.Glob.vers().substr(9, 3);
        if (xv  < "A58" || xv > "a00") {
            sectMap['xdc.noload'] = new prog.SectionSpec();
            sectMap['xdc.noload'].loadSegment = prog.platform.codeMemory;
            sectMap['xdc.noload'].type = "COPY";
        }
    }

    var exclude = ("sectionsExclude" in prog)
                    ? new RegExp(prog.sectionsExclude): null;
    for (var sn in sectMap) {
        if (exclude == null || sn.match(exclude) == null) {
            /* only generate sections the user doesn't handle "manually" */
            res += "    " + sectLine(sectMap, sn) + "\n";
        }
    }

    /* filter content using prog.sectionsTemplate */
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
 *  ======== sectLine ========
 *  Generate a line to be placed in a linker command file for the specified
 *  section.
 */
function sectLine(sectMap, sn)
{

    /* map section name to a memory name */
    if (sectMap[sn] == null) {
        throw new Error("Allocation for section name '" + sn
            + "' is not defined by the program '" + prog.name
            + "' or the platform '" + Program.platformName + "'");
    }

    var map = sectMap[sn];
    var res = sn + " ";

    if (sn[sn.length-1] != '}') {
        if (map.runAddress != null && map.runAddress != undefined) {
            res += "0x" + map.runAddress.toString(16) + " ";
        }
        if (map.type != null && map.type!= undefined) {
            res += "(" + map.type + ") ";
        }
        res += ": ";

        if (map.loadAddress != null && map.loadAddress != undefined) {
            res += "AT (0x" + map.loadAddress.toString(16) + ") ";
        }

        if (map.loadAlign != null && map.loadAlign != undefined) {
            res += "ALIGN (0x" + map.loadAlign.toString(16) + ") ";
        }
        else if (map.runAlign != null && map.runAlign != undefined) {
            res += "ALIGN (0x" + map.runAlign.toString(16) + ") ";
        }

        if (sn == "xdc.meta" || sn == "xdc.noload") {
            res += "{KEEP(*(" + sn + "))} ";
        }
        else {
            res += "{*(" + sn + ")} ";
        }
    }

    if ( (sectMap[sn].runSegment != null) &&
         (sectMap[sn].runSegment != undefined) ) {
        res += " > " + sectMap[sn].runSegment;
    }

    if ( (sectMap[sn].loadSegment != null) &&
         (sectMap[sn].loadSegment != undefined) ) {
        res += " AT> " + sectMap[sn].loadSegment;
    }

    if ("fill" in map && map.fill != null && map.fill != undefined) {
        res += "= 0x" + map.fill.toString(16);
    }

    return (res);
}
/*
 *  @(#) gnu.targets.arm; 1, 0, 0,; 7-28-2021 06:57:32; /db/ztree/library/trees/xdctargets/xdctargets-w20/src/ xlibrary

 */

