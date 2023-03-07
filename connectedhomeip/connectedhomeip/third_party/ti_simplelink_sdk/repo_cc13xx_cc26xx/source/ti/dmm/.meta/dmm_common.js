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
 *  ======== dmm_common.js ========
 */

"use strict";

const docs = system.getScript("/ti/dmm/dmm_docs.js");
const lprfCommon = system.getScript("/ti/common/lprf_common.js");

// Max number of application states
const maxAppStatesSupported = 32;

// DMM allows more than 32 policies, but above 32 policies in practice would
// not be used in general
const maxDMMPoliciesSupported = 32;

// Max number of Custom Activities
const maxCustomActivitiesSupported = 32;

// Max number of stack roles
const maxStackRoles = 2;

// Function argument options labeled for readability
const options = {
    HIDDEN: true,
    NOT_HIDDEN: false,
    TEXT: true,
    BOOLEAN: false
};

const stackDisplayNames = {
    ti154Collector: "15.4 Collector",
    ti154Sensor: "15.4 Sensor",
    blePeripheral: "BLE Peripheral",
    custom1: "Custom 1",
    custom2: "Custom 2",
    rxAlwaysOn: "RX Always On",
    wsnNode: "WSN Node",
    zigbeeEndDevice: "Zigbee End Device",
    zigbeeRouter: "Zigbee Router",
    zigbeeCoordinator: "Zigbee Coordinator",
    threadFTD: "Thread FTD",
    threadMTD: "Thread MTD"
};

// Settings for ti/devices/CCFG module
const dmmCCFGSettings = {
    CC1312R1_LAUNCHXL_CCFG_SETTINGS: {},
    CC1352R1_LAUNCHXL_CCFG_SETTINGS: {},
    CC1352P1_LAUNCHXL_CCFG_SETTINGS: {},
    CC1352P_2_LAUNCHXL_CCFG_SETTINGS: {},
    CC1352P_4_LAUNCHXL_CCFG_SETTINGS: {},
    CC26X2R1_LAUNCHXL_CCFG_SETTINGS: {},
    LP_CC2652RB_CCFG_SETTINGS: {},
    LP_CC1352P7_1_CCFG_SETTINGS: {},
    LP_CC1352P7_4_CCFG_SETTINGS: {},
    LP_CC1312R7_CCFG_SETTINGS: {},
    LP_CC2652R7_CCFG_SETTINGS: {}
};

const currBoardName = lprfCommon.getDeviceOrLaunchPadName(true);
const ccfgSettings = dmmCCFGSettings[currBoardName + "_CCFG_SETTINGS"];

// Dictionary mapping current device/board name regex to supported target
const supportedMigrations = {
    // No migrations are supported at this time
    CC1312R1: {},
    CC1352R1: {},
    CC1352P1_LAUNCHXL: {},
    CC1352P_2_LAUNCHXL: {},
    CC1352P_4_LAUNCHXL: {},
    CC1352P1F3RGZ: {},
    /* Represents RSIP board and device */
    "CC26.2R.*SIP": {
        CC2652R1FSIP: {},
        LP_CC2652RSIP: {},
        CC2652R1FRGZ: {},
        CC26X2R1_LAUNCHXL: {}
    },
    /* Represents 26X2R1 board and device */
    "CC26.2R1": {
        CC2652R1FRGZ: {},
        CC26X2R1_LAUNCHXL: {},
        CC2652R1FSIP: {},
        LP_CC2652RSIP: {},
        CC2652RB: {}
    },
};

/**
 *  ======== stackRoles ========
 *  Returns the protocol stack roles configurable object
 *
 *  @param isHidden  - Indicates if the configurable should be hidden by default
 *  @returns         - The protocol stack roles configurable object
 */
function stackRoles(isHidden)
{
    return({
        name: "stackRoles",
        displayName: "Protocol Stack Roles",
        description: docs.stackRoles.description,
        longDescription: docs.stackRoles.longDescription,
        default: ["custom1", "custom2"],
        hidden: isHidden,
        options: [{
            name: "ti154Collector",
            displayName: stackDisplayNames.ti154Collector
        },
        {
            name: "ti154Sensor",
            displayName: stackDisplayNames.ti154Sensor
        },
        {
            name: "blePeripheral",
            displayName: stackDisplayNames.blePeripheral
        },
        {
            name: "custom1",
            displayName: stackDisplayNames.custom1
        },
        {
            name: "custom2",
            displayName: stackDisplayNames.custom2
        },
        {
            name: "rxAlwaysOn",
            displayName: stackDisplayNames.rxAlwaysOn
        },
        {
            name: "wsnNode",
            displayName: stackDisplayNames.wsnNode
        },
        {
            name: "zigbeeEndDevice",
            displayName: stackDisplayNames.zigbeeEndDevice
        },
        {
            name: "zigbeeRouter",
            displayName: stackDisplayNames.zigbeeRouter
        },
        {
            name: "zigbeeCoordinator",
            displayName: stackDisplayNames.zigbeeCoordinator
        },
        {
            name: "threadFTD",
            displayName: stackDisplayNames.threadFTD
        },
        {
            name: "threadMTD",
            displayName: stackDisplayNames.threadMTD
        }
        ],
        onChange: stackRoleOnChange
    });
}

