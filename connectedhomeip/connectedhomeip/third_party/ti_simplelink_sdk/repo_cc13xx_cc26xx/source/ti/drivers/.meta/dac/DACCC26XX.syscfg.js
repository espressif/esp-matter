/*
 * Copyright (c) 2021 Texas Instruments Incorporated - http://www.ti.com
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
 *  ======== DACCC26XX.syscfg.js ========
 */

"use strict";

/* get Common /ti/drivers utility functions */
let Common = system.getScript("/ti/drivers/Common.js");

/*
 *  ======== devSpecific ========
 *  Device-specific extensions to be added to base DAC configuration
 */
let devSpecific = {
    config: [
        {
            name: "voltageRefSource",
            displayName: "Voltage Reference Source",
            default: "VDDS",
            description: "Specifies the DAC's voltage reference source.",
            longDescription: `
The selected voltage reference determines the output voltage range.

* VDDS: Use the main supply voltage.
* DCOUPL: Use the core voltage decoupling pin.
* ADCREF: Use the ADC reference voltage.
            `,
            options: [
                { name: "VDDS" },
                { name: "DCOUPL" },
                { name: "ADCREF" }
            ],
            onChange: (inst, ui) => {
                if (inst.voltageRefSource === "DCOUPL") {
                    ui.preCharge.hidden = false;
                }
                else {
                    ui.preCharge.hidden = true;
                    inst.preCharge = false;
                }
            }
        },
        {
            name: "preCharge",
            displayName: "Enable DAC Precharge",
            default: false,
            description: "Controls the DAC's output range.",
            longDescription: `
DAC output voltage range when using the Core Voltage Decoupling Pin (DCOUPL) as voltage reference source.

* Precharge Disabled: 0 V to 1.28 V.
* Precharge Enabled: 1.28 V to 2.56 V.
            `,
            hidden: true
        }
    ],

    /* override generic requirements with device-specific reqs (if any) */
    pinmuxRequirements: pinmuxRequirements,
    modules: Common.autoForceModules(["Board", "Power"]),

    /* PIN instances */
    moduleInstances: moduleInstances,

    /* Auxiliary ADC instance */
    sharedModuleInstances: sharedModuleInstances,

    templates: {
        boardc: "/ti/drivers/dac/DACCC26XX.Board.c.xdt",
        boardh: "/ti/drivers/dac/DAC.Board.h.xdt"
    },

    _getPinResources : _getPinResources
};


/*
 *  ======== _getPinResources ========
 */
function _getPinResources(inst)
{
    let pin;

    let devicePinName = inst.dac.dacPin.$solution.devicePinName;
    pin = "DIO" + devicePinName.substring(4);

    if (inst.$hardware && inst.$hardware.displayName) {
        pin += ", " + inst.$hardware.displayName;
    }

    return (pin);
}


/*
 *  ======== pinmuxRequirements ========
 *  Returns peripheral pin requirements of the specified instance
 */
function pinmuxRequirements(inst)
{
    let dac = {
        name: "dac",
        hidden: true,
        displayName: "DAC Peripheral",
        /* We can safely use "ADC" for the "interfaceName" and "canShareWith" properties
         * given that in sysconfig the ADC interface is used to describe the set of pins that
         * are analog capable (i.e. AUXIO19 yo AUXIO26).
         */
        interfaceName: "ADC",
        canShareWith: "ADC",
        resources: [
            {
                name: "dacPin",
                hidden: false,
                displayName: "DAC Pin",
                interfaceNames: [
                    "PIN0", "PIN1", "PIN2", "PIN3",
                    "PIN4", "PIN5", "PIN6", "PIN7"
                ]
            }
        ],
        signalTypes: { dacPin: ["AIN"] }
    };

    return ([dac]);
}

/*
 *  ======== moduleInstances ========
 *  returns PIN instances
 */
function moduleInstances(inst)
{
    let shortName = inst.$name.replace("CONFIG_", "");
    let pinInstances = new Array();

    pinInstances.push(
        {
            name: "dacPinInstance",
            displayName: "DAC Output Pin Configuration While Pin is Not In Use",
            moduleName: "/ti/drivers/GPIO",
            collapsed: true,
            requiredArgs: {
                parentInterfaceName: "dac",
                parentSignalName: "dacPin",
                parentSignalDisplayName: "DAC Pin"
            },
            args: {
                $name: "CONFIG_GPIO_" + shortName + "_AOUT",
                mode: "Output",
                pull: "None"
            }
        }
    );

    return (pinInstances);
}


/*
 *  ======== sharedModuleInstances ========
 *  The DAC driver uses an ADC auxiliary instance to provide an accurate
 *  measurement of VDDS.
 */
function sharedModuleInstances(inst)
{

    let auxadcInstance = new Array();

    auxadcInstance.push(
        {
            name: "auxADC",
            displayName: "Auxiliary ADC Instance",
            moduleName: "/ti/drivers/ADC",
            collapsed: true,
            readOnly: false,
            requiredArgs: {
                $name: "CONFIG_ADC_AUX",
                referenceSource: "Fixed",
                referenceVoltage: 3300000,
                samplingDuration: "170 us",
                inputScaling: true,
                adjustSampleValue: true,
                internalSignal: "Battery Channel"
            }
        }
    );

    return (auxadcInstance);
}

/*
 *  ======== extend ========
 *  Extends a base exports object to include any device specifics
 *
 *  This function is invoked by the generic DAC module to
 *  allow us to augment and override as needed.
 */
function extend(base)
{
    /* display which driver implementation can be used */
    base = Common.addImplementationConfig(base, "DAC", null,
        [{name: "DACCC26XX"}], null);



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
    /* required function, called by base DAC module */
    extend: extend
};
