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
 *  ======== dmm_code_gen.js ========
 */

"use strict";

const stackRole = system.getScript("/ti/dmm/dmm_stack_role.js");

// Map of the SysConfig identifier to the C identifier for balance mode
const balancedModeDefineMap = {
    none: "DMMPOLICY_BALANCED_NONE",
    timeBm1: "DMMPOLICY_BALANCED_TIME_BM_1",
    timeBm1OnMin: "DMMPOLICY_BALANCED_TIME_MODE_1_ON_MIN",
    timeBm1OffMax: "DMMPOLICY_BALANCED_TIME_MODE_1_OFF_MAX",
    timeBm1MinMax: "DMMPOLICY_BALANCED_TIME_MODE_1"
};

// ======== Classes ========

/**
 * Project
 *
 * A singleton class that represents the project configuration. The primary
 * intent of this class is to provide a level of abstraction for mapping the
 * the SysConfig configurable structure & properties to code.
 */
class Project
{
    constructor(module)
    {
        // Enforce a single instance
        const instance = this.constructor.instance;
        if(instance)
        {
            return instance;
        }

        this.constructor.instance = this;
        this.module = module;
    }

    // Returns the list of application state C identifiers
    get appStates()
    {
        const states = [];

        for(let i = 0; i < this.module.numApplicationStates; i++)
        {
            states.push(this.module["applicationState" + i]);
        }

        return states;
    }

    // Returns the list of custom activity C identifiers
    get customActivities()
    {
        const customActivities = [];

        for(let i = 0; i < this.module.numCustomActivities; i++)
        {
            customActivities.push(this.module["customActivity" + i]);
        }

        return customActivities;
    }

    // Returns the number of policies currently chosen
    get numPolicies()
    {
        return this.module.policyArray.length;
    }

    /**
     *  ======== roles ========
     *  Returns a composite object containing detailed information
     *  regarding the stack role
     *
     *  Properties:
     *  - name: SysConfig identifier
     *  - displayName: UI name of the role
     *  - policyIndexString: C identifier for the policy index
     *  - dmmStackRole: Enum identifier for the stack role
     *  - appliedActivity: stack defined states
     */
    get roles()
    {
        return this.module.stackRoles.map(role => ({
            name: role,
            displayName: stackRole.getInfo(role).displayName,
            policyIndexString: stackRole.getInfo(role).policyIndexString,
            dmmStackRole: stackRole.getInfo(role).dmmStackRole,
            appliedActivity: stackRole.getInfo(role).appliedActivity
        }));
    }

    /**
     *  ======== policyTable ========
     *  Returns a composite object containing information about the policy table
     *
     *  Properties:
     *  - balancedMode: C define for the balanaced mode selected
     *  - roles: List containing stack role information for the given policy
     *           table entry
     *      - states: List of selected application states
     *      - weight: Numerical weight or C define of the priority
     *      - pause: C define of the stack pausing status
     */
    get policyTable()
    {
        const policyTable = [];

        // Get all the policies and roles and store them in an object to return
        for(let policyIndex = 0; policyIndex < this.module.policyArray.length;
            policyIndex++)
        {
            const roles = [];
            const policy = this.module.policyArray[policyIndex];

            for(let roleIndex = 0; roleIndex < this.module.stackRoles.length;
                roleIndex++)
            {
                // Map via name rather than by index
                const roleName = this.module.stackRoles[roleIndex];
                const policyRole = policy[roleName];

                roles[roleName] = {
                    // Find all selected application + stack states for the
                    // given policy role and store them in states
                    states: formatAppStatesStr(this.module,
                        policyRole.applicationStates),
                    weight: policyRole.weight,
                    pause: policyRole.pause,
                    appliedActivity: formatAppliedActivityStr(this.module,
                        policyRole.appliedActivity)
                };
            }

            let balancedModeConfigurable = "";

            // Balanced Mode only supported for Legacy Policy Table
            balancedModeConfigurable = getBalancedModeDefineStr(policy
                .balancedMode, policy.balancedMinWindow, policy
                .balancedMaxWindow);

            policyTable.push({
                balancedMode: balancedModeConfigurable,
                roles: roles
            });
        }

        return policyTable;
    }
}

