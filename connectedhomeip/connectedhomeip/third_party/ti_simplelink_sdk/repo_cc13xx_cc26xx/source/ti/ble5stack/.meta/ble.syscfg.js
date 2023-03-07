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
 *  ======== ble.syscfg.js ========
 */

"use strict";

// Get ble long descriptions
const Docs = system.getScript("/ti/ble5stack/ble_docs.js");

//Get Radio Script
const radioScript = system.getScript("/ti/ble5stack/rf_config/"
                            + "ble_rf_config");
//Get General Script
const generalScript = system.getScript("/ti/ble5stack/general/"
                            + "ble_general");
//Get Bond Manager Script
const bondMgrScript = system.getScript("/ti/ble5stack/bondManager/"
                            + "ble_bondmgr");

//Get Central Script
const centralScript = system.getScript("/ti/ble5stack/central/"
                            + "ble_central");

//Get Observer Script
const observerScript = system.getScript("/ti/ble5stack/observer/"
                            + "ble_observer");

//Get Peripheral Script
const peripheralScript = system.getScript("/ti/ble5stack/peripheral/"
                            + "ble_peripheral");

//Get broadcaster Script
const broadcasterScript = system.getScript("/ti/ble5stack/broadcaster/"
                            + "ble_broadcaster");

//Get Adv Settings Script
const advSetScript = system.getScript("/ti/ble5stack/adv_config/"
                            + "ble_adv_config");
//Get bleMehs Script
const bleMeshScript = system.getScript("/ti/ble5stack/mesh/"
                            + "ble_mesh");

//Get mesh features Script
const meshFeaturesScript = system.getScript("/ti/ble5stack/mesh/"
                            + "ble_mesh_features");
//Get mesh provisioning data Script
const meshProvDataScript = system.getScript("/ti/ble5stack/mesh/"
                            + "ble_mesh_prov_data");
// Get common Script
const Common = system.getScript("/ti/ble5stack/ble_common.js");

