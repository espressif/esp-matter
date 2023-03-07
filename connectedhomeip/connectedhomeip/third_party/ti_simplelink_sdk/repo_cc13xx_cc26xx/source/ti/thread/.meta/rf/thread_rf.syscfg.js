/*
 * Copyright (c) 2018, Texas Instruments Incorporated
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
 * =========================== thread_rf.syscfg.js ===========================
 *
 * Submodule definition for the SysConfig Thread RF submodule
 */

"use strict";

const deviceId = system.deviceData.deviceId;

// Get RF command handler
const CmdHandler = system.getScript("/ti/devices/radioconfig/cmd_handler.js");

// Get RF design functions
const RfDesign = system.getScript("/ti/devices/radioconfig/rfdesign");

// Get radio config module functions
const RadioConfig = system.getScript("/ti/devices/radioconfig/radioconfig_common.js");

/* Description text for configurables */
const rfDesignDescription = "Select which RF design to use as template";
const rfDesignLongDescription = `The user must select an existing TI RF design \
to reference for radio configuration. This value must match the Based on RF \
Design parameter in the RF module.

__Default__: The RF design reference selected in this project is automatically \
configured based on the example. Please move to a custom board or see the \
other examples provided in the SDK if another reference board is desired.`;

const channelDescription = `The default IEEE 802.15.4 frequency channel for \
the device`;

const channelLongDescription = `The default IEEE 802.15.4 frequency channel \
for the device

**Default Channel:** 14

**Range:** Channel 11-26`;

const coexDescription = `
Enable the Thread/Wi-Fi coexistence interface
`;

const coexLongDescription = `
Enable the TI-OpenThread interface for controlling the Thread/Wi-Fi coexistence
module. This only enables the TI-OpenThread API for controlling CoEx request
priority. The configuration of the coexistence interface is handled by the RF
module.

Ensure that the IEEE 802.15.4 CoEx PHY is selected in the RF coexistence
configuration.
`;

const coexPriorityDescription = `
Sets the global TI-OpenThread priority configuration
`;

const coexPriorityLongDescription = `
Sets the global command priority configuration used by TI-OpenThread. This is
used in 3-Wire and greater CoEx to control the priority request per-command.

The API enabled by this configuration may be used to modify the priority of
radio operations at runtime according to application requirements.
`;

const coexPriorityDefaultDescription = `
The command level priority request is not modified from the default
`;

const coexPriorityHighDescription = `
The priority line will be asserted for every command until set otherwise
`;

const coexPriorityLowDescription = `
The priority line will be deasserted for every command until set otherwise
`;


const coexRequestDescription = `
Sets the global TI-OpenThread request configuration
`;

const coexRequestLongDescription = `
Sets the global command request configuration used by TI-OpenThread. This
configuration controls the behavior of the request signal during receive
operations. Normal operation will not assert the request line for receive
operations. This is used in 2-Wire and greater CoEx.

The API enabled by this configuration may be used to modify the behavior of the
request line at runtime according to application requirements.
`;

const coexRequestDefaultDescription = `
The command level request is not modified from the default
`;

const coexRequestAssertRxDescription = `
The request line will be asserted for receive operations until set otherwise
`;

const coexRequestNoAssertRxDescription = `
The request line will not be asserted for receive operations until set
otherwise
`;


const txpowerDescription = `The default transmit power in dBm`;

const txpowerLongDescription = `The default transmit power in dBm\

**Default Power:** 0`;

/* RadioConfig module definition and default configurations */
function moduleInstances(inst)
{
    const cmdList = [
        "cmdIeeeTx",
        "cmdIeeeRx",
        "cmdIeeeCsma",
        "cmdIeeeEdScan",
        "cmdIeeeRxAck",
        "cmdTxTest"
    ];

    const radioConfigArgs = {
        frequency: 2405 + (5 * (inst.channel - 11)),
        codeExportConfig: {
            symGenMethod: "Legacy",
            paExport: "active",
            useConst: true,
            cmdList_ieee_15_4: cmdList
        }
    };

    const radioConfigModule = {
        name: "radioConfig",
        displayName: "Advanced Radio Settings",
        moduleName: "/ti/devices/radioconfig/settings/ieee_15_4",
        readOnly: false, // The user needs to be able to edit this config if boards are migrated
        collapsed: true,
        args: radioConfigArgs
    };

    if(deviceId.match(/CC(265[12]R|2674R|1352R1|1354R)/))
    {
        cmdList.push("cmdRadioSetup");
        radioConfigArgs.txPower = RfDesign.getTxPowerOptions(2405, false)[0].name;
    }
    else if(deviceId.match(/CC(265[12]P|2653P|2674P|1352P)/))
    {
        cmdList.push("cmdRadioSetupPa");
        radioConfigArgs.highPA = true;
        radioConfigArgs.codeExportConfig.paExport = "combined";
        radioConfigArgs.txPowerHi = RfDesign.getTxPowerOptions(2405, true)[0].name;
    }
    else if(deviceId.match(/CC(2654P|1354P)/))
    {
        // currently not characterized for high PA
        cmdList.push("cmdRadioSetupPa");
        radioConfigArgs.txPower = RfDesign.getTxPowerOptions(2405, false)[0].name;
    }
    else
    {
        throw new Error("Could not match platform to any known platform types");
    }

    /* Special handling for the P-4 2.4GHz front end characterization settings */
    if(inst.rfDesign.match(/(LP_|LAUNCHXL-)CC1352P(7)?-4/))
    {
        radioConfigArgs.phyType = "ieee154p10";
        radioConfigArgs.txPowerHi = "10";
    }

    return([radioConfigModule]);
}

