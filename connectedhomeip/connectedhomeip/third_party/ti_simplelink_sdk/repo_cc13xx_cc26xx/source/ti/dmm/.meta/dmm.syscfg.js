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
 *  ======== dmm.syscfg.js ========
 */

"use strict";

const docs = system.getScript("/ti/dmm/dmm_docs.js");
const commonConfig = system.getScript("/ti/dmm/dmm_common.js");
const lprfCommon = system.getScript("/ti/common/lprf_common.js");
const genLibs = system.getScript("/ti/utils/build/GenLibs.syscfg.js");

const stackDisplayNameMap = {
    ble: "BLE",
    custom: "Custom",
    easylink: "EasyLink",
    ti154stack: "TI 15.4 Stack",
	  tiop: "TI OpenThread",
    zstack: "Z-Stack"
};

/*
 *  ======== DMM Module ========
 *  Top-level DMM configuration settings
 */

/**
 *  ======== numApplicationStates ========
 *  Generates the configurable for setting
 *  the amount of application states
 *
 *  @param onChange - Function to invoke on configurable value change
 *  @returns        - The number configurable for setting the amount
 *                    of application states
 */
function numApplicationStates(onChange)
{
    const numAppStatesOptions = [];

    for(let i = 1; i <= commonConfig.maxAppStatesSupported; i++)
    {
        numAppStatesOptions.push({
            name: i
        });
    }

    return({
        name: "numApplicationStates",
        displayName: "Number of Application States",
        description: docs.numApplicationStates.description,
        longDescription: docs.numApplicationStates.longDescription,
        default: 1,
        onChange: onChange,
        options: numAppStatesOptions
    });
}

/**
 *  ======== numCustomActivities ========
 *  Generates the configurable for setting
 *  the amount of custom activities
 *
 *  @param onChange - Function to invoke on configurable value change
 *  @returns        - The number configurable for setting the amount
 *                    of custom activities
 */
function numCustomActivities(onChange)
{
    const numCustomActivityOptions = [];

    for (let i = 0; i <= commonConfig.maxCustomActivitiesSupported; i++)
    {
        numCustomActivityOptions.push({
            name: i
        });
    }

    return({
        name: "numCustomActivities",
        displayName: "Number of Custom Activities",
        description: docs.numCustomActivities.description,
        longDescription: docs.numCustomActivities.longDescription,
        default: 1,
        onChange: onChange,
        options: numCustomActivityOptions,
        hidden: true
    });
}

/**
 *  ======== generateAppStatesConfig ========
 *  Generates a series of configurables to allow the
 *  user to add custom application states
 *
 *  @returns - An array containing a configurable to specify the number of app
 *             states and a series of text/boolean configurables
 */
function generateAppStatesConfig()
{
    const config = [numApplicationStates((inst, ui) =>
    {
        for(let i = 0; i < commonConfig.maxAppStatesSupported; i++)
        {
            // Only show the amount of states that the user wants to create
            if(i < inst.numApplicationStates)
            {
                ui["applicationState" + i].hidden = false;
            }
            else
            {
                ui["applicationState" + i].hidden = true;
            }
        }
    })];

    // Generate the configurable text for specifying state identifiers
    for(let i = 0; i < commonConfig.maxAppStatesSupported; i++)
    {
        config.push({
            name: "applicationState" + i,
            displayName: "Application State Name " + i,
            description: docs.applicationState.description,
            longDescription: docs.applicationState.longDescription,
            default: "DMMPOLICY_APPSTATE_STATE" + i,
            placeholder: "DMMPOLICY_APPSTATE_STATE" + i,
            hidden: i !== 0
        });
    }

    return config;
}

/**
 *  ======== generateCustomActivityConfig ========
 *  Generates a series of configurables to allow the
 *  user to add custom activities
 *
 *  @returns - An array containing a configurable to specify the number of
 *             custom activities and a series of text/boolean configurables
 */
function generateCustomActivityConfig()
{
    const config = [numCustomActivities((inst, ui) =>
    {

        if (inst.stackRoles.includes("custom1") || inst.stackRoles.includes("custom2"))
        {
            for(let i = 0; i < commonConfig.maxCustomActivitiesSupported; i++)
            {

                // Only show the amount of custom activities that the user wants to create
                if(i < inst.numCustomActivities)
                {
                    ui["customActivity" + i].hidden = false;
                }
                else
                {
                    ui["customActivity" + i].hidden = true;
                }
            }
        }
    })];

    if (config.numCustomActivities > 0)
    {
        config.numCustomActivities.hidden = false;
    }

    // Generate the configurable text for specifying activity identifiers
    for(let i = 0; i < commonConfig.maxCustomActivitiesSupported; i++)
    {
        config.push({
            name: "customActivity" + i,
            displayName: "Custom Activity Name " + i,
            description: docs.customActivities.description,
            longDescription: docs.customActivities.longDescription,
            default: "DMMPOLICY_ACTIVITY_CUSTOM" + i,
            placeholder: "DMMPOLICY_ACTIVITY_CUSTOM" + i,
            hidden: i !== 0
        });
    }

    return config;
}

