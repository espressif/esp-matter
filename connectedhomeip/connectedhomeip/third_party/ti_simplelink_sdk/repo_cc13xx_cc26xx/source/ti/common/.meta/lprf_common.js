/*
 * Copyright (c) 2019 Texas Instruments Incorporated - http://www.ti.com
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
 *  ======== lprf_common.js ========
 */

"use strict";

const stacks = [
    {name: "DMM", path: "/ti/dmm/dmm"},
    {name: "BLE", path: "/ti/ble5stack/ble"},
    {name: "Custom", path: "/ti/devices/radioconfig/custom"},
    {name: "EasyLink", path: "/ti/easylink/easylink"},
    {name: "TI 15.4 Stack", path: "/ti/ti154stack/ti154stack"},
    {name: "TI-OpenThread", path: "/ti/thread/thread"},
    {name: "Z-Stack", path: "/ti/zstack/zstack"}
];

/* Dictionary mapping a device name to default LaunchPad; used to discover the
 * appropriate RF settings when a device is being used without a LaunchPad
 */
const deviceToBoard = {
    CC1352P7: "LP_CC1352P7_1",
    CC1312R7: "LP_CC1312R7",
    CC1352R: "CC1352R1_LAUNCHXL",
    CC1352P: "CC1352P1_LAUNCHXL",
    CC1312: "CC1312R1_LAUNCHXL",
    CC2642: "CC26X2R1_LAUNCHXL",
    CC2652R7: "LP_CC2652R7",
    CC2652P1FSIP: "LP_CC2652PSIP",
    CC2652R1FSIP: "LP_CC2652RSIP",
    CC2652R1: "CC26X2R1_LAUNCHXL",
    CC2652RB: "LP_CC2652RB",
    CC2652PR: "CC1352P2_LAUNCHXL",
    CC1354R10: "LP_CC1354R10_RGZ",
    CC2674R10: "LP_CC2674R10_FPGA",
};

/*!
 *  ======== getDeviceOrLaunchPadName ========
 *  Get the name of the board (or device)
 *
 *  @param convertToBoard - Boolean. When true, return the associated LaunchPad
 *                          name if a device is being used without a LaunchPad
 *
 *  @returns String - Name of the board with prefix /ti/boards and
 *                    suffix .syscfg.json stripped off.  If no board
 *                    was specified, the device name is returned.
 */
function getDeviceOrLaunchPadName(convertToBoard)
{
    let name = system.deviceData.deviceId;

    if(!_.isNil(system.deviceData.board))
    {
        name = system.deviceData.board.source;

        // Strip off everything up to and including the last '/'
        name = name.replace(/.*\//, "");

        // Strip off everything after and including the first '.'
        name = name.replace(/\..*/, "");
    }

    // Check if this is a standalone device without a LaunchPad
    if(convertToBoard && !name.includes("LAUNCHXL") && !name.includes("LP_"))
    {
        // Find the LaunchPad name in deviceToBoard dictionary
        let key = null;
        for(key in deviceToBoard)
        {
            if(name.includes(key))
            {
                name = deviceToBoard[key];
                break;
            }
        }
    }

    return(name);
}

/*!
 *  ======== device2DeviceFamily ========
 *  Map a pimux deviceID to a TI-driver device family string
 *
 *  @param deviceId  - a pinmux deviceId (system.deviceData)
 *
 *  @returns String - the corresponding "DeviceFamily_xxxx" string
 *                    used by driverlib header files.
 */
function device2DeviceFamily(deviceId)
{
    let driverString = null;

    // Determine libraries required by device name
    if(deviceId.match(/CC13.2/))
    {
        driverString = "DeviceFamily_CC13X2";
    }
    else if(deviceId.match(/CC13.0/))
    {
        driverString = "DeviceFamily_CC13X0";
    }
    else if(deviceId.match(/CC26.0R2/))
    {
        driverString = "DeviceFamily_CC26X0R2";
    }
    else if(deviceId.match(/CC26.2/))
    {
        driverString = "DeviceFamily_CC26X2";
    }
    else if(deviceId.match(/CC26.0/))
    {
        driverString = "DeviceFamily_CC26X0";
    }
    else if(deviceId.match(/CC3220/))
    {
        driverString = "DeviceFamily_CC3220";
    }
    else if(deviceId.match(/MSP432E.*/))
    {
        driverString = "DeviceFamily_MSP432E401Y";
    }
    else if(deviceId.match(/MSP432P4.1.I/)
            || deviceId.match(/MSP432P4111/))
    {
        driverString = "DeviceFamily_MSP432P4x1xI";
    }
    else if(deviceId.match(/MSP432P4.1.T/))
    {
        driverString = "DeviceFamily_MSP432P4x1xT";
    }
    else if(deviceId.match(/MSP432P401/))
    {
        driverString = "DeviceFamily_MSP432P401x";
    }
    else
    {
        driverString = "";
    }

    return(driverString);
}

/*
 * ======== findConfig ========
 * Finds and returns the configurable with the matching provided name
 *
 * @param config     - A module's configurable array
 * @param configName - The name of the configurable to search for
 *
 * @returns - undefined if the configurable is not found, otherwise the entire
 *            configurable object
 */
function findConfig(config, configName)
{
    let enumDef;

    let element = null;
    for(element of config)
    {
        // If the element contains a group, need to search its configurables
        if("config" in element)
        {
            // Recursively search the sub-groups config array
            enumDef = findConfig(element.config, configName);

            // Stop searching if the configurable was found in the sub-group
            if(enumDef !== undefined)
            {
                break;
            }
        }
        else if(element.name === configName)
        {
            // Stop searching if the current element is the correct configurable
            enumDef = element;
            break;
        }
    }

    return(enumDef);
}

exports = {
    stacks: stacks,
    device2DeviceFamily: device2DeviceFamily,
    getDeviceOrLaunchPadName: getDeviceOrLaunchPadName,
    findConfig: findConfig
};
