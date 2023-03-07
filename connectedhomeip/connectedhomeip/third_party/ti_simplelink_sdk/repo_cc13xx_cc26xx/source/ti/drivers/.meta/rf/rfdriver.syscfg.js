/*
 * Copyright (c) 2020, Texas Instruments Incorporated - http://www.ti.com
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
 *  ======== rfdriver.syscfg.js ========
 */

"use strict";

/* Common /ti/drivers utility functions */
const Common = system.getScript("/ti/drivers/Common.js");

/* Common radioconfig utility functions */
const CommonRadioconfig = system.getScript("/ti/devices/radioconfig/radioconfig_common.js");

/* Documentation for module config options */
const Docs = system.getScript("/ti/drivers/rf/RF.docs.js");

/* Pin symbol default prefix */
const SYM_PREFIX = "CONFIG_RF_";

/* Device information */
const deviceData = system.deviceData;

/* HW supported for antenna switch code generation */
const hwSupported = [
    "SKY13317-373LF",  // CC13X2_CC26X2
    "XMSSJJ3G0PA-054", // CC13X2_CC26X2, SIP
    "RTC6608OU"        // CC13X1_CC26X1
];

/* Options for global event mask */
const globalEventOptions = [
    {
        name: "RF_GlobalEventRadioSetup",
        description: Docs.globalEvent.radioSetup.description
    },
    {
        name: "RF_GlobalEventRadioPowerDown",
        description: Docs.globalEvent.radioPowerDown.description
    },
    {
        name: "RF_GlobalEventInit",
        description: Docs.globalEvent.init.description
    },
    {
        name: "RF_GlobalEventCmdStart",
        description: Docs.globalEvent.cmdStart.description
    },
    {
        name: "RF_GlobalEventCmdStop",
        description: Docs.globalEvent.cmdStop.description
    },
    {
        name: "RF_GlobalEventCoexControl",
        description: Docs.globalEvent.coexControl.description
    },
    {
        name: "RF_GlobalEventTempNotifyFail",
        description: Docs.globalEvent.radioSetup.description
    }
];

/* Required RF events for specific features */
const requiredGlobalEvents = {
    antennaSwitch: [
        "RF_GlobalEventInit",
        "RF_GlobalEventRadioSetup",
        "RF_GlobalEventRadioPowerDown"
    ],
    coex: [
        "RF_GlobalEventInit",
        "RF_GlobalEventCmdStart",
        "RF_GlobalEventCmdStop",
        "RF_GlobalEventCoexControl"
    ]
};

/* Supported coex mode per PHY */
const coexModeSupport = {
    ble: [
        "coexMode3Wire",
        "coexMode1WireRequest",
        "coexMode1WireGrant"
    ],
    ieee_15_4: [
        "coexMode3Wire",
        "coexMode2Wire"
    ]
}

/* Structure for coex config */
const coexConfig = {
    coExEnable: {
        bCoExEnable: 0,
        bUseREQUEST: 0,
        bUseGRANT: 0,
        bUsePRIORITY: 0,
        bRequestForChain: 0
    },
    coExTxRxIndication: 1,
    priorityIndicationTime: 20,
    ble: {
        overrideConfig: {
            bUseOverridePriority: 0,
            overridePriority: 0,
            bUseOverrideRequestForRx: 0,
            overrideRequestForRx: 0
        },
        cmdBleMasterSlaveConfig: {
            defaultPriority: 0,
            bAssertRequestForRx: 1,
            bIgnoreGrantInRx: 0,
            bKeepRequestIfNoGrant: 0
        },
        cmdBleAdvConfig: {
            defaultPriority: 0,
            bAssertRequestForRx: 1,
            bIgnoreGrantInRx: 0,
            bKeepRequestIfNoGrant: 0
        },
        cmdBleScanConfig: {
            defaultPriority: 0,
            bAssertRequestForRx: 1,
            bIgnoreGrantInRx: 0,
            bKeepRequestIfNoGrant: 0
        },
        cmdBleInitConfig: {
            defaultPriority: 0,
            bAssertRequestForRx: 1,
            bIgnoreGrantInRx: 0,
            bKeepRequestIfNoGrant: 0
        },
        cmdBleGenericRxConfig: {
            defaultPriority: 0,
            bAssertRequestForRx: 1,
            bIgnoreGrantInRx: 0,
            bKeepRequestIfNoGrant: 0
        },
        cmdBleTxTestConfig: {
            defaultPriority: 0,
            bAssertRequestForRx: 1,
            bIgnoreGrantInRx: 0,
            bKeepRequestIfNoGrant: 0
        }
    },
    ieee_15_4: {
        overrideConfig: {
            bUseOverridePriority: 0,
            overridePriority: 0,
            bUseOverrideRequestForRx: 0,
            overrideRequestForRx: 0
        },
        cmdIeeeRxConfig: {
            defaultPriority: 0,
            bAssertRequestForRx: 1,
            bIgnoreGrantInRx: 0,
            bKeepRequestIfNoGrant: 0
        },
        cmdIeeeRxAckConfig: {
            defaultPriority: 0,
            bAssertRequestForRx: 1,
            bIgnoreGrantInRx: 0,
            bKeepRequestIfNoGrant: 0
        },
        cmdIeeeCcaConfig: {
            defaultPriority: 0,
            bAssertRequestForRx: 1,
            bIgnoreGrantInRx: 0,
            bKeepRequestIfNoGrant: 0
        },
        cmdIeeeTxConfig: {
            defaultPriority: 0,
            bAssertRequestForRx: 1,
            bIgnoreGrantInRx: 0,
            bKeepRequestIfNoGrant: 0
        }
    },
    grantLatencyTime: 60
};

/* Map option to enum name */
const priorityEnumLookup = {
    0: "RF_PriorityCoexLow",
    1: "RF_PriorityCoexHigh"
};
const requestEnumLookup = {
    0: "RF_RequestCoexNoAssertRx",
    1: "RF_RequestCoexAssertRx"
};

/* Structure for BLE specific config */
const coexConfigBle = {};

/* Array of level options */
const coexLevelOptions = [
    {
        name: 0,
        displayName: "Low"
    },
    {
        name: 1,
        displayName: "High"
    }
];

/* Define max number of antenna pins */
const N_MAX_ANTENNA_PINS = 10;

/* Construct configuration instance for interrupt priority */
const intPriority = Common.newIntPri()[0];
intPriority.description = Docs.intPriority.description;

/* Construct configuration instance for software interrupt priority */
const swiPriority = Common.newSwiPri();
swiPriority.description = Docs.swiPriority.description;

