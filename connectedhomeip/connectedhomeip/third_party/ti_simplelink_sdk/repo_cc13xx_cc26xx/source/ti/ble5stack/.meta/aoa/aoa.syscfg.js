/*
 * Copyright (c) 2018 Texas Instruments Incorporated - http://www.ti.com
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
 *  ======== aoa.syscfg.js ========
 */

"use strict";

//static implementation of the AoA module
const moduleStatic = {
    //configurables for the static AoA module
    config:
    [{
        name: "enableAoa",
        displayName: "Enable AoA",
        default: true,
    }],
    moduleInstances: moduleInstances,
}

/*
 *  ======== moduleInstances ========
 *  Determines what modules are added as non-static submodules
 *
 *  @param inst  - Module instance containing the config that changed
 *  @returns     - Array containing dependency modules
 */
function moduleInstances(inst)
{
    let dependencyModule = [];

    dependencyModule.push({
        name: "antTbl",
        displayName: "AoA Antenna Table",
        moduleName: "/ti/ble5stack/aoa/aoa_ant_tbl",
        collapsed: false,
        useArray: false,
    });

    return(dependencyModule);
}

/*
 *  ======== aoaModule ========
 *  Define the AoA module properties and methods
 */
const aoaModule = {
    displayName: "BLE AoA",
    longDescription:  "The AoA (Angle of Arrival) module is intended to simplify the AoA configuration. \
    \n\n The __Enable AoA__ checkbox is used to enable/disable AoA from the example. \
    \n\n The __AoA Antenna Table__ is used to configure the device's antenna array: \
    \n- It maps DIOs to antenna. \
    \n- It should be adapted to the antenna board design. \
    \n- It only defines the device's antenna array, it does not define the antenna switching pattern.",
    alwaysShowLongDescription: true,
    moduleStatic: moduleStatic,
    templates: {
        "/ti/ble5stack/templates/aoa_config.h.xdt":
        "/ti/ble5stack/templates/aoa_config.h.xdt",

        "/ti/ble5stack/templates/aoa_app_config.opt.xdt":
        "/ti/ble5stack/templates/aoa_app_config.opt.xdt"
    }
};

/*
 *  ======== exports ========
 *  Export the AoA module
 */
exports = aoaModule;
