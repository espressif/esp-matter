/*
 * Copyright (c) 2019-2022 Texas Instruments Incorporated - http://www.ti.com
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
 *  ======== cmd_handler.js ========
 *  Module to access RF Command raw data
 */

"use strict";

// Common utility functions
const Common = system.getScript("/ti/devices/radioconfig/radioconfig_common.js");

// Other dependencies
const DeviceInfo = Common.getScript("device_info.js");
const OverrideHandler = Common.getScript("override_handler.js");
const ParHandler = Common.getScript("parameter_handler.js");
const RfDesign = Common.getScript("rfdesign");

// Utility functions
const int2hex = Common.int2hex;
const getBitfieldValue = Common.getBitfieldValue;
const calculateWidth = Common.calculateWidth;

// Constants
const BLE_DEVICE_ADDRESS_LENGTH = 6;

const DeviationSteps = [250.0, 1000.0, 15.625, 62.5];

// Ensure that Command Handlers are not created twice
const CmdHandlerCache = {};

// TX power cache
const TxPowerCache = {
    default: "-1",
    t169: "-1",
    t433: "-1",
    t2400: "-1",
    high: "-1",
    t433Hi: "-1"
};

// Exported functions
exports = {
    get: get,
    getUpdatedRfCommands: getUpdatedRfCommands,
    getFrontendSettings: getFrontendSettings
};

/*!
 *  ======== get ========
 *  Get the Command Handler for a PHY layer. Create the
 *  Command Handler if it does not already exist.
 *
 *  @param phyGroup - ble, prop, ieee_154
 *  @param phyName - short name for the PHY layer
 *  @useSelectivity - set true if sensitivity preferred to selectivity
 */
function get(phyGroup, phyName, useSelectivity = false) {
    let key = phyName;

    if (useSelectivity) {
        key += "H";
    }
    if (!(key in CmdHandlerCache)) {
        CmdHandlerCache[key] = create(phyGroup, phyName, true, useSelectivity);
    }

    return CmdHandlerCache[key];
}

/*!
 *  ======== getUpdatedRfCommands ========
 *  Create a new PHY setting and update with data from the instance.
 *  This leaves the original PHY setting unchanged.
 *
 *  @param inst - inst from which update data are fetched
 *  @param phyGroup - BLE, IEEE_15_4 or PROP
 */
function getUpdatedRfCommands(inst, phyGroup) {
    const phyName = Common.getPhyType(inst, phyGroup);
    const useSelectivity = inst.settingGroup === "Selectivity";

    // Update performed on the duplicate instance
    const cmdHandlerClone = create(phyGroup, phyName, false, useSelectivity);
    cmdHandlerClone.updateRfCommands(inst);

    return cmdHandlerClone;
}

/*!
*  ======== getFrontendSettings ========
*  Get the current frontend settings.
*
*  @phyGroup - BLE, IEEE_15_4, PROP
*  @param id - front-end identifier (ID, XD, XS_RFP ...)
*/
function getFrontendSettings(phyGroup, id) {
    // Get frontend settings
    const feFile = DeviceInfo.getFrontEndFile(phyGroup);
    const feData = system.getScript(feFile);

    let fe = null;
    _.each(feData.frontends.FrontEnd, (feEntry) => {
        if (id === feEntry._name) {
            fe = feEntry;
        }
    });
    return fe;
}

/*!
 *  ======== create ========
 *  Create a setting specific instance of the command handler
 *
 *  @param phyGroup - ble, prop, ieee_154
 *  @param phyName - short name for the corresponding PHY layer
 *  @param first - first time execution
 *  @param useSelectivity - set true if sensitivity preferred to selectivity
 */