/*
 *  ======== getRfDesignOptions ========
 *  Generates an array of SRFStudio compatible rfDesign options based on device
 *
 * @param deviceId - device being used
 *
 * @returns Array - Array of rfDesign options, if the device isn't supported,
 *                  returns null
 */
function getRfDesignOptions()
{
    const deviceId = system.deviceData.deviceId;
    let newRfDesignOptions = null;

    if(deviceId === "CC1352P1F3RGZ")
    {
        newRfDesignOptions = [
            {name: "LAUNCHXL-CC1352P-2"},
            {name: "LAUNCHXL-CC1352P-4"}
        ];
    }
    else if(deviceId === "CC1352R1F3RGZ")
    {
        newRfDesignOptions = [{name: "LAUNCHXL-CC1352R1"}];
    }
    else if(deviceId === "CC2652R1FRGZ")
    {
        newRfDesignOptions = [{name: "LAUNCHXL-CC26X2R1"}];
    }
    else if(deviceId === "CC2652RB1FRGZ")
    {
        newRfDesignOptions = [{name: "LP_CC2652RB"}];
    }
    else if(deviceId === "CC2652R1FSIP")
    {
        newRfDesignOptions = [{name: "LP_CC2652RSIP"}];
    }
    else if(deviceId === "CC2652P1FSIP")
    {
        newRfDesignOptions = [{name: "LP_CC2652PSIP"}];
    }
    else if(deviceId === "CC2652R7RGZ")
    {
        newRfDesignOptions = [{name: "LP_CC2652R7"}];
    }
    else if(deviceId === "CC1352P7RGZ")
    {
        newRfDesignOptions = [{name: "LP_CC1352P7-4"}];
    }
    else if(deviceId === "CC2651P3RGZ")
    {
        newRfDesignOptions = [{name: "LP_CC2651P3"}];
    }
    else if(deviceId === "CC2651R3RGZ")
    {
        newRfDesignOptions = [{name: "LP_CC2651R3"}];
    }
    else if(deviceId === "CC1354P10RSK")
    {
        newRfDesignOptions = [
            {name: "LP_EM_CC1354P10_1"},
            {name: "LP_EM_CC1354P10_6"}
        ];
    }
    else if(deviceId === "CC1314R10RSK")
    {
        newRfDesignOptions = [{name: "LP_EM_CC1314R10"}];
    }
    else if(deviceId === "CC2653P10RSK")
    {
        newRfDesignOptions = [{name: "LP_EM_CC2653P10"}];
    }
    else if(deviceId === "CC2674R10RSK")
    {
        newRfDesignOptions = [{name: "LP_CC2674R10_FPGA"}];
    }
    else
    {
        throw new Error("Unknown deviceId " + deviceId + ".");
    }

    return(newRfDesignOptions);
}

/*!
 * ======== getTxPowerConfigOptions ========
 *
 * Get list of available Tx power values
 *
 * @returns a list of available transmit power options from the radio config
 */
function getTxPowerConfigOptions()
{
    /* TX power options are based on the chip, not the current RF design. There
     * is no way to propagate the "based on" RF design into the RF design
     * option. This may lead to some incorrect options in the TX power
     * configurable.
     */
    let txPowerValueList = [];
    // Regular PA options
    txPowerValueList = _.concat(txPowerValueList, RfDesign.getTxPowerOptions(2405, false));
    // High PA options (if available)
    if(deviceId.match(/CC(2652P|2674P|1352P)/))
    {
        txPowerValueList = _.concat(txPowerValueList, RfDesign.getTxPowerOptions(2405, true));
    }

    // Round all tx power values
    _.forEach(txPowerValueList, (option) =>
    {
        option.name = _.round(parseInt(option.name));
    });

    // Remove any duplicates
    txPowerValueList = _.uniqBy(txPowerValueList, "name");

    // Sort values in descending order
    txPowerValueList = _.orderBy(txPowerValueList, "name", "desc");

    return(txPowerValueList);
}

