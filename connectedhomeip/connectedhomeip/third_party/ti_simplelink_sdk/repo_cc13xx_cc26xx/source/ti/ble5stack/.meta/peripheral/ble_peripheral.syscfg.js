 /* Copyright (c) 2018 Texas Instruments Incorporated - http://www.ti.com
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
 *  ======== ble_peripheral.syscfg.js ========
 */

"use strict";

// Get peripheral long descriptions
const Docs = system.getScript("/ti/ble5stack/peripheral/ble_peripheral_docs.js");

const config = {
    name: "peripheralConfig",
    displayName: "Peripheral Configuration",
    description: "Configure Peripheral Role Settings",
    config: [
        {
            name: "hidePeripheralGroup",
            default: false,
            hidden: true
        },
        {
            name: "paramUpdateReq",
            displayName: "Parameter Updates Request Decision",
            default: "GAP_UPDATE_REQ_PASS_TO_APP",
            hidden: false,
            longDescription: Docs.paramUpdateReqLongDescription,
            options: [
                {
                    displayName: "Accept All",
                    name: "GAP_UPDATE_REQ_ACCEPT_ALL",
                    description: "Accept all parameter update requests"
                },
                {
                    displayName: "Deny All",
                    name: "GAP_UPDATE_REQ_DENY_ALL",
                    description: "Deny all parameter update requests"
                },
                {
                    displayName: "Pass to App",
                    name: "GAP_UPDATE_REQ_PASS_TO_APP",
                    description: "Pass a GAP_UPDATE_LINK_PARAM_REQ_EVENT to the app for it to decide by"
                                + "responding with GAP_UpdateLinkParamReqReply"
                }
            ]
        },
        {
            name: "sendParamsUpdateReq",
            displayName: "Send Parameter Update Request",
            description: "Sends parameter update request after connection establishment",
            default: true,
            hidden: false,
            longDescription: Docs.sendParamsUpdateReqLongDescription,
        }
    ]
};

/*
 * ======== validate ========
 * Validate this inst's configuration
 *
 * @param inst       - Peripheral instance to be validated
 * @param validation - object to hold detected validation issues
 */
function validate(inst, validation)
{
    if(inst.paramUpdateDelay < 0 || inst.paramUpdateDelay > (Math.pow(2,32) - 1))
    {
        validation.logError("Parameter Update Delay range is 0 to sizeof(uint32)", inst, "paramUpdateDelay");
    }
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
    const dependencyModule = [];

    if(!inst.hidePeripheralGroup && inst.sendParamsUpdateReq)
    {
        dependencyModule.push({
            name: "connUpdateParamsPeripheral",
            group: "peripheralConfig",
            displayName: "Connection Update Request Params",
            moduleName: "/ti/ble5stack/general/ble_conn_update_params",
            collapsed: true
        });
    }

    return(dependencyModule);
}

// Exports to the top level BLE module
exports = {
    config: config,
    validate: validate,
    moduleInstances: moduleInstances
};
