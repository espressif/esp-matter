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
 *  ======== dmm_stack_role.js ========
 */

"use strict";

// Stack roles supported by DMM (DMMPolicy_StackRole)
const DmmStackRoles = {
    invalid: "DMMPolicy_StackRole_invalid",
    blePeripheral: "DMMPolicy_StackRole_BlePeripheral",
    wsnNode: "DMMPolicy_StackRole_WsnNode",
    ti154Sensor: "DMMPolicy_StackRole_154Sensor",
    threadMTD: "DMMPolicy_StackRole_threadMtd",
    threadFTD: "DMMPolicy_StackRole_threadFtd",
    ti154Collector: "DMMPolicy_StackRole_154Collector",
    zigbeeEndDevice: "DMMPolicy_StackRole_ZigbeeEndDevice",
    zigbeeRouter: "DMMPolicy_StackRole_ZigbeeRouter",
    zigbeeCoordinator: "DMMPolicy_StackRole_ZigbeeCoordinator",
    custom1: "DMMPolicy_StackRole_custom1",
    custom2: "DMMPolicy_StackRole_custom2"
};

// Applied Activities supported by DMM
const DmmAppliedActivityBLE = [
    "DMMPOLICY_APPLIED_ACTIVITY_BLE_CONNECTION",
    "DMMPOLICY_APPLIED_ACTIVITY_BLE_LINK_EST",
    "DMMPOLICY_APPLIED_ACTIVITY_BLE_BROADCASTING",
    "DMMPOLICY_APPLIED_ACTIVITY_BLE_OBSERVING"
];

const DmmAppliedActivity154 = [
    "DMMPOLICY_APPLIED_ACTIVITY_154_DATA",
    "DMMPOLICY_APPLIED_ACTIVITY_154_LINK_EST",
    "DMMPOLICY_APPLIED_ACTIVITY_154_TX_BEACON",
    "DMMPOLICY_APPLIED_ACTIVITY_154_RX_BEACON",
    "DMMPOLICY_APPLIED_ACTIVITY_154_FH",
    "DMMPOLICY_APPLIED_ACTIVITY_154_SCAN",
    "DMMPOLICY_APPLIED_ACTIVITY_154_RXON"
];

const DmmAppliedActivityThread = [
    "DMMPOLICY_APPLIED_ACTIVITY_THREAD_ALL"
];

const DmmAppliedActivityWSN = [
    "DMMPOLICY_APPLIED_ACTIVITY_WSN_RETRANSMIT",
    "DMMPOLICY_APPLIED_ACTIVITY_WSN_TRANSMIT",
    "DMMPOLICY_APPLIED_ACTIVITY_WSN_RECEIVE"
];

// ======== Classes ========

/**
 * StackRole
 *
 * Represents a stack role containing information required to generate
 * code in the policy table template.
 *
 * Properties:
 * - protocol: Protocol for the object (irrespective of the device role)
 * - displayName: UI name for the stack role
 * - policyIndexString: C identifier for the role index in the policy table
 * - dmmStackRole: Enum identifier for the stack role
 * - states: Stack states associated with the protocol (depreciated in 3Q 2019)
 */
class StackRole
{
    constructor(protocol, displayName, policyIndexString, dmmStackRole)
    {
        this.protocol = protocol;
        this.displayName = displayName;
        this.policyIndexString = policyIndexString;
        this.dmmStackRole = dmmStackRole;

        this.states = [];
        this.appliedActivity = [];
    }
}


// ======== Stack Role Definitions ========

const ti154CollectorRole = new StackRole("ti154stack", "15.4 Collector",
    "TI154_STACK_POLICY_IDX", DmmStackRoles.ti154Collector);
ti154CollectorRole.appliedActivity = DmmAppliedActivity154;

const ti154SensorRole = new StackRole("ti154stack", "15.4 Sensor",
    "TI154_STACK_POLICY_IDX", DmmStackRoles.ti154Sensor);