/* Configurables for the rfdriver module */
const config = [
    {
        name: "$hardware",
        filterHardware: filterHardware,
        onChange: onHardwareChanged,
        getUIDefault: (components) => {
            const [key, value] = Object.entries(components).find(c => c[1].type == "RF") || [];
            return key ? components[key] : null;
        }
    },
    {
        name: "pinSelectionAntenna",
        displayName: Docs.pinSelectionAntenna.displayName,
        description: Docs.pinSelectionAntenna.description,
        longDescription: Docs.pinSelectionAntenna.longDescription,
        default: isAntennaSwitchInPackage() ? Object.keys(getRfHardware().signals).length : 0,
        options: Array.from(Array(N_MAX_ANTENNA_PINS+1), (x,i) => i).map(i => ({name:i})),
        hidden: false,
        readOnly: isAntennaSwitchInPackage(),
        onChange: onPinSelectionAntennaChanged
    },
    {
        name: "pinSelection",
        displayName: "RF Antenna Multiplexer Pin Selections",
        description: "This option is deprecated",
        longDescription: `
This option is deprecated and replaced by 'pinSelectionAntenna'.
`,
        minSelections: 0,
        hidden: true,
        default: [],
        options: getPinOptions(),
        deprecated: true,
        onChange: onPinSelectionChanged
    },
    {
        name: "coexEnable",
        displayName: Docs.coex.enable.displayName,
        description: Docs.coex.enable.description,
        longDescription: Docs.coex.enable.longDescription,
        hidden: !isCoexSupport(),
        skipTests: ["configLongDescription"],
        default: false,
        onChange: (inst, ui) => onCoexEnableChanged(inst, ui, "coexEnable")
    },
    {
        name: "coexConfigGroup",
        displayName: Docs.coex.configGroup.displayName,
        collapsed: false,
        config: [
            {
                name: "coexPhy",
                displayName: Docs.coex.phy.displayName,
                description: Docs.coex.phy.description,
                longDescription: Docs.coex.phy.longDescription,
                getDisabledOptions: getDisabledCoexPhyOptions,
                hidden: true,
                default: ["ble"],
                minSelections: 1,
                options: [
                    {
                        name: "ble",
                        displayName: Docs.coex.phy.ble.displayName,
                        description: Docs.coex.phy.ble.description
                    },
                    {
                        name: "ieee_15_4",
                        displayName: Docs.coex.phy.ieee_15_4.displayName,
                        description: Docs.coex.phy.ieee_15_4.description
                    }
                ],
                onChange: (inst, ui) => onCoexEnableChanged(inst, ui, "coexPhy")
            },
            {
                name: "coexMode",
                displayName: Docs.coex.mode.displayName,
                description: Docs.coex.mode.description,
                longDescription: Docs.coex.mode.longDescription,
                getDisabledOptions: getDisabledCoexModeOptions,
                hidden: true,
                default: "coexMode3Wire",
                options: [
                    {
                        name: "coexMode3Wire",
                        displayName: Docs.coex.mode.threeWire.displayName,
                        description: Docs.coex.mode.threeWire.description
                    },
                    {
                        name: "coexMode2Wire",
                        displayName: Docs.coex.mode.twoWire.displayName,
                        description: Docs.coex.mode.twoWire.description
                    },
                    {
                        name: "coexMode1WireRequest",
                        displayName: Docs.coex.mode.oneWireRequest.displayName,
                        description: Docs.coex.mode.oneWireRequest.description
                    },
                    {
                        name: "coexMode1WireGrant",
                        displayName: Docs.coex.mode.oneWireGrant.displayName,
                        description: Docs.coex.mode.oneWireGrant.description
                    }
                ],
                onChange: onCoexModeChanged
            },
            {
                name: "coexPinRequestIdleLevel",
                displayName: Docs.coex.pinIdleLevel.displayName.replace("%S%", "REQUEST"),
                description: Docs.coex.pinIdleLevel.description.replace("%S%", "REQUEST"),
                longDescription: Docs.coex.pinIdleLevel.longDescription.replace("%S%", "REQUEST"),
                hidden: true,
                default: 0,
                options: coexLevelOptions
            },
            {
                name: "coexPinPriorityIdleLevel",
                displayName: Docs.coex.pinIdleLevel.displayName.replace("%S%", "PRIORITY"),
                description: Docs.coex.pinIdleLevel.description.replace("%S%", "PRIORITY"),
                longDescription: Docs.coex.pinIdleLevel.longDescription.replace("%S%", "PRIORITY"),
                hidden: true,
                default: 0,
                options: coexLevelOptions
            },
            {
                name: "coexPinPriorityIndicationTime",
                displayName: Docs.coex.priorityIndicationTime.displayName,
                description: Docs.coex.priorityIndicationTime.description,
                longDescription: Docs.coex.priorityIndicationTime.longDescription,
                hidden: true,
                default: 20,
                options: [10, 15, 20, 25, 30].map(it => ({name: it})),
                onChange: onCoexPriorityIndicationTimeChanged
            },
            {
                name: "coexPinGrantIdleLevel",
                displayName: Docs.coex.pinIdleLevel.displayName.replace("%S%", "GRANT"),
                description: Docs.coex.pinIdleLevel.description.replace("%S%", "GRANT"),
                longDescription: Docs.coex.pinIdleLevel.longDescription.replace("%S%", "GRANT"),
                hidden: true,
                default: 1,
                options: coexLevelOptions
            },
            {
                name: "coexGrantLatencyTime",
                displayName: Docs.coex.grantLatencyTime.displayName,
                description: Docs.coex.grantLatencyTime.description,
                longDescription: Docs.coex.grantLatencyTime.longDescription,
                hidden: true,
                default: 60,
                onChange: onCoexGrantLatencyTimeChanged
            },
            {
                name: "coexUseCaseConfigGroupBle",
                displayName: Docs.coex.useCaseConfigGroupBle.displayName,
                collapsed: true,
                config: [
                    {
                        name: "bleIniGroup",
                        displayName: Docs.coex.useCaseConfigGroupBle.ini.displayName,
                        collapsed: false,
                        config: [
                            {
                                name: "bleIniDefaultPriority",
                                displayName: Docs.coex.defaultPriority.displayName,
                                description: Docs.coex.defaultPriority.description,
                                longDescription: Docs.coex.defaultPriority.longDescription,
                                hidden: true,
                                default: 0,
                                options: coexLevelOptions,
                                onChange: updateCoexConfig
                            },
                            {
                                name: "bleIniAssertRequestForRx",
                                displayName: Docs.coex.assertRequestForRx.displayName,
                                description: Docs.coex.assertRequestForRx.description,
                                longDescription: Docs.coex.assertRequestForRx.longDescription,
                                hidden: true,
                                default: true,
                                onChange: updateCoexConfig
                            }
                        ]
                    },
                    {
                        name: "bleConGroup",
                        displayName: Docs.coex.useCaseConfigGroupBle.con.displayName,
                        collapsed: false,
                        config: [
                            {
                                name: "bleConDefaultPriority",
                                displayName: Docs.coex.defaultPriority.displayName,
                                description: Docs.coex.defaultPriority.description,
                                longDescription: Docs.coex.defaultPriority.longDescription,
                                hidden: true,
                                default: 0,
                                options: coexLevelOptions,
                                onChange: updateCoexConfig
                            },
                            {
                                name: "bleConAssertRequestForRx",
                                displayName: Docs.coex.assertRequestForRx.displayName,
                                hidden: true,
                                default: true,
                                onChange: updateCoexConfig
                            }
                        ]
                    },
                    {
                        name: "bleBroGroup",
                        displayName: Docs.coex.useCaseConfigGroupBle.bro.displayName,
                        collapsed: false,
                        config: [
                            {
                                name: "bleBroDefaultPriority",
                                displayName: Docs.coex.defaultPriority.displayName,
                                description: Docs.coex.defaultPriority.description,
                                longDescription: Docs.coex.defaultPriority.longDescription,
                                hidden: true,
                                default: 0,
                                options: coexLevelOptions,
                                onChange: updateCoexConfig
                            },
                            {
                                name: "bleBroAssertRequestForRx",
                                displayName: Docs.coex.assertRequestForRx.displayName,
                                hidden: true,
                                default: true,
                                onChange: updateCoexConfig
                            }
                        ]
                    },
                    {
                        name: "bleObsGroup",
                        displayName: Docs.coex.useCaseConfigGroupBle.obs.displayName,
                        collapsed: false,
                        config: [
                            {
                                name: "bleObsDefaultPriority",
                                displayName: Docs.coex.defaultPriority.displayName,
                                description: Docs.coex.defaultPriority.description,
                                longDescription: Docs.coex.defaultPriority.longDescription,
                                hidden: true,
                                default: 0,
                                options: coexLevelOptions,
                                onChange: updateCoexConfig
                            },
                            {
                                name: "bleObsAssertRequestForRx",
                                displayName: Docs.coex.assertRequestForRx.displayName,
                                hidden: true,
                                default: true,
                                onChange: updateCoexConfig
                            }
                        ]
                    }
                ]
            }
        ]
    },
    intPriority,
    swiPriority,
    {
        name: "xoscNeeded",
        displayName: Docs.xoscNeeded.displayName,
        description: Docs.xoscNeeded.description,
        longDescription: Docs.xoscNeeded.longDescription,
        default: true
    },
    {
        name: "globalEventMask",
        displayName: Docs.globalEventMask.displayName,
        description: Docs.globalEventMask.description,
        longDescription: Docs.globalEventMask.longDescription,
        minSelections: 0,
        default: isAntennaSwitchInPackage() ? requiredGlobalEvents.antennaSwitch : [],
        onChange: onGlobalEventMaskChanged,
        options: globalEventOptions
    },
    {
        name: "globalCallbackFunction",
        displayName: Docs.globalCallbackFunction.displayName,
        description: Docs.globalCallbackFunction.description,
        longDescription: Docs.globalCallbackFunction.longDescription,
        default: isAntennaSwitchInPackage() ? "rfDriverCallback" : "NULL",
        readOnly: !isAntennaSwitchInPackage()
    },
    {
        name: "pinSymGroup",
        displayName: Docs.pinSymGroup.displayName,
        collapsed: false,
        config: getPinSymbolConfigurables()
    }
];

