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
 *  ======== ble_mesh.syscfg.js ========
 */

"use strict";

// Get mesh long descriptions
const Docs = system.getScript("/ti/ble5stack/mesh/ble_mesh_docs.js");
//Get mesh features Script
const meshFeaturesScript = system.getScript("/ti/ble5stack/mesh/"
                            + "ble_mesh_features");
//Get mesh provisioning data Script
const meshProvDataScript = system.getScript("/ti/ble5stack/mesh/"
                            + "ble_mesh_prov_data");
//Get meshDCD Script
const meshDcdScript = system.getScript("/ti/ble5stack/mesh_dcd/"
                            + "mesh_dcd");
// Get common Script
const Common = system.getScript("/ti/ble5stack/ble_common.js");

const config = {
    name: "bleMeshConfig",
    displayName: "BLE Mesh",
    description: "Configure BLE Mesh",
    config: [
        {
            name: "useNV",
            displayName: "Use NV",
            description: "Enables the option to store (and restore) the mesh node state and configuration persistently in flash",
            hidden: true,
            default: true,
            longDescription: Docs.useNVLongDescription
        },
        {
            name: "useExtAdv",
            displayName: "Use Extended Advertising",
            onChange: onUseExtAdvChange,
            description: "Enables the option to use extended advertising instead of legacy advertising",
            hidden: true,
            default: false
        },
        {
            name: "staticProv",
            displayName: "Static Provisioning",
            onChange: onStaticProvChange,
            description: "Enables the option to use static provisioning",
            hidden: true,
            default: false,
            longDescription: Docs.staticProvLongDescription
        },
        {
            name: "deviceOwnAddress",
            displayName: "Device Own Address",
            description: "The node address, this is the primary element address",
            hidden: true,
            default: 1
        },
        meshFeaturesScript.config,
        {
            name: "gattBearerConfiguration",
            displayName: "GATT Bearer Configuration",
            description: "GATT Bearer Configuration",
            config: [
                {
                    name: "filterSize",
                    displayName: "Filter Size",
                    default: 3,
                    hidden: true,
                    description: "Maximum number of filter entries per Proxy Client",
                    longDescription: Docs.filterSizeLongDescription
                },
                {
                    name: "nodeIdTimeout",
                    displayName: "Node ID Timeout",
                    default: 60,
                    hidden: true,
                    description: "Node Identity advertising timeout",
                    longDescription: Docs.nodeIdTimeoutLongDescription
                }
            ],
            collapsed: true
        },
        {
            name: "provisioningConfiguration",
            displayName: "Provisioning Configuration",
            description: "Provisioning Bearers",
            config: [
                {
                    name: "advbearer",
                    displayName: "ADV Bearer",
                    default: true,
                    readOnly: true,
                    hidden: true,
                    description: "Provisioning support using the advertising bearer (PB-ADV)",
                    longDescription: Docs.advbearerLongDescription
                },
                {
                    name: "gattBearer",
                    displayName: "GATT Bearer",
                    onChange: onGattBearerChange,
                    default: true,
                    hidden: true,
                    description: "Provisioning support using GATT (PB-GATT)",
                    longDescription: Docs.gattBearerLongDescription
                },
                {
                    name         : 'provTimeout',
                    displayName  : 'Provisioning Timeout Value',
                    hidden         : true,
                    description  : "Timeout value in ms, of retransmit provisioning PDUs.",
                    longDescription: Docs.provTimeoutLongDescription,
                    default      : 500
                },

            ]
        },
        {
            name: "networkLayerConfiguration",
            displayName: "Network Layer Configuration",
            description: "Network Layer Configuration",
            config: [
                {
                    name: "appKeyCount",
                    displayName: "App Key Count",
                    default: 2,
                    hidden: true,
                    description: "Maximum number of application keys per network",
                    longDescription: Docs.appKeyCountLongDescription
                },
                {
                    name: "subnetCount",
                    displayName: "Subnet Count",
                    default: 2,
                    hidden: true,
                    description: "Maximum number of mesh subnets per network",
                    longDescription: Docs.subnetCountLongDescription
                },
                {
                    name: "msgCacheSize",
                    displayName: "Msg Cache Size",
                    default: 10,
                    hidden: true,
                    description: "Network message cache size",
                    longDescription: Docs.msgCacheSizeLongDescription
                }
            ]
        },
        {
            name: "transportLayerConfiguration",
            displayName: "Transport Layer",
            description: "Transport Layer Configuration",
            config: [
                {
                    name:"numAdvBuf",
                    displayName:"Num of advertising buffers",
                    description: "Number of advertising buffers available",
                    default:20,
                    hidden: true,
                    description: "Number of advertising buffers",
                    longDescription: Docs.numAdvBufLongDescription
                },
                {
                    name: "transportLayerTX",
                    displayName: "Transport Layer TX",
                    config: [
                        {
                            name: "maxNumOfSegMsgTX",
                            displayName: "Max Num of Segmented Messages",
                            default: 1,
                            hidden: true,
                            description: "Maximum number of simultaneous outgoing segmented messages",
                            longDescription: Docs.maxNumOfSegMsgTXLongDescription
                        },
                        {
                            name: "maxNumSegInMsgTX",
                            displayName: "Max Num of Segments in Message",
                            default: 15,
                            hidden: true,
                            description: "Maximum number of segments in outgoing messages",
                            longDescription: Docs.maxNumSegInMsgTXLongDescription
                        }
                    ]
                },
                {
                    name: "transportLayerRX",
                    displayName: "Transport Layer RX",
                    config: [
                        {
                            name: "maxNumSegMsgRX",
                            displayName: "Max Num of Segmented Messages",
                            default: 1,
                            hidden: true,
                            description: "Maximum number of simultaneous incoming segmented messages",
                            longDescription: Docs.maxNumSegMsgRXLongDescription
                        },
                        {
                            name: "maxNumSegInMsgRX",
                            displayName: "Max Num of Segments in Message",
                            default: 15,
                            hidden: true,
                            description: "Maximum number of segments in incoming messages",
                            longDescription: Docs.maxNumSegInMsgRXLongDescription
                        }
                    ]
                }
            ]
        },
        {
            name: "modelLayerConfiguration",
            displayName: "Model Layer Configuration",
            description: "Model Layer Configuration",
            config: [
                {
                    name: "modelGroupAddrCount",
                    displayName: "Model Group Addresses Count",
                    description: "Specifies how many group addresses each model can at most be subscribed to",
                    default: 2,
                    hidden: true,
                    description: "Maximum number of group address subscriptions per model",
                    longDescription: Docs.modelGroupAddrCountLongDescription
                },
                {
                    name: "modelAppKeysCount",
                    displayName: "Model App Keys Count",
                    description: "Specifies how many application keys each model can at most be bound to",
                    default: 1,
                    hidden: true,
                    description: "Maximum number of application keys per model",
                    longDescription: Docs.modelAppKeysCountLongDescription
                },
                {
                    name: "modelExtensions",
                    displayName: "Model Extensions",
                    default: true,
                    hidden: true,
                    description: "Support for Model extensions",
                    longDescription: Docs.modelExtensionsLongDescription
                }
            ]
        },
        meshProvDataScript.config,
        meshDcdScript.config
    ]
};