ti154SensorRole.appliedActivity = DmmAppliedActivity154;

const blePeripheralRole = new StackRole("ble", "BLE Peripheral",
    "BLE_STACK_POLICY_IDX", DmmStackRoles.blePeripheral);
blePeripheralRole.appliedActivity = DmmAppliedActivityBLE;

const custom1Role = new StackRole("custom", "Custom 1",
    "CUSTOM1_STACK_POLICY_IDX", DmmStackRoles.custom1);

const threadMTDRole = new StackRole("tiop", "TI OpenThread MTD",
    "TIOP_STACK_POLICY_IDX", DmmStackRoles.threadMTD);
threadMTDRole.appliedActivity = DmmAppliedActivityThread;

const threadFTDRole = new StackRole("tiop", "TI OpenThread FTD",
    "TIOP_STACK_POLICY_IDX", DmmStackRoles.threadFTD);
threadFTDRole.appliedActivity = DmmAppliedActivityThread;

const custom2Role = new StackRole("custom", "Custom 2",
    "CUSTOM2_STACK_POLICY_IDX", DmmStackRoles.custom2);

const rxAlwaysOnRole = new StackRole("rx_always_on", "RX Always On",
    "RXALWAYSON_STACK_POLICY_IDX", DmmStackRoles.custom1);

const wsnNodeRole = new StackRole("proprietary", "WSN Node",
    "WSN_STACK_POLICY_IDX", DmmStackRoles.wsnNode);
wsnNodeRole.appliedActivity = DmmAppliedActivityWSN;

const zigbeeEndDeviceRole = new StackRole("zstack", "Zigbee End Device",
    "ZIGBEE_STACK_POLICY_IDX", DmmStackRoles.zigbeeEndDevice);
zigbeeEndDeviceRole.appliedActivity = DmmAppliedActivity154;

const zigbeeRouterRole = new StackRole("zstack", "Zigbee Router",
    "ZIGBEE_STACK_POLICY_IDX", DmmStackRoles.zigbeeRouter);
zigbeeRouterRole.appliedActivity = DmmAppliedActivity154;

const zigbeeCoordinatorRole = new StackRole("zstack", "Zigbee Coordinator",
    "ZIGBEE_STACK_POLICY_IDX", DmmStackRoles.zigbeeCoordinator);
zigbeeCoordinatorRole.appliedActivity = DmmAppliedActivity154;

// ======== Functions ========

/**
 *  ======== getInfo ========
 *  Returns an object containing stack role/protocol information
 *
 *  @param stackRoleId  - The configurable option identifier or protocol
 *                        identifier
 *  @returns            - An object containing stack role/protocol information
 */
function getInfo(stackRoleId)
{
    switch(stackRoleId)
    {
        case ("ti154Collector"):
            return ti154CollectorRole;

        case ("ti154Sensor"):
        case ("ti154stack"):
            return ti154SensorRole;

        case ("blePeripheral"):
        case ("ble"):
            return blePeripheralRole;

        case ("custom1"):
            return custom1Role;

        case ("custom2"):
            return custom2Role;

        case ("rxAlwaysOn"):
        case ("rx_always_on"):
            return rxAlwaysOnRole;

        case ("tiop"):
        case ("threadMTD"):
            return threadMTDRole;
        case ("threadFTD"):
            return threadFTDRole;

        case ("wsnNode"):
        case ("proprietary"):
            return wsnNodeRole;

        case ("zigbeeEndDevice"):
        case ("zstack"):
            return zigbeeEndDeviceRole;

        case ("zigbeeRouter"):
            return zigbeeRouterRole;

        case ("zigbeeCoordinator"):
            return zigbeeCoordinatorRole;

        default:
            return custom1Role;
    }
}


/*
 *  ======== exports ========
 *  Export common components
 */
exports = {
    getInfo: getInfo
};