function create(phyGroup, phyName, first, useSelectivity = false) {
    const PhyName = phyName;
    const PhyGroup = phyGroup;
    const devCfg = DeviceInfo.getConfiguration(phyGroup);
    const Config = devCfg.configs;
    let SettingPath = DeviceInfo.getSettingPath(phyGroup);
    if (useSelectivity) {
        SettingPath = SettingPath.replace("ble_pg21/cmd_settings/", "ble_f021/cmd_settings/");
    }
    const SettingMap = DeviceInfo.getSettingMap(phyGroup);
    const settingsInfo = _.find(SettingMap, (s) => s.name === phyName);
    const SettingFileName = settingsInfo.file;

    // Command buffers
    const CmdBuf = {};

    // Names of commands used by setting
    const CmdUsed = [];

    // Storage for TX power value for high PA (DBm)
    const TxPowerHi = {
        dbm: "",
        raw: "0"
    };

    // Command definitions for this device and PHY (converted from SmartRF Studio)
    const CmdDef = system.getScript(DeviceInfo.getRfCommandDef(phyGroup));

    // RF command mapping (converted from SmartRF Studio)
    let tmp = system.getScript(DeviceInfo.getCmdMapping(phyGroup));
    const ParamMap = addParameterMapping(tmp);

    // Commands used by this RF setting, subset of CmdDef (converted from SmartRF Studio)
    tmp = system.getScript(SettingPath + SettingFileName).setting;
    const Setting = _.cloneDeep(tmp);

    // True if txPower configurable for 2.4 GHz band is used (only for 2.4GHz proprietary settings)
    const FreqBand = getFrequencyBand();
    const UseTxPower2400 = PhyGroup === Common.PHY_PROP && Common.HAS_24G_PROP && FreqBand === 2400;

    // Commands contained in the setting
    addUsedCommands();

    // Add commands that are not used by the setting
    addUnusedCommands();

    // Create buffer for RF commands
    createCommandBuffer();

    if (first) {
        // Apply defaults to configurables
        initConfigurables(Config);
    }

    /*!
     *  ======== addParameterMapping  ========
     *  Map RF parameters to RF commands. An RF parameter
     *  may be mapped to multiple RF commands (e.g. txPower).
     *
     *  @param - map of parameter versus RF commands
     */
    function addParameterMapping(map) {
        const paramMap = [];
        _.each(map.mapping.settings.parameters.parameter, (parItem) => {
            const params = parItem._names.split(",");
            _.each(params, (par) => {
                const cmds = parItem.rfCommand;
                const pmap = Common.forceArray(cmds);
                let lpar = par;
                if (par === "CARRIER_FREQUENCY") {
                    lpar = "CARRIERFREQUENCY";
                }
                paramMap[lpar.toUpperCase()] = pmap;
            });
        });
        return paramMap;
    }

    /*!
     *  ======== createCommandBuffer  ========
     *  Create a cache for RF commands. It contains the
     *  data from which RF Command code is generated.
     */
    function createCommandBuffer() {
        const commands = Setting.Command;

        // Iterate RF commands contained in the setting
        _.each(commands, (cmd) => {
            const cmdBuf = [];

            const cmdDef = getCommandDefByName(cmd._name);
            const fields = cmdDef.Field;
            _.each(fields, (field) => {
                const range = field.ByteIndex.split("..");
                const byteIndex = range[0];

                if ("BitField" in field) {
                    // Word contains bit fields
                    const bitFields = Common.forceArray(field.BitField);

                    // Work-around: bit-fields in setting not defined
                    let name = field._name + "." + bitFields[0]._name;
                    let fullName = cmd._name + "." + name;
                    const missingBitfields = getSettingFieldByName(fullName) === null;

                    // Work-around: inconsistency in setting
                    const whiteningWorkaround = cmd._name.includes("BLE")
                        && field._name.includes("whitening");

                    if (missingBitfields || whiteningWorkaround) {
                        const wordName = field._name.split(".")[0];
                        fullName = cmd._name + "." + wordName;
                        let initValue;
                        if (whiteningWorkaround) {
                            initValue = getSettingFieldDefault(fullName + ".init");
                        }
                        else {
                            initValue = getSettingFieldDefault(fullName);
                        }
                        _.each(bitFields, (bitfield) => {
                            name = field._name + "." + bitfield._name;
                            const bitrange = bitfield.BitIndex.split("..");
                            const width = calculateWidth(bitrange);
                            const item = {
                                name: name,
                                isPointer: false,
                                byteOffset: byteIndex,
                                width: 0,
                                default: getBitfieldValue(initValue, bitrange[0], width)
                            };
                            cmdBuf.push(item);
                        });
                    }
                    else {
                        _.each(bitFields, (bitfield) => {
                            name = field._name + "." + bitfield._name;
                            fullName = cmd._name + "." + name;
                            const item = {
                                name: name,
                                isPointer: false,
                                byteOffset: byteIndex,
                                width: 0,
                                default: getSettingFieldDefault(fullName)
                            };
                            cmdBuf.push(item);
                        });
                    }
                }
                else {
                    // Check if the word is disabled in code generation
                    let isEnabled = true;
                    if ("CodeGen" in field) {
                        isEnabled = field.CodeGen.Action !== "DISABLE_FIELD";
                    }
                    // Whole word together
                    if (isEnabled) {
                        const isPointer = "_type" in field && field._type === "pointer";
                        const fullName = cmd._name + "." + field._name;
                        const item = {
                            name: field._name,
                            isPointer: isPointer,
                            byteOffset: byteIndex,
                            width: calculateWidth(range) * 2,
                            default: getSettingFieldDefault(fullName)
                        };
                        if ("Offset" in field && isPointer) {
                            item.ptrOffset = field.Offset;
                            item.default = field.PtrName;
                        }
                        cmdBuf.push(item);
                    }
                }
            });
            CmdBuf[cmd._name] = cmdBuf;
        });
    }

    /*!
     *  ======== initConfigurables ========
     *  Initialize the configurables with default values and options
     *
     *  @param config  - array of configurables
     */
    function initConfigurables(config) {
        const freq = getFrequency();
        const freqBand = getFrequencyBand();

        // 1. Create options list for TxPower configurables
        function txPowerSwitch(item) {
            switch (item.name) {
            case "txPower":
                // 868 + BLE/IEEE 802.15.4 use "txPower"
                if (freqBand === 868 || (freqBand === 2400 && PhyGroup !== Common.PHY_PROP)) {
                    item.options = RfDesign.getTxPowerOptions(freq, false);
                    item.default = item.options[0].name;
                    TxPowerCache.default = item.default;
                }
                break;
            case "txPower169":
                // 169 MHz band uses "txPower169"
                if (freqBand === 169) {
                    item.options = RfDesign.getTxPowerOptions(169, false);
                    item.default = item.options[0].name;
                    TxPowerCache.t169 = item.default;
                }
                break;
            case "txPower433":
                // 420-527 MHz band uses "txPower433"
                if (freqBand === 433) {
                    item.options = RfDesign.getTxPowerOptions(433, false);
                    item.default = item.options[0].name;
                    TxPowerCache.t433 = item.default;
                }
                break;
            case "txPower2400":
                // 2400 - 2480 MHz (proprietary only) use "txPower2400"
                if (UseTxPower2400) {
                    item.options = RfDesign.getTxPowerOptions(2400, false);
                    item.default = item.options[0].name;
                    TxPowerCache.t2400 = item.default;
                }
                break;
            case "txPowerHi":
                // CC1352P: 868 + 2400 MHz bands use "txPowerHi" for High PA
                if (freqBand === 868 || freqBand === 2400) {
                    // NB! Using dynamic enumerable due to 10 dBm PA table for P4 Launchpad and CC2652PSIP
                    const txOptions = RfDesign.getTxPowerOptionsDefault(freq, true);
                    item.options = (inst) => RfDesign.getTxPowerOptions(freq, true);
                    if (txOptions.length > 0) {
                        item.default = txOptions[0].name;
                    }
                    TxPowerCache.high = item.default;
                }
                break;
            case "txPower433Hi":
                // CC1352P4: 433 MHz band uses "txPower433Hi" for High PA
                if (freqBand === 433) {
                    item.options = RfDesign.getTxPowerOptions(433, true);
                    item.default = item.options[0].name;
                    TxPowerCache.t433Hi = item.default;
                }
                break;
            default:
                break;
            }
        }

        _.each(config, (item) => {
            // TX power options depend on frequency
            if (_.has(item, "config")) {
                _.each(item.config, (subItem) => {
                    txPowerSwitch(subItem);
                });
            }
            else {
                txPowerSwitch(item);
            }
        });

        const rf = getRfData();
        // Initialize the other configurables
        function setOtherConfigurables(item) {
            // Override with defaults from setting file
            if (item.name in rf) {
                if (!item.name.includes("txPower")) {
                    item.default = rf[item.name];
                }
            }
        }

        _.each(config, (item) => {
            if (_.has(item, "config")) {
                _.each(item.config, (subItem) => {
                    setOtherConfigurables(subItem);
                });
            }
            else {
                setOtherConfigurables(item);
            }
        });
    }

    /*!
     *  ======== getCmdList ========
     *  Get list of commands available for this setting
     *
     *  @param cmdSet - "all": all commands defined for this setting's PHY group
     *                  "basic": commands needed for packet RX/TX
     *                  "advanced": commands needed for packet RX/TX + test (cont. RX/TX)
     */
    function getCmdList(cmdSet) {
        let ret = [];
        if (cmdSet === "all") {
            ret = Object.keys(CmdBuf);
        }
        else if (cmdSet === "advanced") {
            ret = CmdUsed;
        }
        else if (cmdSet === "basic") {
            ret = _.without(CmdUsed, "CMD_TX_TEST", "CMD_RX_TEST");
        }
        else {
            throw Error("Invalid command selection: " + cmdSet);
        }
        return ret;
    }

    /*!
     *  ======== getRfData ========
     *  Get initial settings
     */
    function getRfData() {
        const txPower = getTxPowerAll();
        const freq = getFrequency();

        // Update TX power override
        const txPowActual = getTxPower(txPower);
        const highPA = getCmdFieldValue("txPower") === "0xFFFF";
        OverrideHandler.init(Setting.Command, highPA);
        OverrideHandler.updateTxPowerOverride(txPowActual, freq, txPower.high);

        const cfgCommon = {};

        if (DeviceInfo.hasHighPaSupport()) {
            cfgCommon.txPowerHi = txPower.high;
        }

        if (PhyGroup === Common.PHY_PROP) {
            let cfgPropBase = {
                whitening: getWhitening(),
                symbolRate: parseFloat(getSymbolRate()),
                deviation: parseFloat(getDeviation()),
                rxFilterBw: getRxFilterBw(),
                carrierFrequency: parseFloat(freq),
                packetLengthRx: parseInt(getPacketLengthRx()),
                syncWord: parseInt(getSyncWord()),
                syncWordLength: getSyncWordLength(),
                preambleCount: getPreambleCount(),
                preambleMode: getPreambleMode()
            };

            const cfgTxPower = {};
            if (UseTxPower2400 && FreqBand === 2400) {
                cfgTxPower.txPower2400 = txPower.t2400;
            }
            else if (FreqBand === 169) {
                cfgTxPower.txPower169 = txPower.t169;
            }
            else if (FreqBand === 433) {
                cfgTxPower.txPower433 = txPower.t433;
            }
            else {
                cfgTxPower.txPower = txPower.default;
            }
            cfgPropBase = {...cfgPropBase, ...cfgTxPower};

            let cfgPropExtended = {};
            if (CmdUsed.includes("CMD_PROP_RX")) {
                cfgPropExtended = {
                    fixedPacketLength: parseInt(getFixedPacketLength()),
                    packetLengthConfig: getPktConf(),
                    addressMode: getAddressMode(),
                    address0: parseInt(getAddress0()),
                    address1: parseInt(getAddress1())
                };
            }
            return Object.assign(cfgCommon, cfgPropBase, cfgPropExtended);
        }
        else if (PhyGroup === Common.PHY_BLE) {
            const cfgBle = {
                txPower: txPower.default,
                frequency: freq,
                whitening: getWhitening()
            };
            return Object.assign(cfgCommon, cfgBle);
        }
        else if (phyGroup === Common.PHY_IEEE_15_4) {
            const cfg154 = {
                txPower: txPower.default,
                frequency: freq
            };
            return Object.assign(cfgCommon, cfg154);
        }
        return undefined;
    }

    /*!
     *  ======== getFrequency ========
     *  Calculate Frequency in MHz based on raw CMD value
     *  (CMD_FS, frequency and fraction fields)
     *
     *  @returns number as string - frequency
     */
    function getFrequency() {
        if (PhyGroup === Common.PHY_PROP) {
            let frequency = parseInt(getCmdFieldValue("frequency"));
            const fract = parseInt(getCmdFieldValue("fractFreq"));
            frequency += fract / 65536.0;

            return frequency.toFixed(4);
        }
        return getCmdFieldValueByOpt("frequency", "frequency");
    }

    /*!
     *  ======== getDeviation ========
     *  Calculate deviation based on raw CMD value
     *  (CMD_PROP_RADIO_DIV_SETUP)
     *
     *  @returns number as string - deviation
     */
    function getDeviation() {
        const dev = getCmdFieldValue("modulation.deviation");
        const stepSz = getCmdFieldValue("modulation.deviationStepSz");
        const deviation = (dev * DeviationSteps[stepSz]) / 1e3;

        return deviation.toFixed(1);
    }

    /*!
     *  ======== getDecimationMode ========
     *  Get the decimation mode bit-field
     *  (CMD_PROP_RADIO_DIV_SETUP)
     *
     *  @returns number as string
     */
    function getDecimationMode() {
        return getCmdFieldValue("symbolRate.decimMode");
    }

    /*!
     *  ======== getSymbolRate ========
     *  Get Symbol rate based on raw CMD value
     *  (CMD_PROP_RADIO_DIV_SETUP)
     *
     *  @returns number as string - symbolRate
     */
    function getSymbolRate() {
        const rateWord = getCmdFieldValue("symbolRate.rateWord");
        const preScaler = getCmdFieldValue("symbolRate.preScale");
        let calcSymbolRate = (rateWord * 24 * 1e6) / (preScaler * (2.0 ** 20));
        calcSymbolRate /= 1e3;
        calcSymbolRate /= OverrideHandler.getRepetitionFactor();

        return calcSymbolRate.toFixed(3);
    }

    /*!
     *  ======== getWhitening ========
     *  Get Whitening as a descriptive string ("No whitening", "PN9 whitening" ....)
     */
    function getWhitening() {
        if (PhyGroup === Common.PHY_PROP) {
            return getCmdFieldValueByOpt("whitening", "formatConf.whitenMode");
        }
        else if (PhyGroup === Common.PHY_BLE) {
            const val = getCmdFieldValue("whitening.init");
            return val !== 0;
        } // Not applicable to IEEE 802.15.4
        return "error";
    }

    /*!
     *  ======== getPreambleCount ========
     *  Set Preamble count as string:
     *  ("1 Bits", "4 Bits", "1 Byte", "2 Bytes" .. "30 Bytes", "32 Bytes")
     */
    function getPreambleCount() {
        return getCmdFieldValueByOpt("preambleCount", "preamConf.nPreamBytes");
    }

    /*!
     *  ======== getRxBandwidth ========
     *  Get RX filter bandwidth as a float, unit kHz
     */
    function getRxFilterBw() {
        return getCmdFieldValueByOpt("rxFilterBw", "rxBw");
    }

    /*!
     *  ======== getTxPower ========
     *  Get actual Tx Power value (DBm) according to frequency
     *  band and front-end PA state.
     *
     *  @txPowerAll - object containing TX power settings
     */
    function getTxPower(txPowerAll) {
        const freq = getFrequency();
        const freq433 = freq > Common.FreqHigher169 && freq < Common.FreqHigher433;
        const freq169 = freq < Common.FreqHigher169;
        let ret;

        if (DeviceInfo.hasHighPaSupport()) {
            if (UseTxPower2400) {
                ret = txPowerAll.t2400;
            }
            else {
                ret = freq433 ? txPowerAll.t433Hi : txPowerAll.high;
            }
        }
        else if (UseTxPower2400) {
            ret = txPowerAll.t2400;
        }
        else if (freq169) {
            ret = txPowerAll.t169;
        }
        else {
            ret = freq433 ? txPowerAll.t433 : txPowerAll.default;
        }
        return ret;
    }

    /*!
     *  ======== getTxPower ========
     *  Get actual Tx Power value (DBm) according to frequency
     *  band and front-end PA state.
     *
     *  @param inst - current instance
     */
    function getTxPowerFromInst(inst) {
        const freq = getFrequency();
        const freq433 = freq > Common.FreqHigher169 && freq < Common.FreqHigher433;
        const freq169 = freq < Common.FreqHigher169;

        let ret;

        if ("highPA" in inst && inst.highPA) {
            if (UseTxPower2400) {
                ret = inst.txPower2400;
            }
            else {
                ret = freq433 ? inst.txPower433Hi : inst.txPowerHi;
            }
        }
        else if (UseTxPower2400) {
            ret = inst.txPower2400;
        }
        else if (freq169) {
            ret = inst.txPower169;
        }
        else {
            ret = freq433 ? inst.txPower433 : inst.txPower;
        }
        return ret;
    }

    /*!
     *  ======== getTxPowerAll ========
     *  Get Tx Power values for all txPower configurables
     */
    function getTxPowerAll() {
        const ret = TxPowerCache;
        const freq = getFrequency();
        const freq433 = freq > Common.FreqHigher169 && freq < Common.FreqHigher433;
        const freq169 = freq < Common.FreqHigher433;

        if (DeviceInfo.hasHighPaSupport() && !UseTxPower2400) {
            // High PA
            let raw = RfDesign.getTxPowerValueByDbm(freq, true, TxPowerHi.dbm);
            if (raw !== null) {
                TxPowerHi.raw = raw;
                if (freq433) {
                    ret.t433Hi = TxPowerHi.dbm;
                }
                else {
                    ret.high = TxPowerHi.dbm;
                }
            }
            else {
                TxPowerHi.dbm = freq433 ? TxPowerCache.t433Hi : TxPowerCache.high;
                TxPowerHi.raw = "0xFFFF";
            }

            // Default PA
            let dbm;
            if (freq433) {
                dbm = getCmdFieldValueByOpt("txPower433", "txPower");
            }
            else if (freq169) {
                dbm = getCmdFieldValueByOpt("txPower169", "txPower");
            }
            else {
                dbm = getCmdFieldValueByOpt("txPower", "txPower");
            }

            raw = RfDesign.getTxPowerValueByDbm(freq, false, dbm);
            if (raw !== null) {
                if (freq433) {
                    ret.t433 = dbm;
                }
                else if (freq169) {
                    ret.t169 = dbm;
                }
                else {
                    ret.default = dbm;
                }
            }
        }
        // Not P-device, read direct from register
        else if (freq433) {
            ret.t433 = getCmdFieldValueByOpt("txPower433", "txPower");
        }
        else if (freq169) {
            ret.t169 = getCmdFieldValueByOpt("txPower169", "txPower");
        }
        else if (UseTxPower2400) {
            ret.t2400 = getCmdFieldValueByOpt("txPower2400", "txPower");
        }
        else {
            ret.default = getCmdFieldValueByOpt("txPower", "txPower");
        }
        return ret;
    }

    /*!
     *  ======== is154g ========
     *  True if the setting supports IEEE 802.15.4g (proprietary advanced RX/TX commands)
     */
    function is154g() {
        return CmdUsed.includes("CMD_PROP_RX_ADV");
    }

    /*!
     *  ======== getSyncWordLength ========
     *  Get the Sync Word length as a string ("8 Bits" ... "32 Bits")
     */
    function getSyncWordLength() {
        return getCmdFieldValueByOpt("syncWordLength", "formatConf.nSwBits");
    }

    /*!
     *  ======== getPreambleMode ========
     *  Get the Preamble mode as a descriptive string
     */
    function getPreambleMode() {
        return getCmdFieldValueByOpt("preambleMode", "preamConf.preamMode");
    }

    /*!
     *  ======== getFixedPacketLength ========
     *  Get fixed packet length for TX
     */
    function getFixedPacketLength() {
        return getCmdFieldValue("pktLen");
    }

    /*!
     *  ======== getPktConf ========
     *  Return TX packet length configuration, "Variable" or "Fixed"
     */
    function getPktConf(val) {
        return getCmdFieldValueByOpt("packetLengthConfig", "pktConf.bVarLen");
    }

    /*!
     *  ======== getSyncWord ========
     *  Get RX sync word
     */
    function getSyncWord() {
        if (CmdUsed.includes("CMD_PROP_RX")) {
            return getCmdFieldValue("CMD_PROP_RX.syncWord");
        }
        else if (CmdUsed.includes("CMD_PROP_RX_ADV")) {
            return getCmdFieldValue("CMD_PROP_RX_ADV.syncWord0");
        }
        return getCmdFieldValue("syncWord");
    }

    /*!
     *  ======== getAddress0 ========
     *  Get value of CMD_PROP_RX.address0. No conversion required.
     */
    function getAddress0() {
        return getCmdFieldValue("address0");
    }

    /*!
     *  ======== getAddress1 ========
     *  Get value of CMD_PROP_RX.address1. No conversion required.
     */
    function getAddress1() {
        return getCmdFieldValue("address1");
    }

    /*!
     *  ======== getAddressMode ========
     *  Get value of CMD_PROP_RX.pktConf.bChkAddress (0 or 1)
     *  and convert it to a string ("No address check" or "Address check")
     */
    function getAddressMode() {
        return getCmdFieldValueByOpt("addressMode", "pktConf.bChkAddress");
    }

    /*!
     *  ======== getPacketLengthRx ========
     *  Get maximum RX packet length
     */
    function getPacketLengthRx() {
        return getCmdFieldValue("maxPktLen");
    }

    /*!
     *  ======== setSymbolRate ========
     *  Apply symbol rate raw value after calculating it from kBaud
     *
     *  @param symRate - symbol rate in kBaud
     */
    function setSymbolRate(symRate) {
        // Get current pre-scaler value
        const preScaler = getCmdFieldValue("symbolRate.preScale");
        const repFactor = OverrideHandler.getRepetitionFactor();

        // Initial calculations
        const rate = (symRate * 1e3 * preScaler * (2.0 ** 20)) / (24 * 1e6);
        const intPart = Math.floor(rate);
        let rateWord = intPart;
        const calcSymbolRate = (rateWord * 24 * 1e6) / (preScaler * (2.0 ** 20));

        // Check if the rateWord can be trimmed to give a calculated value closer to the given input
        const inSymbolRate = symRate * 1e3;
        let trimRateWord = true;
        let tempRateWord = rateWord;
        let tempSymbolRate = calcSymbolRate;
        let diffBefore;
        let diffAfter;

        while (trimRateWord) {
            diffBefore = Math.abs(tempSymbolRate - inSymbolRate);

            if (diffBefore > 0) {
                if (tempSymbolRate < inSymbolRate) {
                    tempRateWord += 1;
                    tempSymbolRate = (tempRateWord * 24 * 1e6) / (preScaler * (2.0 ** 20));
                }
                else {
                    tempRateWord -= 1;
                    tempSymbolRate = (tempRateWord * 24 * 1e6) / (preScaler * (2.0 ** 20));
                }
                diffAfter = Math.abs(tempSymbolRate - inSymbolRate);

                if (diffAfter < diffBefore) {
                    rateWord = tempRateWord;
                }
                else {
                    // The difference after trimming the rate word is greater than before, end trimming
                    trimRateWord = false;
                }
            }
            else {
                // The calculated value is equal the input value, end trimming
                trimRateWord = false;
            }
        }
        // Apply calculated rate word
        setCmdFieldValue("symbolRate", "symbolRate.rateWord", rateWord * repFactor);
    }

    /*!
     *  ======== updateRfCommandsBLE ========
     *  Calculates RF command values based on the content
     *  the passed instance.
     *
     *  @param inst - current instance
     */
    function updateRfCommandsBLE(inst) {
        const frequency = inst.frequency;

        // Update target settings
        updateTarget(inst);

        // Update TX power override
        const txPower = getTxPowerFromInst(inst);
        OverrideHandler.updateTxPowerOverride(txPower, frequency, false);

        // Hard-code value for commands that are not included in the setting
        const val = inst.whitening ? 0x51 : 0;
        setCmdFieldValue("whitening", "whitening.init", val);
        setCmdFieldValue("whitening", "whitening.bOverride", 1);
        setCmdFieldValueDirect("condition.rule", 1, false);

        // Frequency
        setCmdFieldValueByOpt("frequency", "frequency", frequency);

        // Channel
        const channel = ParHandler.bleFreqToChanRaw(frequency);
        setCmdFieldValue("channel", "channel", channel);

        // PDU length
        setCmdFieldValue("packetLengthBle", "advLen", inst.packetLengthBle - BLE_DEVICE_ADDRESS_LENGTH);
    }

    /*!
     *  ======== updateRfCommands154 ========
     *  Calculates RF command values based on the content
     *  the passed instance.
     *
     *  @param inst - current instance
     */
    function updateRfCommands154(inst) {
        const frequency = inst.frequency;

        // Update target settings
        updateTarget(inst);

        // Update TX power override
        const txPower = getTxPowerFromInst(inst);
        OverrideHandler.updateTxPowerOverride(txPower, frequency, false);

        // Frequency
        setCmdFieldValueByOpt("frequency", "frequency", frequency);
    }

    /*!
     *  ======== updateRfCommandsProp ========
     *  Calculates RF command values based on the content
     *  the passed instance.
     *
     *  @param inst - current instance
     */
    function updateRfCommandsProp(inst) {
        const frequency = inst.carrierFrequency;

        // Update target settings
        updateTarget(inst);

        // Update TX power override
        const txPower = getTxPowerFromInst(inst);
        let highPA = false;
        if ("highPA" in inst) {
            highPA = inst.highPA;
        }
        OverrideHandler.updateTxPowerOverride(txPower, frequency, highPA);

        // Deviation
        const stepSz = getCmdFieldValue("modulation.deviationStepSz");
        const step = DeviationSteps[stepSz];
        const devField = Math.floor(inst.deviation * 1e3) / step;
        setCmdFieldValue("deviation", "modulation.deviation", devField);

        // Symbol rate
        setSymbolRate(inst.symbolRate);

        // RX Bandwidth
        setCmdFieldValueByOpt("rxFilterBw", "rxBw", inst.rxFilterBw);

        // Whitening
        setCmdFieldValueByOpt("whitening", "formatConf.whitenMode", inst.whitening);

        // Frequency
        const freqCmd = Math.floor(Math.abs(frequency));
        const fractFreq = (frequency - Math.floor(frequency)) * 65536.0;

        // Adjust frequency fraction */
        const fractCmd = Math.ceil(Math.round(fractFreq / 51.2) * 51.2);

        // Frequency
        setCmdFieldValue("carrierFrequency", "centerFreq", freqCmd);
        setCmdFieldValue("carrierFrequency", "frequency", freqCmd);
        setCmdFieldValue("carrierFrequency", "fractFreq", fractCmd);
        setCmdFieldValue("carrierFrequency", "loDivider", ParHandler.getLoDivider(frequency));

        // Preamble count
        setCmdFieldValueByOpt("preambleCount", "preamConf.nPreamBytes", inst.preambleCount);

        // Preamble mode
        setCmdFieldValueByOpt("preambleMode", "preamConf.preamMode", inst.preambleMode);

        // Sync word length
        setCmdFieldValueByOpt("syncWordLength", "formatConf.nSwBits", inst.syncWordLength);

        // RX packet length
        setCmdFieldValue("packetLengthRx", "maxPktLen", inst.packetLengthRx);

        // Sync word
        setCmdFieldValue("syncWord", "syncWord", inst.syncWord);
        setCmdFieldValue("syncWord", "syncWord0", inst.syncWord);
        setCmdFieldValue("syncWord", "syncWord1", 0);

        // Address mode: "No address check" or "Address check"
        setCmdFieldValueByOpt("addressMode", "pktConf.bChkAddress", inst.addressMode);

        // Fixed packet length
        setCmdFieldValue("fixedPacketLength", "pktLen", inst.fixedPacketLength);

        // Tx Packet length
        setCmdFieldValueByOpt("packetLengthConfig", "pktConf.bVarLen", inst.packetLengthConfig);

        // RX address filter
        setCmdFieldValue("address0", "address0", inst.address0);
        setCmdFieldValue("address1", "address1", inst.address1);
    }

    /*!
     *  ======== getPatchInfo ========
     *  Extract patch information from the database
     *
     *  @param protocol - single, multi, coex
     *
     */
    function getPatchInfo(protocol) {
        const patches = Setting.Patch;

        const ret = {
            mode: "RF_MODE_AUTO",
            cpe: "0",
            rfe: "0",
            mce: "0"
        };

        if ("Define" in patches) {
            ret.mode = patches.Define;
        }

        if ("Cpe" in patches) {
            if (protocol === "multi") {
                ret.cpe = "rf_patch_cpe_multi_protocol";
            }
            else if (protocol === "coex_ble") {
                ret.cpe = "rf_patch_cpe_multi_bt5_coex";
            }
            else if (protocol === "coex_ieee") {
                ret.cpe = "rf_patch_cpe_ieee_coex";
            }
            else {
                ret.cpe = patches.Cpe;
            }
        }

        if ("Mce" in patches) {
            ret.mce = patches.Mce;
        }

        if ("Rfe" in patches) {
            if (protocol === "coex_ble") {
                ret.rfe = "rf_patch_rfe_ble_coex";
            }
            else {
                ret.rfe = patches.Rfe;
            }
        }

        return ret;
    }

    /*!
     *  ======== generatePatchCode ========
     *  Extract patch code from the database
     *
     *  @param protocol - single, multi, coex
     */
    function generatePatchCode(protocol) {
        const patch = getPatchInfo(protocol);
        let code = "    .rfMode = " + patch.mode + ",\n";

        if (typeof (patch.cpe) === "string") {
            code += "    .cpePatchFxn = &" + patch.cpe + ",\n";
        }
        else {
            code += "    .cpePatchFxn = 0,\n";
        }

        if (typeof (patch.mce) === "string") {
            code += "    .mcePatchFxn = &" + patch.mce + ",\n";
        }
        else {
            code += "    .mcePatchFxn = 0,\n";
        }

        if (typeof (patch.rfe) === "string") {
            code += "    .rfePatchFxn = &" + patch.rfe;
        }
        else {
            code += "    .rfePatchFxn = 0";
        }

        return code;
    }

    /*!
     *  ======== generateOverrideCode ========
     *  Generate code for the Command overrides
     *
     *  @param ovr - Override variable name
     *  @param custom - Info on custom overrides
     */
    function generateOverrideCode(ovr, custom) {
        const freq = getFrequency();
        let txPowerDef = getCmdFieldValue("txPower");
        // High PA
        if (txPowerDef === "0xFFFF") {
            txPowerDef = RfDesign.getTxPowerValueDefault(freq, false);
        }
        // Workaround for inconsistent HEX notation in pasettings.json for 2.4 GHz
        if (!txPowerDef.includes("0x")) {
            txPowerDef = "0x" + txPowerDef;
        }
        const data = {
            txPower: txPowerDef,
            txPowerHi: RfDesign.getTxPowerValueByDbm(freq, true, TxPowerHi.dbm),
            loDivider: getCmdFieldValue("loDivider"),
            freq: freq,
            frontend: getCmdFieldValue("config.frontEndMode")
        };
        return OverrideHandler.generateCode(ovr, data, custom);
    }

    /*!
     *  ======== generateRfCmdCode ========
     *  Generate code for the given RF command, returns the content of a C struct.
     *  The enclosing variable declaration resides in the template.
     *
     *  @param cmd - Object describing the RF command
     *  @param symNames - symbol names from Code Generation config
     *  @legacy - generate code with legacy symbol names
     */
    function generateRfCmdCode(cmd, symNames, legacy) {
        const fields = CmdBuf[cmd._name];
        const ovr = symNames.overrides;
        const ovrNames = OverrideHandler.getStructNames(ovr);

        const code = {
            parStructName: "",
            rfCmd: [],
            rfPar: []
        };

        let processingParam = false;

        // Special processing for Command Number
        let field = fields[0];
        let str = "    ." + field.name + " = " + getCommandNumber(cmd._name) + ",";
        code.rfCmd.push(str);

        // Iterate over the other fields
        let j = 1;
        let nCmd = 1;
        let nPar = 0;
        let parOffset = Number.MAX_VALUE;

        while (j < fields.length) {
            field = fields[j];
            const val = "value" in field ? field.value : field.default;
            const width = field.width;

            if (field.isPointer) {
                if (field.name.includes("pRegOverride")) {
                    const symName = field.name.replace("pRegOverride", ovr);
                    if (ovrNames.indexOf(symName) !== -1) {
                        str = "    ." + field.name + " = " + symName + ",";
                    }
                    else {
                        str = "    ." + field.name + " = 0,";
                    }
                }
                else if (field.name.includes("pParams")) {
                    // Continue with RF Command parameters struct
                    let parName;
                    if (legacy) {
                        parName = val;
                    }
                    else {
                        const cmdKey = _.camelCase(cmd._name);
                        parName = symNames[cmdKey].replace(symNames.cmdPrefix + "cmd", "") + "Par";
                        parName = parName.replace("Ble", "ble");
                        if (cmdKey === "cmdBle5GenericRx") {
                            parName = parName.replace("ble5", "ble");
                        }
                        else if (cmdKey === "cmdBleAdvNc") {
                            parName = parName.replace("AdvNc", "Adv");
                        }
                    }
                    str = "    ." + field.name + " = &" + parName + ",";
                    parOffset = field.ptrOffset;
                    code.parStructName = parName;
                    code.parTypeName = val;
                }
                else {
                    str = "    ." + field.name + " = " + val + ",";
                }
            }
            else {
                const isTxPower = field.name.includes("txPower");

                if (isTxPower) {
                    str = "    ." + field.name + " = " + int2hex(parseInt(val, 16), width) + ",";
                }
                else {
                    str = "    ." + field.name + " = " + int2hex(val, width) + ",";
                }
            }
            // Reached the offset defined in pParam, the rest is a parameter struct
            if (field.byteOffset >= parOffset) {
                processingParam = true;
            }
            // Aggregate command and parameter struct
            if (processingParam) {
                code.rfPar.push(str);
                nPar += 1;
            }
            else {
                code.rfCmd.push(str);
                nCmd += 1;
            }
            j += 1;
        }
        // Strip comma and newline from the last field
        code.rfCmd[nCmd - 1] = code.rfCmd[nCmd - 1].slice(0, -1);
        code.rfCmd = code.rfCmd.join("\n");

        if (nPar > 0) {
            code.rfPar[nPar - 1] = code.rfPar[nPar - 1].slice(0, -1);
            code.rfPar = code.rfPar.join("\n");
        }
        return code;
    }

    /*!
     *  ======== getCommandMap ========
     *  Get a list of commands that map to the given RF parameter
     *
     *  @param rfParam - RF parameter name
     */
    function getCommandMap(rfParam) {
        const uc = rfParam.toUpperCase();
        if (uc in ParamMap) {
            return ParamMap[uc];
        }
        return null;
    }

    /*!
     *  ======== getCommandMap ========
     *  Get a list of commands that map to the given RF parameter
     *
     *  @param inst - PHY setting instance
     */
    function getUsedCommands(inst) {
        let usedCmds;

        switch (PhyGroup) {
        case Common.PHY_BLE:
            usedCmds = inst.codeExportConfig.cmdList_ble;
            break;
        case Common.PHY_IEEE_15_4:
            usedCmds = inst.codeExportConfig.cmdList_ieee_15_4;
            break;
        default:
            usedCmds = inst.codeExportConfig.cmdList_prop;
            break;
        }
        usedCmds = _.map(usedCmds, (cmd) => _.snakeCase(cmd).toUpperCase().replace("BLE_5", "BLE5"));

        return usedCmds;
    }

    /*!
     *  ======== isParameterUsed ========
     *  Check if an RF parameter is supported by the current selection of RF commands
     *
     *  @param rfParam - RF parameter name
     *  @param usedCmds - currently used RF commands
     */
    function isParameterUsed(rfParam, usedCmds) {
        // Special case: multiple txPower configurables
        let key = rfParam;
        if (key.includes("txPower")) {
            key = "txPower";
        }
        // Get paramter- > command map
        const cmdMap = getCommandMap(key);
        if (cmdMap === null) {
            return false;
        }
        // Check if the command is selected by the user
        const cmds = _.intersection(usedCmds, cmdMap);
        if (cmds.length === 0) {
            return false;
        }
        return true;
    }

    /*!
     *  ======== getParameterSummary ========
     *  Generate a parameter summary as C++ comments
     *
     *  @param inst - the current instance
     */
    function getParameterSummary(inst) {
        // Pre-process and sort the parameters alphabetically
        const keys = [];
        const displayNames = [];
        const freq = getFrequency();
        const freq433 = freq > Common.FreqHigher169 && freq < Common.FreqHigher433;
        const freq169 = freq <= Common.FreqHigher169;

        function pushKeys(cfg) {
            const name = cfg.name;
            keys.push(name);
            displayNames[name] = cfg.displayName;
        }

        _.each(inst.$module.config, (cfg) => {
            if (_.has(cfg, "config")) {
                _.each(cfg.config, (subCfg) => {
                    pushKeys(subCfg);
                });
            }
            else {
                pushKeys(cfg);
            }
        });
        keys.sort();

        // Generate summary
        let summary = "";
        let useHighPA = false;

        const usedCmds = getUsedCommands(inst);

        _.each(keys, (key) => {
            const displayName = displayNames[key];
            let value = inst[key];

            // Filter on High PA
            if (key === "highPA") {
                useHighPA = value;
            }

            // Ensure only the relevant "txPower" configurable is displayed
            if (key === "txPower2400" && !UseTxPower2400) {
                return true;
            }

            if ((key === "txPower") && (useHighPA || freq433 || freq169 || UseTxPower2400)) {
                return true;
            }

            if ((key === "txPowerHi") && (!useHighPA || freq433 || freq169)) {
                return true;
            }

            if ((key === "txPower433") && (useHighPA || !freq433)) {
                return true;
            }

            if ((key === "txPower169") && (useHighPA || !freq169)) {
                return true;
            }

            if ((key === "txPower433Hi") && (!useHighPA || !freq433)) {
                return true;
            }

            // Ignore modulation and loDivider (invisible)
            if (key === "loDivider" || key === "modulation") {
                return true;
            }

            // Ignore RF parameters that are not supported by an RF command
            if (!isParameterUsed(key, usedCmds)) {
                return true;
            }

            // Overwrite with calculates values where applicable
            if (key === "carrierFrequency") {
                value = freq;
            }

            if (key === "symbolRate") {
                value = getSymbolRate();
            }

            if (key === "deviation") {
                value = getDeviation();
            }

            // Convert integers to hex where applicable
            if (key === "address0" || key === "address1" || key === "syncWord") {
                value = int2hex(value);
            }

            // Append generated line
            summary += "// " + displayName + ": " + value + "\n";

            // Continue _.each iteration
            return true;
        });

        if (summary === "") {
            summary = "// NB! Setting RF parameters in this design has no effect as no RF commands are selected.\n";
        }
        return summary;
    }

    /*!
     *  ======== getFrequencyBand ========
     *  Get the frequency band of this setting
     */
    function getFrequencyBand() {
        let freqBand;
        const freq = Setting.Frequency;

        if (freq < Common.FreqHigher169) {
            freqBand = 169;
        }
        else if (freq <= Common.FreqHigher433) {
            freqBand = 433;
        }
        else if (freq >= Common.FreqLower24G) {
            freqBand = 2400;
        }
        else {
            freqBand = 868;
        }
        return freqBand;
    }

    /*!
     *  ======== getCmdFieldValue ========
     *  Get the value of a field in an RF command
     *
     *  Field name formats:
     *  - a (e.g. "txPower")
     *  - a.b (e.g."preamConf.nPreamBytes")
     *  - CMD.a (e.g. "CMD_PROP_TX.syncWord")
     *
     *  @param name - field name
     */
    function getCmdFieldValue(name) {
        const field = getCmdFieldByName(name);

        if (field != null) {
            return ("value" in field && field.value !== null ? field.value : field.default);
        }
        return null;
    }

    /*!
     *  ======== setCmdFieldValue ========
     *  Set the value of a field in an RF command
     *  (the same field may be present in several commands)
     *
     *  NB! This function only writes to commands that are present in the parameter mapping.
     *
     *  Field name formats:
     *  - a  (e.g. "txPower")
     *  - a.b (e.g."preamConf.nPreamBytes")
     *
     *  @param paramName - RF parameter name (filter for RF parameter mapping)
     *  @param fieldName - field name
     *  @param value - value to populate the command fields with
     *
     */
    function setCmdFieldValue(paramName, fieldName, value) {
        const fields = getCmdFieldsByName(fieldName);

        _.each(fields, (item) => {
            const field = item.field;
            const cmd = item.cmd;

            // Check if the field is present in the Parameter Map
            const parUc = paramName.toUpperCase();
            if (parUc in ParamMap) {
                // Check if command contains this field
                const parArray = ParamMap[parUc];

                if (parArray.includes(cmd)) {
                    // eslint-disable-next-line
                    if (value != field.default) {
                        field.value = value;
                    }
                    else {
                        delete field.value;
                    }
                }
            }
        });
    }

    /*!
     *  ======== setCmdFieldValueDirect ========
     *  Set the value of a field in an RF command without referring to RF Parameter
     *  (the same field may be present in several commands).
     *
     *  NB! This function ignores the parameter mapping.
     *
     *  Field name formats:
     *  - a  (e.g. "txPower")
     *  - a.b (e.g."preamConf.nPreamBytes")
     *
     *  @param fieldName - field name
     *  @param value - value to populate the command field
     *  @param force - apply also to settings controlled by RF parameters
     */
    function setCmdFieldValueDirect(fieldName, value, force) {
        const fields = getCmdFieldsByName(fieldName);
        _.each(fields, (item) => {
            const field = item.field;
            const cmd = item.cmd;
            // Don't overwrite commands that are included in a setting
            if (!_.includes(CmdUsed, cmd) || force) {
                // eslint-disable-next-line
                if (field.name == fieldName) {
                    // eslint-disable-next-line
                    if (value != field.default) {
                        field.value = value;
                    }
                    else {
                        delete field.value;
                    }
                }
            }
        });
    }

    /*!
    *  ======== addUsedCommands ========
    *  Initial list of used commands
    */
    function addUsedCommands() {
        const currentCmds = Setting.Command;

        _.each(currentCmds, (cmd) => {
            CmdUsed.push(cmd._name);
        });
    }

    /*!
    *  ======== updateFrontendSettings ========
    *  Update the front-end settings with values fetched from the RF Design module.
    */
    function updateFrontendSettings() {
        // Get target settings (index to front-end setting)
        const id = RfDesign.getFrontEnd(getFrequencyBand());

        const fe = getFrontendSettings(PhyGroup, id);
        if (fe === null) {
            throw Error("FrontEnd not found[" + id + "]");
        }

        let frontEndCmds;
        if ("FrequencyRange" in fe) {
            frontEndCmds = Common.forceArray(fe.FrequencyRange[0].Command);
        }
        else {
            frontEndCmds = Common.forceArray(fe.Command);
        }

        let hasFrontEndOverride = false;

        // Patch front-end setting into current setting
        const currentCmds = Setting.Command;
        _.each(frontEndCmds, (feCmd) => {
            let patchedCmd = null;
            _.each(currentCmds, (cmd) => {
                if (feCmd._name === cmd._name) {
                    patchedCmd = cmd;
                    return false;
                }
                // Continue
                return true;
            });

            if (patchedCmd !== null) {
                const temp = _.merge(patchedCmd.Field, feCmd.Field);
                patchedCmd.Field = _.uniqBy(temp, "_name");

                // Apply to Command buffer
                _.each(feCmd.Field, (field) => {
                    setCmdFieldValueDirect(field._name, field.$, true);
                });

                if ("OverrideField" in feCmd) {
                    patchedCmd.OverridePatch = feCmd.OverrideField;
                    hasFrontEndOverride = true;
                }
            }
        });

        if (hasFrontEndOverride) {
            // Update override table
            const highPA = getCmdFieldValue("txPower") === "0xFFFF";
            OverrideHandler.init(Setting.Command, highPA);
        }
    }

    /*!
    *  ======== addUnusedCommands ========
    *  Add unused commands to the list as these may optionally be used
    *  by the code export. Unused commands are those not referred to
    *  in settings files.
    */
    function addUnusedCommands() {
        const currentCmds = Setting.Command;
        const commands = CmdDef.CommandGroups.CommandGroup[0].Command;

        _.each(commands, (cmd) => {
            const name = cmd._name;

            // Check if duplicate
            let present = false;
            _.each(currentCmds, (currentCmd) => {
                if (name === currentCmd._name) {
                    present = true;
                    return false;
                }
                // Continue _.each iteration
                return true;
            });

            // Add if not already in place
            if (!present) {
                currentCmds.push(cmd);
            }
        });
    }

    /*!
     * ======== getCmdFieldByName ========
     * Get an RF command field by name (CMD_FS.frequency ....)
     *
     * The name argument may optionally be prepended with the command name
     * in order to solve ambiguities (e.g. ".syncWord" is part of both CMD_PROP_TX
     * and CMD_PROP_TX.
     *
     * If the command name is not part of the argument, the function will
     * return the first matching field.
     *
     * @param name - field name
     */
    function getCmdFieldByName(name) {
        const commands = CmdBuf;
        let cmdName = null;
        let fieldName = name;
        let ret = null;

        if (name.includes("CMD")) {
            // Extract command name (all characters up to '.')
            cmdName = name.substr(0, name.indexOf("."));
            fieldName = name.substr(name.indexOf(".") + 1);
        }

        // Iterate RF commands
        _.each(commands, (cmd, thisCmdName) => {
            // Iterate command fields (byte(s) or bit fields
            for (let j = 0; j < cmd.length; j++) {
                const field = cmd[j];
                if (fieldName === field.name && cmdName === null) {
                    ret = field;
                    return false;
                }
                else if (fieldName === field.name && cmdName === thisCmdName) {
                    ret = field;
                    return false;
                }
            }
            // Continue _.each iteration
            return true;
        });
        return ret;
    }

    /*!
     * ======== getCmdFieldsByName ========
     * Get RF command fields by name (e.g. frequency ....)
     * The same field may be present in several commands so the
     * corresponding list is returned.
     *
     * @param name - field name
     */
    function getCmdFieldsByName(name) {
        const commands = CmdBuf;
        const ret = [];

        // Iterate RF commands
        _.each(commands, (cmd, thisCmdName) => {
            // Iterate command fields (byte(s) or bit fields
            for (let j = 0; j < cmd.length; j++) {
                const field = cmd[j];
                if (name === field.name) {
                    ret.push({
                        cmd: thisCmdName, field: field
                    });
                }
            }
        });
        return ret;
    }

    /*!
     * ======== getSettingFieldDefault ========
     * Get the default value of a field (from settings file)
     *
     * @param name - setting field name
     */
    function getSettingFieldDefault(name) {
        const ret = "0";
        const field = getSettingFieldByName(name);
        if (field != null) {
            if ("$" in field) {
                return field.$;
            }
        }
        return ret;
    }

    /*!
     * ======== getSettingFieldByName ========
     * Get an RF command buffer field by name (CMD_FS.frequency ....)
     *
     * The name argument may optionally be prepended with the command name
     * in order to solve ambiguities (e.g. ".syncWord" is part of both CMD_PROP_TX
     * and CMD_PROP_TX.
     *
     * If the command name is not part of the argument, the function will
     * return the first matching field.
     *
     * @param name - field name
     */
    function getSettingFieldByName(name) {
        const commands = Setting.Command;
        let cmdName = null;
        let fieldName = name;
        let ret = null;

        if (name.includes("CMD")) {
            // Extract command name (all characters up to '.')
            cmdName = name.substr(0, name.indexOf("."));
            fieldName = name.substr(name.indexOf(".") + 1);
        }

        // Iterate RF commands
        _.each(commands, (cmd) => {
            // Iterate command fields (byte(s) or bit fields
            for (let j = 0; j < cmd.Field.length; j++) {
                const field = cmd.Field[j];

                if (fieldName === field._name && cmdName === null) {
                    ret = field;
                    return false;
                }
                else if (fieldName === field._name && cmdName === cmd._name) {
                    ret = field;
                    return false;
                }
            }
            // Continue _.each iteration
            return true;
        });
        return ret;
    }

    /*!
     * ======== setCmdFieldValueByOpt ========
     * Set an RF command value by parameter option name. This function is
     * used for applying values that are mapped via a option list,
     * e.g txPower, whitening, syncWordLength.
     *
     * @param paramName - parameter name (e.g. whitening)
     * @param fieldName - RF command field name (e.g. formatConf.whitenMode)
     * @param optValue - option value (e.g. "none")
     */
    function setCmdFieldValueByOpt(paramName, fieldName, optValue) {
        const opts = getOptions(paramName);
        let parameterName = paramName;

        if (paramName === "txPower433" || paramName === "txPower169" || paramName === "txPower2400") {
            parameterName = "txPower";
        }
        _.each(opts, (opt) => {
            if (opt.name === optValue) {
                setCmdFieldValue(parameterName, fieldName, opt.key);
            }
        });
    }

    /*!
     * ======== getCmdFieldValueByOpt ========
     * Convert an RF Command value to a corresponding RF parameter
     * option value. See setCmdFieldValueByOpt.
     *
     * @param paramName - parameter name
     * @param fieldName - RF command field name
     */
    function getCmdFieldValueByOpt(paramName, fieldName) {
        const opts = getOptions(paramName);
        const key = getCmdFieldValue(fieldName);

        // Look for an options with the given key
        let val = null;
        _.each(opts, (opt) => {
            // eslint-disable-next-line
            if (opt.key == key) {
                val = opt.name;
                return false;
            }
            // Continue _.each iteration
            return true;
        });

        if (val === null) {
            // Workaround for problem with default TX Power values in some settings.
            // The default is not in the PA table, so we use the first in the list.
            val = opts[0].name;
        }
        return val;
    }

    // Get the options array of a configurable
    function getOptions(name) {
        let opts = null;

        _.each(Config, (item) => {
            if (item.name === name) {
                opts = item.options;
                return false;
            }
            else if (_.has(item, "config")) {
                let returns;
                _.each(item.config, (subItem) => {
                    if (subItem.name === name) {
                        opts = subItem.options;
                        returns = false;
                        return returns;
                    }
                    // Continue _.each iteration
                    return true;
                });
                return returns;
            }
            // Continue _.each iteration
            return true;
        });
        return opts;
    }

    /* Get a command definition by name.
     *
     *  Extracts a device's RF Command definition from
     *  the file "rf_command_definitions.json"
     *
     *  @param name - RF Command (CMD_FS, CMD_PROP_RADIO_DIV_SETUP ...)
     */
    function getCommandDefByName(name) {
        const commands = CmdDef.CommandGroups.CommandGroup[0].Command;
        let ret = null;

        _.each(commands, (cmd) => {
            if (name === cmd._name) {
                ret = cmd;
                return false;
            }
            // Continue _.each iteration
            return true;
        });
        return ret;
    }

    /*!
     *  ======== getCommandDescription ========
     *  Get the RF Command description.
     *  This is a longer description of the RF Command.
     *  Example: CMD_FS has the description "Frequency Synthesizer Programming Command"
     *
     *  @param cmd - RF Command (CMD_FS, CMD_PROP_RADIO_DIV_SETUP ...)
     */
    function getCommandDescription(cmd) {
        const cmdDef = getCommandDefByName(cmd);

        return cmdDef.Description;
    }

    /*!
     *  ======== getCommandNumber ========
     *  Get the RF Command number (ID) by name.
     *  This is 16-bit HEX value containing in the field ".commandNo" in the command struct
     *
     *  @param name - RF Command name (CMD_FS, CMD_PROP_RADIO_DIV_SETUP ...)
     */
    function getCommandNumber(name) {
        const cmd = getCommandDefByName(name);

        return cmd.id;
    }

    /*!
     *  ======== updateTarget ========
     *  Updated target information according to PHY
     *
     *  @param inst - current instance
     */
    function updateTarget(inst) {
        // TxPower
        updateTxPower(inst);

        // Create override table
        const highPA = getCmdFieldValue("txPower") === "0xFFFF";
        OverrideHandler.init(Setting.Command, highPA);
    }

    /*!
     *  ======== updateTxPower ========
     *  Update txPower according to the content of configurables
     *  holding the TX power values.
     *
     *  @param inst - current instance
     */
    function updateTxPower(inst) {
        const freq = getFrequency();
        const freq433 = freq > Common.FreqHigher169 && freq < Common.FreqHigher433;
        const freq169 = freq < Common.FreqHigher169;
        let raw;

        if (inst.highPA) {
            // TX power is handled by overrides
            setCmdFieldValue("txPower", "txPower", "0xFFFF");
            TxPowerHi.dbm = freq433 ? inst.txPower433Hi : inst.txPowerHi;
        }
        else if (freq433 && "txPower433" in inst) {
            raw = RfDesign.getTxPowerValueByDbm(freq, false, inst.txPower433);
            setCmdFieldValue("txPower", "txPower", raw);
        }
        else if (freq169 && "txPower169" in inst) {
            raw = RfDesign.getTxPowerValueByDbm(freq, false, inst.txPower169);
            setCmdFieldValue("txPower", "txPower", raw);
        }
        else if (UseTxPower2400) {
            raw = RfDesign.getTxPowerValueByDbm(freq, false, inst.txPower2400);
            setCmdFieldValue("txPower", "txPower", raw);

            if ("highPA" in inst) {
                // TX Power high not used; set to default
                const txOptions = RfDesign.getTxPowerOptionsDefault(freq, true);
                if (txOptions.length > 0) {
                    TxPowerHi.dbm = txOptions[0].name;
                }
            }
        }
        else {
            raw = RfDesign.getTxPowerValueByDbm(freq, false, inst.txPower);
            setCmdFieldValue("txPower", "txPower", raw);
            if ("highPA" in inst) {
                // TX Power high not used; set to default
                const txOptions = RfDesign.getTxPowerOptionsDefault(freq, true);
                if (txOptions.length > 0) {
                    TxPowerHi.dbm = txOptions[0].name;
                }
            }
        }
    }

    /*!
     *  ======== updateRfCommands ========
     *  Update the contents of the RF Command database with data extracted
     *  from the RF parameters instance.
     *
     *  @param inst - current instance
     */
    function updateRfCommands(inst) {
        switch (PhyGroup) {
        case Common.PHY_PROP:
            updateRfCommandsProp(inst);
            break;
        case Common.PHY_BLE:
            updateRfCommandsBLE(inst);
            break;
        case Common.PHY_IEEE_15_4:
            updateRfCommands154(inst);
            break;
        default:
            throw Error("No such PHY group: " + PhyGroup);
        }
    }

    // Return the module's exported functions
    return {
        getPhyGroup: function() {
            return PhyGroup;
        },
        getPhyName: function() {
            return PhyName;
        },
        getSettingFile: function() {
            return SettingFileName;
        },
        getCommands: function() {
            return Setting.Command;
        },
        getSettingLongName: function() {
            return Setting.Name;
        },
        getSettingDescription: function() {
            return Setting.Description;
        },
        getCommandName: function(cmd) {
            return cmd._name;
        },
        is154g: is154g,
        updateRfCommands: updateRfCommands,
        initConfigurables: initConfigurables,
        getCmdList: getCmdList,
        getCommandDescription: getCommandDescription,
        getRfData: getRfData,
        getParameterSummary: getParameterSummary,
        getCommandMap: getCommandMap,
        getPatchInfo: getPatchInfo,
        generatePatchCode: generatePatchCode,
        generateRfCmdCode: generateRfCmdCode,
        generateOverrideCode: generateOverrideCode,
        updateFrontendSettings: updateFrontendSettings,
        getFrequencyBand: getFrequencyBand,
        getFrequency: getFrequency,
        getDecimationMode: getDecimationMode,
        getUsedCommands: getUsedCommands,
        isParameterUsed: isParameterUsed
    };
}
