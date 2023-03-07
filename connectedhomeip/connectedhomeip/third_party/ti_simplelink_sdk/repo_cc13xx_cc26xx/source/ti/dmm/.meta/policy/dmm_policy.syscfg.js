/*
 * Copyright (c) 2019 Texas Instruments Incorporated - http://www.ti.com
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
 *  ======== dmm_policy.syscfg.js ========
 */

"use strict";

const commonConfig = system.getScript("/ti/dmm/dmm_common.js");
const docs = system.getScript("/ti/dmm/dmm_docs.js");
const stackRole = system.getScript("/ti/dmm/dmm_stack_role.js");

const maxBalanceWindow = 4294967295;

/*
 *  ======== DMM Policy Module ========
 */
const dmmPolicyConfig = [
    commonConfig.stackRoles(commonConfig.options.HIDDEN),
    {
        name: "isDefineSpecific",
        displayName: "Define Specific",
        description: docs.isDefineSpecific.description,
        longDescription: docs.isDefineSpecific.longDescription,
        default: false,
        hidden: false,
        onChange: onIsDefineSpecificChange
    },
    {
        name: "buildDefine",
        displayName: "Define Variable Name",
        description: docs.buildDefine.description,
        default: "Define_Name",
        hidden: true
    },
    {
        name: "balancedMode",
        displayName: "Balanced Mode",
        description: docs.balancedMode.description,
        longDescription: docs.balancedMode.longDescription,
        default: "none",
        options: [
            {
                name: "none",
                displayName: "None"
            },
            {
                name: "timeBm1",
                displayName: "Dynamic"
            },
            {
                name: "timeBm1OnMin",
                displayName: "Min Time Window - High Priority"
            },
            {
                name: "timeBm1OffMax",
                displayName: "Max Time Window - Low Priority"
            },
            {
                name: "timeBm1MinMax",
                displayName: "Min/Max Time Windows"
            }],
        onChange: onBalancedModeChange,
        hidden: false
    },
    {
        name: "balancedMinWindow",
        displayName: "Minimum Time Window (ms)",
        description: docs.balancedMinWindow.description,
        longDescription: docs.balancedMinWindow.longDescription,
        default: 0,
        hidden: true
    },
    {
        name: "balancedMaxWindow",
        displayName: "Maximum Time Window (ms)",
        description: docs.balancedMaxWindow.description,
        longDescription: docs.balancedMaxWindow.longDescription,
        default: 0,
        hidden: true
    }
];

/**
 *  ======== onIsModeSpecificChange ========
 *  Allows user to create policy entry for specific predifined symbols
 *
 *  @param inst - Module instance containing the config that changed
 *  @param ui   - The User Interface object
 */
function onIsDefineSpecificChange(inst, ui)
{
    if(inst.isDefineSpecific)
    {
        ui.buildDefine.hidden = false;
    }
    else
    {
        ui.buildDefine.hidden = true;
    }
}

/**
 * Hides the min/max text boxes depending on the mode selected
 * @param inst - Module instance containing the config that changed
 * @param ui   - The User Interface object
 */
function onBalancedModeChange(inst, ui)
{
    if(inst.balancedMode === "none" || inst.balancedMode === "timeBm1")
    {
        ui.balancedMinWindow.hidden = true;
        ui.balancedMaxWindow.hidden = true;
    }
    else if(inst.balancedMode === "timeBm1OnMin")
    {
        ui.balancedMinWindow.hidden = false;
        ui.balancedMaxWindow.hidden = true;
    }
    else if(inst.balancedMode === "timeBm1OffMax")
    {
        ui.balancedMinWindow.hidden = true;
        ui.balancedMaxWindow.hidden = false;
    }
    else if(inst.balancedMode === "timeBm1MinMax")
    {
        ui.balancedMinWindow.hidden = false;
        ui.balancedMaxWindow.hidden = false;
    }
}

/**
 *  ======== moduleInstances ========
 *  Determines what modules are added as non-static submodules
 *
 *  @param inst  - Module instance containing the config that changed
 *  @returns     - Array containing dependency modules
 */
function moduleInstances(inst)
{
    const stackModules = [];
    let stackRolesLength = inst.stackRoles.length;

    if(inst.stackRoles.length > commonConfig.maxStackRoles)
    {
        stackRolesLength = commonConfig.maxStackRoles;
    }

    for(let i = 0; i < stackRolesLength; i++)
    {
        stackModules.push({
            name: inst.stackRoles[i],
            displayName: stackRole.getInfo(inst.stackRoles[i]).displayName,
            description: docs.stackPolicy.description,
            moduleName: "/ti/dmm/policy/stack/dmm_stack_"
                + stackRole.getInfo(inst.stackRoles[i]).protocol + ".syscfg.js",
            collapsed: true
        });
    }

    return stackModules;
}

/**
 * ======== validate ========
 * Validate this inst's configuration
 *
 * @param inst       - Module instance to be validated
 * @param validation - Object to hold detected validation issues
 */
function validate(inst, validation)
{
    if(inst.isDefineSpecific && !inst.buildDefine)
    {
        validation.logError("Define name must be set or define specific"
            + " disabled", inst, "buildDefine");
    }

    if(inst.balancedMinWindow < 0 || inst.balancedMinWindow > maxBalanceWindow)
    {
        validation.logError("Time window must be between 0 and UINT_32_MAX",
            inst, "balancedMinWindow");
    }

    if(inst.balancedMaxWindow < 0 || inst.balancedMaxWindow > maxBalanceWindow)
    {
        validation.logError("Time window must be between 0 and UINT_32_MAX",
            inst, "balancedMaxWindow");
    }
}

/*
 *  ======== exports ========
 *  Export the DMM policy sub-module definition
 */
exports = {
    config: dmmPolicyConfig,
    moduleInstances: moduleInstances,
    validate: validate
};
