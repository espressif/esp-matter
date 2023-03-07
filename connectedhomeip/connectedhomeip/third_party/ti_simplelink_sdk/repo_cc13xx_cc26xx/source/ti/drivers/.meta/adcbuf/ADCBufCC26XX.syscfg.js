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
 *  ======== ADCBufCC26XX.syscfg.js ========
 */

"use strict";

const MAXCHANNELS = 8; /* max number of channels per ADCBuf */

/* get Common /ti/drivers utility functions */
let Common = system.getScript("/ti/drivers/Common.js");
let logError = Common.logError;

let intPriority = Common.newIntPri()[0];
intPriority.name = "interruptPriority";
intPriority.displayName = "Hardware Interrupt Priority";

let swiPriority = Common.newSwiPri();
swiPriority.name = "softwareInterruptPriority";
swiPriority.displayName = "Software Interrupt Priority";

/*
 *  ======== devSpecific ========
 *  Device-specific extensions to be added to base ADCBuf configuration
 */
let devSpecific = {
    config: [
        intPriority,
        swiPriority,
        {
            name: "channels",
            displayName: "Channels",
            description: "Number of external channels to configure. Must be"
                + " between 0 and " + MAXCHANNELS + ".",
            longDescription:
`Select __0__ if you only wish to use one of the __Battery__,
__Decoupling__, or __Ground__ channel(s).
`,
            default: 1
        },
        {
            name: "batteryChannel",
            displayName: "Battery Channel",
            description: "Adds a channel to sample the internal"
                + " battery voltage, VDDS.",
            default: false
        },
        {
            name: "decouplingChannel",
            displayName: "Decoupling Channel",
            description: "Adds a channel to sample the decoupling capacitor"
                + " voltage, DCOUPL.",
            longDescription:"The digital core is supplied by a 1.28-V"
                + " regulator connected to VDDR. The output of this"
                + " regulator requires an external decoupling capacitor"
                + " for proper operation; this capcitor is connected to"
                + " DCOUPL.",
            default: false
        },
        {
            name: "groundChannel",
            displayName: "Ground Channel",
            description: "Adds a channel to sample the ground voltage, VSS.",
            default: false
        },
        {
            name: "acquireADCSem",
            displayName: "Acquire ADC Semaphore",
            description: "Specifies if the ADC semaphore is acquired in ADCBuf_open()",
            longDescription: "The ADC semaphore is used to prevent the sensor"
                + " controller and the application processor from simultaneously"
                + " accessing the ADC peripheral. If set to 'true', the semaphore"
                + " will be acquired when the device is opened and"
                + " released when it's closed.",
            default: false
        }
    ],

    moduleInstances: moduleInstances,

    validate: validate,

    /* override generic requirements with  device-specific reqs (if any) */
    pinmuxRequirements: pinmuxRequirements,

    templates: {
        boardc: "/ti/drivers/adcbuf/ADCBufCC26XX.Board.c.xdt",
        boardh: "/ti/drivers/adcbuf/ADCBufCC26XX.Board.h.xdt"
    },

    _getPinResources: _getPinResources,
    getChannels: getChannels
};

/*
 *  ======== _getPinResources ========
 *  Assume 'inst' is a channel
 */
function _getPinResources(inst)
{
    let pin;

    if (inst.adc && inst.adc.adcPin) {
        let adcPin = inst.adc.adcPin.$solution.devicePinName;
        pin = "DIO" + adcPin.substring(4);

        if (inst.$hardware && inst.$hardware.displayName) {
            pin += ", " + inst.$hardware.displayName;
        }
    }
    else if (inst.channelString) {

        switch (inst.channelString) {
            case "Battery":
                pin = "Battery Channel: ADC_COMPB_IN_VDDS";
                break;
            case "Ground":
                pin = "Ground Channel: ADC_COMPB_IN_VSS";
                break;
            case "Decoupling":
                pin = "Decoupling Channel: ADC_COMPB_IN_DCOUPL";
                break;
            default:
                break;
        }
    }

    return (pin);
}

/*
 *  ======== validate ========
 */
function validate(inst, vo)
{
    if (inst.channels == 0) {
        if (inst.batteryChannel == false &&
            inst.decouplingChannel == false &&
            inst.groundChannel == false) {
            logError(vo, inst, "channels", "Must select at least one"
                + " channel to configure.");
        }
    }

    if (inst.channels < 0 || inst.channels > MAXCHANNELS ||
        !Number.isInteger(inst.channels)) {
        logError(vo, inst, "channels", "Channels must be an integer between"
            + " 1 and " + MAXCHANNELS);
    }
}

/*
 *  ======== pinmuxRequirements ========
 *  Return peripheral pin requirements as a function of config
 */
