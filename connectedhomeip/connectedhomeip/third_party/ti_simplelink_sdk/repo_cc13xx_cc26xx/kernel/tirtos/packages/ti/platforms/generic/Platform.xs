/*
 * Copyright (c) 2016, Texas Instruments Incorporated
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * *  Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *
 * *  Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * *  Neither the name of Texas Instruments Incorporated nor the names of
 *    its contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 * THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 * OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 * OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

/*
 *  ======== Platform.xs ========
 *  Platform support for the generic platform
 *
 */

var Utils = xdc.useModule('xdc.platform.Utils');

/*
 *  ======== Platform.getCpuDataSheet ========
 *  This Platform's implementation xdc.platform.IPlatform.getCpuDataSheet.
 *
 *  Return the xdc.platform.ICpuDataSheet.Instance object that corresponds
 *  to the device that runs executables loaded on the specified cpuId.
 */
function getCpuDataSheet(cpuId)
{
    return (Utils.getCpuDataSheet(this.CPU));
}

/*
 *  ======== Platform.getExeContext ========
 *  This Platform's implementation xdc.platform.IPlatform.getExeContext.
 *
 *  Return the xdc.platform.IPlatform.ExeContext object that will run the
 *  specified program prog.
 */
function getExeContext(prog)
{
    /* create an execution context using the "stock" implementation */
    var ExeContext = xdc.useModule('xdc.platform.ExeContext');
    var cpu = ExeContext.create(this.CPU, this.$module.BOARD);

    cpu.memoryMap = Utils.assembleMemoryMap(cpu, this, true);

    if (prog.build.target.isa == "28" || prog.build.target.isa == "28FP") {
        for (var i = 0; i < cpu.memoryMap.length; i++) {
            if (cpu.memoryMap[i].page == null) {
                if (cpu.memoryMap[i].name == "PIEVECT") {
                    cpu.memoryMap[i].page = 1;
                }
                else {
                    cpu.memoryMap[i].page = 0;
                }
            }
        }
    }

    // check for the overlap in the memory map
    var overlap = Utils.checkOverlap(cpu.memoryMap);

    if (overlap != null) {
        this.$module.$logError("Memory objects " + overlap + " overlap", this,
            overlap);
    }

    /* assign values to codeMemory and dataMemory */
    initDefaultMemory(this, cpu.memoryMap);
    Utils.checkDefaults(this, cpu.memoryMap);
    return (cpu);
}

/*
 *  ======== Platform.getExecCmd ========
 *  This Platform's implementation of xdc.platform.IPlatform.getExecCmd.
 */
function getExecCmd(prog)
{
    return ("@$(ECHO) " + this.$package.$name + " cannot run " + prog.name);
}

/*
 *  ======== Platform.getLinkTemplate ========
 *  This is Platform's implementation of xdc.platform.IPlatform.getLinkTemplate
 */
function getLinkTemplate(prog)
{
    /* use the target's linker command template */
    /* we compute the target because this same platform is supported by
     * many different tool chains; e.g., ti.targets, gnu.targets, ...
     */
    var tname = prog.build.target.$name;
    var tpkg = tname.substring(0, tname.lastIndexOf('.'));
    var templateName = tpkg.replace(/\./g, "/") + "/linkcmd.xdt";

    /* Use bare metal linker command file if appropriate */
    if (tname.match(/gnu.targets.arm.A/)) {
        templateName = tpkg.replace(/\./g, "/") + "/linkcmd_bm_v7a.xdt";
    }

    if (xdc.findFile(templateName) != null) {
        return (templateName);
    }
    else if (tname.indexOf("ti.targets.") == 0) {
        return ("ti/targets/linkcmd.xdt");
    }
    else {
        throw new Packages.xdc.services.global.XDCException(
            this.$package.$name + ".LINK_TEMPLATE_ERROR",
            "Target package " + tpkg + " does not contain linker command "
            + "template 'linkcmd.xdt'.");
    }
}

/*
 *  ======== Platform.Instance.init ========
 *  This function is called to initialize a newly created instance of a
 *  platform.  Platform instances are created just prior to running
 *  program configuration scripts.
 *
 *  Platform instances may also be created in the build domain.
 *
 *  @param(name)        the name used to identify this instance (without
 *                      the package name prefix)
 */