//static implementation of the BLE module
const moduleStatic = {
    
    //configurables for the static BLE module
    config: [
        {
            name: "lockProject",
            displayName: "Lock Project",
            default: false,
            hidden: true,
            onChange: onLockProjectChange
        },
        {
            name: "genLibs",
            displayName: "Generate BLE Libraries",
            default: true,
            hidden: true,
            description: "Configures genLibs usage for local libraries. Always hidden"
        },
        {
            name: "calledFromDeviceRole",
            default: false,
            hidden: true
        },
        {
            name: "hidePtm",
            displayName: "Hide PTM",
            default: false,
            hidden: true,
            onChange: onHidePtmChange,
            description: "Used to hide the PTM configurable. Always hidden"
        },
        {
            name: "oadProject",
            displayName: "OAD Project",
            default: false,
            hidden: true,
            onChange: onOadProjectChange,
            description: "Used to indicate that this is an OAD project. Always hidden"
        },
        {
            name: "deviceRole",
            displayName: "Device Role",
            description: "The BLE device role",
            default: "PERIPHERAL_CFG",
            readOnly: false,
            onChange: ondeviceRoleChange,
            longDescription: Docs.deviceRoleLongDescription,
            options: [
                {
                    displayName: "Observer",
                    name: "OBSERVER_CFG"
                },
                {
                    displayName: "Broadcaster",
                    name: "BROADCASTER_CFG"
                },
                {
                    displayName: "Peripheral",
                    name: "PERIPHERAL_CFG"
                },
                {
                    displayName: "Central",
                    name: "CENTRAL_CFG"
                },
                {
                    displayName: "Broadcaster + Observer",
                    name: "BROADCASTER_CFG+OBSERVER_CFG"
                },
                {
                    displayName: "Central + Broadcaster",
                    name: "CENTRAL_CFG+BROADCASTER_CFG"
                },
                {
                    displayName: "Peripheral + Observer",
                    name: "PERIPHERAL_CFG+OBSERVER_CFG"
                },
                {
                    displayName: "Peripheral + Central",
                    name: "PERIPHERAL_CFG+CENTRAL_CFG"
                }
            ]
        },
        {
            name: "bleFeatures",
            displayName: "BLE Features",
            description: "BLE Stack Features",
            config:[
                {
                    name: "bondManager",
                    displayName: "Bond Manager",
                    description: "The Gap Bond Manager is always enabled",
                    longDescription: Docs.bondManagerLongDescription,
                    default: true,
                    readOnly: true
                },
                {
                    name: "extAdv",
                    displayName: "Extended Advertising",
                    description: "BLE5 extended advertising feature",
                    longDescription: Docs.extAdvLongDescription,
                    onChange: onExtAdvChange,
                    default: true
                },
                {
                    name: "periodicAdv",
                    displayName: "Periodic Advertising",
                    longDescription: Docs.periodicAdvLongDescription,
                    hidden: false,
                    default: false
                },
                {
                    name: "periodicAdvSync",
                    displayName: "Periodic Advertising Sync",
                    longDescription: Docs.periodicAdvSyncLongDescription,
                    hidden: true,
                    default: false
                },
                {
                    name: "disablePairing",
                    displayName: "Disable Pairing",
                    onChange: onDisablePairingChange,
                    hidden: isFlashOnlyDevice(),
                    default: false
                },
                {
                    name: "gattDB",
                    displayName: "GATT Database Off Chip",
                    description: "Indicates that the GATT database is maintained off the chip on the"
                                    + "Application Processor (AP)",
                    longDescription: Docs.gattDBLongDescription,
                    default: false
                },
                {
                    name: "gattNoClient",
                    displayName: "GATT No Client",
                    description: "The app must have GATT client functionality "
                               + "to read the Resolvable Private Address Only "
                               + "characteristic and the Central Address Resolution "
                               + "characteristic. To enable it, Uncheck GATT "
                               + "No Client.",
                    longDescription: Docs.gattNoClientLongDescription,
                    default: false,
                    hidden: false
                },
                {
                    name: "L2CAPCOC",
                    displayName: "L2CAP Connection Oriented Channels",
                    default: false,
                    longDescription: Docs.L2CAPCOCLongDescription
                },
                {
                    name: "delayingAttReadReq",
                    displayName: "Delaying An ATT Read Request",
                    longDescription: Docs.delayingAttReadReqLongDescription,
                    default: false,
                    hidden: false
                },
                {
                    name: "trensLayer",
                    displayName: "Transport Layer",
                    default:"HCI_TL_NONE",
                    description: "When using PTM configuration please choose HCI_TL_NONE",
                    longDescription: Docs.trensLayerLongDescription,
                    hidden: true,
                    options: [
                        {
                            displayName: "None",
                            name: "HCI_TL_NONE"
                        },
                        {
                            displayName: "Full",
                            name: "HCI_TL_FULL"
                        }
                    ]
                },
                {
                    name:"mesh",
                    displayName: "Mesh",
                    default: false,
                    hidden: false,
                    onChange: onMeshChange
                },
                {
                    name:"meshApp",
                    displayName: "Mesh Application",
                    default: "meshOnly",
                    onChange: onMeshAppChange,
                    hidden: true,
                    options: [
                        {
                            displayName: "Mesh Only",
                            name: "meshOnly"
                        },
                        {
                            displayName: "Mesh and Peripheral",
                            name: "meshAndPeri"
                        },
                        {
                            displayName: "Mesh and Peripheral OAD Offchip",
                            name: "meshAndPeriOadOffchip"
                        },
                        {
                            displayName: "Mesh and Peripheral OAD Onchip",
                            name: "meshAndPeriOadOnchip"
                        }
                    ]
                },
                {
                    name: "nwpMode",
                    displayName: "Network Processor Mode",
                    default: false,
                    hidden: true
                }
            ]
        },
        {
            name: "enableGattBuilder",
            displayName: "Enable GATT Builder",
            default: false,
            hidden: true,
            onChange: onEnableGattBuildeChange
        },
        {
            name: "gattBuilder",
            displayName: "Custom GATT",
            description: "Adding services and characteristic ",
            default: false,
            hidden: true
        },
        radioScript.config,
        generalScript.config,
        bondMgrScript.config,
        advSetScript.config,
        centralScript.config,
        observerScript.config,
        peripheralScript.config,
        broadcasterScript.config,
        bleMeshScript.config
    ],

    validate: validate,
    moduleInstances: moduleInstances,
    modules: modules
}

