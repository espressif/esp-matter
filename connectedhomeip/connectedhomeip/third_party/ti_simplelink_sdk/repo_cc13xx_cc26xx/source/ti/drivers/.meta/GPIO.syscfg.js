/*
 * Copyright (c) 2018-2021, Texas Instruments Incorporated - http://www.ti.com
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
 *  ======== GPIO.syscfg.js ========
 */

"use strict";

/* get Common /ti/drivers utility functions */
let Common     = system.getScript("/ti/drivers/Common.js");
let logError   = Common.logError;
let logWarning = Common.logWarning;
let logInfo = Common.logInfo;

/* compute /ti/drivers family name from device object */
let family = Common.device2Family(system.deviceData, "GPIO");

const inversionAvailable = (family !== "CC32XX");

let intPriority = Common.newIntPri()[0];
intPriority.name = "interruptPriority";
intPriority.displayName = "Interrupt Priority";

/* generic configuration parameters for GPIO instances */
let config = [
    {
        name: "$hardware",
        getDisabledOptions: getDisabledHWOptions,
        filterHardware: filterHardware,
        onChange: onHardwareChanged
    },
    {
        name: "mode",
        displayName: "Mode",
        description: "Select the GPIO mode",
        longDescription: "The mode configuration parameter is used to"
            + " determine the initial state of GPIO, eliminating the need to"
            + " configure the GPIO pin at runtime prior to using it."
            + "\n[More ...](/drivers/syscfg/html/ConfigDoc.html"
            + "#ti_drivers_GPIO_mode \"Full descriptions of all GPIO modes\""
            + ")",

        default: "Input",
        options: [
            {
                name: "Input",
                description: "This GPIO is initially configured for input"
            },
            {
                name: "Output" ,
                description: "This GPIO is initially configured for output"
            }
        ],
        onChange: updateConfigs
    },
    {
        name: "outputType",
        displayName: "Output Type",
        description: "Specifies the output type",
        hidden: true,
        default: "Standard",
        options: [
            { name: "Standard" },
            { name: "Open Drain" }
        ],
        onChange: updateConfigs
    },
    {
        name: "outputStrength",
        displayName: "Output Strength",
        description: "Specifies the output strength",
        hidden: true,
        default: "Medium",
        options: [
            { name: "High" },
            { name: "Medium" },
            { name: "Low" }
        ]
    },
    {
        name: "initialOutputState",
        displayName: "Initial Output State",
        description: "Specifies the initial output state",
        longDescription: "This specifies if an output will be driven high or"
            + " low after __GPIO_init()__.",
        hidden: true,
        default: "Low",
        options: [
            { name: "High" },
            { name: "Low" }
        ]
    },
    {
        name: "pull",
        displayName: "Pull",
        description: "Specifies the internal pull-up or pull-down resistor"
            + " configuration of this GPIO pin.",
        hidden: false,
        default: "None",
        options: [
            { name: "None" },
            { name: "Pull Up" },
            { name: "Pull Down" }
        ]
    },
    {
        name: "invert",
        displayName: "Value Inversion",
        description: "Invert input/output value in hardware",
        hidden: !inversionAvailable,
        default: false
    },
    {
        name: "outputSlew",
        displayName: "Reduce Slew Rate",
        description: "Reduce pin slew rate",
        hidden: true,
        default: false
    },
    {
        name: "hysteresis",
        displayName: "Enable Hysteresis",
        description: "Enable input hysteresis",
        hidden: !inversionAvailable,
        default: false
    },
    {
        name: "interruptTrigger",
        displayName: "Interrupt Trigger",
        description: "Specifies when or if interrupts are triggered",
        longDescription: `
This parameter configures when the GPIO pin interrupt will trigger. Even when this config is set, interrupts are not enabled until
[\`GPIO_enableInt()\`](/drivers/doxygen/html/_g_p_i_o_8h.html#a31c4e65b3855424418262e35521c7051) is called at runtime.`,
        hidden: false,
        default: "None",
        options: [
            { name: "None" },
            { name: "High" },
            { name: "Low" },
            { name: "Falling Edge" },
            { name: "Rising Edge" },
            { name: "Both Edges" }
        ]
    },
    {
        name: "callbackFunction",
        hidden: false,
        displayName: "Callback Function",
        description: "The name of the callback function called when this GPIO pin triggers an interrupt, or 'NULL' if it's specified at runtime",
        longDescription: `
If you need to set the callback at runtime, set this configuration parameter
to 'NULL' and call [\`GPIO_setCallback()\`](/drivers/doxygen/html/_g_p_i_o_8h.html#a24c401f32e65f60f11a1594fdafb9d2a) with the name of the function you
want to be triggered.

[More ...](/drivers/syscfg/html/ConfigDoc.html#ti_drivers_GPIO_callbackFunction "Function's type signature and an example")
`,
        documentation: `
This function is of type [\`GPIO_CallbackFxn\`](/drivers/doxygen/html/_g_p_i_o_8h.html#a46b0c9afbe998c88539abc92082a1173),
it's called in the context of a hardware ISR, and it's passed
a single parameter: the index of the GPIO that triggered the interrupt.

Example: [Creating an input callback](/drivers/doxygen/html/_g_p_i_o_8h.html#ti_drivers_GPIO_Example_callback "C/C++ source").
`,

        placeholder: "<a callback is never needed>",
        default: "NULL" /* a callback may be set at runtime */
    },
    /* Compatibility - these deprecated options mirror PIN configurables */
    {
        name: "outputState",
        description: "GPIO uses initialOutputState instead of outputState.",
        deprecated: true,
        default: "",
        onChange: updateDeprecatedConfigs
    },
    {
        name: "irq",
        displayName: "GPIO uses interruptTrigger instead of irq",
        deprecated: true,
        default: "",
        onChange: updateDeprecatedConfigs
    },
    /* Internal only - used by modules that instantiate GPIO objects to convey ownership information */
    {
        name: "parentSignalName",
        displayName: "Parent Signal Name",
        hidden: true,
        default: ""
    },
    {
        name: "parentInterfaceName",
        displayName: "Parent Interface Name",
        hidden: true,
        default: ""
    },
    {
        name: "parentSignalDisplayName",
        displayName: "Parent Signal Display Name",
        description: "Used in comments to indicate what this pin is for. Examples would be SCLK, TX, AOUT.",
        hidden: true,
        default: ""
    }
];

