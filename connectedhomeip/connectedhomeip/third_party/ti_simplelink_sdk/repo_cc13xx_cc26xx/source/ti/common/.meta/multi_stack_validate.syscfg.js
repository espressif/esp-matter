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
 *  ======== multi_stack_validate.syscfg.js ========
 */

"use strict";

let firstStack = null;

// Static implementation of Multi-Stack Validation module
const moduleStatic = {

    // Validation function
    validate: validate
};

/*
 * ======== validate ========
 * Validate this inst's configuration
 *
 * @param inst       - Multi-Stack Validation instance to be validated
 * @param validation - object to hold detected validation issues
 */
function validate(inst, validation)
{
    // DMM validation override
    if(system.modules["/ti/dmm/dmm"] !== undefined)
    {
        const dmmModuleScript = system.getScript("/ti/dmm/dmm");
        dmmModuleScript.validateProjectStacks(inst, validation);
        return;
    }

    const stackList = inst.$sharedBy; // Get list of stacks using this module
    const numStacks = stackList.length; // Number of stacks

    if(numStacks === 1)
    {
        // Number of stacks is 1, save the stack for use later
        firstStack = stackList[0];
    }
    else
    {
        // Multiple stacks present, loop through all stacks issuing errors
        let i = 0;
        for(i = 0; i < numStacks; i++)
        {
            // Don't issue an error on the first stack added
            if(stackList[i] !== firstStack)
            {
                validation.logError("SysConfig Does Not Currently Support "
                    + "Multiple Stack Configuration", stackList[i]);
            }
        }
    }
}

/*
 *  ======== Multi-Stack Validation module ========
 *  Define the Multi-Stack module properties and methods
 */
exports = {
    displayName: "Multi-Stack Validation",
    description: "Allows only 1 stack module at a time",
    moduleStatic: moduleStatic
};