/*
 *  ======== onCoexEnableChanged ========
 *  Called when config coexEnable changes
 *
 *  @param inst   - Module instance object containing config that changed
 *  @param ui     - User Interface state object
 */
function onCoexEnableChanged(inst, ui) {
    const {coexEnable: enabled} = inst;

    // show configuration
    ui["coexPriority"].hidden = !enabled;
    ui["coexRequest"].hidden  = !enabled;
}

/*!
 * ======== rfModule ========
 *
 * Syscfg submodule for TI-OpenThread RF
 */
const rfModule = {
    config: [
        {
            name: "rfDesign",
            displayName: "Based On RF Design",
            description: rfDesignDescription,
            longDescription: rfDesignLongDescription,
            options: getRfDesignOptions(),
            default: getRfDesignOptions()[0].name
        },
        {
            name: "channel",
            displayName: "Default Channel",
            description: channelDescription,
            longDescription: channelLongDescription,
            default: 14,
            options: [
                {name: 11, displayName: "11 - 2405 MHz"},
                {name: 12, displayName: "12 - 2410 MHz"},
                {name: 13, displayName: "13 - 2415 MHz"},
                {name: 14, displayName: "14 - 2420 MHz"},
                {name: 15, displayName: "15 - 2425 MHz"},
                {name: 16, displayName: "16 - 2430 MHz"},
                {name: 17, displayName: "17 - 2435 MHz"},
                {name: 18, displayName: "18 - 2440 MHz"},
                {name: 19, displayName: "19 - 2445 MHz"},
                {name: 20, displayName: "20 - 2450 MHz"},
                {name: 21, displayName: "21 - 2455 MHz"},
                {name: 22, displayName: "22 - 2460 MHz"},
                {name: 23, displayName: "23 - 2465 MHz"},
                {name: 24, displayName: "24 - 2470 MHz"},
                {name: 25, displayName: "25 - 2475 MHz"},
                {name: 26, displayName: "26 - 2480 MHz"}
            ]
        },
        {
            name: "txpower",
            displayName: "Default Power",
            description: txpowerDescription,
            longDescription: txpowerLongDescription,
            options: getTxPowerConfigOptions(),
            default: 0
        },
        {
            name: "coexEnable",
            displayName: "Enable Coexistence",
            description: coexDescription,
            longDescription: coexLongDescription,
            default: false,
            onChange: onCoexEnableChanged
        },
        {
            name: "coexConfigGroup",
            displayName: "TI-OpenThread Coexistence Configuration",
            collapsed: false,
            config: [
                {
                    name: "coexPriority",
                    displayName: "Priority Request",
                    description: coexPriorityDescription,
                    longDescription: coexPriorityLongDescription,
                    hidden: true,
                    default: "default",
                    options: [
                        {
                            name: "default",
                            displayName: "Default",
                            description: coexPriorityDefaultDescription
                        },
                        {
                            name: "high",
                            displayName: "High",
                            description: coexPriorityHighDescription
                        },
                        {
                            name: "low",
                            displayName: "Low",
                            description: coexPriorityLowDescription
                        }
                    ],
                },
                {
                    name: "coexRequest",
                    displayName: "Request Behavior",
                    description: coexRequestDescription,
                    longDescription: coexRequestLongDescription,
                    hidden: true,
                    default: "default",
                    options: [
                        {
                            name: "default",
                            displayName: "Default",
                            description: coexRequestDefaultDescription
                        },
                        {
                            name: "assertRx",
                            displayName: "Assert RX",
                            description: coexRequestAssertRxDescription
                        },
                        {
                            name: "noAssertRx",
                            displayName: "No Assert RX",
                            description: coexRequestNoAssertRxDescription
                        }
                    ]
                }
            ]
        }
    ],
    moduleInstances: moduleInstances
};

/* Validation function for the RF submodule */
function validate(inst, validation)
{
    // Get the RF Design module
    const rfDesign = system.modules["/ti/devices/radioconfig/rfdesign"].$static;

    if(rfDesign.rfDesign === "LAUNCHXL-CC1352P1")
    {
        validation.logError(
            `This stack does not support this board configuration`,
            rfDesign, "rfDesign"
        );
    }

    if(inst.rfDesign !== rfDesign.rfDesign)
    {
        validation.logError(`Must match ${system.getReference(rfDesign,
            "rfDesign")} in the RF Design Module`, inst, "rfDesign");
    }
}

exports = rfModule;
