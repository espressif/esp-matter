/***************************************************************************//**
 * @brief Zigbee Poll Control Server Cluster component configuration header.
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

// <h>Zigbee Poll Control Server Cluster configuration

// <o EMBER_AF_PLUGIN_POLL_CONTROL_SERVER_MAX_CLIENTS> Maximum Clients Supported <1-255>
// <i> Default: 4
// <i> Maximum number of clients supported
#define EMBER_AF_PLUGIN_POLL_CONTROL_SERVER_MAX_CLIENTS   4

// <o EMBER_AF_PLUGIN_POLL_CONTROL_SERVER_CHECK_IN_RESPONSE_TIMEOUT> Check-in timeout duration (in QS) <1-255>
// <i> Default: 32
// <i> The amount of time, in quarter seconds, the plugin will wait for Check-In Response commands from Poll Control clients.
#define EMBER_AF_PLUGIN_POLL_CONTROL_SERVER_CHECK_IN_RESPONSE_TIMEOUT   32

// <q EMBER_AF_PLUGIN_POLL_CONTROL_SERVER_ACCEPT_SET_LONG_POLL_INTERVAL_COMMAND> Accept Set Long Poll Interval commands
// <i> Default: TRUE
// <i> Indicates whether this device will process the optional Set Long Poll Interval command from Poll Control clients.  The Set Long Poll Interval command allows clients to remotely change the long poll interval of this device.
#define EMBER_AF_PLUGIN_POLL_CONTROL_SERVER_ACCEPT_SET_LONG_POLL_INTERVAL_COMMAND   1

// <q EMBER_AF_PLUGIN_POLL_CONTROL_SERVER_ACCEPT_SET_SHORT_POLL_INTERVAL_COMMAND> Accept Set Short Poll Interval commands
// <i> Default: TRUE
// <i> Indicates whether this device will process the optional Set Short Poll Interval command from Poll Control clients.  This Set Short Poll Interval command allows clients to remotely change the short poll interval of this device.
#define EMBER_AF_PLUGIN_POLL_CONTROL_SERVER_ACCEPT_SET_SHORT_POLL_INTERVAL_COMMAND   1

// <o EMBER_AF_PLUGIN_POLL_CONTROL_SERVER_TRUST_CENTER_CHECK_IN_FAILURE_THRESHOLD> Maximum poll control checkin failures <1-255>
// <i> Default: 3
// <i> The amount of poll control checkin failures with a trust center permitted before a callback is fired.
#define EMBER_AF_PLUGIN_POLL_CONTROL_SERVER_TRUST_CENTER_CHECK_IN_FAILURE_THRESHOLD   3

// </h>

// <<< end of configuration section >>>
