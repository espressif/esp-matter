/*
 * Copyright (c) 2018 Texas Instruments Incorporated - http://www.ti.com
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
 *  ======== ble_rf_config.syscfg.js ========
 */

"use strict";

// Get rf_config long descriptions
const Docs = system.getScript("/ti/ble5stack/rf_config/ble_rf_config_docs.js");

// Get common Script
const Common = system.getScript("/ti/ble5stack/ble_common.js");

// Get the rfDesign options according to the device
const rfDesignOptions = getRfDesignOptions(system.deviceData.deviceId);

const config = {
    name: "bleRadioConfig",
    displayName: "Radio",
    description: "Configure BLE Radio Settings",
    config: [
        {
            name: "rfDesign",
            displayName: "Based On RF Design",
            description: "Select which RF Design to use as a template",
            options: rfDesignOptions,
            default: rfDesignOptions[0].name,
            onChange: onRfDesignChange,
            hidden: false
        },
        {
            // RF Front End Settings
            // Note: The use of these values completely depends on how the PCB is laid out.
            //       Please see Device Package and Evaluation Module (EM) Board below.
            name: "frontEndMode",
            displayName: "Front End Mode",
            default: "RF_FE_DIFFERENTIAL",
            deprecated: true,
            longDescription: Docs.frontEndModeLongDescription,
            options: [
                {
                    displayName: "Differential",
                    name: "RF_FE_DIFFERENTIAL"
                },
                {
                    displayName: "Single Ended RFP",
                    name: "RF_FE_SINGLE_ENDED_RFP"
                },
                {
                    displayName: "Single Ended RFN",
                    name: "RF_FE_SINGLE_ENDED_RFN"
                },
                {
                    displayName: "Antenna Diversity RFP First",
                    name: "RF_FE_ANT_DIVERSITY_RFP_FIRST"
                },
                {
                    displayName: "Antenna Diversity RFN First",
                    name: "RF_FE_ANT_DIVERSITY_RFN_FIRST"
                },
                {
                    displayName: "Single Ended RFP External Pins",
                    name: "RF_FE_SINGLE_ENDED_RFP_EXT_PINS"
                },
                {
                    displayName: "Single Ended RFN External Pins",
                    name: "RF_FE_SINGLE_ENDED_RFN_EXT_PINS"
                }
            ]
        },
        {
            name: "biasConfiguration",
            displayName: "Bias Configuration",
            default: "RF_FE_INT_BIAS",
            deprecated: true,
            longDescription: Docs.biasConfigurationLongDescription,
            options: [
                {
                    displayName: "Internal BIAS",
                    name: "RF_FE_INT_BIAS"
                },
                {
                    displayName: "External BIAS",
                    name: "RF_FE_EXT_BIAS"
                }
            ]
        },
        {
            name: "defaultTxPower",
            displayName: "Default Tx Power",
            default: "HCI_EXT_TX_POWER_0_DBM",
            description: "This is the Tx Power value the BLE stack will use",
            options: (inst) => {
                const configurable = [
                    { displayName: "-20", name: "HCI_EXT_TX_POWER_MINUS_20_DBM"},
                    { displayName: "-18", name: "HCI_EXT_TX_POWER_MINUS_18_DBM"},
                    { displayName: "-15", name: "HCI_EXT_TX_POWER_MINUS_15_DBM"},
                    { displayName: "-12", name: "HCI_EXT_TX_POWER_MINUS_12_DBM"},
                    { displayName: "-10", name: "HCI_EXT_TX_POWER_MINUS_10_DBM"},
                    { displayName: "-9",  name: "HCI_EXT_TX_POWER_MINUS_9_DBM"},
                    { displayName: "-6",  name: "HCI_EXT_TX_POWER_MINUS_6_DBM"},
                    { displayName: "-5",  name: "HCI_EXT_TX_POWER_MINUS_5_DBM"},
                    { displayName: "-3",  name: "HCI_EXT_TX_POWER_MINUS_3_DBM"},
                    { displayName: "0",   name: "HCI_EXT_TX_POWER_0_DBM"},
                    { displayName: "1",   name: "HCI_EXT_TX_POWER_1_DBM"},
                    { displayName: "2",   name: "HCI_EXT_TX_POWER_2_DBM"},
                    { displayName: "3",   name: "HCI_EXT_TX_POWER_3_DBM"},
                    { displayName: "4",   name: "HCI_EXT_TX_POWER_4_DBM"},
                    { displayName: "5",   name: "HCI_EXT_TX_POWER_5_DBM"},
                    { displayName: "6",   name: "HCI_EXT_TX_POWER_P2_14_DBM_P4_6_DBM"},
                    { displayName: "7",   name: "HCI_EXT_TX_POWER_P2_15_DBM_P4_7_DBM"},
                    { displayName: "8",   name: "HCI_EXT_TX_POWER_P2_16_DBM_P4_8_DBM"},
                    { displayName: "9",   name: "HCI_EXT_TX_POWER_P2_17_DBM_P4_9_DBM"},
                    { displayName: "10",  name: "HCI_EXT_TX_POWER_P2_18_DBM_P4_10_DBM"},
                    { displayName: "14",  name: "HCI_EXT_TX_POWER_P2_14_DBM_P4_6_DBM"},
                    { displayName: "15",  name: "HCI_EXT_TX_POWER_P2_15_DBM_P4_7_DBM"},
                    { displayName: "16",  name: "HCI_EXT_TX_POWER_P2_16_DBM_P4_8_DBM"},
                    { displayName: "17",  name: "HCI_EXT_TX_POWER_P2_17_DBM_P4_9_DBM"},
                    { displayName: "18",  name: "HCI_EXT_TX_POWER_P2_18_DBM_P4_10_DBM"},
                    { displayName: "19",  name: "HCI_EXT_TX_POWER_P2_19_DBM"},
                    { displayName: "20",  name: "HCI_EXT_TX_POWER_P2_20_DBM"}]
                return getPaTableValues(inst.rfDesign, configurable);
            }
        }
    ]
}