/*
 *******************************************************************************
 Status Functions
 *******************************************************************************
 */

/*
 *  ======== filterHardware ========
 *  Check component signals for compatibility with RF.
 *
 *  @param component    - A hardware component
 *  @return             - Bool
 */
function filterHardware(component) {
    return (Common.typeMatches(component.type, ["RF"]));
}

/*
 *  ======== isCustomDesign ========
 *  Check if device used is custom (i.e. not LaunchPad).
 *
 *  @return - Bool
 */
function isCustomDesign() {
    return !("board" in deviceData);
}

/*
 *  ======== isPhy ========
 *  Check if module is shared by (i.e. child of) a specific phy.
 *
 *  @param inst - Module instance object
 *  @param phy  - "ble", "ieee_15_4" or "prop"
 *  @return     - Bool
 */
function isPhy(inst, phy) {
    const status = (inst.$sharedBy.find(obj => obj.$module.$name.includes(`settings/${phy}`))) ? true : false;
    return status;
}

/*
 *  ======== isConfigDefaultValue ========
 *  Check if specified configuration option has its default value.
 *
 *  @param inst     - Module instance object containing the config
 *  @param config   - Config option to get default value of
 *  @return         - Bool
 */
function isConfigDefaultValue(inst, config){
    return (inst[config] === getDefaultValue(inst, config));
}

/*
 *  ======== isAntennaSwitchInPackage ========
 *  Check if antenna switch is in device package.
 *
 *  @return - Bool
 */
function isAntennaSwitchInPackage() {
    /* If P device and SIP package */
    return (
        !!deviceData.deviceId.match(/CC....P/)
        && (deviceData.package === "SIP")
    );
}

/*
 *  ======== isCoexSupport ========
 *  Check if coex feature is supported by device.
 *
 *  @return - Bool
 */
function isCoexSupport() {
    /* If device family:
     * - cc13x2_cc26x2
     * - cc13x2x7_cc13x2x7
     * 
     * AND not a sub1g only device
     */
    return (
        (
            !!deviceData.deviceId.match(/CC(13|26).2[RP][^7]/)
            || !!deviceData.deviceId.match(/CC(13|26).2[RP]7/)
        )
        && !CommonRadioconfig.isSub1gOnlyDevice()
    );
}

/*
 *******************************************************************************
 Get Functions
 *******************************************************************************
 */

/*
 *  ======== getDefaultValue ========
 *  Get default value for specified configuration option.
 *
 *  @param inst     - Module instance object containing the config
 *  @param config   - Config option to get default value of
 *  @return         - Default/initial value used for config
 */
function getDefaultValue(inst, config){
    return (config in inst) ? inst.$module.$configByName[config].default : undefined;
}

/*
 *  ======== getRfHardware ========
 *  Get $hardware configurable.
 *
 *  This function will return a $hardware object, regardless of
 *  it being defined as a component in the board metadata or
 *  defined withing the RF module (RF.hardware.js).
 *
 *  Note: When fetched from RF.hardware.js, the configurable returned will have less
 *        members than the original $hardware definition, but an additional signal `.dio`.
 *
 *  @param inst     - Module instance object containing the config
 *  @return         - A $hardware configurable
 */
