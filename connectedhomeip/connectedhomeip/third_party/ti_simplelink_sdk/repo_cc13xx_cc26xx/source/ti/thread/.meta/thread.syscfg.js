/*
 * Copyright (c) 2018, Texas Instruments Incorporated
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
 */

/*
 *  =========================== thread.syscfg.js ===========================
 *  Top level module definition for the SysConfig Thread module
 */

"use strict";

const genLibs = system.getScript("/ti/utils/build/GenLibs.syscfg.js");

/* Description text for modules and configurables */
const deviceTypeLongDescription = `The device type for the Thread node. \
This can either be Full Thread Device (FTD), capable of full Thread \
functionality, or Minimal Thread Device (MTD), a limited functionality \
device capable of sleeping.

The device type is read only as SysConfig does not have the capability to \
import new libraries and change the dependencies of a project. To change the \
device type, you would have to do both of these things.

For more information, refer to [Device Types](/thread/html/thread/\
thread-protocol.html#device-types) of the Thread User's Guide.`;

const moduleLongDescription = `[__Thread__][1] is an IPv6 based low power \
wireless networking stack designed for IoT. TI-OpenThread is the TI \
implementation of Thread based on the OpenThread, the open-source Thread \
implementation developed by Nest.

* [Usage Synopsis][2]
* [Examples][3]

*Important Note*

If a valid Thread dataset exists in non-volatile storage, that dataset will \
be used over SysConfig defaults. To guarantee SysConfig settings are applied, \
perform a [factory reset][4] of the device to clear non-volatile storage \
settings.

[1]: /thread/openthread-stack-apis.html
[2]: /thread/thread-users-guide.html
[3]: /thread/html/thread/example-apps-cc13x2_26x2.html#end-product-examples
[4]: /thread/html/thread/example-apps-cc13x2_26x2.html#factory-reset
`;

const genLibDescription = "Configures genLibs usage for local libraries. Always hidden";
const genLibLongDescription = `This is a hidden parameter that controls whether the \
stack module contributes libraries to the generated linker command file.

__Default__: True (checked)`;

/* Static module definition for Thread module */
const moduleStatic = {
    config: [
        /* Device Type Configurable */
        {
            name: "deviceType",
            displayName: "Device Type",
            description: "The device type for the Thread node",
            longDescription: deviceTypeLongDescription,
            default: "ftd",
            options: [
                {name: "ftd", displayName: "Full Thread Device"},
                {name: "mtd", displayName: "Minimal Thread Device"}
            ]
        },
        {
            name: "genLibs",
            displayName: "Generate Thread Libraries",
            default: true,
            hidden: true,
            description: genLibDescription,
            longDescription: genLibLongDescription
        },
        {
            name: "deviceTypeReadOnly",
            default: false,
            hidden: true,
            onChange: onDeviceTypeReadOnlyChange
        }
    ],
    moduleInstances: moduleInstances,
    modules: modules
};

/*
 * ======== getLibs ========
 * Contribute libraries to linker command file
 *
 * @param inst  - thread module instance
 * @returns     - Object containing the name of component, array of dependent
 *                components, and array of library names
 */
function getLibs(inst)
{
    // Create a GenLibs input argument
    let results = {
        name: "/ti/thread",
        deps: [],
        libs: []
    };

    if(inst.$static.genLibs)
    {
        // Add prebuilt libs to results.libs
        // Not currently in use
    }

    return(results);
}

/* Submodule instance definitions */
function moduleInstances(inst)
{
    const submodules = [];

    submodules.push({
        name: "pm",
        displayName: "Power Management",
        description: "Configure power settings for radio operations",
        moduleName: "/ti/thread/pm/thread_pm",
        collapsed: false,
        args: {deviceType: inst.deviceType}
    });
    submodules.push({
        name: "rf",
        displayName: "Radio",
        description: "Configure radio settings for client operations",
        moduleName: "/ti/thread/rf/thread_rf",
        collapsed: true
    });
    submodules.push({
        name: "network",
        displayName: "Network",
        description: "Setup Thread network information",
        moduleName: "/ti/thread/network/thread_network",
        collapsed: true
    });
    submodules.push({
        name: "security",
        displayName: "Security",
        description: "Configure security information for Thread network setup",
        moduleName: "/ti/thread/security/thread_security",
        collapsed: true
    });

    return submodules;
}

/* Static submodule instance definitions */
function modules(inst)
{
    const submodules = [];

    submodules.push({
        name: "multiStack",
        displayName: "Multi-Stack Validation",
        moduleName: "/ti/common/multi_stack_validate",
        hidden: true
    });
    submodules.push({
        name: "rfDriver",
        displayName: "RF Driver",
        moduleName: "/ti/drivers/RF",
        hidden: true
    });
    submodules.push({
        name: "rfDesign",
        displayName: "RF Design",
        moduleName: "/ti/devices/radioconfig/rfdesign"
    });

    return submodules;
}

/* Makes the device type read only when deviceTypeReadOnly is true */
function onDeviceTypeReadOnlyChange(inst, ui)
{
    if(inst.deviceTypeReadOnly === true)
    {
        ui.deviceType.readOnly = "Device is read-only due to not being able "
                                 + "to update libraries and dependencies with "
                                 + "SysConfig";
    }
    else
    {
        ui.deviceType.readOnly = false;
    }
}

/* Thread module definition */
const threadModule = {
    displayName: "TI-OpenThread",
    description: "TI-OpenThread stack configuration module",
    longDescription: moduleLongDescription,
    moduleStatic: moduleStatic,

    templates: {
        "/ti/thread/templates/tiop_config.h.xdt":
                            "/ti/thread/templates/tiop_config.h.xdt",
        "/ti/thread/templates/tiop_config.c.xdt":
                            "/ti/thread/templates/tiop_config.c.xdt",
        "/ti/utils/build/GenLibs.cmd.xdt":
        {
            modName: "/ti/thread/thread",
            getLibs: getLibs
        }
    }
};

function validate(inst, validation)
{
    // Verify that the RF module is using the board hardware
    if(system.deviceData.board && system.deviceData.board.components.RF)
    {
        const RF = system.modules["/ti/drivers/RF"];

        if(RF.$static.$hardware !== system.deviceData.board.components.RF)
        {
            validation.logWarning("Thread stack requires the use of the RF "
                + "Antenna Switch hardware", RF.$static, "$hardware");
        }
    }
}

exports = threadModule;
