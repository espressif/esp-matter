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
 *  ======== PINCC26XX.syscfg.js ========
 */

"use strict";

/*
 *  ======== devSpecific ========
 *  Device-specific extensions to be added to base PIN configuration
 */
let devSpecific = {
    config: [
        {
            name: "$hardware",
            filterHardware: () => true,
            getDisabledOptions: getDisabledOptions
        },
        {
            name: "parentMod",
            displayName: "Parent Mod",
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
            name: "parentSignalName",
            displayName: "Parent Signal Name",
            hidden: true,
            onChange: updateConfigs,
            default: ""
        },
        {
            name: "parentSignalDisplayName",
            displayName: "Parent Signal Display Name",
            hidden: true,
            default: ""
        },
        {
            name: "ioid",
            displayName: "IOID",
            hidden: true,
            onChange: updateConfigs,
            default: ""
        }
    ],

    /* override generic requirements with device-specific reqs */
    pinmuxRequirements: pinmuxRequirements,

    /* helper functions */
    getIOID:      getIOID,
    getIOIDValue: getIOIDValue,
    getDIO:       getDIO,

    /* override device-specific templates */
    templates:
    {
        boardc : "/ti/drivers/pin/PINCC26XX.Board.c.xdt",
        board_initc : "/ti/drivers/pin/PINCC26XX.Board_init.c.xdt",
        boardh : "/ti/drivers/pin/PIN.Board.h.xdt"
    },

    /* ensure PIN is initialized before other modules */
    initPriority: 0 /* low numbers are called earlier, undefined last */
};

/*
 *  ======== updateConfigs ========
 *  Adjust UI properties of configs based on current config settings
 */
function updateConfigs(inst, ui)
{
    //ui.$name.hidden = false;
    if (inst.ioid != "") {
        ui.ioid.hidden = false;
    }
}

/*
 *  ======== getDisabledOptions ========
 */
function getDisabledOptions(inst, components)
{
    let invalidComponents = [];

    if (!inst.$ownedBy) {
        return ([]);
    }

    for (let i = 0; i < components.length; i++) {
        invalidComponents.push({
            component: components[i],
            /* This is a GUI only feature. PIN instances cannot be added
             * without a parent in the GUI; therefore, inst.$ownedBy */
            reason: "Hardware is disabled by "
                + system.getReference(inst.$ownedBy)
        });
    }

    return (invalidComponents);
}

/*
 *  ======== getIOID ========
 *  Returns the IOID for this PIN instance
 */
function getIOID(inst)
{
    if (inst.ioid != "") return (inst.ioid);

    if (!inst.$ownedBy) {
        return ("IOID_" + inst.pinPin.$solution.devicePinName.substring(4));
    }
    else {
        let parentInterface;
        let parentInstance = inst.$ownedBy;
        let ioid = "";
        if (inst.parentInterfaceName) {
            parentInterface = parentInstance[inst.parentInterfaceName];
        }
        else {
            parentInterface = parentInstance;  /* GPIO has no interface */
        }
        if (parentInterface[inst.parentSignalName].$solution) {
            ioid = "IOID_" + parentInterface[inst.parentSignalName].$solution.devicePinName.substring(4);
        }

        return (ioid);
    }
}

/*
 *  ======== getIOIDValue ========
 */
function getIOIDValue(inst)
{
    /* map an IOID_* name to its value (as defined in driverlib/ioc.h) */
    let num = Number(getIOID(inst).replace("IOID_", ""));

    /* output hex 32-bit representation */
    return ("0x" + num.toString(16).padStart(8, '0'));
}

/*
 *  ======== getDIO ========
 *  Returns the DIO for this PIN instance
 */
function getDIO(inst)
{
     return (getIOID(inst).replace("IOID_","DIO"));
}

/*
 *  ======== pinmuxRequirements ========
 *  Return peripheral pin requirements as a function of config
 */
function pinmuxRequirements(inst)
{
    if ((inst.parentMod != "") || (inst.ioid != "")) {
        return ([]);
    }

    /*
     * name is the name of a property that will be added to inst;
     * this property's value is an object returned by the
     * pinmux solver
     */
    let pin = {
        name: "pinPin",
        displayName: "Pin",
        interfaceName: "GPIO",    /* pmux interface name (not module name!!) */
        signalTypes: ["DIN", "DOUT"]
    };

    return ([pin]);
}

/*
 *  ======== extend ========
 *  Extends a base exports object to include any device specifics
 *
 *  This function is invoked by the generic PIN module to
 *  allow us to augment/override as needed for the CC26XX
 */
function extend(base)
{
    /* merge and overwrite base module attributes */
    let result = Object.assign({}, base, devSpecific);

    /* concatenate device-specific configs */
    result.config = base.config.concat(base.pinConfig).concat(devSpecific.config);

    return (result);
}

/*
 *  ======== exports ========
 *  Export device-specific extensions to base exports
 */
exports = {
    /* required function, called by generic PIN module */
    extend: extend
};
