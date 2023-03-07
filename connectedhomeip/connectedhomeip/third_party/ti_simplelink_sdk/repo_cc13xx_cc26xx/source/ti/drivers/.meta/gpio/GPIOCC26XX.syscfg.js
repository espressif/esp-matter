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
 *  ======== GPIOCC26XX.syscfg.js ========
 */

"use strict";

/* get Common /ti/drivers utility functions */
let Common = system.getScript("/ti/drivers/Common.js");

/*
 *  ======== devSpecific ========
 *  Device-specific extensions to be added to base GPIO configuration
 */
let devSpecific = {
    templates:
    {
        boardc : "/ti/drivers/gpio/GPIOCC26XX.Board.c.xdt",
        board_initc : "/ti/drivers/gpio/GPIO.Board_init.c.xdt",
        boardh : "/ti/drivers/gpio/GPIO.Board.h.xdt"
    },

    _getPinResources: _getPinResources,
    _getHwSpecificAttrs: _getHwSpecificAttrs,
    _pinToDio: _pinToDio
};

/*
 *  ======== _getPinResources ========
 */
function _getPinResources(inst)
{
    let pin;

    if (inst.gpioPin) {
        pin = inst.gpioPin.$solution.devicePinName;
        pin = pin.replace("_", "");

        if (inst.$hardware && inst.$hardware.displayName) {
            pin += ", " + inst.$hardware.displayName;
        }
    }

    return (pin);
}

/*
 *  ======== _getHwSpecificAttrs ========
 */
function _getHwSpecificAttrs(inst)
{
    return [];
}

/*
 *  ======== _pinToDio ========
 */
function _pinToDio(pinSolution, devicePin)
{
    /* The description passed here comes from the device metadata and is
     * of the format DIO_n - substring gets rid of DIO_ and we parse the result
     */
    return parseInt(devicePin.description.split("_")[1], 10);
}

/*
 *  ======== pinmuxRequirements ========
 *  Return peripheral pin requirements as a function of config
 *  Called on instantiation and every config change.
 *
 *  $super is needed to call the generic module's functions
 */
function pinmuxRequirements(inst, $super)
{
    let result = $super.pinmuxRequirements ? $super.pinmuxRequirements(inst) : [];
    return (result);
}

/*
 *  ======== extend ========
 *  Extends a base exports object to include any device specifics
 *
 *  This function is invoked by the generic module to
 *  allow us to augment and override as needed.
 */
function extend(base)
{
    /* override base pinmuxRequirements */
    devSpecific.pinmuxRequirements = function (inst) {
        return pinmuxRequirements(inst, base);
    };

    /* display which driver implementation can be used */
    base = Common.addImplementationConfig(
        base,
        "GPIO",
        null,
        [{name: "GPIOCC26XX"}],
        null
    );

    /* merge and overwrite base module attributes */
    return (Object.assign({}, base, devSpecific));
}

/*
 *  ======== exports ========
 *  Export device-specific extensions to base exports
 */
exports = {
    extend: extend
};
