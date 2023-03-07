/***************************************************************************//**
 * @brief Zigbee End Device Support component configuration header.
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

// <h>Zigbee End Device Support configuration

// <o EMBER_AF_PLUGIN_END_DEVICE_SUPPORT_SHORT_POLL_INTERVAL_SECONDS> Short Poll Interval (seconds) <1-65535>
// <i> Default: 1
// <i> The amount of time (in seconds) the device will wait between MAC data polls when it is expecting data.
#define EMBER_AF_PLUGIN_END_DEVICE_SUPPORT_SHORT_POLL_INTERVAL_SECONDS   1

// <o EMBER_AF_PLUGIN_END_DEVICE_SUPPORT_LONG_POLL_INTERVAL_SECONDS> Long Poll Interval (seconds) <1-65535>
// <i> Default: 300
// <i> The amount of time (in seconds) the device will wait between MAC data polls to ensure connectivity with its parent.
#define EMBER_AF_PLUGIN_END_DEVICE_SUPPORT_LONG_POLL_INTERVAL_SECONDS   300

// <o EMBER_AF_PLUGIN_END_DEVICE_SUPPORT_WAKE_TIMEOUT_SECONDS> Wake Timeout (seconds) <1-65535>
// <i> Default: 3
// <i> The amount of time (in seconds) the device will wait for a task in the wake bitmask to finish.  While waiting, the device will short poll.
#define EMBER_AF_PLUGIN_END_DEVICE_SUPPORT_WAKE_TIMEOUT_SECONDS   3

// <o EMBER_AF_PLUGIN_END_DEVICE_SUPPORT_WAKE_TIMEOUT_BITMASK> Wake Timeout Bitmask <0-4294967295>
// <i> Default: 0x00000018
// <i> The bitmask that determines which tasks will timeout automatically and which tasks require manual removal from the task list.  The default value corresponds to EMBER_AF_WAITING_FOR_ZDO_RESPONSE (0x00000008) and EMBER_AF_WAITING_FOR_ZCL_RESPONSE (0x00000010).  See EmberAfApplicationTask for more information.
#define EMBER_AF_PLUGIN_END_DEVICE_SUPPORT_WAKE_TIMEOUT_BITMASK   0x00000018

// <o EMBER_AF_PLUGIN_END_DEVICE_SUPPORT_MAX_MISSED_POLLS> Max Number of Missed Polls <1-255>
// <i> Default: 3
// <i> The maximum number of missed MAC data polls that will trigger a move (rejoin).
#define EMBER_AF_PLUGIN_END_DEVICE_SUPPORT_MAX_MISSED_POLLS   3

// <o EMBER_AF_REJOIN_ATTEMPTS_MAX> Max Number of Sequential Rejoin (move) attempts <0-255>
// <i> Default: 3
// <i> This is the maximum number of sequential attempts that will be made to rejoin the ZigBee network and find a new parent the device will make before giving up.  If this is set to 255, then the device will never stop attempting to rejoin when it has lost its parent.  A successful rejoin resets the number of attempts to 0.
#define EMBER_AF_REJOIN_ATTEMPTS_MAX   3

// <q EMBER_AF_PLUGIN_END_DEVICE_SUPPORT_ENABLE_POLL_COMPLETED_CALLBACK> Enable Poll Completed Callback
// <i> Default: FALSE
// <i> This option enables a callback function when a poll is completed. Enabling this option on hosts will result in increased traffic between the host and the NCP.
#define EMBER_AF_PLUGIN_END_DEVICE_SUPPORT_ENABLE_POLL_COMPLETED_CALLBACK   0

// <q EMBER_AF_PLUGIN_END_DEVICE_SUPPORT_ALLOW_REJOINS_WITH_WELL_KNOWN_LINK_KEY> Attempt Rejoins using a well-known Trust Center Link Key (NOT RECOMMENDED, NOT SECURE)
// <i> Default: FALSE
// <i> This option will allow Trust Center Rejoin (unsecure rejoin) attempts when the device is using a well-known key, such as "ZigBeeAlliance09".  This exposes a potential security hole on the Trust Center side but is an option for backward compatibility with networks that need it. Note that the Trust Center may have its own policy for handling insecure rejoins and may choose to not send the network key to the rejoining device if it is using a well known link key.
#define EMBER_AF_PLUGIN_END_DEVICE_SUPPORT_ALLOW_REJOINS_WITH_WELL_KNOWN_LINK_KEY   0

// </h>

// <<< end of configuration section >>>