/*
 * ======== validate ========
 * Validate this inst's configuration
 *
 * @param inst       - BLE instance to be validated
 * @param validation - object to hold detected validation issues
 */
function validate(inst, validation)
{
    radioScript.validate(inst, validation);
    generalScript.validate(inst, validation);
    bondMgrScript.validate(inst, validation);
    advSetScript.validate(inst, validation);
    centralScript.validate(inst, validation);
    observerScript.validate(inst, validation);
    peripheralScript.validate(inst, validation);
    broadcasterScript.validate(inst, validation);
    bleMeshScript.validate(inst, validation);
    // When using CC2652RB (BAW) device and the BLE role is central/observer/multi_role
    // the LF src clock (srcClkLF) should be different from "LF RCOSC".
    // Therefore, throwing a warning on the CCFG srcClkLF configurable.
    if(system.modules["/ti/devices/CCFG"] && system.deviceData.device == "CC2652RB")
    {
        if((inst.deviceRole != "BROADCASTER_CFG" && inst.deviceRole != "PERIPHERAL_CFG")
            && system.modules["/ti/devices/CCFG"].$static.srcClkLF == "LF RCOSC")
        {
            validation.logWarning("Only BLE Broadcaster and Peripheral roles should use LF RCOSC for CC2652RB device",
                                    system.modules["/ti/devices/CCFG"].$static, "srcClkLF");
        }
    }
    // Throw a warning on the useRcosc configurable when oadProject and useRcosc are set to true,
    // to indicate the user that the CCFG LF clock configuration should be done at the BIM project
    if(inst.oadProject && inst.useRcosc)
    {
        validation.logWarning("For OAD projects, the LF Clock configuration is set in the CCFG file of the BIM project", inst, "useRcosc");
    }
}

/*
 *  ======== isFlashOnlyDevice ========
 *  Check which device is used.
 *  @return Bool - True if FlashOnly device
 *                 False if FlashRom device
 */
function isFlashOnlyDevice() {
    return (
        // Return true if the device is from CC26X1 family
        Common.device2DeviceFamily(system.deviceData.deviceId) == "DeviceFamily_CC26X1"
    );
}

/*
 *  ======== ondeviceRoleChange ========
 * Change the bond manager value when changing the role combination
 * Broadcaster and observer are not using Bond Manager
 * @param inst  - Module instance containing the config that changed
 * @param ui    - The User Interface object
 */
function ondeviceRoleChange(inst,ui)
{
    if(inst.deviceRole == "BROADCASTER_CFG" || inst.deviceRole == "OBSERVER_CFG" || inst.deviceRole == "BROADCASTER_CFG+OBSERVER_CFG")
    {
        inst.maxConnNum = 0;
        inst.maxPDUNum = 0;
        inst.bondManager = false;

        if(!isFlashOnlyDevice())
        {
            // Hide disablePairing
            ui.disablePairing.hidden = true;
        }

        // Change Device Name
        inst.deviceRole == "BROADCASTER_CFG" ? inst.deviceName = "Simple Broadcaster": inst.deviceName = "Simple Observer";
    }
    else
    {
        inst.maxConnNum = generalScript.maxConnNumDefaultValue();
        inst.maxPDUNum = 5;
        inst.bondManager = true;

        if(!isFlashOnlyDevice())
        {
            // Show disablePairing
            ui.disablePairing.hidden = false;
        }

        // Change Device Name
        if(inst.deviceRole == "PERIPHERAL_CFG")
        {
            inst.deviceName = "Simple Peripheral";
        }
        else if(inst.deviceRole == "CENTRAL_CFG")
        {
            inst.deviceName = "Simple Central";
        }
        else
        {
            inst.deviceName = "Multi Role";
        }
    }

    // Use ptm only in the relevant roles
    if(_.isEqual(inst.deviceRole, "PERIPHERAL_CFG") && !inst.hidePtm)
    {
        inst.ptm = false;
        ui.ptm.hidden = false;
    }
    else
    {
        ui.ptm.hidden = true;
    }

    // Enable bondFailAction only when using Central role
    if(inst.deviceRole.includes("CENTRAL_CFG"))
    {
        ui.bondFailAction.hidden = false;
    }
    else
    {
        ui.bondFailAction.hidden = true;
    }

    // Enable peerConnParamUpdateRejectInd only when using Central or Peripheral role combinations
    if(inst.deviceRole == "BROADCASTER_CFG" || inst.deviceRole == "OBSERVER_CFG" || inst.deviceRole == "BROADCASTER_CFG+OBSERVER_CFG")
    {
        ui.peerConnParamUpdateRejectInd.hidden = true;
    }
    else
    {
        ui.peerConnParamUpdateRejectInd.hidden = false;
    }

    inst.calledFromDeviceRole = true;
    // Hide/Unhide groups since the device role was changed
    changeGroupsState(inst,ui);
    inst.calledFromDeviceRole = false;

    // Change the Periodic configurable state
    onExtAdvChange(inst,ui);
}

