/*
 * Copyright (c) 2020-2021 Texas Instruments Incorporated - http://www.ti.com
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
 *  ======== ITMCC26XX.syscfg.js ========
 */

"use strict";

/* get Common /ti/drivers utility functions */
let Common = system.getScript("/ti/drivers/Common.js");

/*
 *  ======== devSpecific ========
 *  Device-specific extensions to be added to base ITM configuration
 */
let devSpecific = {
    moduleStatic        : {
        config   : [],
        pinmuxRequirements: pinmuxRequirements,
        moduleInstances: moduleInstances,
        modules: Common.autoForceModules(["Board"])
    },
    templates : {
        boardc: "/ti/drivers/itm/ITMCC26XX.Board.c.xdt"
    },
    _getPinResources: _getPinResources
};


/*
 *  ======== _getPinResources ========
 */
function _getPinResources(inst)
{
    let pin;
    let swoPinStr;

    if (inst.swoPin) {
        swoPinStr = inst.itm.SWOPin.$solution.devicePinName.replace("_", "");
        pin = "\nSWO: " + swoPinStr;
    }

    return (pin);
}

/*
 *  ======== moduleInstances ========
 *  returns GPIO instances
 */
function moduleInstances(inst)
{
    let gpioInstances = new Array();

    gpioInstances.push({
        name: "swoPinInstance",
        displayName: "Serial Wire Output (SWO) Instance - Output",
        moduleName: "/ti/drivers/GPIO",
        requiredArgs: {
            parentInterfaceName: "GPIO",
            parentSignalName: "swoPin",
            parentSignalDisplayName: "SWO"
        },
        args: {
            $name: "CONFIG_GPIO_ITM_SWO",
            mode: "Output"
        }
    });

    return (gpioInstances);
}

/*
 *  ======== pinmuxRequirements ========
 */
function pinmuxRequirements(inst)
{

    let swo = {
        name: "swoPin",
        displayName: "SWO Pin",
        interfaceName: "GPIO"
    };
    let resources = [];
    resources.push(swo);

    return ([swo]);
}

/*
 *  ======== validate ========
 *  Validate this instance's configuration
 *
 *  param inst       - ITM instance to be validated
 *  param validation - object to hold detected validation issues
 *
 *  @param $super    - needed to call the generic module's functions
 */
function validate(inst, validation, $super)
{
    if ($super.validate) {
        $super.validate(inst, validation);
    }
}

/*
 *  ======== extend ========
 */
function extend(base)
{
    /* override base validate */
    devSpecific.validate = function (inst, validation) {
        return validate(inst, validation, base);
    };

    /* display which driver implementation can be used */
    base = Common.addImplementationConfig(base, "ITM", null,
        [{name: "ITMCC26XX"}], null);

    /* merge and overwrite base module attributes */
    let result = Object.assign({}, base, devSpecific);

    /* concatenate device-specific configs */
    result.moduleStatic.config = base.moduleStatic.config.concat(devSpecific.moduleStatic.config);

    return (result);
}

/*
 *  ======== exports ========
 *  Export device-specific extensions to base exports
 */
exports = {
    /* required function, called by base ITM module */
    extend: extend
};
