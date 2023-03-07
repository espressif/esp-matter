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

// Get mesh features long descriptions
const Docs = system.getScript("/ti/ble5stack/mesh/ble_mesh_features_docs.js");
// Get common Script
const Common = system.getScript("/ti/ble5stack/ble_common.js");

const config = {
    name: "meshProfileFeatures",
    displayName: "Mesh Profile Features",
    description: "Configure BLE Mesh Settings",
    config: [
        {
            name: "relay",
            displayName: "Relay",
            onChange: onFeatureChange,
            default: true,
            hidden: true,
            longDescription: Docs.relayLongDescription,
            description: "The Relay state indicates support for the Relay feature. If the Relay feature is supported,\n\
                          then this also enables the Relay mode state in the configuration server."
        },
        {
            name: "proxy",
            displayName: "Proxy",
            onChange: onFeatureChange,
            default: true,
            hidden: true,
            longDescription: Docs.proxyLongDescription,
            description: "The GATT Proxy state indicates if the Proxy feature is supported. If the Proxy feature is supported, then\n\
                          this also enables the GATT Proxy state in the configuration server."
        },
        {
            name: "friend",
            displayName: "Friend",
            onChange: onFeatureChange,
            default: false,
            hidden: true,
            longDescription: Docs.friendLongDescription,
            description: "The Friend state indicates support for the Friend feature. If the Friend feature is supported,\n\
                          then this also enables the Friend state in the configuration server."
        },
		{
            name: "friendConfiguration",
			displayName: "Friend Configuration",
			description: "Friend feature configuration",
			config: [
				{
                    name: "recieveWindow",
                    displayName: "Recive Window",
                    default: 50,
                    hidden: true,
                    description: "Friend Receive Window",
                    longDescription: Docs.recieveWindowLongDescription
				},
				{
                    name: "queueSize",
                    displayName: "Queue Size",
					default: 16,
                    hidden: true,
                    description: "Minimum number of buffers supported per Friend Queue",
                    longDescription: Docs.queueSizeLongDescription
				},
				{
                    name: "subscriptionListSize",
                    displayName: "Subscribtion List Size",
					default: 3,
                    hidden: true,
                    description: "Friend Subscription List Size",
                    longDescription: Docs.subscriptionListSizeLongDescription
				},
				{
                    name: "lpnCount",
                    displayName: "Low Power Nodes Count",
					default: 2,
                    hidden: true,
                    description: "Number of supported LPN nodes",
                    longDescription: Docs.lpnCountLongDescription
				},
				{
                    name: "segmentLists",
                    displayName: "Segment Lists",
					default: 1,
                    hidden: true,
                    description: "Number of incomplete segment lists per LPN",
                    longDescription: Docs.segmentListsLongDescription
				}
			],
			collapsed: true
		},
        {
            name: "lowPower",
            displayName: "Low Power (LPN)",
            onChange: onFeatureChange,
            default: false,
            hidden: true,
            longDescription: Docs.lowPowerLongDescription,
            readOnly: "Low Power feature can't be enabled when other Mesh features are enabled"
        },
		{
            name: "lowPowerConfiguration",
			displayName: "Low Power Configuration",
			description: "Low Power feature configuration",
			config: [
				{
                    name: "friendshipEstablishment",
                    displayName: "Friendship Establishment",
                    default: true,
                    hidden: true,
                    description: "Perform Friendship establishment using low power",
                    longDescription: Docs.friendshipEstablishmentLongDescription
				},
				{
                    name: "autoEnableLpn",
                    displayName: "Automatically enable LPN",
                    default: true,
                    hidden: true,
                    description: "Automatically start looking for Friend nodes once provisioned",
                    longDescription: Docs.autoEnableLpnLongDescription
				},
				{
                    name: "autoEnableLpnTimeout",
                    displayName: "Automatically enable LPN Timeout",
					default: 15,
                    hidden: true,
                    description: "Time from last received message before going to LPN mode",
                    longDescription: Docs.autoEnableLpnTimeoutLongDescription
				},
				{
                    name: "retryTimeout",
                    displayName: "Retry Timeout",
					default: 8,
                    hidden: true,
                    description: "Retry timeout for Friend requests",
                    longDescription: Docs.retryTimeoutLongDescription
				},
				{
                    name: "rssiFactor",
                    displayName: "RSSI Factor",
					default: 1,
                    hidden: true,
                    description: "Used in the Friend Offer Delay calculation",
                    longDescription: Docs.rssiFactorLongDescription
                },
				{
                    name: "receiveWindowFactor",
                    displayName: "Receive Window Factor",
					default: 0,
                    hidden: true,
                    description: "Used in the Friend Offer Delay calculation",
                    longDescription: Docs.receiveWindowFactorLongDescription
                },
				{
                    name: "minQueueSize",
                    displayName: "Min Queue Size",
					default: 1,
                    hidden: true,
                    description: "Minimum size of acceptable friend queue (MinQueueSizeLog)",
                    longDescription: Docs.minQueueSizeLongDescription
                },
				{
                    name: "receiveDelay",
                    displayName: "Receive Delay",
					default: 100,
                    hidden: true,
                    description: "Receive delay requested by the local node",
                    longDescription: Docs.receiveDelayLongDescription
                },
				{
                    name: "pollTimeout",
                    displayName: "Poll Timeout",
					default: 300,
                    hidden: true,
                    description: "The value of the PollTimeout timer",
                    longDescription: Docs.pollTimeoutLongDescription
                },
				{
                    name: "initialValuePollTimeout",
                    displayName: "Initial Value of the Poll Timeout",
					default: 300,
                    hidden: true,
                    description: "The starting value of the PollTimeout timer",
                    longDescription: Docs.initialValuePollTimeoutLongDescription
                },
				{
                    name: "scanLatency",
                    displayName: "Scan Latency",
					default: 10,
                    hidden: true,
                    description: "Latency for enabling scanning",
                    longDescription: Docs.scanLatencyLongDescription
                },
				{
                    name: "groups",
                    displayName: "Groups",
					default: 8,
                    hidden: true,
                    description: "Number of groups the LPN can subscribe to",
                    longDescription: Docs.groupsLongDescription
                }
			],
			collapsed: true
        },
        {
            name         : 'beaconState',
            displayName  : 'Secure Network Beacon State',
            hidden       : true,
            default      : true,
            longDescription: Docs.beaconStateLongDescription,
            description: "If enabled, the Secure Network Beacon state in the \
                          configuration server will be enabled.",
        },
    ]
};