/*
 *  ======== onUseExtAdvChange ========
 * Change the TX/RX Max Num of Segments in Message parameters.
 *
 * @param inst  - Module instance containing the config that changed
 * @param ui    - The User Interface object
 */
function onUseExtAdvChange(inst,ui)
{
    if(inst.useExtAdv)
    {
        inst.maxNumSegInMsgTX > 4 ? inst.maxNumSegInMsgTX = 4 : true;
        inst.maxNumSegInMsgRX > 4 ? inst.maxNumSegInMsgRX = 4 : true;
    }
}
/*
 *  ======== onStaticProvChange ========
 * Add/Remove the configuration client model and
 * the option to set the device address.
 *
 * @param inst  - Module instance containing the config that changed
 * @param ui    - The User Interface object
 */
function onStaticProvChange(inst,ui)
{
    if(inst.staticProv)
    {
        inst.configurationClient = true
        ui.deviceOwnAddress.hidden = false
    }
    else
    {
        inst.configurationClient = false
        ui.deviceOwnAddress.hidden = true
    }
}

/*
 *  ======== onGattBearerChange ========
 * Hide/unhide the gattBearerConfiguration when
 * gattBearer is changed.
 * @param inst  - Module instance containing the config that changed
 * @param ui    - The User Interface object
 */
