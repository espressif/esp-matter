/*
 * Copyright (c) 2018-2021 Texas Instruments Incorporated - http://www.ti.com
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
 *  ======== UARTCC26XX.syscfg.js ========
 */

"use strict";

/* get Common /ti/drivers utility functions */
let Common = system.getScript("/ti/drivers/Common.js");

let intPriority = Common.newIntPri()[0];
intPriority.name = "interruptPriority";
intPriority.displayName = "Interrupt Priority";
intPriority.description = "UART peripheral interrupt priority";

let swiPriority = Common.newSwiPri();
swiPriority.name = "softwareInterruptPriority";
swiPriority.displayName = "Software Interrupt Priority";
swiPriority.description = "UART module software interrupt priority";

let logError = Common.logError;
let logInfo  = Common.logInfo;

/*
 *  ======== devSpecific ========
 *  Device-specific extensions to be added to base UART configuration
 */
let devSpecific = {
    config: [
        {
            name        : "flowControl",
            displayName : "Flow Control",
            default     : false,
            description : "Enable hardware flow control",
            longDescription : `Hardware flow control between two devices is
accomplished by connecting the UART Request-To-Send (RTS) pin to the
Clear-To-Send (CTS) input on the receiving device, and connecting the
RTS output of the receiving device to the UART CTS pin`
        },
        {
            name        : "ringBufferSize",
            displayName : "Ring Buffer Size",
            description : "Number of bytes in the ring buffer",
            longDescription : `The ring buffer serves as an extension of the
FIFO. If data is received when UART_read() is not called, data will be stored
in the ring buffer. The size can be changed to suit the application.`,
            default     : 32
        },
        intPriority,
        swiPriority,

        /* RX and TX Fifo level thresholds */
        {
            name        : "txInterruptFifoThreshold",
            displayName : "TX Interrupt FIFO Threshold",
            description : "Select the FIFO level for the TX interrupt",
            default     : '1/8',
            options     : [
                { name : "1/8" },
                { name : "2/8" },
                { name : "4/8" },
                { name : "6/8" },
                { name : "7/8" }
            ]
        },
        {
            name        : "rxInterruptFifoThreshold",
            displayName : "RX Interrupt FIFO Threshold",
            description : "Select the FIFO level for the RX interrupt",
            default     : '4/8',
            options     : [
                { name:   "1/8" },
                { name:   "2/8" },
                { name:   "4/8" },
                { name:   "6/8" },
                { name:   "7/8" }
            ]
        }
    ],

    moduleStatic : {
        name : "uartGlobal",
        config : [
            {
                name        : "useUARTCC26X2",
                displayName : "Use UARTCC26X2",
                default     : false,
                deprecated  : true,
                onChange    : onChangeUpdateImplementation
            }
        ]
    },

    /* override generic pin requirements */
    pinmuxRequirements    : pinmuxRequirements,

    /* PIN instances */
    moduleInstances: moduleInstances,

    onHardwareChanged: onHardwareChanged,

    /* override device-specific templates */
    templates: {
        boardc : "/ti/drivers/uart/UARTCC26XX.Board.c.xdt",
        boardh : "/ti/drivers/uart/UART.Board.h.xdt"
    },

    /* override generic filterHardware with ours */
    filterHardware        : filterHardware,

    _getPinResources: _getPinResources
};

/*
 *  ======== _getPinResources ========
 */
function _getPinResources(inst)
{
    let pin;
    let rxPin = "Unassigned";
    let txPin = "Unassigned";
    let ctsPin;
    let rtsPin;

    if (inst.uart) {
        if (inst.uart.rxPin) {
            rxPin = inst.uart.rxPin.$solution.devicePinName.replace("_", "");
        }
        if (inst.uart.txPin) {
            txPin = inst.uart.txPin.$solution.devicePinName.replace("_", "");
        }

        pin = "\nTX: " + txPin + "\nRX: " + rxPin;

        if (inst.uart.ctsPin) {
            ctsPin = inst.uart.ctsPin.$solution.devicePinName.replace("_", "");
            pin += "\nCTS: " + ctsPin;
        }
        if (inst.uart.rtsPin) {
            rtsPin = inst.uart.rtsPin.$solution.devicePinName.replace("_", "");
            pin += "\nRTS: " + rtsPin;
        }

        if (inst.$hardware && inst.$hardware.displayName) {
            pin += "\n" + inst.$hardware.displayName;
        }
    }

    return (pin);
}

/*
 *  ======== onChangeUpdateImplementation ========
 *  If users utilized the deprecated "useUARTCC26X2" configuration to select a
 *  UART implementation, use the new "uartImplementation" config to do so.
 */
function onChangeUpdateImplementation(inst, ui)
{
    if (inst.useUARTCC26X2 !== inst.$module.$configByName.useUARTCC26X2.default) {
        inst.uartImplementation = "UARTCC26X2";
    }
    else if (inst.useUARTCC26X2 ===
             inst.$module.$configByName.useUARTCC26X2.default) {
        inst.uartImplementation = "UARTCC26XX";
    }
}

/*
 *  ======== onHardwareChanged ========
 */
function onHardwareChanged(inst, ui)
{
    if (inst.$hardware) {
        let component = inst.$hardware;

        /* Determine if hardware supports flow control */
        if (Common.findSignalTypes(component, ["UART_CTS", "UART_RTS"])) {
            inst.flowControl = true;
        }
        else {
            inst.flowControl = false;
        }
        ui.flowControl.readOnly = true;
    }
    else {
        inst.flowControl = false;
        ui.flowControl.readOnly = false;
    }
}