/*
 * ======== onRfDesignChange ========
 * Different devices have different valid pa levels.
 * Therefore, when selecting different rfDesing value (different device),
 * and the pa value is invalid set it to 0.
 *
 * @param inst - BLE instance
 * @param ui   - The User Interface object
 */
function onRfDesignChange(inst, ui)
{
    if(!validateDefaultTxPower(inst))
    {
        inst.defaultTxPower = "HCI_EXT_TX_POWER_0_DBM";
    }
}

/*
 * ======== getPaTableValues ========
 * Generates an array of SRFStudio compatible rfDesign options based on device
 *
 * @param rfDesign     - the selected device
 * @param tableOptions - the table with all pa levels options
 *
 * @returns - a list with the valid pa levels from the tableOptions
 */
function getPaTableValues(rfDesign, tableOptions)
{
    // Get the device PA table levels
    const txPowerTableType = Common.getRadioScript(rfDesign,
                             system.deviceData.deviceId).radioConfigParams.paExport;
    let currentOptions;

    // If using a device that not support high PA
    if(txPowerTableType != "combined")
    {
        currentOptions = tableOptions.filter(config => config.displayName.valueOf() <= 5);
    }
    // If using CC1352P-2 device
    else if(rfDesign == "LAUNCHXL-CC1352P-2")
    {
        currentOptions = tableOptions.filter(config => config.displayName.valueOf() <= 5 ||
                                                    config.displayName.valueOf() >= 14);
    }
    // If using CC1352P-4 device
    else if(rfDesign == "LAUNCHXL-CC1352P-4" || rfDesign == "LP_CC2652PSIP" || rfDesign == "LP_CC1352P7-4" || rfDesign == "LP_CC2651P3")
    {
        currentOptions = tableOptions.filter(config => config.displayName.valueOf() <= 10);
    }

    return currentOptions;
}
/*
 * ======== getRfDesignOptions ========
 * Generates an array of SRFStudio compatible rfDesign options based on device
 *
 * @param deviceId - device being used
 *
 * @returns Array - Array of rfDesign options, if the device isn't supported,
 *                  returns null
 */