/*
 *  ======== onFeatureChange ========
 * Hide/unhide the groups/features according to
 * the selected feature.
 * @param inst  - Module instance containing the config that changed
 * @param ui    - The User Interface object
 */
function onFeatureChange(inst,ui)
{
    if(inst.relay || inst.proxy || inst.friend)
    {
        inst.lowPower = false;
        ui.lowPower.readOnly = "Low Power feature can't be enabled when other Mesh features are enabled";
        inst.beaconState = true;
    }
    else
    {
        ui.lowPower.readOnly = false;
    }

    if(inst.lowPower)
    {
        inst.friend = false;
        ui.friend.readOnly = "Friend feature can't be enabled when Low Power feature is enabled";
        inst.beaconState = false;
    }
    else
    {
        ui.friend.readOnly = false;
    }

    if(inst.relay)
    {
        ui.relay_retransmit_count.hidden = false;
        ui.relay_retransmit_int_ms.hidden = false;
    }
    else
    {
        ui.relay_retransmit_count.hidden = true;
        ui.relay_retransmit_int_ms.hidden = true;
    }

    inst.proxy || inst.gattBearer ?
    system.utils.showGroupConfig("gattBearerConfiguration", inst, ui):
    system.utils.hideGroupConfig("gattBearerConfiguration", inst, ui);

    inst.friend ?
    system.utils.showGroupConfig("friendConfiguration", inst, ui):
    system.utils.hideGroupConfig("friendConfiguration", inst, ui);

    inst.lowPower ?
    system.utils.showGroupConfig("lowPowerConfiguration", inst, ui):
    system.utils.hideGroupConfig("lowPowerConfiguration", inst, ui);

    if(!inst.calledFromDeviceRole)
    {
        // Change the device role according to the proxy value
        // When Proxy is used, the central role should be enables as well
        inst.proxy || inst.gattBearer || inst.meshApp != "meshOnly" ?
        inst.deviceRole = "PERIPHERAL_CFG+OBSERVER_CFG" :
        inst.deviceRole = "BROADCASTER_CFG+OBSERVER_CFG";
        changeGroupsStateMesh(inst,ui);
    }
}

/*
 * ======== changeGroupsStateMesh ========
 * This function hide/unhide the BLE roles groups, when the Mesh module
 * exist (mesh is enabled).
 * The selected features and meshApp used determine which groups will
 * be used.
 *
 * @param inst  - Module instance containing the config that changed
 * @param ui    - The User Interface object
 */
