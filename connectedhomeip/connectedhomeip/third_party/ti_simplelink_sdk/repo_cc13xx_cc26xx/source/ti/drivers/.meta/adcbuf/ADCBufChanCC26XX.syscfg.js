/*
 * Copyright (c) 2018-2020 Texas Instruments Incorporated - http://www.ti.com
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
 *  ======== ADCBufChanCC26XX.syscfg.js ========
 */

"use strict";

let config = [
    {
        name: "channelString",
        default: "null",
        hidden: true,
        onChange: onChannelStringChange
    },
    {
        name: "$name",
        defaultPrefix: "ADCBUF_CHANNEL_",
        description: "C identifier used to identify this ADCBuf channel.",
        hidden: false
    }
];

function onChannelStringChange(inst, ui)
{
    let name = inst.channelString;

    if (name === "null") {
        return;
    }

    /* Assign a default channel name */
    if (name === "Battery" || name === "Decoupling" || name === "Ground") {
        inst.$name = "ADCBUF_CHANNEL_" + name.toUpperCase();
    }
}

/*
 *  ======== pinmuxRequirements ========
 *  Returns peripheral pin requirements of the specified instance
 *
 *  @param inst    - a fully configured ADC instance
 *
 *  @returns req[] - an array of pin requirements needed by inst
 */
function pinmuxRequirements(inst)
{
    let name = inst.channelString;
    let adcChanNames = [
        "PIN0", "PIN1", "PIN2", "PIN3",
        "PIN4", "PIN5", "PIN6", "PIN7"
    ];

    if (name === "null" || name === "Battery" ||
        name === "Decoupling" || name === "Ground") {
        return ([]);
    }

    let adcbuf = {
        name: "adc",
        hidden: true,
        displayName: "ADC Peripheral",
        interfaceName: "ADC",
        canShareWith: "ADCBuf",
        resources: [
            {
                name: "adcPin",
                displayName: "ADC Pin",
                interfaceNames: adcChanNames
            }
        ],
        signalTypes: { adcPin: ["AIN"] }
    };

    return ([adcbuf]);
}

/*
 *  ======== moduleInstances ========
 */
function moduleInstances(inst)
{
    let modInstances = new Array();
    let name = inst.channelString;
    let shortName = inst.$name.replace("CONFIG_", "");

    if (name === "null" || name === "Battery" ||
        name === "Decoupling" || name === "Ground") {
        return (modInstances);
    }

    modInstances.push(
        {
            name: "adcPinInstance" + inst.channelString,
            displayName: "ADCBuf Input configuration when not in use",
            moduleName: "/ti/drivers/GPIO",
            collapsed: true,
            requiredArgs: {
                parentInterfaceName: "adc",
                parentSignalName: "adcPin",
                parentSignalDisplayName: "ADCBuf Channel " + inst.channelString
            },
            args: {
                $name: "CONFIG_GPIO_" + shortName + "_CHANNEL_" + inst.channelString,
                mode: "Input"
            }
        }
    );

    return (modInstances);
}

/*
 *  ========= filterHardware ========
 *  Check 'component' signals for compatibility with ADC
 *
 *  @param component - hardware object describing signals and
 *                     resources they're attached to
 *  @returns matching pinRequirement object if ADC is supported.
 */
function filterHardware(component)
{
    for (let sig in component.signals) {
        if (component.signals[sig].type == "AIN") {
            return (true);
        }
    }

    return (false);
}

/*
 *  ======== exports ========
 */
exports = {
    config: config,

    displayName: "ADCBuf_Channel",

    /* override generic requirements with  device-specific reqs (if any) */
    pinmuxRequirements: pinmuxRequirements,

    moduleInstances: moduleInstances,

    filterHardware: filterHardware
};