/*
 *  ======== onExtAdvChange ========
 * Lock or unlock the deviceRole configurable,
 * disable/enable the option to change the deviceRole.
 * @param inst  - Module instance containing the config that changed
 * @param ui    - The User Interface object
 */
function onExtAdvChange(inst,ui)
{
    const devFamily = Common.device2DeviceFamily(system.deviceData.deviceId);
    // Hide/UnHide periodicAdv if extended advertising is enabled and the Broadcaster/Peripheral roles
    // is used
    inst.extAdv && (inst.deviceRole.includes("BROADCASTER_CFG") || inst.deviceRole.includes("PERIPHERAL_CFG")) ?
    ui.periodicAdv.hidden = false : ui.periodicAdv.hidden = true;
    // Hide/UnHide periodicAdv if extended advertising is enabled and the Observer/Central roles
    // is used
    // CC26X1 only support periodic advertising
    if(devFamily != "DeviceFamily_CC26X1")
    {
        inst.extAdv && (inst.deviceRole.includes("OBSERVER_CFG") || inst.deviceRole.includes("CENTRAL_CFG")) ?
        ui.periodicAdvSync.hidden = false : ui.periodicAdvSync.hidden = true;
    }
}

/*
 *  ======== onHidePtmChange ========
 * Show/hide the ptm configurable
 * @param inst  - Module instance containing the config that changed
 * @param ui    - The User Interface object
 */
function onHidePtmChange(inst,ui)
{
    // Use ptm only in the relevant roles
    if(_.isEqual(inst.deviceRole, "PERIPHERAL_CFG") && !inst.hidePtm)
    {
        inst.ptm = false;
        ui.ptm.hidden = false;
    }
    else
    {
        ui.ptm.hidden = true;
    }
}

/*
 *  ======== onLockProjectChange ========
 * Lock or unlock the deviceRole configurable,
 * disable/enable the option to change the deviceRole.
 * @param inst  - Module instance containing the config that changed
 * @param ui    - The User Interface object
 */
function onLockProjectChange(inst,ui)
{
    inst.lockProject ? ui.deviceRole.readOnly = "Only this role is supported" :
                       ui.deviceRole.readOnly = false;
}

/*
 *  ======== onOadProjectChange ========
 * Hide/show parameters
 * @param inst  - Module instance containing the config that changed
 * @param ui    - The User Interface object
 */
function onOadProjectChange(inst,ui)
{
    inst.oadProject ? ui.useRcosc.hidden = false : ui.useRcosc.hidden = true;
}
/*
 *  ======== onDisablePairingChange ========
 * When disablePairing is selected, change the bondPairing configurable
 * value to GAPBOND_PAIRING_MODE_NO_PAIRING and make it readOnly
 * @param inst  - Module instance containing the config that changed
 * @param ui    - The User Interface object
 */
function onDisablePairingChange(inst,ui)
{
    if(inst.disablePairing)
    {
        inst.bondPairing = "GAPBOND_PAIRING_MODE_NO_PAIRING";
        ui.bondPairing.readOnly = true;
    }
    else
    {
        inst.bondPairing = "GAPBOND_PAIRING_MODE_WAIT_FOR_REQ";
        ui.bondPairing.readOnly = false;
    }
}