/**
 *  ======== getBalancedModeDefineStr ========
 *  Generates the stack role module with its own customized stack
 *  states configurable
 *
 *  @param modeStr   - Identifier of the mode selected by the user
 *  @param minWindow - Min window value inputted by the user
 *  @param maxWindow - Max window value inputted by the user
 *  @returns         - The string for the balance mode define
 */
function getBalancedModeDefineStr(modeStr, minWindow, maxWindow)
{
    let modeDefineStr = balancedModeDefineMap[modeStr];

    if(modeStr === "timeBm1OnMin" || modeStr === "timeBm1OffMax"
        || modeStr === "timeBm1MinMax")
    {
        modeDefineStr += "(";

        if(modeStr === "timeBm1OnMin" || modeStr === "timeBm1MinMax")
        {
            modeDefineStr += minWindow;
        }
        if(modeStr === "timeBm1MinMax")
        {
            modeDefineStr += ", ";
        }
        if(modeStr === "timeBm1OffMax" || modeStr === "timeBm1MinMax")
        {
            modeDefineStr += maxWindow;
        }

        modeDefineStr += ")";
    }

    return modeDefineStr;
}

/**
 *  ======== formatStackStatesStr ========
 *  Constructs the bit flag of the application states specified by the user
 *  and of the protocol stack states
 *
 *  @param module      - dmm module instance
 *  @param appStates - List of selected application states for the given policy
 *                     table entry
 *  @returns         - A string of the bit flag of the stack and application
 *                     states
 */
function formatAppStatesStr(module, appStates)
{
    let returnStr = (appStates.length) > 1 ? "(" : "";

    for(let i = 0; i < appStates.length; i++)
    {
        if(parseInt(appStates[i].match(/(\d+)$/)[0], 10)
            < module.numApplicationStates)
        {
            if((appStates.length) > 1 && i > 0)
            {
                returnStr += " | ";
            }
            returnStr += module[appStates[i]];
        }
    }

    if(appStates.length > 1)
    {
        returnStr += ")";
    }

    return returnStr;
}


/**
 *  ======== formatAppliedActivityStr ========
 *  Constructs the bit flag of the applied activities specified by the user
 *
 *  @param appliedActivities      - List of applied activites selected
 *  @returns         - A string of the bit flag of applied activities
 */
function formatAppliedActivityStr(module, appliedActivities)
{
    let returnStr = (appliedActivities.length) > 1 ? "(" : "";

    for(let i = 0; i < appliedActivities.length; i++)
    {
        // If a custom activity was chosen, use the custom name instead of the
        //  default name
        const customActivity = appliedActivities[i].match(/(\d+)$/);
        if(customActivity)
        {
            if(customActivity[0] < module.numCustomActivities)
            {
                if((appliedActivities.length) > 1 && i > 0)
                {
                    returnStr += " | ";
                }
                returnStr += module[appliedActivities[i]];
            }
        }
        else
        {
            returnStr += appliedActivities[i];
        }

    }

    if(appliedActivities.length > 1)
    {
        returnStr += ")";
    }

    if(returnStr.includes("DMMPOLICY_APPLIED_ACTIVITY_ALL"))
    {
        returnStr = "DMMPOLICY_APPLIED_ACTIVITY_ALL";
    }

    return returnStr;
}

/**
 *  ======== toHexString ========
 *  Returns a 32 bit hex value of a number
 *
 *  @param value  - Value to convert to hex
 *  @returns      - String of the hex value
 */
function toHexString(value)
{
    const hexStr = value.toString(16).toUpperCase();
    return("0x" + hexStr.padStart(8, "0"));
}


/*
 *  ======== exports ========
 *  Export code generation components
 */
exports = {
    Project: Project,
    toHexString: toHexString
};