/*
 *  ======== pinmuxRequirements ========
 *  Control RX, TX pin usage by the user specified dataDirection.
 *  Add CTS, RTS Pins if use specified flowControl is selected.
 *
 *  param inst      - UART instance
 *
 *  returns req[] - array of requirements needed by inst
 */
function pinmuxRequirements(inst)
{
    let cts = {
        name           : "ctsPin",    /* config script name */
        displayName    : "CTS Pin",   /* GUI name */
        interfaceNames : ["CTS"]      /* pinmux tool name */
    };

    let rts = {
        name           : "rtsPin",
        displayName    : "RTS Pin",
        interfaceNames : ["RTS"]
    };

    let tx = {
        name              : "txPin",  /* config script name */
        displayName       : "TX Pin", /* GUI name */
        interfaceNames    : ["TXD"]   /* pinmux tool name */
    };

    let rx = {
        name              : "rxPin",
        displayName       : "RX Pin",
        interfaceNames    : ["RXD"]
    };

    let resources = [];

    if (inst.dataDirection != 'Receive Only') {
        resources.push(tx);
    }

    if (inst.dataDirection != 'Send Only') {
        resources.push(rx);
    }

    if (inst.flowControl === true) {
        resources.push(cts);
        resources.push(rts);
    }

    let uart = {
        name          : "uart",
        displayName   : "UART Peripheral",
        interfaceName : "UART",
        resources     : resources,
        signalTypes   : {
            txPin     : ['UART_TXD'],
            rxPin     : ['UART_RXD'],
            ctsPin    : ['UART_CTS'],
            rtsPin    : ['UART_RTS']
        }
    };

    return [uart];
}

/*
 *  ======== filterHardware ========
 *  Check 'component' signals for compatibility with UART
 *
 *  param component - hardware object describing signals and
 *                     resources they're attached to
 *
 *  returns Boolean indicating whether or not to allow the component to
 *           be assigned to an instance's $hardware config
 */
function filterHardware(component)
{
    return (Common.typeMatches(component.type, ["UART"]));
}

/*
 *  ======== moduleInstances ========
 *  returns PIN instances
 */
function moduleInstances(inst)
{
    let pinInstances = new Array();

    if (inst.dataDirection != "Receive Only") {
        pinInstances.push(
            {
                name: "txPinInstance",
                displayName: "TX Configuration While Not In Use",
                moduleName: "/ti/drivers/GPIO",
                collapsed: true,
                args: {
                    parentInterfaceName: "uart",
                    parentSignalName: "txPin",
                    parentSignalDisplayName: "TX",
                    mode: "Output",
                    initialOutputState: "Low",
                    pull: "None"
                }
            }
        );
    }

    if (inst.dataDirection != "Send Only") {
        pinInstances.push({
                name: "rxPinInstance",
                displayName: "RX Configuration While Not In Use",
                moduleName: "/ti/drivers/GPIO",
                collapsed: true,
                args: {
                    parentInterfaceName: "uart",
                    parentSignalName: "rxPin",
                    parentSignalDisplayName: "RX",
                    mode: "Input",
                    pull: "Pull Down"
                }
            }
        );
    }

    if (inst.flowControl) {
        pinInstances.push({
                name: "ctsPinInstance",
                displayName: "CTS Configuration While Not In Use",
                moduleName: "/ti/drivers/GPIO",
                collapsed: true,
                args: {
                    parentInterfaceName: "uart",
                    parentSignalName: "ctsPin",
                    parentSignalDisplayName: "CTS",
                    mode: "Input",
                    pull: "Pull Down"
                }
            }
        );

        pinInstances.push({
                name: "rtsPinInstance",
                displayName: "RTS Configuration While Not In Use",
                moduleName: "/ti/drivers/GPIO",
                collapsed: true,
                args: {
                    parentInterfaceName: "uart",
                    parentSignalName: "rtsPin",
                    parentSignalDisplayName: "RTS",
                    mode: "Output",
                    initialOutputState: "Low",
                    pull: "None"
                }
            }
        );
    }

    return (pinInstances);
}

/*
 *  ======== validate ========
 *  Validate this instance's configuration
 *
 *  param inst       - UART instance to be validated
 *  param validation - object to hold detected validation issues
 *
 *  @param $super    - needed to call the generic module's functions
 */
function validate(inst, validation, $super)
{
    if (inst.ringBufferSize < 0) {
        logError(validation, inst, "ringBufferSize", "value must be positive");
    }

    /* don't allow an unreasonably large ring buffer size */
    if (inst.ringBufferSize > 1024) {
        logInfo(validation, inst, "ringBufferSize",
            "consider reducing size for space optimization");
    }

    if ($super.validate) {
        $super.validate(inst, validation);
    }
}

/*
 *  ======== extend ========
 */
function extend(base)
{
    let driverDescription = `
The [__UARTCC26XX__][1] driver does not support polling mode or text mode
processing for data.  Text mode processing allows UART_read() to return on
receiving a newline character.  Support for polling mode and text mode has
been added to the [__UARTCC26X2__][2] driver.

[1]: /drivers/doxygen/html/_u_a_r_t_c_c26_x_x_8h.html#details "C API reference"
[2]: /drivers/doxygen/html/_u_a_r_t_c_c26_x2_8h.html#details "C API reference"
    `;

    /* display which driver implementation can be used */
    devSpecific = Common.addImplementationConfig(devSpecific, "UART", "UARTCC26XX",
        [{name: "UARTCC26XX"}, {name: "UARTCC26X2"}], driverDescription);

    /* override base validate */
    devSpecific.validate = function (inst, validation) {
        return validate(inst, validation, base);
    };

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
    /* required function, called by base UART module */
    extend: extend
};
