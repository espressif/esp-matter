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
 *  ======== radioconfig_common.js ========
 *  Common functionality for the RadioConfig module
 */

"use strict";

const Device = system.deviceData.deviceId;
const BasePath = "/ti/devices/radioconfig/";

// Manage protocol support
const hasProp = Device.match(/CC13|CC26[57][1234][RP][137]|CC2642R/);
const hasBle = Device.match(/CC..[457][1234]/);
const hasIeee = Device.match(/CC..[57][1234][RP][137B]/);
const has24gProp = hasProp && !Device.includes("CC131");

// Exported from this module
exports = {
    getScript: (file) => system.getScript(BasePath + file),
    basePath: BasePath,
    Device: Device,
    PHY_BLE: "ble",
    PHY_PROP: "prop",
    PHY_IEEE_15_4: "ieee_15_4",
    HAS_PROP: hasProp,
    HAS_BLE: hasBle,
    HAS_24G: hasBle,
    HAS_IEEE_15_4: hasIeee,
    HAS_24G_PROP: has24gProp,
    isSub1gDevice: () => Device.includes("CC13") || Device.includes("CC2672"),
    isSub1gOnlyDevice: () => Device.includes("CC131"),
    is24gOnlyDevice: () => Device.match(/CC26[457]/) && !Device.includes("CC2672"),
    isDeviceClass3: () => Device.match(/1[PR]3R/) !== null,
    isDeviceClass7: () => Device.match(/2[PR]7R/) !== null,
    isDeviceClass10: () => Device.match(/[157]4[RP]10|2653P10/) !== null,
    isDeviceStandard: () => Device.match(/2[PR]B?1F|CC2672/),
    FreqLower169: 169.4,
    FreqHigher169: 169.475,
    FreqLower433: 420,
    FreqHigher433: 528,
    FreqLower24G: 2400,
    int2hex: int2hex,
    isHex: isHex,
    fract: fract,
    forceArray: forceArray,
    calculateWidth: calculateWidth,
    getBitfieldValue: getBitfieldValue,
    getBoardName: getBoardName,
    getPhyType: getPhyType,
    getPhyGroup: getPhyGroup,
    getCoexConfig: getCoexConfig,
    validateBasic: (inst, validation) => {
        validateNames(inst, validation);
        validateTxPower(inst, validation);
    },
    flattenConfigs: flattenConfigs,
    logError: logError,
    logWarning: logWarning,
    logInfo: logInfo,
    isCName: isCName,
    initLongDescription: initLongDescription,
    autoForceModules: autoForceModules
};

const deferred = {
    errs: [],
    warn: [],
    info: [],
    logError: function(inst, field, msg) {
        this.errs.push({inst: inst, field: field, msg: msg});
    },
    logWarning: function(inst, field, msg) {
        this.warn.push({inst: inst, field: field, msg: msg});
    },
    logInfo: function(inst, field, msg) {
        this.info.push({inst: inst, field: field, msg: msg});
    }
};

/*!
 *  ======== getBoardName ========
 *  Get the SmartRF Studio name of the board
 *
 *  @returns String - Name of the board with prefix /ti/boards and
 *                    suffix .syscfg.json stripped off. If no board
 *                    was specified, an empty string  is returned.
 */
