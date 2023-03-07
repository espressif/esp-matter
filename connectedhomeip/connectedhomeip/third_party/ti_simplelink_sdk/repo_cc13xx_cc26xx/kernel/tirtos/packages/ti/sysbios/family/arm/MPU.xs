/*
 * Copyright (c) 2015-2020, Texas Instruments Incorporated
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
 *  ======== MPU.xs ========
 */

var BIOS = null;
var Build = null;
var Cache = null;
var device = null;
var MPU = null;
var Settings = null;
var Startup = null;

/*
 * ======== getAsmFiles ========
 * return the array of assembly language files associated
 * with targetName (ie Program.build.target.$name)
 */
function getAsmFiles(targetName)
{
    switch(targetName) {
        case "ti.targets.arm.elf.R4F":
        case "ti.targets.arm.elf.R4Ft":
        case "ti.targets.arm.elf.R5F":
        case "ti.targets.arm.elf.R5Ft":
            return (["MPU_asm.sv7R"]);
            break;

        default:
            return (null);
            break;
    }
}

if (xdc.om.$name == "cfg" || typeof(genCdoc) != "undefined") {
    var deviceTable = {
        "RM57D8XX": {
            isMemoryMapped  : false,
            numRegions      : 16
        },
        "AWR16XX": {
            isMemoryMapped  : false,
            numRegions      : 12
        },
        "MSP432P401R": {
            isMemoryMapped  : true,
            numRegions      : 8
        },
        "CC26.2.*": {
            isMemoryMapped  : true,
            numRegions      : 8
        }
    };

    /* Cortex-R devices */
    deviceTable["AWR1.*"]        = deviceTable["AWR16XX"];
    deviceTable["IWR1.*"]        = deviceTable["AWR16XX"];
    deviceTable["IWR6.*"]        = deviceTable["AWR16XX"];
    deviceTable["RM48L.*"]       = deviceTable["AWR16XX"];
    deviceTable["RM57D8.*"]      = deviceTable["RM57D8XX"];
    deviceTable["RM57L8XX"]      = deviceTable["RM57D8XX"];
    deviceTable["RM57L8.*"]      = deviceTable["RM57D8XX"];

    /* MSP432 devices */
    deviceTable["MSP432.*"]      = deviceTable["MSP432P401R"];

    /* Tiva devices */
    deviceTable["TM4C129CNCPDT"] = deviceTable["MSP432P401R"];
    deviceTable["TM4.*"]         = deviceTable["MSP432P401R"];

    /* SimpleLink devices */
    deviceTable["CC13.1.*"] = deviceTable["CC26.2.*"];
    deviceTable["CC26.1.*"] = deviceTable["CC26.2.*"];
    deviceTable["CC13.2.*"] = deviceTable["CC26.2.*"];

    /* Keystone3 devices */
    deviceTable["AM65.*"]     = deviceTable["RM57D8XX"];
    deviceTable["J7.*"]       = deviceTable["RM57D8XX"];
}

/*
 *  ======== deviceSupportCheck ========
 *  Check validity of device
 */
function deviceSupportCheck()
{
    /* look for exact match first */
    for (device in deviceTable) {
        if (device == Program.cpu.deviceName) {
            return (device);
        }
    }

    /* now look for a wildcard match */
    for (device in deviceTable) {
        if (Program.cpu.deviceName.match(device)) {
            return (device);
        }
    }

    return (null);
}

/*
 *  ======== module$meta$init ========
 */
function module$meta$init()
{
    /* Only process during "cfg" phase */
    if (xdc.om.$name != "cfg") {
        return;
    }

    this.$private.getAsmFiles = getAsmFiles;

    MPU = this;

    /* set fxntab default */
    MPU.common$.fxntab = false;

    device = deviceSupportCheck();

    if (device != null) {
        MPU.isMemoryMapped = deviceTable[device].isMemoryMapped;
        MPU.numRegions = deviceTable[device].numRegions;
        MPU.regionEntry.length = MPU.numRegions;
        for (var i = 0; i < MPU.numRegions; i++) {
            MPU.regionEntry[i].baseAddress = 0;
            MPU.regionEntry[i].sizeAndEnable = 0;
            MPU.regionEntry[i].regionAttrs = 0;
        }
    }
    else {
        MPU.numRegions = 0;
        MPU.isMemoryMapped = true;
        MPU.regionEntry.length = 0;
    }
}

/*
 *  ======== module$use ========
 */