function getRfHardware(inst=null) {
    let hardware = inst ? inst.$hardware : null;

    /* If P device SIP package */
    if (isAntennaSwitchInPackage()) {
        /* Device information */
        const RfHardware = system.getScript("/ti/drivers/rf/hardware/RF.hardware.js");
        const DriverLib = system.getScript("/ti/devices/DriverLib");
        const deviceFamily = DriverLib.getAttrs(deviceData.deviceId).deviceDir.replace(/x2x7/g, "x2");

        /* Hardware definition stored with RF module */
        hardware = RfHardware.antennaSwitch[deviceFamily][deviceData.package]
    }

    return hardware;
}

/*
 *  ======== getDisabledCoexModeOptions ========
 *  Generates a list of options that should be disabled in the coex mode
 *  drop-down.
 *
 * @param inst     - Module instance object containing the config
 * @returns        - Array of options that should be disabled
 */
function getDisabledCoexModeOptions(inst)
{
    const coexPhy = inst.coexPhy;
    const disabledOptions = [];
    const reason = "Coex mode not supported by the current PHY configuration";

    if(coexPhy.includes("ble"))
    {
        disabledOptions.push({
                name: "coexMode2Wire",
                reason: reason
        });
    }

    if(coexPhy.includes("ieee_15_4"))
    {
        disabledOptions.push({
            name: "coexMode1WireRequest",
            reason: reason
        });

        disabledOptions.push({
        name: "coexMode1WireGrant",
        reason: reason
        });
    }

    return(disabledOptions);
}

/*
 *  ======== getDisabledCoexPhyOptions ========
 *  Generates a list of options that should be disabled in the coexPhy
 *  drop-down.
 *
 * @param inst - Module instance object containing the config
 * @returns    - Array of options that should be disabled
 */
function getDisabledCoexPhyOptions(inst)
{
    const device = deviceData.deviceId;
    const board = ("board" in system.deviceData) ? system.deviceData.board.name : "custom"
    const disabledOptions = [];
    const reason = `Coex PHY not supported by device ${device} (board: ${board})`

    /* IEEE Coex not supported */
    if(!CommonRadioconfig.HAS_IEEE_15_4){
        disabledOptions.push({
            name: "ieee_15_4",
            reason: reason
        });
    }

    /* BLE Coex not supported */
    if(!CommonRadioconfig.HAS_BLE
        || device.match(/CC(13|26).(3|4)[RP]/)
    ){
        disabledOptions.push({
                name: "ble",
                reason: reason
        });
    }

    return(disabledOptions);
}

/*
 *  ======== getPinByDIO ========
 *  Get a pin description.
 *
 *  @param   dio - Formatted "DIO_XX"
 *  @return      - Pin object
 */
function getPinByDIO(dio) {
    const ipin = _.findKey(deviceData.devicePins, pin => pin.designSignalName === dio);
    if (ipin === undefined) {
        throw Error("Pin not found: " + dio);
    }
    return deviceData.devicePins[ipin];
}

/*
 *  ======== getPinOptions ========
 *  Get a list of available DIO pins.
 *
 *  @return - Array of pin options {name, displayName}
 */
function getPinOptions() {
    const dioPins = Object.values(deviceData.devicePins).filter(pin => pin.designSignalName.includes("DIO_"));

    const pinOptions = dioPins.map(pin => ({
            name: pin.designSignalName,
            /* Display as "DIO_X / Pin Y" */
            displayName: pin.designSignalName + " / Pin " + pin.ball
    }));

    return pinOptions;
}

/*
 *  ======== getPinSymbolConfigurables ========
 *  Get pin symbol configurables based on device pin options.
 *
 *  @return - Array of pin symbol objects
 */
function getPinSymbolConfigurables() {
    const config = [];

    /*
    *  ======== NOT USED ========
    *  The following was used to map custom symbols to DIOs,
    *  by adding all available pin options as module configurables.
    *  Not in use, but kept for backwards compatibility.
    */
    const pinOptions = getPinOptions();
    _.each(pinOptions, (opt) => {
        const name = opt.name;
        const pinSymCfg = {
            name: name,
            displayName: name,
            default: SYM_PREFIX + name.replace("_", ""),
            hidden: true
        };
        config.push(pinSymCfg);
    });
    /* ==========================

    /* Specific pin symbol names, if antenna switch required for device */
    const antennaSwitch = getRfHardware();
    const antennaSwitchRequired = isAntennaSwitchInPackage();
    const signalNames = antennaSwitch ? Object.keys(antennaSwitch.signals) : [];

    /* Add pin symbol config for antenna pins */
    for(let index = 0; index < N_MAX_ANTENNA_PINS; index++) {
        config.push({
            name: `rfAntennaPinSymbol${index}`,
            displayName: `RF Antenna Pin ${index}`,
            default: SYM_PREFIX + ((index < signalNames.length) ? signalNames[index] : `ANTENNA_PIN_${index}`),
            hidden: !(index < signalNames.length),
            readOnly: ((index < signalNames.length) && antennaSwitchRequired)
        });
    }

    /* Add pin symbol config for coex pins*/
    const coexSignalNames = ["REQUEST", "PRIORITY", "GRANT"];
    coexSignalNames.forEach(signal => {
        const displayName = `RF Coex ${signal} Pin`;
        config.push({
            name: _.camelCase(displayName + " Symbol"),
            displayName: displayName,
            default: SYM_PREFIX + `COEX_${signal}`,
            hidden: true
        });
    });

    return config;
}

/*
 *  ======== getGrantIntTrigger ========
 *  Get the interrupt trigger for the grant signal
 *
 *  @param inst     - Module instance object containing the config
 *  @return         - Interrupt trigger in string format e.g. "Rising Edge"
 */
function getGrantIntTrigger(inst) {

    if (inst["coexPinGrantIdleLevel"]) {
        return "Rising Edge";
    }
    else {
        return "Falling Edge";
    }
}

/*
 *  ======== getCoexPinInfo ========
 *  Get list with info on enabled coex signals.
 *
 *  @param inst     - Module instance object containing the config
 *  @return         - Array of coex pin objects
 */
