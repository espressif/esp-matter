/***************************************************************************//**
 * @brief Zigbee Network Creator Security component configuration header.
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

// <h>Zigbee Network Creator Security configuration

// <o EMBER_AF_PLUGIN_NETWORK_CREATOR_SECURITY_NETWORK_OPEN_TIME_S> Network Open Time (seconds) <0-65535>
// <i> Default: 300
// <i> The time, in seconds, that the network will remain open.
#define EMBER_AF_PLUGIN_NETWORK_CREATOR_SECURITY_NETWORK_OPEN_TIME_S   300

// <q EMBER_AF_PLUGIN_NETWORK_CREATOR_SECURITY_ALLOW_HA_DEVICES_TO_STAY> Allow Home Automation Devices to remain on network (non-compliant)
// <i> Default: TRUE
// <i> Set this option to allow Home Automation devices to remain on the network after joining.
#define EMBER_AF_PLUGIN_NETWORK_CREATOR_SECURITY_ALLOW_HA_DEVICES_TO_STAY   1

// <q EMBER_AF_PLUGIN_NETWORK_CREATOR_SECURITY_BDB_JOIN_USES_INSTALL_CODE_KEY> Trust Center requires install code for joining
// <i> Default: FALSE
// <i> The TC will allow joining of a device through install code only. Applications that implement the emberAfPluginNetworkCreatorSecurityGetInstallCodeCallback callback will be given a chance to add an install code for a joining device during the join procedure.
#define EMBER_AF_PLUGIN_NETWORK_CREATOR_SECURITY_BDB_JOIN_USES_INSTALL_CODE_KEY   0

// <q EMBER_AF_PLUGIN_NETWORK_CREATOR_SECURITY_ALLOW_TC_REJOIN_WITH_WELL_KNOWN_KEY> Allow Trust Center rejoin with well known key
// <i> Default: FALSE
// <i> The TC will allow rejoining of a device using the well known key for the number of seconds mentioned in the timeout option below.
#define EMBER_AF_PLUGIN_NETWORK_CREATOR_SECURITY_ALLOW_TC_REJOIN_WITH_WELL_KNOWN_KEY   0

// <o EMBER_AF_PLUGIN_NETWORK_CREATOR_SECURITY_ALLOW_TC_REJOINS_USING_WELL_KNOWN_KEY_TIMEOUT_SEC> Allow Trust Center rejoin with well known key timeout (seconds) <0-65535>
// <i> Default: 0
// <i> The TC will allow rejoining of a device using the well known key for the seconds mentioned here unless it has been set to 0 in which case trust center rejoin with well known key will be allowed forever.
#define EMBER_AF_PLUGIN_NETWORK_CREATOR_SECURITY_ALLOW_TC_REJOINS_USING_WELL_KNOWN_KEY_TIMEOUT_SEC   0

// </h>

// <<< end of configuration section >>>