function instance$meta$init(name)
{
    if (this.deviceName == undefined) {
        this.$module.$logError("Parameter deviceName must be specified for "
            + "'ti.platforms.generic' platform.", this, this.deviceName);
    }

    if (this.catalogName == undefined) {
        this.$module.$logError("Parameter catalogName must be specified for "
            + "'ti.platforms.generic' platform.", this, this.deviceName);
    }

    if (this.clockRate == undefined) {
        this.$module.$logError("Parameter clockRate must be specified for "
            + "'ti.platforms.generic' platform.", this, this.clockRate);
    }

    /* since we are a generic platform we don't know the catalog package until
     * now, we must dynamically import it here; "real" platforms would declare
     * this statically in their package.xdc file.
     */
    xdc.loadPackage(this.catalogName);

    if (!(this.deviceName in xdc.om[this.catalogName])) {
        this.$module.$logError("Device " + this.deviceName + " does not"
            + " exist in " + this.catalogName, this, this.deviceName);
    }

    this.CPU.deviceName = this.deviceName;
    this.CPU.catalogName = this.catalogName;
    this.CPU.clockRate = this.clockRate;
}

/*
 *  ======== initDefaultMemory ========
 *  Initialize platform's config parameters that define in which memory
 *  segments to place code and data sections
 */
function initDefaultMemory(plat, memMap)
{
    /* We need to automatically select codeMemory, dataMemory and stackMemory
     * if they are not defined. The three flags defined below are used to
     * differentiate between user selected objects and selections made in this
     * function. If we already selected a memory object, when we later find
     * another one that could be used and has more space, we switch to that
     * new one. But, if the user selected a memory object, we don't want to
     * redefine it.
     */
    var userCodeMem = (plat.codeMemory != null);
    var userDataMem = (plat.dataMemory != null);
    var userStackMem = (plat.stackMemory != null);

    /* The references to the select memory objects are needed to be able to
     * quickly read their 'len' parameters.
     */
    var codeObj = null;
    var dataObj = null;

    /* look for a memory of the right type */
    for (var j = 0; j < memMap.length; j++) {
        var mem = memMap[j];
        /* If neither 'space' nor 'page' are defined, the memory segment
         * can be used for both, code and data.
         */
        if (mem.space == undefined && mem.page == undefined) {
            if (!userCodeMem && (codeObj == null || codeObj.len < mem.len)) {
                codeObj = mem;
                plat.codeMemory = codeObj.name;
            }
            if (!userDataMem && (dataObj == null || dataObj.len < mem.len)) {
                dataObj = mem;
                plat.dataMemory = dataObj.name;
            }
        }

        else if (mem.space != undefined) {
            if (mem.space.indexOf("code") >= 0) {
                if (!userCodeMem &&
                    (codeObj == null || codeObj.len < mem.len)) {
                    codeObj = mem;
                    plat.codeMemory = codeObj.name;
                }
            }
            if (mem.space.indexOf("data") >= 0) {
                if (!userDataMem &&
                    (dataObj == null || dataObj.len < mem.len)) {
                    dataObj = mem;
                    plat.dataMemory = dataObj.name;
                }
            }
        }

        else if (mem.page != undefined) {
            if (mem.page == 0) {
                if (!userCodeMem &&
                    (codeObj == null || codeObj.len < mem.len)) {
                    codeObj = mem;
                    plat.codeMemory = codeObj.name;
                }
            }
            if (mem.page == 1) {
                if (!userDataMem &&
                    (dataObj == null || dataObj.len < mem.len)) {
                    dataObj = mem;
                    plat.dataMemory = dataObj.name;
                }
            }

        }
    }

    /* if any of two config parameters is null, we cannot continue because the
     * name of the segment for some section would be 'null'.
     */
    if (plat.dataMemory == null) {
        throw new Error("dataMemory in the generic platform is null");
    }
    if (plat.codeMemory == null) {
        throw new Error("codeMemory in the generic platform is null");
    }
    if (!userStackMem) {
        plat.stackMemory = plat.dataMemory;
    }
}