function getCoexPinInfo(inst) {
    const {coexEnable: enable, coexMode: mode} = inst;

    const request = {
        name: "rfCoexRequestPin",
        displayName: "RF Coex REQUEST Pin",
        signal: "REQUEST",
        requiredArgs: {
            $name: inst["rfCoexRequestPinSymbol"],
            mode: "Output",
            initialOutputState: "Low",
            outputStrength: "High",
            invert: inst["coexPinRequestIdleLevel"] ? true : false
        }
    };

    const priority = {
        name: "rfCoexPriorityPin",
        displayName: "RF Coex PRIORITY Pin",
        signal: "PRIORITY",
        requiredArgs: {
            $name: inst["rfCoexPriorityPinSymbol"],
            mode: "Output",
            initialOutputState: "Low",
            outputStrength: "High",
            invert: inst["coexPinPriorityIdleLevel"] ? true : false
        }
    };

    const grant = {
        name: "rfCoexGrantPin",
        displayName: "RF Coex GRANT Pin",
        signal: "GRANT",
        requiredArgs: {
            $name: inst["rfCoexGrantPinSymbol"],
            mode: "Input",
            interruptTrigger: getGrantIntTrigger(inst),
            callbackFunction: "rfDriverCoexPinsHwi",
            invert: inst["coexPinGrantIdleLevel"] ? false : true
        }
    };

    const coexPinInfo = [];
    if(enable){
        switch(mode) {
            case "coexMode3Wire":
                coexPinInfo.push(request, priority, grant);
                break;
            case "coexMode2Wire":
                    coexPinInfo.push(request, grant);
                    break;
            case "coexMode1WireRequest":
                coexPinInfo.push(request);
                break;
            case "coexMode1WireGrant":
                coexPinInfo.push(grant);
                break;
        }
    }
    return coexPinInfo;
}

/*
 *******************************************************************************
 OnChange functions
 *******************************************************************************
 */

function onHardwareChanged(inst, ui) {
    const hardware = inst.$hardware;
    const hasHardware = hardware !== null;

    /* Update pin symbol names, if any signals */
    const hwSignals = hasHardware ? Object.keys(hardware.signals) : [];
    hwSignals.forEach((signal, index) => {
        inst[`rfAntennaPinSymbol${index}`] = SYM_PREFIX + hardware.signals[signal].name;
    });

    /* 
     * Temporary workaround:
     * Correction of signal mapping for the antenna switch
     * on board LP_CC1311P3. The antenna switch logic will be
     * wrong if signal "HIGH_PA" is mapped to device ball 42.
     */
    if (
        hasHardware
        && !!deviceData.deviceId.match(/CC1311P3/)
        && (hardware.signals["HIGH_PA"].devicePin.ball === "42")
    ) {
        /* Swap antenna pin symbol defines to adjust signal mapping */
        const tmp                = inst.rfAntennaPinSymbol0;
        inst.rfAntennaPinSymbol0 = inst.rfAntennaPinSymbol1;
        inst.rfAntennaPinSymbol1 = tmp;
    }

    /* Update antenna pin selection to number of HW signals */
    updateConfigValue(inst, ui, "pinSelectionAntenna", hwSignals.length);
    ui.pinSelectionAntenna.readOnly = hasHardware;
}

/*
 *  ======== onPinSelectionChanged ========
 *  Called when config pinSelection changes.
 *
 *  @param inst - Module instance object containing config that changed
 *  @param ui   - User Interface state object
 */
function onPinSelectionChanged(inst, ui) {
    /* Deprecated.
     * Write length of pinSelection list to
     * pinSelectionAntenna to support new selection approach.
     */
    const length = inst.pinSelection.length;
    const nPins = length > N_MAX_ANTENNA_PINS ? N_MAX_ANTENNA_PINS : length;
    updateConfigValue(inst, ui, "pinSelectionAntenna", nPins);
}

/*
 *  ======== onPinSelectionAntennaChanged ========
 *  Called when config pinSelectionAntenna changes.
 *
 *  @param inst - Module instance object containing config that changed
 *  @param ui   - User Interface state object
 */
function onPinSelectionAntennaChanged(inst, ui) {
    updatePinSymbols(inst, ui);

    /* Update global event mask */
    let events = [];
    if(inst.pinSelectionAntenna > 0) {
        /* Combine existing event mask with required events */
        events = [].concat(inst.globalEventMask, requiredGlobalEvents.antennaSwitch);
    }
    else {
        /* Strip out the required events from the event mask */
        events = inst.globalEventMask.filter(event => !requiredGlobalEvents.antennaSwitch.includes(event));
    }
    updateConfigValue(inst, ui, "globalEventMask", events);
}

/*
 *  ======== onGlobalEventMaskChanged ========
 *  Called when config globalEventMask changes
 *
 *  @param inst - Module instance object containing config that changed
 *  @param ui   - User Interface state object
 */
function onGlobalEventMaskChanged(inst, ui) {
    const noEvents = (inst.globalEventMask.length === 0)
    if (noEvents) {
        ui.globalCallbackFunction.readOnly = true;
        resetDefaultValue(inst, "globalCallbackFunction");
    }
    else {
        ui.globalCallbackFunction.readOnly = false;
        if (isConfigDefaultValue(inst, "globalCallbackFunction")) {
            inst.globalCallbackFunction = "rfDriverCallback";
        }
    }
}

/*
 *  ======== onCoexEnableChanged ========
 *  Called when config coexEnable changes
 *
 *  @param inst   - Module instance object containing config that changed
 *  @param ui     - User Interface state object
 *  @param config - Name of calling config
 */
function onCoexEnableChanged(inst, ui, config) {
    const {coexEnable: enabled, coexPhy: coexPhy} = inst;

    const coexInstances = Object.keys(ui).filter(key => ((key.includes("coex")
        || key.includes("ble") || key.includes("ieee")) && key !== "coexEnable" && key !== config));

    /* Show coex config instances according to coexEnable state */
    coexInstances.forEach(instance => {
        if(instance.includes("ble"))
        {
            ui[instance].hidden = !(enabled && coexPhy.includes("ble"));
        }
        else if (instance.includes("ieee"))
        {
            ui[instance].hidden = !(enabled && coexPhy.includes("ieee_15_4"));
        }
        else if (instance.includes("Latency"))
        {
            ui[instance].hidden = !(enabled && coexPhy.includes("ieee_15_4"));
        }
        else {
            ui[instance].hidden = !enabled;
        }
    });

    /* Reset config instances to default value if hidden */
    coexInstances.filter(instance => ui[instance].hidden).forEach(instance => resetDefaultValue(inst, instance));

    /* Update global event mask */
    let events = [];
    if(enabled) {
        /* Combine existing event mask with required events */
        events = [].concat(inst.globalEventMask, requiredGlobalEvents.coex);
    }
    else {
        /* Strip out the required events from the event mask */
        events = inst.globalEventMask.filter(event => !requiredGlobalEvents.coex.includes(event));
    }
    updateConfigValue(inst, ui, "globalEventMask", events);

    /* Update pin symbols */
    updatePinSymbols(inst, ui);

    /* Update coex config structure */
    updateCoexConfig(inst);
}