const dmmConfig = [
    {
        name: "project",
        displayName: "Project Name",
        description: docs.project.description,
        longDescription: docs.project.longDescription,
        default: "ti_dmm_empty",
        hidden: true
    },
    {
        name: "projectStackConfiguration",
        displayName: "Project Stack Configuration",
        description: docs.projectStackConfiguration.description,
        longDescription: docs.projectStackConfiguration.longDescription,
        options: [{
            name: "any",
            displayName: "Any"
        },
        {
            name: "ble",
            displayName: "BLE"
        },
        {
            name: "custom",
            displayName: "Custom"
        },
        {
            name: "easylink",
            displayName: "EasyLink"
        },
        {
            name: "ti154stack",
            displayName: "TI 15.4 Stack"
        },
		{
            name: "tiop",
            displayName: "TI OpenThread"
        },
        {
            name: "zstack",
            displayName: "Z-Stack"
        }],
        default: ["any"],
        hidden: true
    },
    {
        name: "lockStackRoles",
        displayName: "Lock Stack Roles",
        description: docs.lockStackRoles.description,
        longDescription: docs.lockStackRoles.longDescription,
        default: false,
        hidden: true,
        onChange: onLockStackRolesChange
    },
    commonConfig.stackRoles(commonConfig.options.NOT_HIDDEN),
    {
        name: "customActivities",
        displayName: "Custom Activities",
        description: docs.customActivities.description,
        config: generateCustomActivityConfig()
    },
    {
        name: "applicationStates",
        displayName: "Application States",
        description: docs.applicationStates.description,
        collapsed: false,
        config: generateAppStatesConfig()
    },
    {
        name: "genLibs",
        displayName: "GenLibs",
        description: docs.genLibs.description,
        default: true,
        hidden: true
    }
];

/**
 *  ======== onLockStackRolesChange ========
 *  Prevents the user from changing the protocol stack roles in
 *  example projects and displays enabled stack roles when hovered over
 *
 *  @param inst - Module instance containing the config that changed
 *  @param ui   - The User Interface object
 */