function module$use()
{
    BIOS = xdc.module('ti.sysbios.BIOS');
    Build = xdc.useModule('ti.sysbios.Build');
    Cache = xdc.useModule('ti.sysbios.hal.Cache');
    Startup = xdc.useModule('xdc.runtime.Startup');

    if (device == null) {
        /*
         * no match, print all catalog devices supported
         * and then raise an error
         */
        var catalog = xdc.loadPackage(Program.cpu.catalogName);

        /* build associative array of supported devices */
        var supportedDevices = new Object();

        for (var i = 0; i < catalog.$modules.length; i++) {
            var catalogName = catalog.$modules[i].$name.substring(
                Program.cpu.catalogName.length + 1);

            for (device in deviceTable) {
                if (catalogName.match(device)) {
                    supportedDevices[catalogName] = catalogName;
                }
            }
        }

        /* copy it into a sortable array */
        var sd = new Array();

        for (var i in supportedDevices) {
            sd[sd.length++] = i;
        }

        /* sort it for a nicer report */
        sd.sort();

        print("The " + Program.cpu.deviceName +
              " device is not currently supported.");
        print("The following devices are supported for the " +
            Program.build.target.name + " target:");

        for (var i=0; i<sd.length; i++) {
            print("\t" + sd[i]);
        }

        throw new Error ("Unsupported device!");
    }

    /* add -D to compile line to optimize MPU_isMemoryMapped check */
    Build.ccArgs.$add("-Dti_sysbios_family_arm_MPU_isMemoryMapped__D=" +
        (MPU.isMemoryMapped ? "TRUE" : "FALSE"));
}

/*
 *  ======== module$static$init ========
 */
function module$static$init(mod, params)
{
    mod.regionEntry.length = MPU.numRegions;

    for (var i = 0; i < MPU.numRegions; i++) {
        mod.regionEntry[i].baseAddress =
            MPU.regionEntry[i].baseAddress;
        mod.regionEntry[i].sizeAndEnable =
            MPU.regionEntry[i].sizeAndEnable;
        mod.regionEntry[i].regionAttrs =
            MPU.regionEntry[i].regionAttrs;
    }

    if (Cache.CacheProxy.delegate$.$name.match(/ti\.sysbios\.hal\.CacheNull/)) {
        Startup.firstFxns.$add(MPU.startup);
    }
}

/*
 *  ======== initRegionAttrsMeta ========
 */
function initRegionAttrsMeta(attrs)
{
    /* Set all fields to their default value */
    attrs.enable = MPU.defaultAttrs.enable;
    attrs.bufferable = MPU.defaultAttrs.bufferable;
    attrs.cacheable = MPU.defaultAttrs.cacheable;
    attrs.shareable = MPU.defaultAttrs.shareable;
    attrs.noExecute = MPU.defaultAttrs.noExecute;
    attrs.accPerm = MPU.defaultAttrs.accPerm;
    attrs.tex = MPU.defaultAttrs.tex;
    attrs.subregionDisableMask = MPU.defaultAttrs.subregionDisableMask;
}

/*
 *  ======== setiRegionMeta ========
 */
function setRegionMeta(regionId, regionBaseAddr, regionSize, attrs)
{
    var index;

    if (MPU.numRegions == 0) {
        MPU.$logError("MPU_setRegionMeta: Number of MPU regions " +
              "(MPU.numRegions) is set to 0. The number of regions must be " +
              "non-zero.", MPU);
    }

    /* Set enable field to default if undefined */
    if (attrs.enable === undefined) {
        attrs.enable = MPU.defaultAttrs.enable;
    }

    /* Set bufferable field to default if undefined */
    if (attrs.bufferable === undefined) {
        attrs.bufferable = MPU.defaultAttrs.bufferable;
    }

    /* Set cacheable field to default if undefined */
    if (attrs.cacheable === undefined) {
        attrs.cacheable = MPU.defaultAttrs.cacheable;
    }

    /* Set shareable field to default if undefined */
    if (attrs.shareable === undefined) {
        attrs.shareable = MPU.defaultAttrs.shareable;
    }

    /* Set noExecute field to default if undefined */
    if (attrs.noExecute === undefined) {
        attrs.noExecute = MPU.defaultAttrs.noExecute;
    }

    /* Set accPerm field to default if undefined */
    if (attrs.accPerm === undefined) {
        attrs.accPerm = MPU.defaultAttrs.accPerm;
    }

    /* Set tex field to default if undefined */
    if (attrs.tex === undefined) {
        attrs.tex = MPU.defaultAttrs.tex;
    }

    /* Set subregionDisableMask to default if undefined */
    if (attrs.subregionDisableMask === undefined) {
        attrs.subregionDisableMask = MPU.defaultAttrs.subregionDisableMask;
    }

    if ((regionId >= 0) && (regionId < MPU.numRegions)) {
        var alignMask;
        if (regionSize == MPU.RegionSize_4G) {
            alignMask = 0x0;
        }
        else {
            alignMask = convertToUInt32(~0 << ((regionSize >>> 1) + 1));
        }

        if (regionBaseAddr != convertToUInt32(regionBaseAddr & alignMask)) {
            MPU.$logWarning("Region base address 0x" +
                regionBaseAddr.toString(16) + " not aligned to region size.",
                MPU);
        }

        MPU.regionEntry[regionId].baseAddress = regionBaseAddr;

        MPU.regionEntry[regionId].sizeAndEnable =
            ((attrs.subregionDisableMask << 8) | regionSize | attrs.enable);

        if ((attrs.tex == 1 &&
             attrs.cacheable == false && attrs.bufferable == true) ||
            (attrs.tex == 1 &&
             attrs.cacheable == true && attrs.bufferable == false)) {

            MPU.$logError("MPU Region attributes for region number " + regionId + " set to reserved combination: tex = 1, cacheable = " + attrs.cacheable + ", bufferable = " + attrs.bufferable, MPU);
        }

        MPU.regionEntry[regionId].regionAttrs =
            convertToUInt32((attrs.noExecute << 12) | (attrs.accPerm << 8) |
            (attrs.tex << 3) | (attrs.shareable << 2) | (attrs.cacheable << 1) |
            (attrs.bufferable));
    }
    else {
        MPU.$logError("MPU Region number (" + regionId + ") passed is " +
            "invalid.", MPU);
    }
}