function changeGroupsStateMesh(inst,ui)
{
    // Hide Central Group
    inst.hideCentralGroup = true;
    Common.hideGroup(Common.getGroupByName(inst.$module.config, "centralConfig"), inst.hideCentralGroup, ui);
    // Hide Observer Group
    inst.hideObserverGroup = true;
    Common.hideGroup(Common.getGroupByName(inst.$module.config, "observerConfig"), inst.hideObserverGroup, ui);

    if(inst.meshApp == "meshOnly")
    {
        // When using Mesh only Broadcaster Group should not be used, therefore, hide Broadcaster group
        inst.hideBroadcasterGroup = true;

        if(inst.deviceRole.includes("PERIPHERAL_CFG"))
        {
            // UnHide Peripheral Group
            inst.hidePeripheralGroup = false;
            // UnHide Bond Manager
            inst.hideBondMgrGroup = false;
            // Enable bondManager
            inst.bondManager = true;
        }

        if(inst.deviceRole.includes("BROADCASTER_CFG"))
        {
            // Hide Peripheral Group
            inst.hidePeripheralGroup = true;
            // Hide Bond Manager
            inst.hideBondMgrGroup = true;
            // Disable bondManager
            inst.bondManager = false;
        }
    }

    if(inst.meshApp == "meshAndPeri" || inst.meshApp == "meshAndPeriOadOffchip" || inst.meshApp == "meshAndPeriOadOnchip")
    {
        // When using Mesh+SP, the Broadcaster, Peripheral and BondMgr Groups should be used
        // UnHide Broadcaster Group
        inst.hideBroadcasterGroup = false;
        // UnHide Peripheral Group
        inst.hidePeripheralGroup = false;
        // UnHide Bond Manager
        inst.hideBondMgrGroup = false;
        // Enable bondManager
        inst.bondManager = true;
    }

    // Hide/UnHide Broadcaster, Peripheral and BondMgr Groups
    Common.hideGroup(Common.getGroupByName(inst.$module.config, "broadcasterConfig"), inst.hideBroadcasterGroup, ui);
    Common.hideGroup(Common.getGroupByName(inst.$module.config, "peripheralConfig"), inst.hidePeripheralGroup, ui);
    Common.hideGroup(Common.getGroupByName(inst.$module.config, "bondMgrConfig"), inst.hideBondMgrGroup, ui);
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
    // Friend feature configuration validation
    if(inst.recieveWindow < 1 || inst.recieveWindow > 255)
    {
        validation.logError("Recive Window range is 1 to 255", inst, "recieveWindow");
    }
    if (inst.recieveWindow > 50)
    {
        validation.logWarning("Power consumption may increese at LPN when Recieve Window > 50 [mS]", inst, "recieveWindow")
    }
    if(inst.queueSize < 2 || inst.queueSize > 65536)
    {
        validation.logError("Queue Size range is 2 to 65536", inst, "queueSize");
    }
    if(inst.subscriptionListSize < 0 || inst.subscriptionListSize > 1023)
    {
        validation.logError("Subscribtion List Size range is 0 to 1023", inst, "subscriptionListSize");
    }
    if(inst.lpnCount < 1 || inst.lpnCount > 1000)
    {
        validation.logError("Low Power Nodes Count range is 1 to 1000", inst, "lpnCount");
    }
    if(inst.segmentLists < 1 || inst.segmentLists > 1000)
    {
        validation.logError("Segment Lists range is 1 to 1000", inst, "segmentLists");
    }

    // Low Power feature configuration validation
    if(inst.autoEnableLpnTimeout < 0 || inst.autoEnableLpnTimeout > 3600)
    {
        validation.logError("Automatically enable LPN Timeout range is 0 to 3600", inst, "autoEnableLpnTimeout");
    }
    if(inst.retryTimeout < 1 || inst.retryTimeout > 3600)
    {
        validation.logError("Retry Timeout range is 1 to 3600", inst, "retryTimeout");
    }
    if(inst.rssiFactor < 0 || inst.rssiFactor > 3)
    {
        validation.logError("RSSI Factor range is 0 to 3", inst, "rssiFactor");
    }
    if(inst.receiveWindowFactor < 0 || inst.receiveWindowFactor > 3)
    {
        validation.logError("Receive Window Factor range is 0 to 3", inst, "receiveWindowFactor");
    }
    if(inst.minQueueSize < 1 || inst.minQueueSize > 7)
    {
        validation.logError("Min Queue Size range is 1 to 7", inst, "minQueueSize");
    }
    if(inst.receiveDelay < 10 || inst.receiveDelay > 255)
    {
        validation.logError("Receive Delay range is 10 to 255", inst, "receiveDelay");
    }
    if(inst.pollTimeout < 10 || inst.pollTimeout > 244735)
    {
        validation.logError("Poll Timeout range is 10 to 244735", inst, "pollTimeout");
    }
    if(inst.initialValuePollTimeout < 10 || inst.initialValuePollTimeout > 244735)
    {
        validation.logError("Initial Value of the Poll Timeout range is 10 to 244735", inst, "initialValuePollTimeout");
    }
    if(inst.scanLatency < 0 || inst.scanLatency > 50)
    {
        validation.logError("Scan Latency range is 0 to 50", inst, "scanLatency");
    }
    if(inst.groups < 0 || inst.groups > 16384)
    {
        validation.logError("Groups range is 0 to 16384", inst, "groups");
    }
}

// Exports to the top level BLE module
exports = {
    config: config,
    validate: validate,
    onFeatureChange: onFeatureChange,
    changeGroupsStateMesh: changeGroupsStateMesh
};
