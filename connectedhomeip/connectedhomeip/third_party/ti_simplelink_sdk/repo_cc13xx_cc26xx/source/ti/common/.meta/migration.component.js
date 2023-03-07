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
 *  ======== migration.component.js ========
 */

"use strict";

// Get common LPRF utility functions
const Common = system.getScript("/ti/common/lprf_common.js");

// Get current board or device
const currTarget = Common.getDeviceOrLaunchPadName(false);

/*
 * ======== getExtraMigrationMarkdown ========
 * Returns aggregated text in markdown format that customers can use to aid in
 * migrating a project between device/boards.
 *
 * This function iterates over all the stacks checking for a
 * getMigrationMarkdown() function and calls it if available. The stack-local
 * getMigrationMarkdown() function shall return following:
 *    - A string with information that highlights general stack-specific
 *      migration challenges and links to further documentation
 *
 * IMPORTANT: It is recommended to format the stack-local getMigrationMarkdown()
 * text as follows:
 *    - 3 bullet points with no more than 120 characters per line
 *
 * Stacks can only populate the markdown dialog text based on the current
 * configuration, but it cannot change based on the target board/device
 * selection in the migration drop-down list.
 *
 * All of the markdown text contributed by stacks is aggregated and presented to
 * the user in stack-specific sections at the end of the fixed migration
 * instructions.
 *
 * @returns String - Full extra markdown text as contributed by selected stacks
 */
function getExtraMigrationMarkdown()
{
    const fixedMarkdown = `__Important:__ Not all migration targets are \
    currently enabled or fully supported, and may require additional steps. In \
    particular, note that PHY and radio settings are not common across all \
    devices. A migration may result in the loss of current PHY and radio \
    configurations and/or require the user to update settings such as RF \
    commands and high PA setup.\n\n`;

    const guidelinesMarkdown = `For more migration information please refer to \
    the guidelines below.\n\n`;

    /*
     * Iterate over all the stacks checking for a getMigrationMarkdown()
     * function and call it if available. It is recommended that that function
     * return a string of short, bullet-pointed stack-specific migration notes
     * to be added to the markdown text
     */
    let stack = null;
    let stackMigrationMarkdown = "";
    let allMigrationMarkdown = "";

    let isStackProject = false;

    for(stack of Common.stacks)
    {
        let stackPath = stack.path;
        let stackName = stack.name;
        if(system.modules[stackPath])
        {
            isStackProject = true;

            // Workaround for RF driver examples provided by EasyLink
            if(stackPath === "/ti/devices/radioconfig/custom")
            {
                stackPath = "/ti/easylink/easylink";
                stackName = "EasyLink";
            }

            // Get the stack's common functions
            const stackCommon = system.getScript(stackPath + "_common.js");

            // Verify stack has a documentation function before calling it
            if(stackCommon.getMigrationMarkdown)
            {
                let stackInfo = stackCommon.getMigrationMarkdown(currTarget);
                stackInfo = _.trim(stackInfo);

                if(stackInfo !== "")
                {
                    // Add individual stack's migration markdown notes
                    const title = `__${stackName} Migration Guidelines__`;
                    stackMigrationMarkdown += `${title}\n\n${stackInfo}\n\n`;
                }
            }
        }
    }

    // Only add extra markdown text for LPRF projects
    if(isStackProject)
    {
        allMigrationMarkdown = fixedMarkdown;

        if(stackMigrationMarkdown !== "")
        {
            allMigrationMarkdown += guidelinesMarkdown + stackMigrationMarkdown;
        }
    }

    return(allMigrationMarkdown);
}