/*
 *  ======== onCoexModeChanged ========
 *  Called when config coexMode changes
 *
 *  @param inst - Module instance object containing config that changed
 *  @param ui   - User Interface state object
 */
function onCoexModeChanged(inst, ui) {
    const {coexPhy: coexPhy, coexMode: mode} = inst;
    const coexPinInstances = Object.keys(ui).filter(key => key.includes("coexPin"));

    /* Update pin config visibility according to mode */
    coexPinInstances.forEach(instance => {
        if(instance.includes("Request")) {
            ui[instance].hidden = (mode == "coexMode1WireGrant");
        }
        else if(instance.includes("Priority")) {
            ui[instance].hidden = (mode !== "coexMode3Wire");
        }
        else if(instance.includes("Grant")) {
            ui[instance].hidden = (mode == "coexMode1WireRequest");
        }
    });

    /* Update ble config visibility according to mode */
    const coexBleInstances = Object.keys(ui).filter(key => key.substring(0,3) === "ble");
    if(coexPhy.includes("ble"))
    {
        coexBleInstances.forEach(instance => {
            if(instance.includes("DefaultPriority")) {
                ui[instance].hidden = (mode !== "coexMode3Wire");
            }
            else if(instance.includes("AssertRequestForRx")) {
                ui[instance].hidden = (mode == "coexMode1WireGrant");
            }
        });
    }
    else
    {
        const coexIeeeInstances = Object.keys(ui).filter(key => key.substring(0,3) === "ieee");
        coexIeeeInstances.forEach(instance => {
            if(instance.includes("DefaultPriority")) {
                ui[instance].hidden = (mode !== "coexMode3Wire");
            }
            else if(instance.includes("AssertRequestForRx")) {
                ui[instance].hidden = (mode == "coexMode2Wire");
            }
        });
    }

    /* Reset config instance to default value if hidden */
    const hiddenInstances = coexPinInstances.concat(coexBleInstances).filter(instance => ui[instance].hidden);
    hiddenInstances.forEach(instance => resetDefaultValue(inst, instance));

    /* Update pin symbols */
    updatePinSymbols(inst, ui);

    /* Update coex config structure */
    updateCoexConfig(inst);
}

/*
 *  ======== onCoexPriorityIndicationTimeChanged ========
 *  Called when config coexPinPriorityIndicationTime changes
 *
 *  @param inst - Module instance object containing config that changed
 *  @param ui   - User Interface state object
 */
function onCoexPriorityIndicationTimeChanged(inst, ui) {
    /* Update coex config structure */
    updateCoexConfig(inst);
}

/*
 *  ======== onCoexGrantLatencyChanged ========
 *  Called when config coexGrantLatencyTime changes
 *
 *  @param inst - Module instance object containing config that changed
 */
function onCoexGrantLatencyTimeChanged(inst) {
    coexConfig.grantLatencyTime = inst.coexGrantLatencyTime;
}

/*
 *******************************************************************************
 Update functions
 *******************************************************************************
 */

/*
 *  ======== resetDefaultValue ========
 *  Set specified configuration option to it's default value
 *
 *  @param inst     - Module instance object containing the config
 *  @param config   - Config option to reset
 */
function resetDefaultValue(inst, config){
    if(config in inst) {
        inst[config] = inst.$module.$configByName[config].default;
    }
}

/*
 *  ======== updateConfigValue ========
 *  Update config value and trigger onChange member function.
 *
 *  @param inst     - Module instance object containing the config
 *  @param ui       - UI state object
 *  @param config   - Config option part of module
 *  @param value    - New value for config
 */
function updateConfigValue(inst, ui, config, value){
    if(config in inst) {
        inst[config] = value;
        inst.$module.$configByName[config].onChange(inst, ui);
    }
}

/*
 *  ======== updatePinSymbols ========
 *  Update pin symbol visibility according to selected pins.
 *
 *  @param inst - Module instance object containing config that changed
 *  @param ui   - User Interface state object
 */
function updatePinSymbols(inst, ui) {
    const pinSymbolInstances = Object.keys(inst).filter(key => key.match("PinSymbol"));

    /* Hide all pin symbol instances */
    pinSymbolInstances.forEach(symbol => ui[symbol].hidden = true);

    /* Show enabled antenna pins */
    const nPins = inst.pinSelectionAntenna;
    const hasHardware = (getRfHardware(inst) !== null);
    for(let i = 0; i < nPins; i++) {
        const symInst = `rfAntennaPinSymbol${i}`
        ui[symInst].hidden = false;
        ui[symInst].readOnly = hasHardware;
    }

    /* Show enabled coex pins */
    const coexPinInfo = getCoexPinInfo(inst);
    coexPinInfo.forEach(pin => {
        const symInst = `${pin.name}Symbol`
        ui[symInst].hidden = false;
        ui[symInst].readOnly = true;
    });

    /* Reset pin to default value if hidden */
    pinSymbolInstances.filter(instance => ui[instance].hidden).forEach(instance => {
        resetDefaultValue(inst, instance);
        ui[instance].readOnly = false;
    });
}

/*
*  ======== updateCoexConfig ========
*  Update coex configuration stored in coexConfig structure.
*
*  @param inst - Module instance object containing config that changed
*/
function updateCoexConfig(inst) {
    const {coexEnable: enable, coexPhy: coexPhy, coexMode: mode, coexPinPriorityIndicationTime: priIndicationTime} = inst;

    /* Coex state info */
    coexConfig.coExEnable = {
        bCoExEnable: Number(enable),
        bUseREQUEST: Number(enable && (mode === "coexMode3Wire"
            || mode === "coexMode2Wire" || mode === "coexMode1WireRequest")),
        bUseGRANT: Number(enable && (mode === "coexMode3Wire"
            || mode === "coexMode2Wire"|| mode === "coexMode1WireGrant")),
        bUsePRIORITY: Number(enable && mode === "coexMode3Wire"),
        bRequestForChain: Number(enable && coexPhy.includes("ieee_15_4"))
    };
    coexConfig.priorityIndicationTime = priIndicationTime;

    /* BLE Use Case Config info */
    coexConfigBle.bleInitiator = {
        defaultPriority: priorityEnumLookup[inst.bleIniDefaultPriority],
        assertRequestForRx: requestEnumLookup[Number(inst.bleIniAssertRequestForRx)]
    };
    coexConfigBle.bleConnected = {
        defaultPriority: priorityEnumLookup[inst.bleConDefaultPriority],
        assertRequestForRx: requestEnumLookup[Number(inst.bleConAssertRequestForRx)]
    };
    coexConfigBle.bleBroadcaster = {
        defaultPriority: priorityEnumLookup[inst.bleBroDefaultPriority],
        assertRequestForRx: requestEnumLookup[Number(inst.bleBroAssertRequestForRx)]
    };
    coexConfigBle.bleObserver = {
        defaultPriority: priorityEnumLookup[inst.bleObsDefaultPriority],
        assertRequestForRx: requestEnumLookup[Number(inst.bleObsAssertRequestForRx)]
    };
}

