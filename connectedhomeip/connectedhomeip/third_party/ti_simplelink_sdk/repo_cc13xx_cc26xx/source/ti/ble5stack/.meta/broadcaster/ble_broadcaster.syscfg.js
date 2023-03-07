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
 *  ======== ble_broadcaster.syscfg.js ========
 */

"use strict";

// Get common Script
const Common = system.getScript("/ti/ble5stack/ble_common.js");

const config = {
    name: "broadcasterConfig",
    displayName: "Broadcaster Configuration",
    description: "Configure Broadcaster Role Settings",
    config: [
        {
            name: "hideBroadcasterGroup",
            default: false,
            hidden: true
        },
        {
            name: "numOfDefAdvSets",
            displayName: "Num of Default Advertisement Sets",
            default: 2,
            hidden: true
        },
        {
            name: "numOfAdvSets",
            displayName: "Number of Advertisement Sets",
            default: 2,
            hidden: false,
            options: [
                {name: 0},
                {name: 1},
                {name: 2},
                {name: 3},
                {name: 4},
                {name: 5},
                {name: 6},
                {name: 7},
                {name: 8},
                {name: 9},
                {name: 10},
                {name: 11},
                {name: 12},
                {name: 13},
                {name: 14},
                {name: 15},
                {name: 16},
                {name: 17},
                {name: 18},
                {name: 19},
                {name: 20}
            ]
        }
    ]
};

/*
 *  ======== moduleInstances ========
 *  Determines what modules are added as non-static submodules
 *
 *  @param inst  - Module instance containing the config that changed
 *  @returns     - Array containing dependency modules
 */
function moduleInstances(inst)
{
    const dependencyModule = [];

    // If mesh is enabled and includes peripheral add the
    // default application adv set parameters
    let meshAndPeri = false;

    inst.mesh &&
    (inst.meshApp == "meshAndPeri" || inst.meshApp == "meshAndPeriOadOffchip" || inst.meshApp == "meshAndPeriOadOnchip")?
    meshAndPeri = true : meshAndPeri = false;

    if(!inst.hideBroadcasterGroup)
    {
        for(let i = 1; i <= inst.numOfAdvSets; i++)
        {
            const args = {
                numOfAdvSet: i,
                meshAdnPeri: meshAndPeri
            }
        
            dependencyModule.push({
                name: "advSet" + i,
                displayName: "Advertisement Set " + i,
                moduleName: "/ti/ble5stack/broadcaster/advertisement_set",
                collapsed: true,
                group: "broadcasterConfig",
                args: args
            });
        }
    }
    return(dependencyModule);
}

/*
 * ======== validate ========
 * Validate this inst's configuration
 *
 * @param inst       - broadcaster instance to be validated
 * @param validation - object to hold detected validation issues
 */
function validate(inst, validation)
{
    if(inst.deviceRole.includes("PERIPHERAL_CFG") || inst.deviceRole.includes("BROADCASTER_CFG"))
    {
        if(inst.numOfAdvSets != inst.numOfDefAdvSets)
        {
            validation.logWarning("The example app code handles " + inst.numOfDefAdvSets + " advertisement sets. "
                                    + "Please make sure to update the example app code " +
                                    "according to the configured number of sets.", inst, "numOfAdvSets");
        }
    }
}

// Exports to the top level BLE module
exports = {
    config: config,
    moduleInstances: moduleInstances,
    validate: validate
};