/*
 *  ======== convertToUInt32 ========
 */
function convertToUInt32(value)
{
    /*
     *  Provide this workaround here due to javascript.
     *  Note: Javascript converts numbers to 32-bit signed numbers before
     *        evaluating bitwise operators.
     */
    if (value < 0) {
        value = value + 0xffffffff + 1;
    }

    return (value);
}

/*
 *  ======== viewPopulateRegionAttrs ========
 */
function viewPopulateRegionAttrs(regionIdx, elem, baseAddress, sizeAndEnable,
    regionAttrs)
{
    elem.RegionIdx = regionIdx;
    elem.Enabled = sizeAndEnable & 0x1;

    if (elem.Enabled) {
        elem.BaseAddress = "0x" + baseAddress.toString(16);
        elem.SubregionDisableMask = "0x" +
            ((sizeAndEnable >>> 8) & 0xFF).toString(16);

        /* Determine MPU region size */
        var sizeIdx = (sizeAndEnable >>> 1) & 0x1F;

        if (sizeIdx < 31) {
            var regionSize = 1 << (sizeIdx + 1);

            if ((regionSize / 1024) < 1) {
                regionSize = Number(regionSize).toString(10) + " Bytes";
            }
            else if ((regionSize / (1024*1024)) < 1) {
                regionSize = Number(regionSize/1024).toString(10) + " KBytes";
            }
            else if ((regionSize / (1024*1024*1024)) < 1) {
                regionSize = Number(regionSize/(1024*1024)).toString(10) +
                             " MBytes";
            }
            else {
                regionSize = Number(regionSize/(1024*1024*1024)).toString(10) +
                             " GBytes";
            }
        }
        else {
            var regionSize = "4 GBytes";
        }

        elem.Size = regionSize;
        elem.Bufferable = regionAttrs & 0x1;
        elem.Cacheable = (regionAttrs >>> 1) & 0x1;
        elem.Shareable = (regionAttrs >>> 2) & 0x1;
        elem.Noexecute = (regionAttrs >>> 12) & 0x1;
        elem.Tex = "0x" + ((regionAttrs >>> 3) & 0x7).toString(16);

        /* Determine MPU region access permissions */
        var accPerm = (regionAttrs >>> 8) & 0x7;
        switch (accPerm) {
            case 0: elem.AccessPerm = "No Access at any Privilege Level";
                    break;
            case 1: elem.AccessPerm = "Supervisor RW access";
                    break;
            case 2: elem.AccessPerm = "Supervisor RW and User R-only access";
                    break;
            case 3: elem.AccessPerm = "Supervisor and User RW access";
                    break;
            case 4: elem.AccessPerm = "Unpredictable";
                    break;
            case 5: elem.AccessPerm = "Supervisor R-only";
                    break;
            case 6: elem.AccessPerm = "Supervisor and User R-only access";
                    break;
            case 7: elem.AccessPerm = "Unpredictable";
                    break;
            default:
                    elem.AccessPerm = "Unpredictable";
        }
    }
    else {
        elem.BaseAddress = "";
        elem.SubregionDisableMask = "";
        elem.Size = "";
        elem.Bufferable = undefined;
        elem.Cacheable = undefined;
        elem.Shareable = undefined;
        elem.Noexecute = undefined;
        elem.Tex = "";
        elem.AccessPerm = "";
    }
}

/*
 *  ======== viewMpuRegionAttrs ========
 */
function viewMpuRegionAttrs(view)
{
    var mpuRegionEntry;
    var Program = xdc.useModule('xdc.rov.Program');
    var MPU = xdc.useModule('ti.sysbios.family.arm.MPU');
    var MPUCfg = Program.getModuleConfig(MPU.$name);

    /* Scan the raw view in order to obtain the module state. */
    var rawView;
    try {
        rawView = Program.scanRawView('ti.sysbios.family.arm.MPU');
    }
    catch (e) {
        print(e.toString());
    }

    /* Walk through the level 1 descriptor table */
    for (var i = 0; i < MPUCfg.numRegions; i++) {
        var elem = Program.newViewStruct('ti.sysbios.family.arm.MPU',
                'MpuRegionAttrsView');

        viewPopulateRegionAttrs(i, elem, MPUCfg.regionEntry[i].baseAddress,
                MPUCfg.regionEntry[i].sizeAndEnable,
                MPUCfg.regionEntry[i].regionAttrs);

        /* Add the element to the list. */
        view.elements.$add(elem);
    }
}
