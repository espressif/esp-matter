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
 *  ======== ble_general.syscfg.js ========
 */

"use strict";

// Get general long descriptions
const Docs = system.getScript("/ti/ble5stack/general/ble_general_docs.js");
// Get common Script
const Common = system.getScript("/ti/ble5stack/ble_common.js");

const config = {
    displayName: "General Configuration",
    description: "Configure General BLE Settings",
    config: [
        {
            name: "deviceName",
            displayName: "Device Name",
            default: "Simple Peripheral",
            longDescription: Docs.deviceNameLongDescription
        },
        {
            name: "hideAddressModeRPA",
            displayName: "Hide Address Mode RPA",
            default: false,
            hidden: true
        },
        {
            name: "addressMode",
            displayName: "Address Mode",
            default: "ADDRMODE_RP_WITH_PUBLIC_ID",
            longDescription: Docs.addressModeLongDescription,
            getDisabledOptions: generateDisabledOptions("addressMode"),
            onChange: onAddressModeChange,
            options: [
                {
                    displayName: "Public Address",
                    name: "ADDRMODE_PUBLIC",
                    description: "Always Use Public Address"
                },
                {
                    displayName: "Random Static Address",
                    name: "ADDRMODE_RANDOM",
                    description: "Always Use Random Static Address"
                },
                {
                    displayName: "RPA with Public ID",
                    name: "ADDRMODE_RP_WITH_PUBLIC_ID",
                    description: "Always Use Resolvable Private Address with Public Identity Address"
                },
                {
                    displayName: "RPA with Random ID",
                    name: "ADDRMODE_RP_WITH_RANDOM_ID",
                    description: "Always Use Resolvable Private Address with Random Identity Address"
                }
            ]
        },
        {
            name: "randomAddress",
            displayName: "Random Address",
            default: "00:00:00:00:00:00",
            placeholder: "00:00:00:00:00:00",
            textType: "mac_address_48",
            longDescription: Docs.randomAddressLongDescription,
            hidden: true
        },
        {
            name: "rpaRead",
            displayName: "RPA Read Period (ms)",
            default: 3000,
            longDescription: Docs.rpaReadLongDescription
        },
        {
            name: "maxConnNum",
            displayName: "Max Number of Connections",
            default: maxConnNumDefaultValue(),
            longDescription: Docs.maxConnNumLongDescription
        },
        {
            name: "maxPDUNum",
            displayName: "Max Number of PDUs",
            default: 5,
            longDescription: Docs.maxPDUNumLongDescription
        },
        {
            name: "maxPDUSize",
            displayName: "Max Size of PDU (bytes)",
            default: 69,
            longDescription: Docs.maxPDUSizeLongDescription
        }
    ]
};

/*
 *  ======== generateDisabledOptions ========
 *  Generates a list of options that should be disabled in a
 *  drop-down
 *
 * @returns Array - array of strings that should be disabled
 */
function generateDisabledOptions(name)
{
    return (inst) => {
        if(inst.hideAddressModeRPA)
        {
            // Find the configurable we're going to generate a disabled list from
            const configurable = inst.$module.$configByName[name].options;
            // List of invalid options
            const disabledOptions = _.filter(configurable,(conf) => conf.name.includes("RP") == true);
            // Add the "reason" why it's disabled, and return that information
            return disabledOptions.map((option) => ({ name: option.name, reason: "This is not a valid option" }));
        }

        return [];
    }

}

/*
 *  ======== onAddressModeChange ========
 *  Add the randomAddress configurable when changing the addressMode
 *  to ADDRMODE_RANDOM or ADDRMODE_RP_WITH_RANDOM_ID.
 *  @param inst  - Module instance containing the config that changed
 *  @param ui    - The User Interface object
 */
function onAddressModeChange(inst,ui)
{
    inst.addressMode == "ADDRMODE_RANDOM" ||
    inst.addressMode == "ADDRMODE_RP_WITH_RANDOM_ID" ?
    ui.randomAddress.hidden = false :
    ui.randomAddress.hidden = true;

}

/*
 *  ======== maxConnNumDefaultValue ========
 *  Return the required default value of the maxConnNum according
 *  to the current device.
 *  @returns - the default max num of conns value
 */
function maxConnNumDefaultValue()
{
    let maxConnNum;
    (Common.device2DeviceFamily(system.deviceData.deviceId)
     == "DeviceFamily_CC26X1") ? maxConnNum = 4 : maxConnNum = 8;
    return maxConnNum;
}

/*
 * ======== validate ========
 * Validate this inst's configuration
 *
 * @param inst       - General instance to be validated
 * @param validation - object to hold detected validation issues
 */
function validate(inst, validation)
{
    // Get the device family
    const devFamily = Common.device2DeviceFamily(system.deviceData.deviceId);

    if(inst.deviceName.length > 21)
    {
        validation.logError("Max Length of Device Name Attribute is 21", inst, "deviceName");
    }

    // Limit Maximum Number of Connections to be 8 for Agama Lite devices
    if(devFamily == "DeviceFamily_CC26X1")
    {
        if(inst.maxConnNum < 0 || inst.maxConnNum > 8)
        {
            validation.logError("Maximum Number of Connections range is 0 to 8", inst, "maxConnNum");
        }
    }
    else // For all other device families
    {
        // Limit Maximum Number of Connections to be 16 when device role != Central
        if(inst.deviceRole != "CENTRAL_CFG" && (inst.maxConnNum < 0 || inst.maxConnNum > 16))
        {
            validation.logError("Maximum Number of Connections range is 0 to 16", inst, "maxConnNum");
        }
        // Limit Maximum Number of Connections to be 31 when device role == Central
        if(inst.deviceRole == "CENTRAL_CFG" && (inst.maxConnNum < 0 || inst.maxConnNum > 32))
        {
            validation.logError("Maximum Number of Connections range is 0 to 32", inst, "maxConnNum");
        }
    }

    if(inst.maxPDUSize < 27 || inst.maxPDUSize > 255)
    {
        validation.logError("Max PDU size Valid range is 27 to 255", inst, "maxPDUSize");
    }
    if(inst.rpaRead < 0 || inst.rpaRead > (Math.pow(2,32) - 1))
    {
        validation.logError("RPA read period range is 0 to sizeof(uint32)", inst, "rpaRead");
    }
    if(inst.maxPDUNum < 0 || inst.maxPDUNum > (Math.pow(2,8) - 1))
    {
        validation.logError("Maximum Number of PDUs range is 0 to sizeof(uint8)", inst, "maxPDUNum");
    }
}

/*
 *  ======== exports ========
 *  Export the BLE General Configuration module
 */
exports = {
    config: config,
    validate: validate,
    maxConnNumDefaultValue: maxConnNumDefaultValue
};
