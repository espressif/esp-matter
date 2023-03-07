/***************************************************************************//**
 * @brief Zigbee Permit Join Manager component configuration header.
 *\n*******************************************************************************
 * # License
 * <b>Copyright 2020 Silicon Laboratories Inc. www.silabs.com</b>
 *******************************************************************************
 *
 * The licensor of this software is Silicon Laboratories Inc. Your use of this
 * software is governed by the terms of Silicon Labs Master Software License
 * Agreement (MSLA) available at
 * www.silabs.com/about-us/legal/master-software-license-agreement. This
 * software is distributed to you in Source Code format and is governed by the
 * sections of the MSLA applicable to Source Code.
 *
 ******************************************************************************/

// <<< Use Configuration Wizard in Context Menu >>>

// <h>Zigbee Permit Join Manager configuration

// <o EMBER_AF_PLUGIN_PERMIT_JOIN_MANAGER_PERMIT_JOINS_DURATION> Permit Joining Duration (seconds) <1-255>
// <i> Default: 40
// <i> Indicates the duration of permit joining in seconds. The value of 255 indicates permit joining forever.
#define EMBER_AF_PLUGIN_PERMIT_JOIN_MANAGER_PERMIT_JOINS_DURATION   40

// <o EMBER_AF_PLUGIN_PERMIT_JOIN_MANAGER_JOINING_DEVICES_QUEUE_LENGTH> Joining Devices Queue length <2-10>
// <i> Default: 4
// <i> Indicates the length of the queue that maintains a list of simultaneous joining devices to unicast Permit Join messages to.
#define EMBER_AF_PLUGIN_PERMIT_JOIN_MANAGER_JOINING_DEVICES_QUEUE_LENGTH   4

// <o EMBER_AF_PLUGIN_PERMIT_JOIN_MANAGER_DEVICE_ANNOUNCE_TIMEOUT> Device Announce timeout (milliseconds) <500-2000>
// <i> Default: 1000
// <i> Indicates the amount of time in milliseconds that we wait for a device announce before unicasting a permit Joins message.
#define EMBER_AF_PLUGIN_PERMIT_JOIN_MANAGER_DEVICE_ANNOUNCE_TIMEOUT   1000

// </h>

// <<< end of configuration section >>>