/*
 *  ======== getDisabledHWOptions ========
 *  Disable $hardware options that are incompatible with the readOnly mode
 *  setting to prevent exceptions from being thrown. When changing from one
 *  $hardware to another $hardware with incompatible mode settings, exceptions
 *  would be thrown from trying to change the readOnly mode that was locked by
 *  a parent module.
 */
function getDisabledHWOptions(inst, comps)
{
    let disabled = [];
    let modeCfg = system.getReference(inst, "mode");
    for (let i = 0; i < comps.length; i++) {
        let comp = comps[i];

        /* Can't configure hardware for 'owned' modules */
        if (inst.$ownedBy && comp !== inst.$hardware) {
            disabled.push({
                component: comp,
                reason: "Can't configure a GPIO owned by " + system.getReference(inst.$ownedBy)
            });
            continue;
        }

        if (inst.$uiState.mode.readOnly) {
            /* Some DIOs only support Input or Output modes */
            for (let sig in comp.signals) {
                let type = comp.signals[sig].type;

                if (inst.mode == "Input" && !Common.typeMatches(type, ["DIN"])) {
                    disabled.push({
                        component: comp,
                        reason: "This hardware component does not support 'Input' " + modeCfg
                    });
                }
                else if (inst.mode == "Output" && !Common.typeMatches(type, ["DOUT"])) {
                    disabled.push({
                        component: comp,
                        reason: "This hardware component does not support 'Output' " + modeCfg
                    });
                }
            }
        }
    }

    return (disabled);
}

/*
 *  ======== pinmuxRequirements ========
 *  Return peripheral pin requirements as a function of config
 */
