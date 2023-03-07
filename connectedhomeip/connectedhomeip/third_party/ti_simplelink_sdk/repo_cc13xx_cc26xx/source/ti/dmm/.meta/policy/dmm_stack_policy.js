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
 *  ======== dmm_stack_policy.js ========
 */

"use strict";

const commonConfig = system.getScript("/ti/dmm/dmm_common.js");
const docs = system.getScript("/ti/dmm/dmm_docs.js");
const stackRole = system.getScript("/ti/dmm/dmm_stack_role.js");

/*
*  ======== DMM Stack Role Module ========
*  Protocol stack configuration settings for specific DMM policy table entries
*/


 /**
 *  ======== generateAppliedActivityDropDown ========
 *  Generates a drop down allowing customers to use activities. Both predefined
 *  and custom.
 *
 *  @param stack  - String of the protocol identifier
 *  @returns - A drop down configurable containing options for all applied
 *  activities
 */
function generateAppliedActivityDropDown(stack)
{

    return({
        name: "appliedActivity",
        options: (inst) =>
        {
            const appliedActivities = stackRole.getInfo(stack).appliedActivity;
            const dmmModule = system.modules["/ti/dmm/dmm"].$static;
            const dropDownOptions =  [{
                name: "DMMPOLICY_APPLIED_ACTIVITY_NONE"
            }];

            for(let i = 0; i < appliedActivities.length; i++)
            {
                dropDownOptions.push({
                    name: appliedActivities[i]
                });
            }

            for(let i = 0; i < dmmModule.numCustomActivities; i++)
            {
                dropDownOptions.push({
                    name: "customActivity" + i,
                    displayName: dmmModule["customActivity" + i]
                });
            }

            dropDownOptions.push({
                name: "DMMPOLICY_APPLIED_ACTIVITY_ALL"
            });

            const configurable = dropDownOptions;

            const currentOptions = configurable.slice(0,
                appliedActivities.length +
                dmmModule.numCustomActivities +
                2); // two for the 'none' and 'all' options book casing the list
            return currentOptions;
        },

        description: docs.appliedActivity.description,
        longDescription: docs.appliedActivity.longDescription,
        displayName: "Applied Activity",
        default: ["DMMPOLICY_APPLIED_ACTIVITY_NONE"],
    });
}

/**
 *  ======== generateAppStatesDropDown ========
 *  Generates a drop down allowing customers to use application states
 *
 *  @returns - A drop down configurable containing options for all app states
 */
function generateAppStatesDropDown()
{
    return({
        name: "applicationStates",
        options: (inst) =>
        {
            const dmmModule = system.modules["/ti/dmm/dmm"].$static;
            const dropDownOptions = [];

            for(let i = 0; i < dmmModule.numApplicationStates; i++)
            {
                dropDownOptions.push({
                    name: "applicationState" + i,
                    displayName: dmmModule["applicationState" + i]
                });
            }
            const configurable = dropDownOptions;
            const currentOptions = configurable.slice(0,
                dmmModule.numApplicationStates);
            return currentOptions;
        },
        description: docs.applicationStateDropDown.description,
        longDescription: docs.applicationStateDropDown.longDescription,
        displayName: "Application States",
        default: ["applicationState0"]
    });
}

/**
 *  ======== generateModule ========
 *  Generates the stack role module with its own customized stack
 *  states configurable
 *
 *  @param stack  - String of the protocol identifier
 *  @returns     - A module of the stack role
 */