/*
 *  ======== onEnableGattBuildeChange ========
 * Lock or unlock the enableGattBuilder configurable,
 * disable/enable the option to change the enableGattBuilder.
 * @param inst  - Module instance containing the config that changed
 * @param ui    - The User Interface object
 */
function onEnableGattBuildeChange(inst,ui)
{
    inst.enableGattBuilder ? ui.gattBuilder.hidden = false :
                             ui.gattBuilder.hidden = true;
}

/*
 *  ======== onMeshChange ========
 * Add/remove the Mesh module
 *
 * @param inst  - Module instance containing the config that changed
 * @param ui    - The User Interface object
 */
function onMeshChange(inst,ui)
{
    if(inst.mesh)
    {
        ui.meshApp.hidden = false;
        ui.nwpMode.hidden = false;
    }
    else
    {
        ui.meshApp.hidden = true;
        ui.nwpMode.hidden = true;
    }

    // Disable the option to configure proxy when mesh+sp app is used
    changeProxyState(inst,ui);
    // Hide/Unhide groups since the Mesh Module was added/removes
    changeGroupsState(inst,ui);
}

/*
 *  ======== onMeshAppChange ========
 * Choose the Mesh app
 *
 * @param inst  - Module instance containing the config that changed
 * @param ui    - The User Interface object
 */
function onMeshAppChange(inst,ui)
{
    if(inst.meshApp != "meshOnly")
    {
        inst.numOfDefAdvSets = 1;
        inst.numOfAdvSets    = 1;
    }
    // Disable the option to configure proxy when mesh+sp app is used
    changeProxyState(inst,ui);
    // Hide/Unhide groups since the meshApp combination has been changed
    changeGroupsState(inst,ui);
}

/*
 *  ======== changeProxyState ========
 * Change Proxy state according to the selected meshApp
 *
 * @param inst  - Module instance containing the config that changed
 * @param ui    - The User Interface object
 */
function changeProxyState(inst,ui)
{
    if(inst.mesh && (inst.meshApp == "meshAndPeri" || inst.meshApp == "meshAndPeriOadOffchip"
                     || inst.meshApp == "meshAndPeriOadOnchip"))
    {
        inst.deviceRole = "PERIPHERAL_CFG+OBSERVER_CFG";
        inst.proxy = false;
        ui.proxy.readOnly = true;
    }
    else if(inst.mesh && inst.meshApp == "meshOnly")
    {
        ui.proxy.readOnly = false;
        // Change the device role according to the proxy value
        // When Proxy is used, the central role should be enables as well
        inst.proxy || inst.gattBearer ?
        inst.deviceRole = "PERIPHERAL_CFG+OBSERVER_CFG" :
        inst.deviceRole = "BROADCASTER_CFG+OBSERVER_CFG";
    }
}


/*
 * ======== changeGroupsState ========
 * Hide/Unhide groups, according to the selected features/deviceRole
 *
 * @param inst  - Module instance containing the config that changed
 * @param ui    - The User Interface object
 */
