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
 *  ======== Radio Configuration ========
 *  Radio configuration support.
 *
 *  Code used by to all RF Settings (Proprietary, BLE, TI 15.4 ...)
 */

"use strict";

// Common utility functions
const Common = system.getScript("/ti/devices/radioconfig/radioconfig_common.js");

// Other dependencies
const DeviceInfo = Common.getScript("device_info.js");
const CmdHandler = Common.getScript("cmd_handler.js");
const RfDesign = Common.getScript("rfdesign");

/*
 *  ======== reloadInstanceFromPhy ========
 *  Update all configurables when the frequency band or PHY type has changed
 *
 *  @param inst - module instance
 *  @param ui - module UI state
 *  @phyName - currently selected setting (PHY)
 *  @phyGroup - currently used PHY group (BLE, PROP, IEEE..)
 *  @preserve - list of configurables to preserve the value of
 */
function reloadInstanceFromPhy(inst, ui, phyName, phyGroup, preserve) {
    const cmdHandler = CmdHandler.get(phyGroup, phyName, inst.settingGroup === "Selectivity");
    const rfData = cmdHandler.getRfData();
    _.each(rfData, (value, key) => {
        // Do NOT refresh preserved configurables
        if (preserve.includes(key)) {
            return;
        }
        inst[key] = value;
    });

    if ("highPA" in inst) {
        updateTxPowerVisibility(inst, ui);

        const settingMap = DeviceInfo.getSettingMap(phyGroup);
        const settingInfo = _.find(settingMap, (s) => s.name === phyName);
        let hidden = false;

        if ("options" in settingInfo) {
            if (settingInfo.options.includes("disable_high_pa")) {
                hidden = true;
            }
        }
        ui.highPA.hidden = hidden;
    }
}

/*
 *  ======== validateRfParams ========
 *  Check that RF parameters are supported by dependant RF commands
 *
 *  @param inst - module instance
 *  @param validation - validation object
 *  @phyType - currently selected setting (PHY)
 *  @phyGroup - currently used PHY group
 */
function validateRfParams(inst, validation, phyType, phyGroup) {
    const cmdHandler = CmdHandler.get(phyGroup, phyType);
    const rfData = cmdHandler.getRfData();
    const usedCmds = cmdHandler.getUsedCommands(inst);

    _.each(rfData, (value, key) => {
        if (!("highPA" in inst && key.includes("txPower"))) {
            if (!cmdHandler.isParameterUsed(key, usedCmds)) {
                Common.logWarning(validation, inst, key,
                    "This RF parameter has no effect on the exported code "
                    + "(no corresponding RF command selected).");
            }
        }
    });
}

/*
 *  ======== validateFrontendSettings ========
 *  Check that RF parameters are supported by dependant RF commands
 *
 *  @param inst - module instance
 *  @param validation - validation object
 *  @phyGroup - currently used PHY group
 */
function validateFrontendSettings(inst, validation, phyGroup) {
    let freqBand = 2400;
    let cfgName = "fe24g";
    if ("freqBand" in inst) {
        freqBand = inst.freqBand;
        cfgName = "feSub1g";
    }
    const id = RfDesign.getFrontEnd(freqBand);
    const fe = CmdHandler.getFrontendSettings(phyGroup, id);
    if (fe === null) {
        const rfinst = RfDesign.$static;
        Common.logError(validation, rfinst, cfgName,
            "This frontend setting is not supported for the current RF Design");
    }
}

/*
 *  ======== updateTxPowerVisibility ========
 *  Update the visibility of txPower configurables
 *
 *  @param inst  - module instance
 *  @param ui - module UI state
 */
function updateTxPowerVisibility(inst, ui) {
    let prop24 = false;
    let freqBand = "2400";

    if ("freqBand" in inst) {
        // This is a proprietary setting
        freqBand = inst.freqBand;
        prop24 = freqBand === "2400";
    }
    const fbLow = freqBand === "433" || freqBand === "169";

    // Visibility of power tables
    ui.txPower.hidden = inst.highPA || fbLow || prop24;
    ui.txPowerHi.hidden = !inst.highPA || fbLow || prop24;

    if ("phyType433" in inst) {
        const otherFreqband = freqBand !== "433";
        ui.txPower433.hidden = inst.highPA || otherFreqband;
        if ("txPower433Hi" in inst) {
            ui.txPower433Hi.hidden = !inst.highPA || otherFreqband;
        }
    }
}

/*!
 *  ======== getPaUsage ========
 *  Return the PA symbols used by this PHY setting
 *
 *  @inst - PHY setting instance
 */
