/*
 * Copyright (c) 2019-2021, Texas Instruments Incorporated - http://www.ti.com
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
 *  ======== Button.syscfg.js ========
 */

"use strict";

/* get Common /ti/drivers utility functions */
let Common = system.getScript("/ti/drivers/Common.js");
let logWarning = Common.logWarning;

/* generic configuration parameters for Button instances */
let config = [
    {
        name: "$hardware",
        getDisabledOptions: getDisabledOptions
    },
    {
        name: "autoHwConfig",
        displayName: "Use Provided Button Configuration",
        description: "Use the button configurations provided by the board file"
        + " for this button.",
        longDescription:"When this setting is enabled, the board specific"
        + " settings will be used to determine the button's hardware"
        + " configuration. This will provide the correct configuration most of"
        + " the time. If the button hardware has been modified, this setting"
        + " should be disabled and the user should manually set the correct"
        + " hardware configuration.",
        hidden : true,
        default : true,
        onChange : onAutoHwConfigChanged
    },
    {
        name: "polarity",
        displayName: "Button Configuration",
        description: "Specify the hardware configuration of the physical"
        + " button.",
        longDescription:"Specify whether the button is connected to ground or"
        + " Vcc when pushed. 'Active High' should be set for a button that is"
        + " connected to Vcc when pushed. 'Active Low' should be set for a"
        + " button that is connected to ground when pushed.",
        options: [
            {name : "Active Low"},
            {name : "Active High"}
        ],
        default : "Active Low"
    },
    {
        name: "pull",
        displayName: "Pull Resistor",
        description: "Specify which pull configuration is needed.",
        longDescription: "Choose whether the GPIO hardware should pull"
        + " the pin. If you have an external pullup resistor, choose 'External'"
        + " and the GPIO pullup will be disabled.",
        options: [
            {name : "Internal"},
            {name : "External"}
        ],
        default : "Internal"
    }
];

/*
 *  ======== onHardwareChanged ========
 */
function onHardwareChanged(inst, ui) {
    /*
     * Select the correct button hwConfig if autoHwConfig is true and
     * a config is provided in available in components settings.
     */

    if (inst.$hardware) {
        ui.autoHwConfig.hidden = false;
    }
    else {
        ui.autoHwConfig.hidden = true;
        inst.autoHwConfig = false;
    }

    if (inst.autoHwConfig && inst.$hardware) {
        ui.pull.readOnly = true;
        ui.polarity.readOnly = true;

        inst.pull = inst.$hardware.settings.DIN.pull;
        inst.polarity = inst.$hardware.settings.DIN.polarity;
    } else {
        ui.pull.readOnly = false;
        ui.polarity.readOnly = false;
    }
}

/*
 *  ======== onAutoHwConfigChanged ========
 */
function onAutoHwConfigChanged(inst, ui) {
    /* Set hwConfig options as read only if using autoHwConfig */
    onHardwareChanged(inst, ui);
}

/*
 *  ======== validate ========
 *  Validate this inst's configuration
 *
 *  param inst       - Button instance to be validated
 *  param validation - object to hold detected validation issues
 */
function validate(inst, validation)
{
    if (inst.gpioPin.interruptTrigger !== "None") {
        logWarning(validation, inst.gpioPin, "interruptTrigger",
            "The Button driver will configure interrupts automatically " +
            "when you call Button_open(). You do not need to configure this" +
            " manually unless you need interrupts for another reason."
        );
    }
}


/*
 *  ======== getDisabledOptions ========
 * Eliminates all components that do not have a settings.DIN.pull/polarity configurable.
 * This means we have to do less validation in the hardware config and also presents
 * the error to the user much more gracefully.
 */
function getDisabledOptions(inst, components)
{
    let invalidComponents = [];

    for (let i = 0; i < components.length; i++) {
        let settings = components[i].settings;
        if (!settings || !settings.DIN || !settings.DIN.pull || !settings.DIN.polarity) {
            invalidComponents.push({
                component: components[i],
                reason: "Button hardware must have settings.DIN.pull and settings.DIN.polarity"
            });
        }
    }

    return (invalidComponents);
}

/*
 *  ======== pinmuxRequirements ========
 *  Returns peripheral pin requirements of the specified instance
 */
function pinmuxRequirements(inst)
{
    let buttonRequirements = {
        name: "button",
        hidden: true,
        displayName: "Button",
        interfaceName: "GPIO",
        canShareWith: "Button",
        signalTypes: ["DIN"]
    };

    return ([buttonRequirements]);
}

/*
 *  ======== moduleInstances ========
 *  Add a gpio pin instance to this button module
 */
function moduleInstances(inst)
{
    let shortName = inst.$name.replace("CONFIG_", "");

    let gpio = [{
        name: "gpioPin",
        displayName: "GPIO",
        description: "Default configuration",
        moduleName: "/ti/drivers/GPIO",
        requiredArgs: {
            /* Can't be changed by the user */
            parentInterfaceName: "GPIO",
            parentSignalName: "button",
            parentSignalDisplayName: "Button GPIO"
        },
        args: {
            /* Sets default but user can reconfigure */
            $name: "CONFIG_GPIO_" + shortName + "_INPUT"
        }
    }];

    return(gpio);
}

/*
 *  ========= filterHardware ========
 *  param component - hardware object describing signals and
 *                    resources
 *
 *  returns Boolean indicating whether or not to allow the component to
 *           be assigned to an instance's $hardware config
 */
function filterHardware(component) {
    if (component.type) {
        if (Common.typeMatches(component.type, ["BUTTON"])) {
            return (true);
        }
    }
    return (false);
}

/*
 *  ======== _getPinResources ========
 */
function _getPinResources(inst)
{
    let mod = system.getScript("/ti/drivers/GPIO.syscfg.js");
    let pin;

    if (inst.gpioPin) {
        pin = mod._getPinResources(inst.gpioPin);
    }

    return (pin);
}


/*
 *  ======== base ========
 *  Define the base properties and methods
 */
let base = {
    displayName: "Button",
    description: "Button Driver",
    longDescription: `
The [__Button driver__][1] provides a simple interface to control Buttons.
* [Usage Synopsis][2]
* [Examples][3]
* [Configuration][4]
[1]: /drivers/doxygen/html/_button_8h.html#details "C API reference"
[2]:
/drivers/doxygen/html/_button_8h.html#ti_drivers_Button_Synopsis "Synopsis"
[3]: /drivers/doxygen/html/_button_8h.html#ti_drivers_Button_Examples
"C usage examples"
[4]: /drivers/syscfg/html/ConfigDoc.html#Button_Configuration_Options "Configuration options reference"
`,
    defaultInstanceName: "CONFIG_BUTTON_",
    config: Common.addNameConfig(config, "/ti/drivers/apps/Button", "CONFIG_BUTTON_"),
    validate: validate,
    modules: Common.autoForceModules(["Board"]),
    pinmuxRequirements: pinmuxRequirements,
    moduleInstances: moduleInstances,
    filterHardware: filterHardware,
    onHardwareChanged : onHardwareChanged,
    templates: {
        boardc: "/ti/drivers/apps/button/Button.Board.c.xdt",
        boardh: "/ti/drivers/apps/button/Button.Board.h.xdt"
    },

    _getPinResources: _getPinResources
};

exports = base;
