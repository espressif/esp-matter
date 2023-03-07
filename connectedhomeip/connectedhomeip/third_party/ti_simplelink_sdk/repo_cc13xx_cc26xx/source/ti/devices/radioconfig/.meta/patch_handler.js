/*
 * Copyright (c) 2019-2021 Texas Instruments Incorporated - http://www.ti.com
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
 *
 */

/*
 *  ======== patch_handler.js ========
 *  Functions for generating patch information
 */

"use strict";

// Common utility functions
const Common = system.getScript("/ti/devices/radioconfig/radioconfig_common.js");

// Module base path
const BasePath = Common.basePath;

// Command handler, contains patch information
const CmdHandler = Common.getScript("cmd_handler.js");

// File constants
const StandardIncludes = "#include <ti/devices/DeviceFamily.h>\n"
  + "#include DeviceFamily_constructPath(driverlib/rf_mailbox.h)\n"
  + driverLibCmdInclude("common");
const RfDriverInclude = "#include <ti/drivers/rf/RF.h>\n";
const RfInclude = "#include \"ti_radio_config.h\"\n";
const CoExIncludeDriverLib = {
    coex_ble: "#include DeviceFamily_constructPath(driverlib/rf_bt5_coex.h)\n",
    coex_ieee: "#include DeviceFamily_constructPath(driverlib/rf_ieee_coex.h)\n"
};

/*!
*  ======== generateIncludesH ========
*  Generated include directives for use in the header file (*.h)
*
*  @param modules - all modules in the SDK
*/
function generateIncludesH(modules) {
    let incl = StandardIncludes;

    _.each(modules, (modpath) => {
        if (modpath === BasePath + "settings/prop") {
            incl += driverLibCmdInclude("prop");
        }
        else if (modpath === BasePath + "settings/ble") {
            incl += driverLibCmdInclude("ble");
            // Handle wBMS
            const mod = system.modules[modpath];
            if (mod.hasWBMS()) {
                incl += driverLibCmdInclude("prop");
            }
        }
        else if (modpath === BasePath + "settings/ieee_15_4") {
            incl += driverLibCmdInclude("ieee");
        } // else: not a RadioConfig module
    });

    // RF driver
    incl += RfDriverInclude;

    return incl;
}

/*!
*  ======== generateIncludesC ========
*  Generated include directives for use in the implementation file (*.c)
*
*  @param modules - all modules in the SDK
*  @param coexType - ble or ieee
*/
function generateIncludesC(modules, coexType) {
    // Common includes (for compatibility with SmartRF Studio)
    let incl = RfInclude;
    let patchIncludes = {};

    _.each(modules, (modpath) => {
        let phyGroup;

        // Add DriverLib includes
        if (modpath === BasePath + "settings/prop") {
            phyGroup = Common.PHY_PROP;
        }
        else if (modpath === BasePath + "settings/ble") {
            phyGroup = Common.PHY_BLE;
        }
        else if (modpath === BasePath + "settings/ieee_15_4") {
            phyGroup = Common.PHY_IEEE_15_4;
        }
        else {
            // Not a RadioConfig module, skip
            return true;
        }

        // Aggregate patch includes
        const mod = system.modules[modpath];
        patchIncludes = Object.assign(patchIncludes, getPatchIncludes(phyGroup, mod.$instances, coexType));

        return true;
    });

    // Use Co-Ex DriverLib entry if applicable
    if (coexType) {
        incl += CoExIncludeDriverLib[coexType];
    }

    // RF driver
    // incl += RfDriverInclude;

    // Iterate patch includes
    _.each(patchIncludes, (value, patchName) => {
        incl += patchInclude(patchName);
    });

    // Radio Config header file
    // incl += RfInclude;

    return incl;
}

/*!
*  ======== getPatchIncludes ========
*  When generated code for multiple settings, header files
*  for all required patches must be included.
*
*  @param phyGroup - ble, prop or ieee_154
*  @param instances - list of instances to generate patch includes for
*  @param coexType - coex_ble or coex_ieee
*/
function getPatchIncludes(phyGroup, instances, coexType) {
    const includes = {};

    _.each(instances, (inst) => {
        let phy = null;

        if ("phyType" in inst) {
            phy = inst.phyType;
        }
        else if ("freqBand" in inst) {
            phy = Common.getPhyType(inst);
        }
        else {
            throw Error("Unknown PHY type");
        }

        let protocol;
        if (coexType) {
            protocol = coexType;
        }
        else {
            protocol = inst.codeExportConfig.useMulti ? "multi" : "single";
        }
        const patch = CmdHandler.get(phyGroup, phy).getPatchInfo(protocol);
        if (typeof (patch.cpe) === "string") {
            includes[patch.cpe] = true;
        }
        if (typeof (patch.rfe) === "string") {
            includes[patch.rfe] = true;
        }
        if (typeof (patch.mce) === "string") {
            includes[patch.mce] = true;
        }
    });
    return includes;
}

/*!
*  ======== driverLibCmdInclude ========
*  Generate include directive for DriverLib RF commands
*
*  @param phyGroup - ble, prop or ieee_154
*/
function driverLibCmdInclude(phyGroup) {
    return "#include DeviceFamily_constructPath(driverlib/rf_" + phyGroup + "_cmd.h)\n";
}

/*!
*  ======== patchInclude ========
*  Generate include directive for RF patches
*
*  @param patchName - name of the patch (e.g. rf_patch_cpe_bt5)
*/
function patchInclude(patchName) {
    return "#include DeviceFamily_constructPath(rf_patches/" + patchName + ".h)\n";
}

// Exported from this module
exports = {
    generateIncludesH: generateIncludesH,
    generateIncludesC: generateIncludesC
};