/*
 * ======== getIsMigrationValidDialog ========
 * Determines whether a migration from one board/device to another board/device
 * is supported by all the currently selected stacks.
 *
 * This function iterates over all the stacks checking for an isMigrationValid()
 * function and calls it if available. The stack-local isMigrationValid()
 * function shall return ONE of the following:
 *    - {} <--- Empty object if migration is valid
 *    - {warn: "Warning markdown text"} <--- Object with warn property
 *                                           if migration is valid but
 *                                           might require user action
 *    - {disable: "Disable markdown text"} <--- Object with disable property
 *                                              if migration is not valid
 *
 * Stacks can populate the 'warn' or 'disable' dialog text based on the current
 * configuration as well as the target board/device selection in the migration
 * drop-down list.
 *
 * IMPORTANT: It is recommended to keep the stack-local isMigrationValid() text
 * within 120 characters.
 *
 * All of the dialog text contributed by stacks is aggregated and presented to
 * the user as hover text in the target board/device drop-down list.
 *
 * The full warning and disable markdown text contributed by stacks is returned
 * by this function as ONE of the following:
 *    - {} <--- Empty object if migration is valid
 *    - {warn: "Aggregated warning markdown text"} <--- Object with warn
 *                                                      properties contributed
 *                                                      by stacks
 *    - {disable: "Aggregated disable markdown text"} <-- Object with disable
 *                                                        properties contributed
 *                                                        by stacks
 *
 * @param args - Object. Only contains the target device/board
 *
 * @returns Object - Object containing aggregated warning or disable hover text
 *                   to determine whether a migration is fully supported,
 *                   disabled, or flagged with a warning symbol
 */
function getIsMigrationValidDialog(args)
{
    // Get the target device/board
    let migrationTarget = args.device;
    if(!_.isNil(args.board))
    {
        // Strip off everything up to and including the last '/'
        migrationTarget = args.board.replace(/.*\//, "");
        // Strip off everything after and including the first '.'
        migrationTarget = migrationTarget.replace(/\..*/, "");
    }

    /*
     * Iterate over all the stacks checking for a isMigrationValid()
     * function and call it if available. It is recommended that that function
     * return dialog text no longer than 120 characters in length
     */
    let stack = null;
    let stacksDisabling = [];
    let stacksWarning = [];
    const allMigrationInfo = {warn: "", disable: ""};
    for(stack of Common.stacks)
    {
        let stackPath = stack.path;
        let stackName = stack.name;
        if(system.modules[stackPath])
        {
            // Workaround for RF driver examples provided by EasyLink
            if(stackPath === "/ti/devices/radioconfig/custom")
            {
                stackPath = "/ti/easylink/easylink";
                stackName = "EasyLink";
            }

            // Get the stack's common functions
            const stackCommon = system.getScript(stackPath + "_common.js");

            // Verify that the stack has a migration function before calling it
            if(stackCommon.isMigrationValid)
            {
                const stackMigrationInfo = stackCommon.isMigrationValid(
                    currTarget, migrationTarget
                );

                // Add individual stack's migration info to the pile
                if(stackMigrationInfo.disable)
                {
                    const stackDisableInfo = _.trim(stackMigrationInfo.disable);
                    allMigrationInfo.disable += `• ${stackName}: \
                    ${stackDisableInfo} `;

                    stacksDisabling = _.union(stacksDisabling, [stackName]);
                }
                else if(stackMigrationInfo.warn)
                {
                    const stackWarnInfo = _.trim(stackMigrationInfo.warn);
                    allMigrationInfo.warn += `• ${stackName}: \
                    ${stackWarnInfo} `;

                    stacksWarning = _.union(stacksWarning, [stackName]);
                }
            }
        }
    }

    if(allMigrationInfo.disable !== "")
    {
        // Invalid migration, don't allow it
        stacksDisabling = _.join(stacksDisabling, ", ");
        const fullDisableInfo = `This migration is not supported by: \
            ${stacksDisabling}. ${allMigrationInfo.disable}`;

        return({disable: fullDisableInfo});
    }

    if(allMigrationInfo.warn !== "")
    {
        // Valid migration but with potential manual steps
        stacksWarning = _.join(stacksWarning, ", ");
        const fullWarnInfo = `This migration may not be fully supported by: \
            ${stacksWarning}. ${allMigrationInfo.warn}`;

        return({warn: fullWarnInfo});
    }

    // Valid migration, no warnings or additional text needed
    return({});
}

/*
 * ======== getMigrationDialogInfo ========
 * Allows for modules to contribute additional markdown text to the migration
 * instructions and to determine whether a migration is fully supported,
 * disabled, or flagged with a warning symbol
 *
 * @returns Object - Object containing extra markdown text and hover text used
 *                   to determine whether a migration is supported
 */
function getMigrationDialogInfo()
{
    return({
        extraMarkdown: getExtraMigrationMarkdown(),
        isValid: (args) => getIsMigrationValidDialog(args)
    });
}

exports = {
    getMigrationDialogInfo: getMigrationDialogInfo
};