/**
 *  ======== stackRoleOnChange ========
 *  If either custom stackRole is selected, provide the ability to add custom
 *   activities for use in the GPT policy.
 *
 *  @param inst - Module instance containing the config that changed
 *  @param ui   - The User Interface object
 */
function stackRoleOnChange(inst, ui)
{
    if(inst.stackRoles != undefined)
    {
        if (inst.stackRoles.includes("custom1") || inst.stackRoles.includes("custom2"))
        {
            if (ui.numCustomActivities != undefined)
            {
                if (inst.numCustomActivities == 0)
                {
                    inst.numCustomActivities = 1;
                }

                ui.numCustomActivities.hidden = false;

                for(let i = 0; i < inst.numCustomActivities; i++)
                {
                    ui["customActivity" + i].hidden = false;
                }
            }
        }
        else
        {
            if (ui.numCustomActivities != undefined)
            {
                ui["numCustomActivities"].hidden = true;

                for(let i = 0; i < inst.numCustomActivities; i++)
                {
                    ui["customActivity" + i].hidden = true;
                }
            }
        }
    }
}
/*
* ======== getMigrationMarkdown ========
* Returns text in markdown format that customers can use to aid in migrating a
* project between device/boards. It is recommended to provide no more
* than 3 bullet points with up to 120 characters per line.
*
* @param currTarget - Board/device being migrated FROM
* @returns string - Markdown formatted string
*/
function getMigrationMarkdown(currTarget)
{
    const inst = system.modules["/ti/dmm/dmm"].$static;

    // May need to add guidelines when other boards are supported
    let migrationText = "";

    return(migrationText);
}


/*
 * ======== isMigrationValid ========
 * Determines whether a migration from one board/device to another board/device
 * is supported by the 15.4 module.
 *
 * @param currentTarget - Current board/device
 * @param migrationTarget - Target board/device for migration
 * @returns One of the following Objects:
 *    - {} <--- Empty object if migration is valid
 *    - {warn: "Warning markdown text"} <--- Object with warn property
 *                                           if migration is valid but
 *                                           might require user action
 *    - {disable: "Disable markdown text"} <--- Object with disable property
 *                                              if migration is not valid
 */
function isMigrationValid(currentTarget, migrationTarget)
{
    let migRegex = null;

    const defaultDisableText = "Consider starting from an example in "
    + " <SDK_INSTALL_DIR>/examples/ that is closer to the desired migration "
    + "target";

    let migSupported = {};

    for(migRegex in supportedMigrations)
    {
        if(currentTarget.match(new RegExp(migRegex))
            && supportedMigrations[migRegex][migrationTarget])
        {
            migSupported = supportedMigrations[migRegex][migrationTarget];

            // If function exists then migration support is conditional
            if(_.isFunction(migSupported))
            {
                migSupported = migSupported(migrationTarget);
            }
            break;
        }
    }

    return(migSupported);
}

/*
 * ======== migrate ========
 * Perform stack specific changes to the SysConfig env POST migration
 *
 * @param currTarget - Board/device being migrated FROM
 * @param migrationTarget - Board/device being migrated TO
 * @param env - SysConfig environment providing access to all configurables
 * @param projectName - Optional name of the project being migrated
 *
 * @returns boolean - true when migration is supported and successful, false when
 *                    migration is not supported and/or unsuccessful
 */
function migrate(currTarget, migrationTarget, env, projectName = null)
{
    const migrationInfo = isMigrationValid(currTarget, migrationTarget);
    let migrationValid = true;

    if(migrationInfo.disable || migrationInfo.warn)
    {
        migrationValid = false;
    }

    if(migrationValid)
    {
        // Currently no dmm specific sysconfig changes required POST migration.
    }

    return(migrationValid);
}

/*
 *  ======== exports ========
 *  Export common components
 */
exports = {
    stackRoles: stackRoles,
    options: options,
    stackDisplayNames: stackDisplayNames,
    ccfgSettings: ccfgSettings,
    maxAppStatesSupported: maxAppStatesSupported,
    maxCustomActivitiesSupported: maxCustomActivitiesSupported,
    maxStackRoles: maxStackRoles,
    maxDMMPoliciesSupported: maxDMMPoliciesSupported,
    getMigrationMarkdown: getMigrationMarkdown,
    isMigrationValid: isMigrationValid,
    migrate: migrate,
};