function pinmuxRequirements(inst)
{
    if (inst.$ownedBy) {
        return ([]);
    }

    let gpio = {
        name: "gpioPin",
        displayName: "GPIO Pin",
        interfaceName: "GPIO",
        signalTypes: ["DIN", "DOUT"]
    };

    return ([gpio]);
}

/*
 *  ========= filterHardware ========
 */
function filterHardware(component)
{
    let signals = component.signals;
    if (!signals) {
        return false;
    }

    if (!Array.isArray(signals)) {
        /* This means the component has its pins defined as a dictionary.
         * Extract an array of pin objects to pass to Common.typeMatches.
         */
        signals = Object.values(component.signals);
    }

    for (let i = 0; i < signals.length; i++) {
        if (Common.typeMatches(signals[i].type, ["DIN", "DOUT"])) {
            return (true);
        }
    }

    return (false);
}

/*
 *  ======== updateConfigs ========
 *  Adjust UI properties of configs based on current config settings
 */
function updateConfigs(inst, ui)
{
    if (inst.mode == "Output") {
        /* Show output-specific options */
        ui.outputType.hidden = false;
        ui.outputSlew.hidden = !inversionAvailable;
        ui.outputStrength.hidden = false;
        ui.initialOutputState.hidden = false;

        /* Hide input-specific options */
        ui.pull.hidden = true;
        ui.hysteresis.hidden = true;
        ui.interruptTrigger.hidden = true;
        ui.callbackFunction.hidden = true;

        /* Clear input-specific text configurables */
        inst.callbackFunction = "";

        /* Special case: Open Drain can have a pull but not a drive strength */
        if (inst.outputType == "Open Drain") {
            ui.pull.hidden = false;
            ui.outputStrength.hidden = true;
        }
    }
    else if (inst.mode == "Input") {
        /* Show input-specific options */
        ui.pull.hidden = false;
        ui.hysteresis.hidden = !inversionAvailable;
        ui.interruptTrigger.hidden = false;
        ui.callbackFunction.hidden = false;

        /* Reset input-specific text configurables */
        inst.interruptTrigger = "None";
        inst.callbackFunction = "NULL";

        /* Hide output-specific options */
        ui.outputType.hidden = true;
        ui.outputSlew.hidden = true;
        ui.outputStrength.hidden = true;
        ui.initialOutputState.hidden = true;
    }
}

/*
 *  ======== updateDeprecatedConfigs ========
 *  Port deprecated configuration values to updated ones
 */
function updateDeprecatedConfigs(inst, ui)
{
    if (inst.outputState !== "") {
        inst.initialOutputState = inst.outputState;
    }
    if (inst.irq !== "") {
        inst.interruptTrigger = inst.irq;
    }
}

/*
 *  ======== validate ========
 *  Validate this instance's configuration
 *
 *  @param inst       - GPIO instance to be validated
 *  @param validation - object to hold detected validation issues
 */
function validate(inst, validation)
{
    if (inst.mode === "Output") {
        /* output GPIOs don't trigger interrupts */
        if (inst.interruptTrigger !== "None") {
            logError(validation, inst, "interruptTrigger",
                "Output mode GPIO resources can't trigger interrupts");
        }

        /* specified callbackFunction is never called for output only GPIOs */
        if (inst.callbackFunction.length > 0) {
            let message = "callback '" + inst.callbackFunction
                + "' set, but won't be triggered by this output-only GPIO";

            logError(validation, inst, "callbackFunction", message);
        }
    }

    /* callbackFunction must be valid C identifier */
    if (!Common.isCName(inst.callbackFunction)) {
        let message = "'" + inst.callbackFunction
            + "' is not a valid a C identifier";
        logError(validation, inst, "callbackFunction", message);
    }

    if (inst.callbackFunction !== "NULL"
        && (inst.callbackFunction.toLowerCase() === "null")) {
        logWarning(validation, inst, "callbackFunction",
            "Did you mean \"NULL\"?");
    }

    if (inst.$hardware) {
        /*
         * This hardware only supports outputs at runtime.
         */
        if (Common.findSignalTypes(inst.$hardware, ["DOUT"]) &&
            !Common.findSignalTypes(inst.$hardware, ["DIN"])) {

            if (inst.mode === "Input") {
                logInfo(validation, inst, "mode", inst.$hardware.displayName +
                    " only supports digital outputs.");
            }
        }

        /*
         * This hardware only supports inputs at runtime.
         */
        if (!Common.findSignalTypes(inst.$hardware, ["DOUT"]) &&
            Common.findSignalTypes(inst.$hardware, ["DIN"])) {

            if (inst.mode === "Output") {
                logInfo(validation, inst, "mode", inst.$hardware.displayName +
                    " only supports digital inputs.");
            }
        }
    }
}

