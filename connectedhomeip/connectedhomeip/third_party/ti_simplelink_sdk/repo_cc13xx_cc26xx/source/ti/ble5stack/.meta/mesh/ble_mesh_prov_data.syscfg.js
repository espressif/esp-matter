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
 *  ======== ble_mesh_features.syscfg.js ========
 */

"use strict";

// Get mesh prov data long descriptions
const Docs = system.getScript("/ti/ble5stack/mesh/ble_mesh_prov_data_docs.js");
// Get common Script
const Common = system.getScript("/ti/ble5stack/ble_common.js");

const config = {
        name: "provisioningData",
        displayName: "Provisioning Data Configuration",
        description: "Provisioning Properties and Capabilities",
        config: [
            {
                name: "uuid",
                displayName: "UUID",
                displayFormat: "hex",
                default: system.utils.bigInt("00",16),
                hidden: true,
                description: "The UUID that's used when advertising unprovisioned beacon",
                longDescription: Docs.uuidLongDescription
            },
            {
                name: "uri",
                displayName: "Uri",
                default: "",
                hidden: true,
                longDescription: Docs.uriLongDescription
            },
            {
                name: "oobInfo",
                displayName: "Out of Band information",
                default: "none",
                hidden: true,
                longDescription: Docs.oobInfoLongDescription,
                options: [
                    {
                        name: "none",
                        displayName: "None"
                    },
                    {
                        name: "BT_MESH_PROV_OOB_OTHER",
                        displayName: "BT_MESH_PROV_OOB_OTHER"
                    },
                    {
                        name: "BT_MESH_PROV_OOB_URI",
                        displayName: "BT_MESH_PROV_OOB_URI"
                    },
                    {
                        name: "BT_MESH_PROV_OOB_2D_CODE",
                        displayName: "BT_MESH_PROV_OOB_2D_CODE"
                    },
                    {
                        name: "BT_MESH_PROV_OOB_BAR_CODE",
                        displayName: "BT_MESH_PROV_OOB_BAR_CODE"
                    },
                    {
                        name: "BT_MESH_PROV_OOB_NFC",
                        displayName: "BT_MESH_PROV_OOB_NFC"
                    },
                    {
                        name: "BT_MESH_PROV_OOB_NUMBER",
                        displayName: "BT_MESH_PROV_OOB_NUMBER"
                    },
                    {
                        name: "BT_MESH_PROV_OOB_STRING",
                        displayName: "BT_MESH_PROV_OOB_STRING"
                    },
                    {
                        name: "BT_MESH_PROV_OOB_ON_BOX",
                        displayName: "BT_MESH_PROV_OOB_ON_BOX"
                    },
                    {
                        name: "BT_MESH_PROV_OOB_IN_BOX",
                        displayName: "BT_MESH_PROV_OOB_IN_BOX"
                    },
                    {
                        name: "BT_MESH_PROV_OOB_ON_PAPER",
                        displayName: "BT_MESH_PROV_OOB_ON_PAPER"
                    },
                    {
                        name: "BT_MESH_PROV_OOB_IN_MANUAL",
                        displayName: "BT_MESH_PROV_OOB_IN_MANUAL"
                    },
                    {
                        name: "BT_MESH_PROV_OOB_ON_DEV",
                        displayName: "BT_MESH_PROV_OOB_ON_DEV"
                    }
                ]
            },
            {
                name: "oobAuthenticationMethod",
                displayName: "Out of Band Authentication Method",
                default: "none",
                hidden: true,
                onChange: onOobAuthenticationMethodChange,
                longDescription: Docs.oobAuthenticationMethodLongDescription,
                options: [
                    {
                        name: "none",
                        displayName: "None",
                    },
                    {
                        name: "staticOob",
                        displayName: "Static OOB",
                    },
                    {
                        name: "outputOob",
                        displayName: "Output OOB",
                    },
                    {
                        name: "inputOob",
                        displayName: "Input OOB",
                    }
                ]
            },
            {
                name: "staticVal",
                displayName: "Static Value",
                displayFormat: "hex",
                default: system.utils.bigInt("00",16),
                hidden: true,
                longDescription: Docs.staticValLongDescription
            },
            {
                name: "outputSize",
                displayName: "Output Size",
                description: "Maximum size of Output OOB supported",
                default: 0,
                hidden: true,
                longDescription: Docs.outputSizeLongDescription
            },
            {
                name: "outputActions",
                displayName: "Output Actions",
                description: "Supported Output OOB Actions",
                displayFormat: "hex",
                default: 0,
                hidden: true,
                longDescription: Docs.outputActionsLongDescription
            },
            {
                name: "inputSize",
                displayName: "Input Size",
                description: "Maximum size of Input OOB supported",
                displayFormat: "hex",
                default: 0,
                hidden: true,
                longDescription: Docs.inputSizeLongDescription
            },
            {
                name: "inputActions",
                displayName: "Input Actions",
                description: "Supported Input OOB Actions",
                displayFormat: "hex",
                default: 0,
                hidden: true,
                longDescription: Docs.inputActionsLongDescription
            },
            {
                name: "outputNumberCbName",
                displayName: "Output Number Callback Name",
                default: "",
                hidden: true,
                longDescription: Docs.outputNumberCbNameLongDescription
            },
            {
                name: "outputStringCbName",
                displayName: "Output String Callback Name",
                default: "",
                hidden: true,
                longDescription: Docs.outputStringCbNameLongDescription
            },
            {
                name: "inputCbName",
                displayName: "Input Callback Name",
                default: "",
                hidden: true,
                longDescription: Docs.inputCbNameLongDescription
            },
            {
                name: "inputCompleteCbName",
                displayName: "Input Complete Callback Name",
                default: "",
                hidden: true,
                longDescription: Docs.inputCompleteCbNameLongDescription
            },
            {
                name: "unprovBeaconCbName",
                displayName: "Unprivisioned Beacon Callback Name",
                default: "",
                hidden: true,
                longDescription: Docs.unprovBeaconCbNameLongDescription
            },
            {
                name: "linkOpenCbName",
                displayName: "Link Open Callback Name",
                default: "",
                hidden: true,
                longDescription: Docs.linkOpenCbNameLongDescription
            },
            {
                name: "linkCloseCbName",
                displayName: "Link Close Callback Name",
                default: "",
                hidden: true,
                longDescription: Docs.linkCloseCbNameLongDescription
            },
            {
                name: "completeCbName",
                displayName: "Complete Callback Name",
                default: "",
                hidden: true,
                longDescription: Docs.completeCbNameLongDescription
            },
            {
                name: "nodeAddedCbName",
                displayName: "Node Added Callback Name",
                default: "",
                hidden: true,
                longDescription: Docs.nodeAddedCbNameLongDescription
            },
            {
                name: "resetCbName",
                displayName: "Reset Callback Name",
                default: "",
                hidden: true,
                longDescription: Docs.resetCbNameLongDescription
            },
        ]
};