function getBoardName() {
    let boardName = "";

    if (system.deviceData.board != null) {
        boardName = system.deviceData.board.source;

        // Strip off everything up to and including the last '/'
        boardName = boardName.replace(/.*\//, "");

        // Strip off everything after and including the first '.'
        boardName = boardName.replace(/\..*/, "");

        // Convert board name to SmartRF Studio notation
        if (boardName.includes("_LAUNCHXL")) {
            boardName = "LAUNCHXL-" + boardName.replace("_LAUNCHXL", "");
            boardName = boardName.replace("_", "-");
        }
        else if (boardName.includes("LPSTK")) {
            boardName = boardName.replace("LPSTK_CC1352R", "LPSTK-CC1352R1");
        }
        else if (boardName.includes("LP_")) {
            boardName = boardName.replace("P7_", "P7-");
        }
        else {
            throw new Error("RadioConfig: Unknown board [" + boardName + "]");
        }
    }
    return boardName;
}

/*!
 *  ======== isHex ========
 *  Return true if string is a hex value
 *
 *  @param str - string to test against
 */
function isHex(str) {
    const value = str.replace("0x", "");
    const regexp = /^[0-9a-fA-F]+$/;
    return regexp.test(value);
}

/*!
 *  ======== int2hex ========
 *  Convert an integer to a hexadecimal string (0xAB...)
 *
 *  @param num - numerical value as decimal
 *  @param width - width of the resulting hexadecimal number
 */
function int2hex(num, width) {
    let ret;
    const temp = parseInt(num).toString(16);

    switch (width) {
    case 2:
        ret = ("00" + temp).slice(-2);
        break;
    case 4:
        ret = ("0000" + temp).slice(-4);
        break;
    case 6:
        ret = ("000000" + temp).slice(-6);
        break;
    case 8:
        ret = ("00000000" + temp).slice(-8);
        break;
    default:
        ret = temp;
        break;
    }

    return "0x" + ret.toUpperCase();
}

/*!
 *  ======== fract ========
 *  Calculate the fraction of a floating point number
 *
 *  @param val - floating point number to calculate the fraction of
 */
function fract(val) {
    const res = parseFloat(val).toString().split(".");
    return res[1] / 1000;
}

/*!
 *  ======== forceArray ========
 *  Force a variable to become an array.
 *
 *  - if array, keep as is
 *  - if not array, create a single element array
 *
 *  @param arg - array or single value
 */
function forceArray(arg) {
    let ret = [];
    if (Array.isArray(arg)) {
        ret = arg;
    }
    else {
        ret.push(arg);
    }
    return ret;
}

/*
 *  ======== calculateWidth ========
 *  Calculate the width of a byte index
 *
 *  @param range - array of two items describing a range
 */
function calculateWidth(range) {
    return (range[1] - range[0] + 1);
}

/*
 *  ======== getBitfieldValue ========
 *  Get the bit field of a word based on bit position and field width.
 *
 *  @param word - word to extract the bit field value from
 *  @param offset - position of the least significant bit of the field
 *  @param width - width of the bit field
 */
function getBitfieldValue(word, offset, width) {
    /* eslint-disable no-bitwise */
    const val = word >> offset;
    const mask = ~(-1 << width);

    return val & mask;
    /* eslint-enable no-bitwise */
}

/*
 *  ======== getPhyGroup ========
 *  Return the PHY group of an instance (prop, ble, ieee_15_4)
 *
 *  @param inst - module instance object
 */
function getPhyGroup(inst) {
    const name = inst.$module.$name;
    let phyGroup = null;

    if (name.includes("ble")) {
        phyGroup = "ble";
    }
    else if (name.includes("prop")) {
        phyGroup = "prop";
    }
    else if (name.includes("ieee")) {
        phyGroup = "ieee_15_4";
    }
    else {
        throw Error("PHY group can not be derived from: " + name);
    }
    return phyGroup;
}

/*
 *  ======== getPhyType ========
 *  Return the PHY type of an instance based on the selected frequency band.
 *
 *  @param inst   - module instance object
 */
function getPhyType(inst) {
    switch (inst.freqBand) {
    case "868":
        return inst.phyType868;
    case "433":
        return inst.phyType433;
    case "169":
        return inst.phyType169;
    case "2400":
        return inst.phyType2400;
    default:
    }
    return inst.phyType;
}

/*
 * ======== flattenConfigs ========
 * Make an array of depth one from a group of configurables that
 * may be a tree structure.
 *
 *  @param configList - object that contains grouped configurables
 */
function flattenConfigs(configList) {
    const flatConfigList = [];

    _.forEach(configList, (config) => {
        if (_.has(config, "config")) {
            _.forEach(config.config, (subconfig) => {
                flatConfigList.push(subconfig);
            });
        }
        else {
            flatConfigList.push(config);
        }
    });

    return flatConfigList;
}

/*
 * ======== getCoexConfig ========
 * Return the Co-ex config structure if the device support BLE/Wi-Fi Coex,
 * otherwise return null;
 *
 */
function getCoexConfig() {
    const modules = system.modules;
    let coexPhy = null;

    // Iterate RadioConfig modules
    _.each(modules, (mod) => {
        if (mod.$name.includes("radioconfig/settings/ble")) {
            coexPhy = "ble";
        }
        if (mod.$name.includes("radioconfig/settings/ieee")) {
            coexPhy = "ieee_15_4";
        }
    });

    if (coexPhy) {
        const RF = system.modules["/ti/drivers/RF"];
        const CoExConfig = RF.getCoexConfig(coexPhy);
        return CoExConfig.coExEnable.bCoExEnable === 1 ? CoExConfig : null;
    }
    return null;
}

/*
 * ======== initLongDescription ========
 * Searches through the docs object to find the correct longDescription
 *
 *  @param configurable - configurable to apply the long description to
 *  @param docs   - documentation object
 */
function initLongDescription(configurable, docs) {
    function setDocs(item) {
        const configurableDocs = _.find(docs, ["configurable", item.name]);
        // If the configurable has a longDescription, add it to the configurable
        if (configurableDocs) {
            item.longDescription = configurableDocs.longDescription;
        }
        else {
            // If the configurable still doesn't got a text, loop through all docs
            // to find if it is in a array
            _.forEach(docs, (docItem) => {
                if (_.isArray(docItem.configurable) && _.includes(docItem.configurable, item.name)) {
                    item.longDescription = docItem.longDescription;
                }
            });
        }
    }
    // Loop through all configurables
    _.each(configurable, (item) => {
        if (_.has(item, "config")) {
            _.each(item.config, (subItem) => {
                setDocs(subItem);
            });
        }
        else {
            setDocs(item);
        }
    });
}

/*
 *  ======== validateTxPower ========
 *  Validate TX Power options
 *
 *  @param inst   - module instance object
 *  @param validation   - validation object
 */
function validateTxPower(inst, validation) {
    // validation for txPowerHi
    if ("txPowerHi" in inst) {
        if (inst.highPA) {
            const validOptions = inst.$module.$configByName.txPowerHi.options(inst);
            const selectedOptions = inst.txPowerHi;
            const found = _.find(validOptions, (o) => o.name === selectedOptions);
            if (!found) {
                validation[`log${"Error"}`]("Selected option "
                    + selectedOptions + " is invalid, please reselect.", inst, "txPowerHi");
            }
        }
    }
}

/*
 *  ======== validateNames ========
 *  Validate that all names defined by inst are globally unique and
 *  valid C identifiers.
 *
 *  @param inst   - module instance object
 *  @param validation   - validation object
 */
function validateNames(inst, validation) {
    const myNames = {}; /* all C identifiers defined by inst */

    // check that $name is a C identifier
    if (inst.$name !== "") {
        const token = inst.$name;
        if (!isCName(token)) {
            logError(validation, inst, "$name",
                "'" + token + "' is not a valid a C identifier");
        }
        myNames[token] = 1;
    }

    // check that cAliases are all C identifiers and there are no duplicates
    let tokens = [];
    /* eslint-disable-next-line */
    if ("cAliases" in inst && inst.cAliases != "") {
        tokens = inst.cAliases.split(/[,;\s]+/);
    }

    for (let i = 0; i < tokens.length; i++) {
        const token = tokens[i];
        if (!isCName(token)) {
            logError(validation, inst, "cAliases",
                "'" + token + "' is not a valid a C identifier");
        }
        if (myNames[token] !== null) {
            logError(validation, inst, "cAliases",
                "'" + token + "' is defined twice");
        }
        myNames[token] = 1;
    }

    // ensure all inst C identifiers are globally unique
    const mods = system.modules;
    /* eslint-disable-next-line */
    for (const i in mods) {
        // for all instances in all modules
        const instances = mods[i].$instances;
        for (let j = 0; j < instances.length; j++) {
            const other = instances[j];

            // skip self
            if (inst.$name !== other.$name) {
                // compute all other names
                let name = other.$name;
                if (name !== "" && name in myNames) {
                    logError(validation, inst, "cAliases",
                        "multiple instances with the same name: '"
                        + name + "': " + inst.$name + " and " + other.$name);
                    break;
                }
                /* eslint-disable-next-line */
                if (other.cAliases != null && other.cAliases != "") {
                    tokens = other.cAliases.split(/[,;\s]+/);
                    for (let k = 0; k < tokens.length; k++) {
                        name = tokens[k];
                        if (name !== "" && name in myNames) {
                            logError(validation, inst, "cAliases",
                                "multiple instances with the same name: '" + name
                                + "': " + inst.$name + " and " + other.$name);
                            break;
                        }
                    }
                }
            }
        }
    }
}

/*
 *  ======== logError ========
 *  Log a new error
 *
 *  @param vo     - a validation object passed to the validate() method
 *  @param inst   - module instance object
 *  @param field  - instance property name, or array of property names, with
 *                  which this error is associated
 *  @msg          - message to display
 */
function logError(vo, inst, field, msg) {
    let lvo = vo;

    /* eslint-disable-next-line */
    if (typeof global !== "undefined" && global.__coverage__) {
        lvo = deferred;
    }
    if (typeof (field) === "string") {
        lvo.logError(msg, inst, field);
    }
    else {
        for (let i = 0; i < field.length; i++) {
            lvo.logError(msg, inst, field[i]);
        }
    }
}

/*
 *  ======== logInfo ========
 *  Log a new remark
 *
 *  @param vo     - a validation object passed to the validate() method
 *  @param inst   - module instance object
 *  @param field  - instance property name, or array of property names, with
 *                  which this remark is associated
 *  @msg          - message to display
 */
function logInfo(vo, inst, field, msg) {
    let lvo = vo;

    /* eslint-disable-next-line */
    if (typeof global !== "undefined" && global.__coverage__) {
        lvo = deferred;
    }
    if (typeof (field) === "string") {
        lvo.logInfo(msg, inst, field);
    }
    else {
        for (let i = 0; i < field.length; i++) {
            lvo.logInfo(msg, inst, field[i]);
        }
    }
}

/*
 *  ======== logWarning ========
 *  Log a new warning
 *
 *  @param vo     - a validation object passed to the validate() method
 *  @param inst   - module instance object
 *  @param field  - instance property name, or array of property names, with
 *                  which this warning is associated
 *  @msg          - message to display
 */
function logWarning(vo, inst, field, msg) {
    let lvo = vo;

    /* eslint-disable-next-line */
    if (typeof global !== "undefined" && global.__coverage__) {
        lvo = deferred;
    }
    if (typeof (field) === "string") {
        lvo.logWarning(msg, inst, field);
    }
    else {
        for (let i = 0; i < field.length; i++) {
            lvo.logWarning(msg, inst, field[i]);
        }
    }
}

/*
 *  ======== isCName ========
 *  Determine if specified id is either empty or a valid C identifier
 *
 *  @param id  - String that may/may not be a valid C identifier
 *
 *  @returns true if id is a valid C identifier OR is the empty
 *           string; otherwise false.
 */
function isCName(id) {
    if ((id !== null && id.match(/^[a-zA-Z_][0-9a-zA-Z_]*$/) !== null)
            || id === "") { /* "" is a special value that means "default" */
        return true;
    }
    return false;
}

/*
 *  ======== autoForceModules ========
 *  Returns an implementation of a module's modules method that just
 *  forces the addition of the specified modules
 *
 *  @param args An array of module name strings.
 *
 *  @return An array with module instance objects
 *
 *  Example:
 *     modules: Common.autoForceModules(["Board", "DMA"])
 */
function autoForceModules(args) {
    return (function() {
        const modArray = [];

        if (args === undefined || args === null || !Array.isArray(args)) {
            return modArray;
        }

        for (let arg = args.length - 1; args >= 0; arg--) {
            let modPath = args[arg];
            if (modPath.indexOf("/") === -1) {
                modPath = "/ti/drivers/" + modPath;
            }
            modArray.push({
                name: modPath.substring(modPath.lastIndexOf("/") + 1),
                moduleName: modPath,
                hidden: true
            });
        }
        return modArray;
    });
}