function changeGroupsState(inst,ui)
{
    if(inst.deviceRole == "BROADCASTER_CFG" || inst.deviceRole == "OBSERVER_CFG" || inst.deviceRole == "BROADCASTER_CFG+OBSERVER_CFG")
    {
        // Hide Bond Manager
        inst.hideBondMgrGroup = true;
        Common.hideGroup(Common.getGroupByName(inst.$module.config, "bondMgrConfig"), inst.hideBondMgrGroup, ui);
    }
    else
    {
        // Show Bond Manager
        inst.hideBondMgrGroup = false;
        Common.hideGroup(Common.getGroupByName(inst.$module.config, "bondMgrConfig"), inst.hideBondMgrGroup, ui);
    }

    if(inst.mesh)
    {
        // UnHide the Mesh Group
        system.utils.showGroupConfig("bleMeshConfig", inst, ui);
        // Keep the Configguration Client configurable hidden
        ui.configurationClient.hidden = true;
        // If static provisioning is not enable, keep the deviceOwnAddress configurable hidden
        if(!inst.staticProv)
        {
            ui.deviceOwnAddress.hidden = true;
        }

        // Call the prov data Authentication Method onChange function to hide/unhide the relevant
        // configurables
        meshProvDataScript.onOobAuthenticationMethodChange(inst,ui);
        // Call the Mesh features onChange function to hide/unhide the relevant configurables
        meshFeaturesScript.onFeatureChange(inst,ui);

        // Hide/UnHide BLE roles groups when using Mesh
        meshFeaturesScript.changeGroupsStateMesh(inst,ui);
    }
    else
    {
        // Hide/UnHide Peripheral Group
        inst.deviceRole.includes("PERIPHERAL_CFG") ? inst.hidePeripheralGroup = false : inst.hidePeripheralGroup = true;
        Common.hideGroup(Common.getGroupByName(inst.$module.config, "peripheralConfig"), inst.hidePeripheralGroup, ui);

        // Hide/UnHide Broadcaster Group
        inst.deviceRole.includes("BROADCASTER_CFG") || inst.deviceRole.includes("PERIPHERAL_CFG") ? inst.hideBroadcasterGroup = false : inst.hideBroadcasterGroup = true;
        Common.hideGroup(Common.getGroupByName(inst.$module.config, "broadcasterConfig"), inst.hideBroadcasterGroup, ui);

        // Hide/UnHide Central Group
        inst.deviceRole.includes("CENTRAL_CFG") ? inst.hideCentralGroup = false : inst.hideCentralGroup = true;
        Common.hideGroup(Common.getGroupByName(inst.$module.config, "centralConfig"), inst.hideCentralGroup, ui);

        // Hide/UnHide Observer Group
        inst.deviceRole.includes("OBSERVER_CFG") || inst.deviceRole.includes("CENTRAL_CFG") ? inst.hideObserverGroup = false : inst.hideObserverGroup = true;
        Common.hideGroup(Common.getGroupByName(inst.$module.config, "observerConfig"), inst.hideObserverGroup, ui);

        system.utils.hideGroupConfig("bleMeshConfig", inst, ui);
    }
}

/*
 * ======== getLibs ========
 * Contribute libraries to linker command file
 *
 * @param inst  - Module instance containing the config that changed
 * @returns     - Object containing the name of component, array of dependent
 *                components, and array of library names
 */
