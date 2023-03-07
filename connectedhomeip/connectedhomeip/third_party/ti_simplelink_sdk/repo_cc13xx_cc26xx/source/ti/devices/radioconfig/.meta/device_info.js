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
 *  ======== device_info.js ========
 *  Device information database
 */

"use strict";

// Module version
const RADIO_CONFIG_VERSION = "1.12";

// Common utility functions
const Common = system.getScript("/ti/devices/radioconfig/radioconfig_common.js");

// Global path to Radio configuration root
const ConfigPath = Common.basePath + "config/";

// Mapping SysCfg device name notation to SmartRF Studio format
const DevNameMap = {
    // SysCfg name: SmartRF Studio name
    CC1352R1F3RGZ: "cc1352r",
    CC1352P1F3RGZ: "cc1352p",
    CC1312R1F3RGZ: "cc1312r",
    CC2672R3RGZ: "cc2672r3",
    CC2672P3RGZ: "cc2672p3",
    CC2652R1FRGZ: "cc2652r",
    CC2642R1FRGZ: "cc2642r",
    CC2652P1FRGZ: "cc2652p",
    // BAW
    CC2652RB1FRGZ: "cc2652rb",
    // SIP
    CC2652R1FSIP: "cc2652rsip",
    CC2652P1FSIP: "cc2652psip",
    // Device class 7
    CC1312R7RGZ: "cc1312r7",
    CC2652R7RGZ: "cc2652r7",
    CC2652P7RGZ: "cc2652p7",
    CC1352P7RGZ: "cc1352p7",
    // Device class 3
    CC2651R3RGZ: "cc2651r3",
    CC2651R3RKP: "cc2651r3",
    CC2651P3RGZ: "cc2651p3",
    CC2651P3RKP: "cc2651p3",
    CC1311R3RGZ: "cc1311r3",
    CC1311R3RKP: "cc1311r3",
    CC1311P3RGZ: "cc1311p3",
    // Device class 10
    CC2674R10RGZ: "cc2674r10",
    CC2674R10RSK: "cc2674r10",
    CC2674P10RGZ: "cc2674p10",
    CC2674P10RSK: "cc2674p10",
    CC2653P10RSL: "cc2653p10",
    CC1314R10RGZ: "cc1314r10",
    CC1314R10RSK: "cc1314r10",
    CC1354R10RGZ: "cc1354r10",
    CC1354R10RSK: "cc1354r10",
    CC1354P10RGZ: "cc1354p10",
    CC1354P10RSK: "cc1354p10"
};

// SmartRF Studio compatible device name
const DeviceName = DevNameMap[Common.Device] || "none";
const DeviceSupported = DeviceName !== "none";

// True if High PA device
const HighPaDevice = DeviceName.includes("cc1352p")
    || DeviceName.includes("cc2652p")
    || DeviceName.includes("cc2672p")
    || DeviceName.includes("cc1311p")
    || DeviceName.includes("cc2651p");

// True if wBMS support
const wbmsSupport = DeviceName === "cc2642r" || DeviceName === "cc2652r";

/*
 * Global device information
 */
const DevInfo = {
    // Path to the configuration data for the device
    devicePath: ConfigPath + DeviceName + "/",
    phyGroup: {
        prop: {},
        ble: {},
        ieee_15_4: {}
    },
    // PHY name: "ble", "prop" or "ieee_15_4"
    phy: "",
    // Board name on SmartRF Studio format
    target: "",
    // True if device supports High PA (CC1352P and CC2652P)
    highPaSupport: HighPaDevice
};

// Load the device configuration database
let DevConfig;
if (DeviceSupported) {
    DevConfig = getDeviceConfig();
}

// Exported from this module
exports = {
    addPhyGroup: addPhyGroup,
    getVersionInfo: getVersionInfo,
    isDeviceSupported: () => DeviceSupported,
    getConfiguration: (phy) => DevInfo.phyGroup[phy].config,
    getSettingMap: (phy) => DevInfo.phyGroup[phy].settings,
    getDeviceName: () => DeviceName,
    getParamPath: (phy) => DevInfo.phyGroup[phy].paramPath,
    getSettingPath: (phy) => getFilePathPhy("categories.json", phy),
    getRfCommandDef: (phy) => getFullPathPhy("rf_command_definitions.json", phy),
    getTargetPath: (phy) => getFilePathPhy("targets.json", phy),
    getTargetIndex: (phy) => getFullPathPhy("targets.json", phy),
    getPaSettingsPath: () => getFilePath("pasettings.json"),
    getFrontEndFile: (phy) => getFullPathPhy("frontend_settings.json", phy),
    getCmdMapping: (phy) => getFullPathPhy("param_cmd_mapping.json", phy),
    hasHighPaSupport: () => DevInfo.highPaSupport
};