function onLockStackRolesChange(inst, ui)
{
    if(inst.lockStackRoles === true)
    {
        let msg = "Stack Roles: ";
        for(let i = 0; i < inst.stackRoles.length; i++)
        {
            if(i !== 0)
            {
                msg += ", ";
            }
            msg += commonConfig.stackDisplayNames[inst.stackRoles[i]];
        }
        ui.stackRoles.readOnly = msg;
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
    const policyEntries = [];

    for(let i = 0; i < commonConfig.maxDMMPoliciesSupported; i++)
    {
        policyEntries.push("policy" + i);
    }

    return[{
        name: "policyArray",
        displayName: "Policy",
        description: docs.policyTable.description,
        longDescription: docs.policyTable.longDescription,
        moduleName: "/ti/dmm/policy/dmm_policy",
        minInstanceCount: 1,
        maxInstanceCount: commonConfig.maxDMMPoliciesSupported,
        useArray: true,
        args: {
            stackRoles: inst.stackRoles
        },
        legacyMigrationOptions: {
            lengthConfigName: "numPolicies",
            instanceNameMapping: policyEntries
        }
    }];
}

/**
 *  ======== modules ========
 *  Determines what modules are added as static submodules
 *
 *  @param inst  - Module instance containing the config that changed
 *  @returns     - Array containing a static dependency modules
 */
function modules(inst)
{
    const dependencyModule = [];

    // Pull in Multi-Stack validation module
    dependencyModule.push({
        name: "multiStack",
        displayName: "Multi-Stack Validation",
        moduleName: "/ti/common/multi_stack_validate",
        hidden: true
    });

    return(dependencyModule);
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
    validation.logInfo("Reserved for bitmask Any", inst,
        "applicationState0");
    // Check if all application states are valid C identifiers
    for(let i = 0; i < inst.numApplicationStates; i++)
    {
        if(inst["applicationState" + i].match("^[A-z_][A-z0-9_]*$") == null
            || inst["applicationState" + i].match("^__") != null)
        {
            validation.logError("Application state must be a valid C "
                + "identifier", inst, "applicationState" + i);
        }
    }

    // Check if all custom activities are valid C identifiers
    for(let i = 0; i < inst.numCustomActivities; i++)
    {
        if(inst["customActivity" + i].match("^[A-z_][A-z0-9_]*$") == null
            || inst["customActivity" + i].match("^__") != null)
        {
            validation.logError("Custom activity must be a valid C "
                + "identifier", inst, "customActivity" + i);
        }
    }

    if(inst.stackRoles.length !== commonConfig.maxStackRoles)
    {
        validation.logError("Number of Stack Roles must be 2", inst,
            "stackRoles");
    }

    // Informational messages
    validation.logInfo("The order of policy table entries matter to "
    + "the DMM scheduler", inst, "numPolicies");
}

/**
 * ======== validateProjectStacks ========
 * Validate that required stacks are added for the project
 *
 * @param inst       - Module instance to be validated
 * @param validation - object to hold detected validation issues
 */
function validateProjectStacks(inst, validation)
{
    const stackList = inst.$sharedBy; // Get list of stacks using this module
    const dmmModule = stackList.find(item => item.$name === "/ti/dmm/dmm");
    let validStacksCount = 0;

    if(!dmmModule.projectStackConfiguration.includes("any"))
    {
        // Ensure that no additional stacks are included than the ones
        // specified for the project
        for(let i = 0; i < stackList.length; i++)
        {
            const fullModuleName = stackList[i].$name;
            const moduleName = fullModuleName.substring(
                fullModuleName.lastIndexOf("/") + 1
            );

            if(moduleName === "dmm")
            {
                // Do nothing. Don't display error on DMM module
            }
            else if(dmmModule.projectStackConfiguration.includes(moduleName))
            {
                validStacksCount += 1;
            }
            else
            {
                validation.logError("This DMM project does not support the "
                + stackDisplayNameMap[moduleName] + " module", stackList[i]);
            }
        }

        // Ensure that the required stacks are present
        if(validStacksCount !== dmmModule.projectStackConfiguration.length)
        {
            validation.logError("This DMM project requires the following "
            + "stack modules: "
            + stackDisplayNameMap[dmmModule.projectStackConfiguration[0]]
            + " and "
            + stackDisplayNameMap[dmmModule.projectStackConfiguration[1]],
            dmmModule);
        }
    }
    else if((stackList.length > 3))
    {
        // If empty project, allow only up to two RF stacks to be added
        // Add error to last added stacks
        for(let i = 0; i < stackList.length; i++)
        {
            if(stackList[i].$name === "/ti/dmm/dmm")
            {
                // Do nothing. Don't display error on DMM module
            }
            else if(validStacksCount < 2)
            {
                // Don't display error on the first two added stacks
                validStacksCount += 1;
            }
            else
            {
                validation.logError("DMM supports a maximum of 2 "
                + "stack modules", stackList[i]);
            }
        }
    }
}

function getLibs(mod) {
    let result = {
        name: "/ti/dmm/dmm",    // products that use NDK reference this
                                // "group" name in their deps
        libs: [],               // libraries that are needed by the current
                                // configuration
        deps: []                // our libs require TI-DRIVERS libraries
    }

    if (mod.$static.genLibs == true) {
        const boardName = lprfCommon.getDeviceOrLaunchPadName(true);

        /* Get current RTOS configuration information */
        const rtos = system.getRTOS();
        let rtosSuffix = "";
        let lib = "ti/dmm/lib/";

        if (rtos === "freertos")
        {
            rtosSuffix ="_freertos";
        }

        let compiler = system.compiler;

        lib += compiler + "/m4f/";

	if (compiler == "gcc")
	{
	    lib += "libdmmlib" +  rtosSuffix + ".a";
	}
	else
	{
            lib += "dmmlib" + rtosSuffix + ".a";
	}
        result.libs.push(lib);
    }
    return result;
}

/*
 *  ======== exports ========
 *  Export the DMM module definition
 */
exports = {
    displayName: "DMM",
    description: docs.module.description,
    longDescription: docs.module.longDescription,
    moduleStatic: {
        config: dmmConfig,
        validate: validate,
        modules: modules,
        moduleInstances: moduleInstances
    },
    templates: {
        "/ti/dmm/templates/ti_dmm_application_policy.c.xdt": true,
        "/ti/dmm/templates/ti_dmm_application_policy.h.xdt": true,
        "/ti/dmm/templates/ti_dmm_custom_activities.h.xdt": true,

        "/ti/utils/build/GenLibs.cmd.xdt": {
            modName: "/ti/dmm/dmm",
            getLibs: getLibs
        }
    },
    validateProjectStacks: validateProjectStacks
};