/*
 *  ======== onHardwareChanged ========
 */
function onHardwareChanged(inst, ui)
{
    if (inst.$hardware) {
        let key = Object.keys(inst.$hardware.signals)[0];
        let signal = inst.$hardware.signals[key];
        let type = signal.type;

        let output = Common.typeMatches(type, ["DOUT"]);
        let input = Common.typeMatches(type, ["DIN"]);

        if (output && !input)
        {
            inst.mode = "Output";
            updateConfigs(inst, ui);
        }
        if (input && !output)
        {
            inst.mode = "Input";
            updateConfigs(inst, ui);
        }
        /* Intentionally do nothing for mixed input/output */
    }
    else {
        /* Return to default settings */
        if (!inst.$uiState.mode.readOnly) {
            inst.mode = "Input";
        }

        inst.pull = "None";

        if (!inst.$uiState.interruptTrigger.readOnly) {
            inst.interruptTrigger = "None";
        }

        inst.callbackFunction = "NULL";
        updateConfigs(inst, ui);
    }
}

/*
 *  ======== getPinBounds ========
 * This function assumes all inaccessible pins are either at the start or the
 * end; if this is ever not true we will need to revisit it to add better
 * verification
 */
function getPinBounds(module)
{
    /* This function does a min-max search for DIO numbers. These initial
     * values will be replaced with the lowest and highest found numbers,
     * but 'lowest' needs to have a large initial value because it is only
     * changed if we find a value that is smaller than it
     */
    let pins = {
        "lowest": 999,
        "highest": 0
    };

    for (let x in system.deviceData.devicePins)
    {
        let pin = system.deviceData.devicePins[x];

        /* CC26XX devices have DIO_0, DIO_16 while CC32XX devices have GP03, GP30 */
        if ((pin.description.startsWith("DIO")) ||
            (pin.description.startsWith("RGCDIO")) ||
            (pin.description.startsWith("GP") && pin.devicePinType === "Default"))
        {
            let dioNum = module._pinToDio(null, pin);
            if (dioNum < pins.lowest)
            {
                pins.lowest = dioNum;
            }
            if (dioNum > pins.highest)
            {
                pins.highest = dioNum;
            }
        }
    }
    return pins;
}

/*
 *  ======== getPinData ========
 * GPIO is slightly different from other modules - we always want to emit values
 * for all pins on the device, then modify that data for configured instances.
 *
 * Here we collect a list of all device pins (with some invalid pins at low
 * indexes where applicable) and then modify that list with instance data.
 */
function getPinData(module)
{
    let localPinData = [];
    let pinBounds = getPinBounds(module);

    /* Start with any dummy pins, if we have a lower bound */
    for (let i = 0; i < pinBounds.lowest; i++)
    {
        localPinData.push({
            "name": "Pin is not available on this device",
            "config": "0",
            "callback": "NULL"
        });
    }

    /* Then all the valid pins */
    for (let i = pinBounds.lowest; i <= pinBounds.highest; i++)
    {
        localPinData.push({
            "name": "DIO_" + i,
            "config": module._getDefaultAttrs(),
            "callback": "NULL"
        });
    }

    /* Go through all the configured pins and overwrite as appropriate */
    for (let inst of module.$instances) {
        let dio = module.getDioForInst(inst);

        localPinData[dio].inst = inst;
        localPinData[dio].name = inst.$name;
        localPinData[dio].config = getAttrs(inst);
    }

    return localPinData;
}