/*
 *  ======== onOobAuthenticationMethodChange ========
 * Hide/unhide the relevant OOB configurables according
 * to the selected method.
 * @param inst  - Module instance containing the config that changed
 * @param ui    - The User Interface object
 */
function onOobAuthenticationMethodChange(inst,ui)
{
    switch(inst.oobAuthenticationMethod) {
        case "none":
            ui.staticVal.hidden = true;
            ui.outputSize.hidden = true;
            ui.outputActions.hidden = true;
            ui.inputSize.hidden = true;
            ui.inputActions.hidden = true;
          break;
        case "staticOob":
            ui.staticVal.hidden = false;
            ui.outputSize.hidden = true;
            ui.outputActions.hidden = true;
            ui.inputSize.hidden = true;
            ui.inputActions.hidden = true;
          break;
          case "outputOob":
              ui.staticVal.hidden = true;
              ui.outputSize.hidden = false;
              ui.outputActions.hidden = false;
              ui.inputSize.hidden = true;
              ui.inputActions.hidden = true;
            break;
        case "inputOob":
            ui.staticVal.hidden = true;
            ui.outputSize.hidden = true;
            ui.outputActions.hidden = true;
            ui.inputSize.hidden = false;
            ui.inputActions.hidden = false;
          break;
      }
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
    if(inst.uuid.toString(16).length > 32)
    {
        validation.logError("The max length of UUID is 16 bytes", inst, "uuid");
    }
    if(inst.staticVal.toString(16).length > 32)
    {
        validation.logError("The max length of Static Val is 16 bytes", inst, "staticVal");
    }
}

// Exports to the top level BLE module
exports = {
    config: config,
    validate: validate,
    onOobAuthenticationMethodChange: onOobAuthenticationMethodChange
};
