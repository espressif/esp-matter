/***************************************************************************//**
 * @brief Zigbee Scenes Server Cluster component configuration header.
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

// <h>Zigbee Scenes Server Cluster configuration

// <o EMBER_AF_PLUGIN_SCENES_TABLE_SIZE> Scenes table size <1-255>
// <i> Default: 3
// <i> Maximum count of scenes across all endpoints
#define EMBER_AF_PLUGIN_SCENES_TABLE_SIZE   3

// <q EMBER_AF_PLUGIN_SCENES_NAME_SUPPORT> Support scene names
// <i> Default: FALSE
// <i> If this option is selected, the plugin will include support for scene names.  Scene names will require additional space in the scene table (16 characters plus a length byte per entry).  The Name Support attribute will be set based on this option.
#define EMBER_AF_PLUGIN_SCENES_NAME_SUPPORT   0

// <q EMBER_AF_PLUGIN_SCENES_USE_TOKENS> On SOC platform, store the table in persistent memory
// <i> Default: TRUE
// <i> On an SOC platform, this option enables the persistent storage of the scenes table into the FLASH memory using the tokens.
#define EMBER_AF_PLUGIN_SCENES_USE_TOKENS   1

// </h>

// <<< end of configuration section >>>