function getPaUsage(inst) {
    const ce = inst.codeExportConfig;
    const paExport = ce.paExport;

    if (paExport === "none") {
        return null;
    }

    const paUsage = {};
    const rfMod = system.modules["/ti/devices/radioconfig/rfdesign"].$static;

    let fb = 2400; // For BLE and IEEE 802.15.4
    let pa = "5";
    let paHi = null;

    if ("freqBand" in inst) {
        fb = parseInt(inst.freqBand);
        if (fb < 1000) {
            pa = "13";
        }
    }

    const hasHighPA = rfMod.pa20 !== "none";
    switch (paExport) {
    case "active":
        if ("highPA" in inst && hasHighPA) {
            if (inst.highPA) {
                pa = "20";
            }
        }
        break;
    case "combined":
        if ("highPA" in inst && hasHighPA) {
            if (inst.highPA) {
                paHi = "20";
            }
        }
        break;
    default:
        break;
    }

    if (paExport === "dual") {
        let code = RfDesign.genPaTableName(fb, pa, null);
        paUsage.code = "#define " + ce.txPower + "TxStd " + code + "\n";
        paUsage.size = "#define " + ce.txPowerSize + "_TXSTD " + code.toUpperCase() + "\n";

        code = RfDesign.genPaTableName(fb, "20", null);
        paUsage.code += "#define " + ce.txPower + "Tx20 " + code + "\n";
        paUsage.size += "#define " + ce.txPowerSize + "_TX20 " + code.toUpperCase() + "\n";
    }
    else {
        const code = RfDesign.genPaTableName(fb, pa, paHi);
        paUsage.code = "#define " + ce.txPower + " " + code + "\n";
        paUsage.size = "#define " + ce.txPowerSize + " " + code.toUpperCase() + "_SIZE\n";
    }
    return paUsage;
}

/*
 *  ======== highPaOnChange ========
 *  On change handler for highPA configurable
 *
 *  @param inst  - module instance
 *  @param ui - module UI state
 */
function highPaOnChange(inst, ui) {
    // Update visibility of TX power configurables
    if ("phyType" in inst) {
        ui.txPower.hidden = inst.highPA;
        ui.txPowerHi.hidden = !inst.highPA;
    }
    if ("phyType868" in inst) {
        ui.txPower.hidden = inst.highPA || ui.phyType868.hidden;
        ui.txPowerHi.hidden = !inst.highPA || ui.phyType868.hidden;
    }
    if ("phyType433" in inst) {
        ui.txPower433.hidden = inst.highPA || ui.phyType433.hidden;
        if ("txPower433Hi" in inst) {
            ui.txPower433Hi.hidden = !inst.highPA || ui.phyType433.hidden;
        }
    }
}

/*
 *  ======== pruneConfig ========
 *  Removed option list members that are not accepted by SysConfig.
 *
 *  @param config - configurable array to be modified
 */
function pruneConfig(config) {
    _.each(config, (item) => {
        deleteKeys(item, "key");
        deleteKeys(item, "info");

        if (_.has(item, "config")) {
            _.each(item.config, (subItem) => {
                deleteKeys(subItem, "key");
                deleteKeys(subItem, "info");
            });
        }
    });

    function deleteKeys(item, key) {
        if (_.has(item, "options")) {
            _.each(item.options, (opt) => {
                delete opt[key];
            });
        }
    }
}

/*
 *  ======== moduleInstances ========
 *  Determines what modules are added as sub-modules
 *
 *  @param inst  - Module instance containing the config that changed
 *  @returns     - Array containing the dependency modules
 */
function moduleInstances(inst) {
    // Controls visibility/access of code export config
    const noAccess = inst.permission === "ReadOnly";
    const phyType = Common.getPhyType(inst);
    const phyGroup = Common.getPhyGroup(inst);

    return [
        {
            name: "codeExportConfig",
            displayName: "Code Export Configuration",
            moduleName: Common.basePath + "code_export_param",
            collapsed: true,
            readOnly: noAccess,
            hidden: noAccess,
            args: {
                phyType: phyType,
                phyGroup: phyGroup
            }
        }
    ];
}

/*
 *  ======== modules ========
 *  Determines what modules are added as static sub-modules
 *
 *  @returns     - Array containing a static dependency modules
 */
function modules() {
    return [{
        name: "RF",
        displayName: "RF Driver",
        moduleName: "/ti/drivers/RF",
        collapsed: true
    },
    {
        name: "rfDesign",
        displayName: "RF Frontend",
        moduleName: "/ti/devices/radioconfig/rfdesign",
        hidden: false
    }
    ];
}

exports = {
    reloadInstanceFromPhy: reloadInstanceFromPhy,
    highPaOnChange: highPaOnChange,
    pruneConfig: pruneConfig,
    getPaUsage: getPaUsage,
    validateRfParams: validateRfParams,
    validateFrontendSettings: validateFrontendSettings,
    modules: modules,
    moduleInstances: moduleInstances,
    templates: {
        "/ti/devices/radioconfig/templates/rf_settings.c.xdt":
            "/ti/devices/radioconfig/settings/phygroup.c.xdt",
        "/ti/devices/radioconfig/templates/rf_settings.h.xdt":
            "/ti/devices/radioconfig/settings/phygroup.h.xdt"
    }
};
