/***************************************************************************//**
 * @brief ZigBee Light Link component configuration header.
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

// <h>Zigbee ZigBee Light Link Library configuration

// <o EMBER_ZLL_GROUP_ADDRESSES> Group addresses <0-255>
// <i> Default: 0
// <i> The number of unique group identifiers that this device requires.
#define EMBER_ZLL_GROUP_ADDRESSES   0

// <o EMBER_ZLL_RSSI_THRESHOLD> RSSI threshold <-128..127:1>
// <i> Default: -40
// <i> The RSSI threshold applied to incoming scan requests.  The stack will ignore scan requests with corrected RSSI values less than this threshold.
#define EMBER_ZLL_RSSI_THRESHOLD   -40

// <q EMBER_ZLL_APPLY_THRESHOLD_TO_ALL_INTERPANS> Apply RSSI threshold to all interpan messages
// <i> Default: FALSE
// <i> True if the RSSI threshold is to be applied to all incoming touchlink interpan messages. This is not required by the Zigbee 3.0 specifications, but may be useful for testing in a busy environment.
#define EMBER_ZLL_APPLY_THRESHOLD_TO_ALL_INTERPANS   0

// </h>

// <<< end of configuration section >>>