function getRfDesignOptions(deviceId)
{
    let newRfDesignOptions = null;
    if(deviceId === "CC1352P1F3RGZ")
    {
        newRfDesignOptions = [
            {name: "LAUNCHXL-CC1352P1"},
            {name: "LAUNCHXL-CC1352P-2"},
            {name: "LAUNCHXL-CC1352P-4"}
        ];
    }
    else if(deviceId === "CC1352P7RGZ")
    {
        newRfDesignOptions = [
            {name: "LP_CC1352P7-1"},
            {name: "LP_CC1352P7-4"}
        ];
    }
    else if(deviceId === "CC1352R1F3RGZ")
    {
        newRfDesignOptions = [{name: "LAUNCHXL-CC1352R1"}];
    }
    else if(deviceId === "CC2642R1FRGZ")
    {
        newRfDesignOptions = [{name: "LAUNCHXL-CC26X2R1"}];
    }
    else if(deviceId === "CC2652R1FRGZ")
    {
        newRfDesignOptions = [{name: "LAUNCHXL-CC26X2R1"}];
    }
    else if(deviceId === "CC2652RB1FRGZ")
    {
        newRfDesignOptions = [{name: "LP_CC2652RB"}];
    }
    else if(deviceId === "CC2652P1FSIP")
    {
        newRfDesignOptions = [{name: "LP_CC2652PSIP"}];
    }
    else if(deviceId === "CC2652R1FSIP")
    {
        newRfDesignOptions = [{name: "LP_CC2652RSIP"}];
    }
    else if(deviceId === "CC2652R7RGZ")
    {
        newRfDesignOptions = [{name: "LP_CC2652R7"}];
    }
    else if(deviceId === "CC2651P3RGZ")
    {
        newRfDesignOptions = [{name: "LP_CC2651P3"}];
    }
    else if(deviceId === "CC2651R3RGZ")
    {
        newRfDesignOptions = [{name: "LP_CC2651R3"}];
    }

    return(newRfDesignOptions);
}

/*
 * ======== validateDefaultTxPower ========
 * Check if a selected defaultTxPower value is valid
 * This check is added since the user can enter an invalid
 * value in the example .syscfg file.
 *
 * @param inst - BLE instance
 *
 * @returns - true if the selected value is in the pa levels list
 *            false if the selected value is not in the list
 */
function validateDefaultTxPower(inst)
{
    let validOptions = inst.$module.$configByName.defaultTxPower.options(inst);
    const selectedOption = inst.defaultTxPower;
    if(!_.find(validOptions, (option)=> option.name == selectedOption))
    {
        return false
    }
    return true;
}
/*
 * ======== validate ========
 * Validate this inst's configuration
 *
 * @param inst       - BLE instance to be validated
 * @param validation - object to hold detected validation issues
 */
function validate(inst, validation)
{
    if(!validateDefaultTxPower(inst))
    {
        validation.logError("Selected option is invalid, please select a valid option", inst, "defaultTxPower");
    }

    // Get the RF Design module
    const rfDesign = system.modules["/ti/devices/radioconfig/rfdesign"].$static;

    // inst.rfDesign configurable value should always be equal to
    // the value of the rfDesign configurable in rfdesign module
    if(inst.rfDesign !== rfDesign.rfDesign)
    {
        validation.logError(`Must match ${system.getReference(rfDesign,
            "rfDesign")} in the RF Design Module`, inst, "rfDesign");
    }
}

/*
 *  ======== moduleInstances ========
 *  Determines what modules are added as non-static submodules
 *
 *  @param inst  - Module instance containing the config that changed
 *  @returns     - Array containing dependency modules
 */
function moduleInstances(inst)
{
    const dependencyModule = [];

    // Get the board default rf settings
    const radioSettings = Common.getRadioScript(inst.rfDesign,system.deviceData.deviceId).radioConfigParams;

    let args = {
        $name: "RF_BLE_Setting",
        phyType: "bt5le2m",
        codeExportConfig: radioSettings,
        paramVisibility: false,
        permission: "ReadOnly"
    }

    if(inst.rfDesign == "LAUNCHXL-CC1352P-2" || inst.rfDesign == "LAUNCHXL-CC1352P-4" || inst.rfDesign == "LP_CC2652PSIP" || inst.rfDesign == "LP_CC1352P7-4" || inst.rfDesign == "LP_CC2651P3")
    {
        args.highPA = true;
        if(inst.rfDesign == "LAUNCHXL-CC1352P-4" || inst.rfDesign == "LP_CC1352P7-4" || inst.rfDesign == "LP_CC2652PSIP")
        {
            args.phyType = "bt5le2mp10";
            args.txPowerHi = "10";
        }
    }

	dependencyModule.push({
        name: "radioConfig",
        group: "bleRadioConfig",
        displayName: "BLE Radio Configuration",
        moduleName: "/ti/devices/radioconfig/settings/ble",
        collapsed: true,
        args: args
     });

    return(dependencyModule);
}

/*
 *  ======== exports ========
 *  Export the BLE RF Settings Configuration
 */
exports = {
    config: config,
    validate: validate,
    moduleInstances: moduleInstances
};