/*
 * ======== getConfiguredCallbacks ========
 * Generates two pieces of data: a list of unique names and an indented string
 * containing calls to GPIO_setCallback. This is needed because if a callback
 * is reused, we don't want to generate two extern definitions for it because
 * that would look ugly.
 *
 * Note that this callback configuration is slightly heavier on runtime cost
 * than having the array contain initialised values, but it saves flash since
 * the table is likely to be quite sparse.
 */
function getConfiguredCallbacks(instances)
{
    let callbackInfo = {
        /* A set of unique callback function names */
        names: [],
        /* String containing calls to GPIO_setCallback */
        calls: ""
    };

    for (let inst of instances)
    {
        if (inst.callbackFunction !== "" && inst.callbackFunction !== "NULL")
        {
            if (!callbackInfo.names.includes(inst.callbackFunction))
            {
                callbackInfo.names.push(inst.callbackFunction);
            }
            callbackInfo.calls += "    GPIO_setCallback(" + inst.$name + ", " + inst.callbackFunction + ");\n";
        }
    }

    /* Snip the trailing linebreak */
    callbackInfo.calls = callbackInfo.calls.trim();

    return callbackInfo;
}

/*
 *  ======== getAttrs ========
 *  Return a symbolic GPIO bit mask corresponding to inst's configs
 */
function getAttrs(inst)
{
    let strengthMapping = {
        "High": "GPIO_CFG_OUT_STR_HIGH",
        "Medium": "GPIO_CFG_OUT_STR_MED",
        "Low": "GPIO_CFG_OUT_STR_LOW"
    };
    let pullMapping = {
        "Pull Up": "GPIO_CFG_PULL_UP_INTERNAL",
        "Pull Down": "GPIO_CFG_PULL_DOWN_INTERNAL",
        "None": "GPIO_CFG_PULL_NONE_INTERNAL"
    };
    let intMapping = {
        "Falling Edge": "GPIO_CFG_IN_INT_FALLING",
        "Rising Edge": "GPIO_CFG_IN_INT_RISING",
        "Both Edges": "GPIO_CFG_IN_INT_BOTH_EDGES",
        "High": "GPIO_CFG_IN_INT_HIGH",
        "Low": "GPIO_CFG_IN_INT_LOW",
        "None": "GPIO_CFG_IN_INT_NONE"
    };
    let outputMapping = {
        "Open Drain": "GPIO_CFG_OUTPUT_OPEN_DRAIN_INTERNAL"
    };

    let listOfDefines = [];

    if (inst.mode == "Output") {
        listOfDefines.push("GPIO_CFG_OUTPUT_INTERNAL");
        listOfDefines.push(strengthMapping[inst.outputStrength]);

        if (inst.outputType != "Standard") {
            listOfDefines.push(outputMapping[inst.outputType]);
        }

        if (inst.initialOutputState == "High") {
            listOfDefines.push("GPIO_CFG_OUT_HIGH");
        }
        else {
            listOfDefines.push("GPIO_CFG_OUT_LOW");
        }
    }
    else {
        listOfDefines.push("GPIO_CFG_INPUT_INTERNAL");
        listOfDefines.push(intMapping[inst.interruptTrigger]);
        listOfDefines.push(pullMapping[inst.pull]);
    }

    if (inst.invert) {
        listOfDefines.push("GPIO_CFG_INVERT_ON");
    }
    if (inst.hysteresis) {
        listOfDefines.push("GPIO_CFG_HYSTERESIS_ON");
    }
    if (inst.outputSlew) {
        listOfDefines.push("GPIO_CFG_SLEW_REDUCED");
    }

    let devSpecificDefines = inst.$module._getHwSpecificAttrs(inst);

    /* ... is the spread operator and splits the list into elements
     * Without this we would have a list embedded in the list
     */
    listOfDefines.push(...devSpecificDefines);
    return listOfDefines.join(" | ");
}