/*
 *******************************************************************************
 Module Dependencies
 *******************************************************************************
 * When change occurs within module, the default module functions (if declared)
 * will be triggered when a configurable changes state:
 *  - pinmuxRequirements(...)
 *  - moduleInstances(...)
 *  - sharedModuleInstances(...)
 *  - modules(...)
 */

/* Store filter functions to keep static between pinmux requirement changes */
const filterFunctions = [];

/*!
 *  ======== pinmuxRequirements ========
 *  Return peripheral pin requirements as a function of config.
 *
 *  Called when a configuration changes in the module.
 *
 *  @param inst - Module instance containing the config that changed
 *  @return     - Array of pin requirements
 */
function pinmuxRequirements(inst) {
    const rfArray = [];

    /* Requirements for antenna pins */
    const hardware = getRfHardware(inst);
    const hasHardware = hardware !== null;
    const signals = hasHardware ? Object.values(hardware.signals) : [];
    const antennaSwitchInPackage = isAntennaSwitchInPackage();

    const nPins = inst.pinSelectionAntenna;
    for(let i = 0; i < nPins; i++) {
        const pinReq = {
            name: "rfAntennaPin" + i,
            displayName: "RF Antenna Pin " + i,
            hidden: false,
            interfaceName: "GPIO"
        };
        if (antennaSwitchInPackage) {
            /* Protect against out of bounds indexing when migrating from any device to PSIP */
            if (i < signals.length) {
                /* Only provide the correct 1:1 mapping as the option */
                if(!filterFunctions[i]) {
                    filterFunctions[i] = (pin) => pin.designSignalName === signals[i].dio;
                }
                pinReq.filter = filterFunctions[i];
            }
        } else if (hasHardware) {
            /* Map pins to the correct unique signal type */
            pinReq.signalTypes = signals[i].type;
        }
        rfArray.push(pinReq);
    }

    /* Requirements for coex pins */
    const coexPinInfo = getCoexPinInfo(inst);
    coexPinInfo.forEach(pin => {
        rfArray.push({
            name: pin.name,
            displayName: pin.displayName,
            interfaceName: "GPIO",
            signalTypes: ["external-to-hardware"]
        });
    });

    return rfArray;
}

/*
 *  ======== moduleInstances ========
 *  Determines what modules are added as non-static submodules.
 *
 *  Called when a configuration changes in the module.
 *
 *  @param inst - Module instance containing the config that changed
 *  @return     - Array of PIN instances
 */
function moduleInstances(inst) {
    const dependencyModules = [];

    /* Add PIN instances for antenna pins */
    const nPins = inst.pinSelectionAntenna;
    for(let i = 0; i < nPins; i++) {
        const pinInstance = {
            name: `rfAntennaPin${i}Instance`,
            displayName: `GPIO Configuration For Antenna ${i} Pin`,
            moduleName: "/ti/drivers/GPIO",
            readOnly: false,
            collapsed: true,
            requiredArgs: {
                $name: inst[`rfAntennaPinSymbol${i}`],
                parentInterfaceName: "GPIO",
                parentSignalName: `rfAntennaPin${i}`,
                parentSignalDisplayName: `RF Antenna Pin ${i}`,
                mode: "Output",
                initialOutputState: "Low",
                outputStrength: "High",
            }
        };
        dependencyModules.push(pinInstance);
    }

    /* Add PIN instances for coex pins */
    const coexPinInfo = getCoexPinInfo(inst);
    coexPinInfo.forEach(pin => {
        const pinInstance = {
            name: pin.name + "Instance",
            displayName: `GPIO Configuration For Coex ${pin.signal} Pin`,
            moduleName: "/ti/drivers/GPIO",
            readOnly: false,
            collapsed: true,
            requiredArgs: {
                parentInterfaceName: "GPIO",
                parentSignalName: pin.name,
                parentSignalDisplayName: pin.displayName,
            }
        };

        /* We don't expect users to change these, so set as required */
        pinInstance.requiredArgs = {...pinInstance.requiredArgs, ...pin.requiredArgs};
        dependencyModules.push(pinInstance);
    })

    return dependencyModules;
}

/*
 *  ======== sharedModuleInstances ========
 *  Determines what modules are added as shared static sub-modules.
 *
 *  Called when a configuration changes in the module.
 *
 *  @param inst - Module instance containing the config that changed
 *  @return     - Array containing dependency modules
 */
function sharedModuleInstances(inst){
    return [];
}

/*
 *  ======== modules ========
 *  Determines what modules are added as static sub-modules.
 *
 *  Called when a configuration changes in the module.
 *
 *  @param inst - Module instance containing the config that changed
 *  @return     - Array containing static dependency modules
 */
function modules(inst) {

    /* Here we rely on the CCFG module to pull in temperature if the user
     * has selected temperature compensation (SW TCXO)
     */
    const dependencies = ["Board", "Power"];

    return Common.autoForceModules(dependencies)();
}

/*
 *******************************************************************************
 GenLibs
 *******************************************************************************
 */

/*!
 *  ======== getLibs ========
 *  Return link option argument for GenLibs.
 *
 *  @param mod  - Module object
 *  @return     - Link option object
 */
function getLibs(mod) {
    /* Toolchain specific information */
    const GenLibs = system.getScript("/ti/utils/build/GenLibs");
    const DriverLib = system.getScript("/ti/devices/DriverLib");
    const isa = GenLibs.getDeviceIsa();
    const toolchain = GenLibs.getToolchainDir();

    /* get device information from DriverLib */
    const deviceId = system.deviceData.deviceId;
    let libName = DriverLib.getAttrs(deviceId).libName;
    if (libName) {
        libName = libName.replace(/x2x7/, "x2");
    }

    /* Create a GenLibs input argument */
    return {
        name: mod.$name,
        libs: [
            `ti/drivers/rf/lib/${toolchain}/${isa}/rf_multiMode_${libName}.a`
        ],
        deps: [
            "/ti/drivers"
        ]
    };
}

/*
 *******************************************************************************
 Module Validation
 *******************************************************************************
 */