function onGattBearerChange(inst,ui)
{
    inst.proxy || inst.gattBearer ?
    system.utils.showGroupConfig("gattBearerConfiguration", inst, ui):
    system.utils.hideGroupConfig("gattBearerConfiguration", inst, ui);
    // Change the device role according to the proxy value
    // When Proxy is used, the central role should be enables as well
    inst.proxy || inst.gattBearer || inst.meshApp != "meshOnly" ?
    inst.deviceRole = "PERIPHERAL_CFG+OBSERVER_CFG" :
    inst.deviceRole = "BROADCASTER_CFG+OBSERVER_CFG";
    meshFeaturesScript.changeGroupsStateMesh(inst,ui);
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
    let dependencyModule = [];
    dependencyModule = dependencyModule.concat(meshDcdScript.moduleInstances(inst));
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
    meshFeaturesScript.validate(inst, validation);
    meshProvDataScript.validate(inst, validation);
    meshDcdScript.validate(inst, validation);

    if(inst.useExtAdv)
    {
        validation.logWarning("The Advertising Extensions are not part of SIG"
                            + " specified Mesh standard, it is a proprietary"
                            + " solution",
                              inst, "useExtAdv");
    }

    if(inst.staticProv)
    {
        validation.logWarning("Static provisioning should only be used for"
                             + " development purposes. This feature not only"
                             + " violates the BLE Mesh Spec, but also introduces"
                             + " potential security vulnerabilities in your network",
                                inst, "staticProv");
    }

    // GATT Bearer Configuration validation
    if(inst.filterSize < 1 || inst.filterSize > 32767)
    {
        validation.logError("Filter Size range is 1 to 32767", inst, "filterSize");
    }
    if(inst.nodeIdTimeout < 1 || inst.nodeIdTimeout > 60)
    {
        validation.logError("Node ID Timeout range is 1 to 60", inst, "nodeIdTimeout");
    }

    // Network Layer Configuration validation
    if(inst.appKeyCount < 1 || inst.appKeyCount > 4096)
    {
        validation.logError("App Key Count range is 1 to 4096", inst, "appKeyCount");
    }
    if(inst.subnetCount < 1 || inst.subnetCount > 4096)
    {
        validation.logError("Subnet Count range is 1 to 4096", inst, "subnetCount");
    }
    if(inst.msgCacheSize < 2 || inst.msgCacheSize > 65535)
    {
        validation.logError("Msg Cache Size range is 2 to 65535", inst, "msgCacheSize");
    }

    // Transport Layer Configuration validation
    if(inst.numAdvBuf < inst.maxNumSegInMsg + 3)
    {
        validation.logError("The `Num of advertising buffers` must be >= from \
                             (`Max Num of Segments in Message` + 3)", inst,
                             ["numAdvBuf", "maxNumSegInMsg"]);
    }
    if(inst.numAdvBuf < 6 || inst.numAdvBuf > 256)
    {
        validation.logError("Num of advertising buffers range is 6 to 256", inst, "numAdvBuf");
    }

    if(inst.useExtAdv)
    {
        if(inst.maxNumSegInMsgTX < 2 || inst.maxNumSegInMsgTX > 4)
        {
            validation.logError("Max Num of Segments in Message range is 2 to 4", inst, "maxNumSegInMsgTX");
        }
        if(inst.maxNumSegInMsgRX < 2 || inst.maxNumSegInMsgRX > 4)
        {
            validation.logError("Max Num of Segments in Message range is 2 to 4", inst, "maxNumSegInMsgRX");
        }
    }
    else
    {
        if(inst.maxNumSegInMsgTX < 2 || inst.maxNumSegInMsgTX > 32)
        {
            validation.logError("Max Num of Segments in Message range is 2 to 32", inst, "maxNumSegInMsgTX");
        }
        if(inst.maxNumSegInMsgRX < 2 || inst.maxNumSegInMsgRX > 32)
        {
            validation.logError("Max Num of Segments in Message range is 2 to 32", inst, "maxNumSegInMsgRX");
        }
    }
    if(inst.maxNumSegMsgRX < 1 || inst.maxNumSegMsgRX > 255)
    {
        validation.logError("Max Num of Segmented Messages range is 1 to 255", inst, "maxNumSegMsgRX");
    }
    if(inst.maxSizeRXSdu < 24 || inst.maxSizeRXSdu > 384)
    {
        validation.logError("Max size of RX SDUs range is 24 to 384", inst, "maxSizeRXSdu");
    }

    // Model Layer Configuration validation
    if(inst.modelGroupAddrCount < 1 || inst.modelGroupAddrCount > 4096)
    {
        validation.logError("Model Group Addresses Count range is 1 to 4096", inst, "modelGroupAddrCount");
    }
    if(inst.modelAppKeysCount < 1 || inst.modelAppKeysCount > 4096)
    {
        validation.logError("Model App Keys Count range is 1 to 4096", inst, "modelAppKeysCount");
    }

    // Provisioning Layer Configuration validation
    if(inst.provTimeout < 100 || inst.provTimeout > 800)
    {
        validation.logError("Provision Tiemout range is 100 to 800", inst, "provTimeout");
    }

}

/*
 *  ======== exports ========
 *  Export the BLE Connection Update Params Configuration module
 */
exports = {
    config: config,
    validate: validate,
    moduleInstances: moduleInstances
};