/*
 *  ======== _getPinResources ========
 */
/* istanbul ignore next */
function _getPinResources(inst)
{
    return;
}

/*
 *  ======== getDioForInst ========
 */
function getDioForInst(inst)
{
    let pinSolution;

    if (inst.$ownedBy) {
        if (inst.parentInterfaceName == "GPIO") {
            /* GPIO interfaces only have one layer */
            pinSolution = inst.$ownedBy[inst.parentSignalName].$solution;
        }
        else {
            /* Other interfaces require us to index again into SignalName */
            pinSolution = inst.$ownedBy[inst.parentInterfaceName][inst.parentSignalName].$solution;
        }
    }
    else {
        pinSolution = inst.gpioPin.$solution;
    }

    let devicePin = system.deviceData.devicePins[pinSolution.packagePinName];

    /* Conflicting pins have no valid mapping until ignored */
    if (devicePin) {
        return inst.$module._pinToDio(pinSolution, devicePin);
    }

    return null;
}

/*
 *  ======== _pinToDio ========
 */
/* istanbul ignore next */
function _pinToDio(pinSolution, devicePin)
{
    throw 'This function must be defined for each device!';
}

/*
 *  ======== _getDefaultAttrs ========
 */
/* istanbul ignore next */
function _getDefaultAttrs()
{
    return "GPIO_CFG_NO_DIR";
}

/*
 *  ======== _getHwSpecificAttrs ========
 *  Override in each implementation to control the mapping of hardware specific
 *  options to hardware specific defines
 */
/* istanbul ignore next */
function _getHwSpecificAttrs(inst)
{
    return [];
}

/*
 *  ======== base ========
 *  Define the base/common GPIO property and method exports
 */
let base = {
    displayName: "GPIO",
    description: "General Purpose I/O Driver",

    longDescription: `
The [__GPIO driver__][1] allows you to manage General Purpose I/O
resources via simple and portable APIs. GPIO pin behavior is
configured statically, but can also be [reconfigured at runtime][2].

Note: If you add only a GPIO to a configuration, you will need to manually
add either Board or Power to generate the correct templates due to a circular
dependency issue.

* [Usage Synopsis][3]
* [Examples][4]
* [Configuration Options][5]

[1]: /drivers/doxygen/html/_g_p_i_o_8h.html#details "C API reference"
[2]: /drivers/doxygen/html/_g_p_i_o_8h.html#ti_drivers_GPIO_Example_reconfigure "Example: Reconfiguring a GPIO pin"
[3]: /drivers/doxygen/html/_g_p_i_o_8h.html#ti_drivers_GPIO_Synopsis "Basic C usage summary"
[4]: /drivers/doxygen/html/_g_p_i_o_8h.html#ti_drivers_GPIO_Examples "C usage examples"
[5]: /drivers/syscfg/html/ConfigDoc.html#GPIO_Configuration_Options "Configuration options reference"
`,

    pinmuxRequirements: pinmuxRequirements,
    updateDeprecatedConfigs: updateDeprecatedConfigs,
    validate: validate,

    defaultInstanceName: "CONFIG_GPIO_",
    config: Common.addNameConfig(config, "/ti/drivers/GPIO", "CONFIG_GPIO_"),

    moduleStatic: {
        name: "gpioGlobal",
        displayName: "GPIO Global",
        config: [
            intPriority
        ],
        getAttrs: getAttrs
    },

    _getPinResources: _getPinResources,
    _getDefaultAttrs: _getDefaultAttrs,
    _getHwSpecificAttrs: _getHwSpecificAttrs,
    _pinToDio: _pinToDio,

    getPinData: getPinData,
    getPinBounds: getPinBounds,
    getDioForInst: getDioForInst,
    getConfiguredCallbacks: getConfiguredCallbacks
};

/* extend our common exports to include the family-specific content */
let deviceGPIO = system.getScript("/ti/drivers/gpio/GPIO" + family);
exports = deviceGPIO.extend(base);