/*!
 *  ======== validate ========
 *  Validate rfdriver module configuration.
 *
 *  @param inst         - Module instance containing the config that changed
 *  @param validation   - Object to hold detected validation issues
 */
function validate(inst, validation) {
    const {
        coexEnable: coexEnable,
        coexPhy: coexPhy,
        coexMode: coexMode,
        coexGrantLatencyTime: coexGrantLatencyTime,
        globalEventMask: globalEventMask,
        globalCallbackFunction: cbFxn,
        pinSelectionAntenna: nAntennaPins
    } = inst;
    const hardware = getRfHardware(inst);

    if(coexEnable) {
        /* Coexistence feature is dependent on BLE or IEEE 15.4 */
        if(!isPhy(inst, "ble") && !isPhy(inst, "ieee_15_4")) {
            Common.logError(validation, inst, "coexEnable",
            "'RF Coexistence' is only supported with BLE and IEEE 15.4");
        }

        /* Coex not supported on both PHYs simultaneously */
        if(coexPhy.length !== 1)
        {
            Common.logError(validation, inst, "coexPhy",
            "Only one PHY can be selected for 'RF Coexistence'");
        }
        else
        {
            /* Coex PHY needs to match RF configuration  */
            coexPhy.forEach(phy => {
                if(!isPhy(inst, phy))
                {
                    Common.logError(validation, inst, "coexPhy",
                    `${Docs.coex.phy[phy].displayName} PHY not selected in current RF configuration`);
                }
            });
        }

        /* Coex mode support depend on PHY */
        coexPhy.forEach(phy => {
            if(!coexModeSupport[phy].includes(coexMode))
            {
                Common.logError(validation, inst, "coexMode",
                "Please select a valid mode for coexistence configuration");
            }
        });

        /* Coex Grant Latency Time should not exceed 80us */
        if (coexGrantLatencyTime > 80 || coexGrantLatencyTime < 20)
        {
            Common.logError(validation, inst, "coexGrantLatencyTime",
            "Grant Latency Time must be between 20-80 usec");
        }
    }

    /* Check that globalCallbackFunction is a C identifier */
    if (!Common.isCName(cbFxn)) {
        Common.logError(validation, inst, "globalCallbackFunction",
        "'" + cbFxn + "' is not a valid a C identifier");
    }

    /* Warn user when a feature requires missing events */
    if (hardware !== null) {
        const missingEventsAntennaSwitch = requiredGlobalEvents.antennaSwitch.filter(event => !globalEventMask.includes(event));
        if (!(requiredGlobalEvents.antennaSwitch.every(event => globalEventMask.includes(event)))) {
            Common.logWarning(validation, inst, "globalEventMask",
                `Event(s) missing required for antenna switching: ${missingEventsAntennaSwitch.join(", ")}.`);
        }
    }
    if (coexEnable) {
        const missingEventsCoex = requiredGlobalEvents.coex.filter(event => !globalEventMask.includes(event));
        if (!(requiredGlobalEvents.coex.every(event => globalEventMask.includes(event)))) {
            Common.logWarning(validation, inst, "globalEventMask",
                `Event(s) missing required for coex: ${missingEventsCoex.join(", ")}.`);
        }
    }

    /* Notify user when the antenna switching callback function must be implemented */
    if (
        (cbFxn !== "NULL")
        && (nAntennaPins > 0)
        && ((hardware === null) || (!hwSupported.includes(hardware.name)))
    ) {
        Common.logInfo(validation, inst, "globalCallbackFunction",
        `Please see function '${cbFxn}AntennaSwitching' in 'ti_drivers_config.c'. The antenna switching functionality must be implemented by the user.`);
    }

    /* Verify that there are no pin name conflicts */
    const pinSymbolInstances = Object.keys(inst).filter(key => key.match("PinSymbol"));
    const pinNames = [];
    pinSymbolInstances.forEach(instance => {
        if(pinNames.includes(inst[instance])) {
            Common.logError(validation, inst, instance, "Conflicting PIN symbol names");
        }
        else {
            pinNames.push(inst[instance]);
        }
    });

    /* Antenna switch is required for P boards */
    const board = isCustomDesign() ? null : system.deviceData.board;
    if ((board !== null) && (!!board.name.match(/CC....P/i)) && (deviceData.package !== "SIP")) {
        if (inst.$hardware === null) {
            Common.logWarning(validation, inst, "$hardware", `The board '${board.displayName}' requires the 'RF Antenna Switch' to be selected.`);
        }
    }
}

/*
 *******************************************************************************
 Module Export
 *******************************************************************************
 */

 /*
 *  ======== rfdriverModule ========
 *  Define the RF Driver module properties and methods.
 */
let rfdriverModule = {
    moduleStatic: {
        config,
        pinmuxRequirements,
        moduleInstances,
        sharedModuleInstances,
        modules,
        validate,
        filterHardware,
        onHardwareChanged
    },
    isCustomDesign: isCustomDesign,
    isCoexSupport: isCoexSupport,
    getRfHardware: getRfHardware,
    getCoexConfig: function(phy = "ble") {
        return {
            coExEnable: coexConfig.coExEnable,
            coExTxRxIndication: coexConfig.coExTxRxIndication,
            priorityIndicationTime: coexConfig.priorityIndicationTime,
            grantLatencyTime: coexConfig.grantLatencyTime,
            ...coexConfig[phy]
        };
    },
    getCoexConfigBle: function() {
        return coexConfigBle;
    },

    /* override device-specific templates */
    templates: {
        /* contribute libraries to linker command file */
        "/ti/utils/build/GenLibs.cmd.xdt":
            {modName: "/ti/drivers/RF", getLibs},

        boardc: "/ti/drivers/rf/RF.Board.c.xdt",
        board_initc: "/ti/drivers/rf/RF.Board_init.c.xdt",
        boardh: "/ti/drivers/rf/RF.Board.h.xdt"
    }
}

/* Config for internal use */
try {
    rfdriverModule = system.getScript("rfc.syscfg.js").extend(rfdriverModule);
}
catch(e) {
    if (!((e instanceof Error) && e.message.includes("No such resource"))) {
        throw e;
    }
}

/*
 *  ======== extend ========
 *  Extends a base exports object to include any device specifics
 *
 *  This function is invoked by the generic RF module to
 *  allow us to augment/override as needed for the CC26XX
 */
function extend(base) {
    /* merge and overwrite base module attributes */
    return({...base, ...rfdriverModule});
}

/*
 *  ======== exports ========
 *  Export device-specific extensions to base exports
 */
exports = {
    /* required function, called by generic RF module */
    extend: extend
};