function pinmuxRequirements(inst)
{
    let reqs = [];

    let adc = {
        name: "adc",
        displayName: "ADC Peripheral",
        interfaceName: "ADC",
        canShareWith: "ADCBuf",
        resources: []
    };

    let dma = [
        {
            name: "dmaADCChannel",
            displayName: "DMA ADC Channel",
            description: "DMA channel used for ADC",
            interfaceNames: ["DMA_ADC"]
        }
    ];

    adc.resources = dma;

    reqs.push(adc);

    return (reqs);
}

/*
 *  ======== moduleInstances ========
 */
function moduleInstances(inst)
{
    let modInstances = new Array();

    modInstances.push(
        {
            name: "timerInstance",
            displayName: "GPTimer Instance",
            moduleName: "/ti/drivers/timer/GPTimerCC26XX",
            hidden: false,
            collapsed: true,
            args: {
                timerType: "16 Bits",
                useCase: "ADCBuf"
            }
        }
    );

    if (inst.batteryChannel == true) {
        modInstances.push({
            name: "adcBufChannelBattery",
            displayName: "ADCBuf Channel Battery",
            moduleName: "/ti/drivers/adcbuf/ADCBufChanCC26XX",
            args: {
                channelString: "Battery"
            }
        });
    }
    if (inst.decouplingChannel == true) {
        modInstances.push({
            name: "adcBufChannelDecoupling",
            displayName: "ADCBuf Channel Decoupling",
            moduleName: "/ti/drivers/adcbuf/ADCBufChanCC26XX",
            args: {
                channelString: "Decoupling"
            }
        });
    }
    if (inst.groundChannel == true) {
        modInstances.push({
            name: "adcBufChannelGround",
            displayName: "ADCBuf Channel Ground",
            moduleName: "/ti/drivers/adcbuf/ADCBufChanCC26XX",
            args: {
                channelString: "Ground"
            }
        });
    }

    /* limit the loop because validate is not called before this method */
    let max = Math.min(inst.channels, MAXCHANNELS);
    for (let i = 0; i < max; i++) {
        modInstances.push({
            name: "adcBufChannel" + i,
            displayName: "ADCBuf Channel " + i,
            moduleName: "/ti/drivers/adcbuf/ADCBufChanCC26XX",
            hardware: inst.$hardware,
            args: {
                channelString: i.toString()
            }
        });
    }

    return (modInstances);
}

function getChannels(inst)
{
    let finalChannels = [];

    for (let i = 0; i < inst.channels; i++) {
        let channel = inst["adcBufChannel" + i];
        /* Generate COMPB index using device metadata */
        let packagePin = parseInt(channel.adc.adcPin.$solution.packagePinName);
        let auxInput = system.deviceData.devicePins[packagePin].attributes.alias_name;
        auxInput = "ADC_COMPB_IN_" + auxInput.replace("_", "IO");

        let comment = "";
        if (inst.$hardware && inst.$hardware.displayName) {
            comment = inst.$hardware.displayName;
        }

        finalChannels.push({
            name: channel.$name,
            comment: comment,
            dio: channel["adcPinInstance" + i].$name,
            comp: auxInput
        });
    }

    if (inst.batteryChannel) {
        finalChannels.push({
            "name": inst.adcBufChannelBattery.$name,
            "comment": "",
            "dio": "GPIO_INVALID_INDEX",
            "comp": "ADC_COMPB_IN_VDDS"
        });
    }
    if (inst.decouplingChannel) {
        finalChannels.push({
            name: inst.adcBufChannelDecoupling.$name,
            comment: "",
            dio: "GPIO_INVALID_INDEX",
            comp: "ADC_COMPB_IN_DCOUPL"
        });
    }
    if (inst.groundChannel) {
        finalChannels.push({
            name: inst.adcBufChannelGround.$name,
            comment: "",
            dio: "GPIO_INVALID_INDEX",
            comp: "ADC_COMPB_IN_VSS"
        });
    }

    return finalChannels;
}

/*
 *  ======== extend ========
 *  Extends a base exports object to include any device specifics
 *
 *  This function is invoked by the generic ADCBuf module to
 *  allow us to augment/override as needed for the CC26XX
 */
function extend(base)
{
    /* display which driver implementation can be used */
    base = Common.addImplementationConfig(base, "ADCBuf", null,
        [{name: "ADCBufCC26X2"}], null);

    /* merge and overwrite base module attributes */
    let result = Object.assign({}, base, devSpecific);

    /* concatenate device-specific configs */
    result.config = base.config.concat(devSpecific.config);

    return (result);
}

/*
 *  ======== exports ========
 *  Export device-specific extensions to base exports
 */
exports = {
    /* required function, called by base ADCBuf module */
    extend: extend
};