function generateModule(stack)
{
    const stackModule = {
        config: [
            {
                name: "weight",
                displayName: "Weight",
                description: docs.weight.description,
                longDescription: docs.weight.longDescription,
                default: 0
            },
            {
                name: "pause",
                displayName: "Pause Stack",
                description: docs.pause.description,
                longDescription: docs.pause.longDescription,
                options: [
                    {
                        name: "DMMPOLICY_NOT_PAUSED",
                        displayName: "None"
                    },
                    {
                        name: "DMMPOLICY_PAUSED",
                        displayName: "Pause"
                    }],
                default: "DMMPOLICY_NOT_PAUSED",
                hidden: false
            },
            generateAppStatesDropDown(),
            generateAppliedActivityDropDown(stack)
        ],
        validate: validate
    };

    return stackModule;
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
    if(inst.weight < 0 || inst.weight > 255)
    {
        validation.logError("Weight must be between 0 and 255", inst, "weight");
    }

    if(inst.appliedActivity.length > 1)
    {
        if(inst.appliedActivity.includes("DMMPOLICY_APPLIED_ACTIVITY_NONE"))
        {
            validation.logWarning("DMMPOLICY_APPLIED_ACTIVITY_NONE selected,"
            + " other activities should be unselected", inst,
            "appliedActivity");
        }
        else if(inst.appliedActivity.includes("DMMPOLICY_APPLIED_ACTIVITY_ALL"))
        {
            validation.logInfo("DMMPOLICY_APPLIED_ACTIVITY_ALL selected,"
            + " other activities do not need to be selected", inst,
            "appliedActivity");
        }
    }

    // Check if default policy entry is correct
    const module = system.modules["/ti/dmm/dmm"].$static;
    const lastPolicy = "policy" + (module.numPolicies - 1).toString();
    const defaultPolicy = module.policyArray[module.policyArray.length - 1];

    if(module.stackRoles.length === commonConfig.maxStackRoles)
    {
        // Throw warning only on last policy entry
        if(inst.$ownedBy.$name.includes(lastPolicy))
        {
            const stack1 = defaultPolicy.stackRoles[0];
            const stack2 = defaultPolicy.stackRoles[1];

            if(defaultPolicy[stack1].weight === defaultPolicy[stack2].weight)
            {
                validation.logWarning("Weights in last policy entry should be"
                + " different in order to serve as a tiebreaker when "
                + "conflicting stack activities have equal priority",
                inst, "weight");
            }

            if(!defaultPolicy[stack1].applicationStates
                .includes("applicationState0") || !defaultPolicy[stack2]
                .applicationStates.includes("applicationState0"))
            {
                validation.logWarning("Application States in last policy"
                    + " should be set to any", inst, "applicationStates");
            }
        }
    }

    // Validation for dynamic Enum
    for(let i = 0; i < inst.applicationStates.length; i++)
    {
        if(!inst.applicationStates[i].includes("applicationState"))
        {
            validation.logError("Only application states can be selected",
                inst, "applicationStates");
        }
    }

    // Check if application States are selected appropriately
    if(inst.applicationStates.length > 1)
    {
        if(inst.applicationStates.includes("applicationState0"))
        {
            validation.logInfo("Bitmask `Any` selected, other application states"
                + " do not need to be selected", inst, "applicationStates");
        }

        for(let i = 0; i < inst.applicationStates.length; i++)
        {
            if(inst.applicationStates[i].replace(/^\D+/g, "")
                >= module.numApplicationStates)
            {
                validation.logWarning("Deleted Application States are selected"
                    + ", they will be ignored", inst, "applicationStates");
                break;
            }
        }
    }
    else if(inst.applicationStates.length === 0)
    {
        validation.logError("At least one application state must be selected",
            inst, "applicationStates");
    }

    // Check if custom activities are selected appropriately
    if(inst.appliedActivity.length > 1)
    {
        for(let i = 0; i < inst.appliedActivity.length; i++)
        {
            if(inst.appliedActivity[i].replace(/^\D+/g, "")
                >= module.numCustomActivities)
            {
                validation.logWarning("Deleted Custom Activities are selected"
                    + ", they will be ignored", inst, "appliedActivity");
                break;
            }
        }
    }
    else if(inst.appliedActivity.length === 0)
    {
        validation.logError("At least one applied activity must be selected",
            inst, "appliedActivity");
    }
}

/*
 *  ======== exports ========
 *  Export the stack policy module definition
 */
exports = {
    generateModule: generateModule
};