function getLibs(inst)
{
    let GenLibs = system.getScript("/ti/utils/build/GenLibs.syscfg.js");
    let libs = [];
    let toolchain = GenLibs.getToolchainDir();

    if(inst.$static.genLibs)
    {
        // Add the BLE libs (oneLib, stackWrapper and Symbols) according to the
        // board/device that is being used.
        // There are 3 different folders (cc26x2r1, cc13x2r1 and cc1352p)
        // Each device should use it from the appropriate folder.
        const devFamily = Common.device2DeviceFamily(system.deviceData.deviceId);
        let basePath = "ti/ble5stack/libraries/";
        const rfDesign = system.modules["/ti/devices/radioconfig/rfdesign"].$static;
        const LPName = rfDesign.rfDesign;
        let devLibsFolder = "cc26x2r1";

        // DeviceFamily_CC26X2 and DeviceFamily_CC26X2X7 devices are using the libs from the
        // cc26x2r1 folder.
        if(devFamily == "DeviceFamily_CC26X2")
        {
            if(LPName != "LP_CC2652PSIP")
            {
                devLibsFolder = "cc26x2r1";
            }
            else
            {
                devLibsFolder = "cc1352p";
            }
        }
        // DeviceFamily_CC13X2 devices are using the libs from the cc13x2r1
        // or the cc1352p folders.
        // Note: Devices with high PA should use the libs from the cc1352p folder.
        else if(devFamily == "DeviceFamily_CC13X2")
        {
            if(LPName == "LAUNCHXL-CC1352P-2" || LPName == "LAUNCHXL-CC1352P-4")
            {
                devLibsFolder = "cc1352p";
            }
            else
            {
                devLibsFolder = "cc13x2r1";
            }
        }
        else if(devFamily == "DeviceFamily_CC26X2X7")
        {
            devLibsFolder = "cc26x2r1";
        }
        else if(devFamily == "DeviceFamily_CC13X2X7")
        {
            if(LPName == "LP_CC1352P7-4")
            {
                devLibsFolder = "cc1352p";
            }
            else
            {
                devLibsFolder = "cc13x2r1";
            }
        }
        else if(devFamily == "DeviceFamily_CC26X1")
        {
            if(LPName == "LP_CC2651P3")
            {
                devLibsFolder = "cc2651p3";
            }
            else
            {
                devLibsFolder = "cc2651r3";
            }
            basePath = "ti/ble5stack_flash/libraries/";
        }

        libs.push(basePath + devLibsFolder + "/OneLib.a");
        libs.push(basePath + devLibsFolder + "/StackWrapper.a");
        // DeviceFamily_CC26X1 devices are using FlashOnly libs, which not required the ROM symbols
        if(devFamily != "DeviceFamily_CC26X1")
        {
            libs.push(basePath + devLibsFolder + "/ble_r2.symbols");
        }
    }

    // Add BLE Mesh libs
    if(inst.$static.mesh)
    {
        if( toolchain == "ticlang")
        {
            libs.push("ti/mesh/ti/kernel/lib/ccs/m4f/zephyr.a");
        }
        else
        {
            libs.push(GenLibs.libPath("ti/mesh/ti/kernel", "zephyr.a"));
        }
        libs.push(GenLibs.libPath("third_party/erpc/ti", "erpc_tirtos_release.a"));
    }

    // Create a GenLibs input argument
    const linkOpts = {
        name: "/ti/ble5stack",
        deps: [],
        libs: libs
    };

    return(linkOpts);
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

    dependencyModule = radioScript.moduleInstances(inst);
    dependencyModule = dependencyModule.concat(centralScript.moduleInstances(inst));
    dependencyModule = dependencyModule.concat(peripheralScript.moduleInstances(inst));
    dependencyModule = dependencyModule.concat(broadcasterScript.moduleInstances(inst));
    if(inst.gattBuilder)
    {
        dependencyModule.push(
        {
            name            : 'services',
            displayName     : 'Service',
            useArray        : true,
            moduleName      : '/ti/ble5stack/gatt_services/Service',
            collapsed       : true,
            args            : {
                hiddenServName : "1",
                numOfServices  : 1
            }
        });
    }
    if(inst.mesh)
    {
        dependencyModule = dependencyModule.concat(bleMeshScript.moduleInstances(inst));
        dependencyModule.push({
            name: "bleTamplates",
            moduleName: "/ti/ble5stack/mesh/ble_mesh_templates"
        });
    }
    return(dependencyModule);
}

/*
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

/*
 *  ======== bleModule ========
 *  Define the BLE module properties and methods
 */
const bleModule = {
    displayName: "BLE",
    longDescription: "The BLE stack module is intended to simplify the stack "
                    + "configuration for the user. This module can be used "
                    + "only with the following applications:\n"
                    + "multi_role, simple_broadcaster, simple_central and "
                    + "simple_peripheral. For more information, refer to "
                    + "the [BLE User's Guide](ble5stack/ble_user_guide/html/"
                    + "ble-stack-5.x/overview.html).",
    moduleStatic: moduleStatic,
    templates: {
        "/ti/ble5stack/templates/ble_config.h.xdt":
        "/ti/ble5stack/templates/ble_config.h.xdt",

        "/ti/ble5stack/templates/ble_config.c.xdt":
        "/ti/ble5stack/templates/ble_config.c.xdt",

        "/ti/ble5stack/templates/build_config.opt.xdt":
        "/ti/ble5stack/templates/build_config.opt.xdt",

        "/ti/ble5stack/templates/ble_app_config.opt.xdt":
        "/ti/ble5stack/templates/ble_app_config.opt.xdt",

        "/ti/utils/build/GenLibs.cmd.xdt":
        {
            modName: "/ti/ble5stack/ble",
            getLibs: getLibs
        }
    }
};

/*
 *  ======== exports ========
 *  Export the BLE module
 */
exports = bleModule;