/*!
 *  ======== getConfiguration ========
 *  Load configuration data of a PHY group
 *
 *  @param phy - ble, prop or ieee_154
 */
function loadConfiguration(phy) {
    const fileName = DevInfo.phyGroup[phy].paramPath + "param_syscfg.json";
    const devCfg = system.getScript(fileName);
    return devCfg;
}

/*!
 *  ======== createSettingMap ========
 *  Create list of PHY settings for a PHY group
 *
 *  @param phy - ble, prop or ieee_154
 */
function createSettingMap(phy) {
    const data = DevInfo.phyGroup[phy].config;

    if (phy === Common.PHY_IEEE_15_4) {
        return data.phys.ieee;
    }
    else if (phy === Common.PHY_BLE) {
        if (wbmsSupport) {
            const settingsWBMS = {
                name: "wbms2m",
                description: "wBMS, 2 Mbps",
                file: "setting_wbms_2m.json"
            };
            data.phys.ble.push(settingsWBMS);
        }
        return data.phys.ble;
    }
    else if (phy === Common.PHY_PROP) {
        let settingMap = [];
        if (Common.isSub1gDevice()) {
            settingMap = settingMap.concat(data.phys.prop868);
            if ("prop433" in data.phys) {
                settingMap = settingMap.concat(data.phys.prop433);
            }
            if ("prop169" in data.phys) {
                settingMap = settingMap.concat(data.phys.prop169);
            }
        }
        if (Common.HAS_24G_PROP) {
            settingMap = settingMap.concat(data.phys.prop2400);
        }
        return settingMap;
    }
    throw Error("Unknown protocol: ", phy);
}

/*!
 *  ======== getVersionInfo ========
 *  Get version information for RadioConfig and SmartRF Studio
 */
function getVersionInfo() {
    const deviceList = system.getScript(ConfigPath + "device_list.json");

    const smartRFDataVersion = deviceList.devicelist.version;
    return {moduleVersion: RADIO_CONFIG_VERSION, dataVersion: smartRFDataVersion};
}

/*!
 *  ======== getDeviceConfig ========
 *  Create a device configuration database.
 *
 *  Grouped by the PHYs that are available for the current device.
 */
function getDeviceConfig() {
    const files = system.getScript(DevInfo.devicePath + "device_config.json").configFiles.path;
    const fileLists = {
        default: [],
        ble: [],
        prop: [],
        ieee_15_4: []
    };

    _.each(files, (value) => {
        const entry = {
            path: value.$,
            file: value._file
        };

        let list;
        if ("_phy" in value) {
            const phy = value._phy;
            list = fileLists[phy];
        }
        else {
            list = fileLists.default;
        }

        if ("_version" in value) {
            // Versioned: remove base entry
            // assuming base entry is immediately before versioned entry
            list.pop();
        }
        list.push(entry);
    });

    return fileLists;
}

/*!
 *  ======== getFullPathPhy ========
 *  Return the path including the file name
 *
 * @param file - name of the file which path is to be determined
 * @param phy - ble, prop or ieee_154
 */
function getFullPathPhy(file, phy) {
    return getFilePathPhy(file, phy) + file;
}

/*!
 *  ======== getFilePath ========
 *  Return the path of a file.
 *
 * @param file - name of the file which path is to be determined
 */
function getFilePath(file) {
    return getFilePathPhy(file, DevInfo.phy);
}

/*!
 *  ======== getFilePathPhy ========
 *  Return the path of a file, restricted by PHY as filter
 *
 * @param file - name of the file which path is to be determined
 */
function getFilePathPhy(file, phy) {
    const fileList = DevConfig[phy].concat(DevConfig.default);

    // Get the file-object in the file list
    const correctFile = _.find(fileList, ["file", file]);

    if (_.isUndefined(correctFile)) {
        throw Error("No PHY path for " + phy + "/" + file);
    }

    return ConfigPath + correctFile.path + "/";
}

/*!
*  ======== addPhyGroup ========
*  Initialize the database for the specified PHY group
*
*  @param phy - PHY group short name (prop, ble, or ieee_15_4)
*/
function addPhyGroup(phy) {
    const phyPath = getFilePathPhy("rf_command_definitions.json", phy);
    const phyDir = phyPath.slice(0, -1);
    const paramPath = getFilePathPhy("categories.json", phy) + "../";

    const phyInfo = {
        phy: phy,
        phyPath: phyPath,
        phyDir: phyDir,
        paramPath: paramPath
    };
    DevInfo.phyGroup[phy] = phyInfo;
    DevInfo.phy = phy;
    phyInfo.config = loadConfiguration(phy);
    phyInfo.settings = createSettingMap(phy);
}
