/***************************************************************************//**
 * @brief Zigbee DMP Tuning and Testing component configuration header.
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

// <h>Zigbee DMP Tuning and Testing configuration
#define EMBER_AF_PLUGIN_DMP_TUNING_PROFILE_CUSTOM     0
#define EMBER_AF_PLUGIN_DMP_TUNING_PROFILE_BLE        1
#define EMBER_AF_PLUGIN_DMP_TUNING_PROFILE_ZIGBEE     2
#define EMBER_AF_PLUGIN_DMP_TUNING_PROFILE_BALANCED   3

// <q EMBER_AF_PLUGIN_DMP_TUNING_PROFILE> DMP Tuning profile selection

// <EMBER_AF_PLUGIN_DMP_TUNING_PROFILE_BLE=> BLE Priority Profile
// <EMBER_AF_PLUGIN_DMP_TUNING_PROFILE_ZIGBEE=> Zigbee Priority Profile
// <EMBER_AF_PLUGIN_DMP_TUNING_PROFILE_BALANCED=> Balanced Priority Profile
// <EMBER_AF_PLUGIN_DMP_TUNING_PROFILE_CUSTOM=> Custom Configuration

// <i> Default: EMBER_AF_PLUGIN_DMP_TUNING_PROFILE_BLE

// <i> Zigbee DMP Tuning and Testing configuration
// <i> Certain networking stack configurations can be crucial for proper networking operation in multiprotocol use-cases,
// for example but not limited to transmission, reception, connection and advertisiement priorites.
// This plugin is to provide better access to such configuration options of the stacks, so that it
// helps in tuning and testing a DMP application. It provides three DMP configuration profiles for three
// major use-cases. These are BLE priority, zigbee priority and balanced priority. These are to provide
// priority for BLE over zigbee transmission and reception, to provide priority for zigbee over BLE
// connection, advertisement etc., or to provide a balanced priority configuration option respectively.
// The plugin also provides a way for users to manually configure the default value of these configuration
// options (plugin option inputs) or to change them run-time over the CLI.
#define EMBER_AF_PLUGIN_DMP_TUNING_PROFILE   EMBER_AF_PLUGIN_DMP_TUNING_PROFILE_BLE

//     <h>Options below only apply when profile = Custom

#if (EMBER_AF_PLUGIN_DMP_TUNING_PROFILE == EMBER_AF_PLUGIN_DMP_TUNING_PROFILE_CUSTOM)
// <o EMBER_AF_PLUGIN_DMP_TUNING_ZB_ACTIVE_TX_PRIORITY> Zigbee Active TX Priority <0-255>
// <i> Default: 100
// <i> Zigbee Active TX Priority
#define EMBER_AF_PLUGIN_DMP_TUNING_ZB_ACTIVE_TX_PRIORITY   100

// <o EMBER_AF_PLUGIN_DMP_TUNING_ZB_ACTIVE_RX_PRIORITY> Zigbee Active RX Priority <0-255>
// <i> Default: 255
// <i> Zigbee Active RX Priority
#define EMBER_AF_PLUGIN_DMP_TUNING_ZB_ACTIVE_RX_PRIORITY   255

// <o EMBER_AF_PLUGIN_DMP_TUNING_ZB_BACKGROUND_RX_PRIORIY> Zigbee Background RX Priority <0-255>
// <i> Default: 255
// <i> Zigbee Background RX Priority
#define EMBER_AF_PLUGIN_DMP_TUNING_ZB_BACKGROUND_RX_PRIORIY   255

// <o EMBER_AF_PLUGIN_DMP_TUNING_BLE_ADVERTISEMENT_PRIORITY_MIN> BLE Advertisement Priority - Min <0-255>
// <i> Default: 175
// <i> BLE Advertisement Priority - Min
#define EMBER_AF_PLUGIN_DMP_TUNING_BLE_ADVERTISEMENT_PRIORITY_MIN   175

// <o EMBER_AF_PLUGIN_DMP_TUNING_BLE_ADVERTISEMENT_PRIORITY_MAX> BLE Advertisement Priority - Max <0-255>
// <i> Default: 127
// <i> BLE Advertisement Priority - Max
#define EMBER_AF_PLUGIN_DMP_TUNING_BLE_ADVERTISEMENT_PRIORITY_MAX   127

// <o EMBER_AF_PLUGIN_DMP_TUNING_BLE_SCAN_PRIORITY_MIN> BLE Scan Priority - Min <0-255>
// <i> Default: 191
// <i> BLE Scan Priority - Min
#define EMBER_AF_PLUGIN_DMP_TUNING_BLE_SCAN_PRIORITY_MIN   191

// <o EMBER_AF_PLUGIN_DMP_TUNING_BLE_SCAN_PRIORITY_MAX> BLE Scan Priority - Max <0-255>
// <i> Default: 143
// <i> BLE Scan Priority - Max
#define EMBER_AF_PLUGIN_DMP_TUNING_BLE_SCAN_PRIORITY_MAX   143

// <o EMBER_AF_PLUGIN_DMP_TUNING_BLE_CONNECTION_INIT_PRIORITY_MIN> BLE Connection Init Priority - Min <0-255>
// <i> Default: 55
// <i> BLE Connection Init Priority - Min
#define EMBER_AF_PLUGIN_DMP_TUNING_BLE_CONNECTION_INIT_PRIORITY_MIN   55

// <o EMBER_AF_PLUGIN_DMP_TUNING_BLE_CONNECTION_INIT_PRIORITY_MAX> BLE Connection Init Priority - Max <0-255>
// <i> Default: 15
// <i> BLE Connection Init Priority - Max
#define EMBER_AF_PLUGIN_DMP_TUNING_BLE_CONNECTION_INIT_PRIORITY_MAX   15

// <o EMBER_AF_PLUGIN_DMP_TUNING_BLE_CONNECTION_PRIORITY_MIN> BLE Connection Priority - Min <0-255>
// <i> Default: 135
// <i> BLE Connection Priority - Min
#define EMBER_AF_PLUGIN_DMP_TUNING_BLE_CONNECTION_PRIORITY_MIN   135

// <o EMBER_AF_PLUGIN_DMP_TUNING_BLE_CONNECTION_PRIORITY_MAX> BLE Connection Priority - Max <0-255>
// <i> Default: 0
// <i> BLE Connection Priority - Max
#define EMBER_AF_PLUGIN_DMP_TUNING_BLE_CONNECTION_PRIORITY_MAX   0

// <o EMBER_AF_PLUGIN_DMP_TUNING_BLE_RAIL_MAPPING_OFFSET> BLE RAIL Pririty Mapping Offset <0-255>
// <i> Default: 16
// <i> BLE RAIL Pririty Mapping Offset
#define EMBER_AF_PLUGIN_DMP_TUNING_BLE_RAIL_MAPPING_OFFSET   16

// <o EMBER_AF_PLUGIN_DMP_TUNING_BLE_RAIL_MAPPING_RANGE> BLE RAIL Pririty Mapping Range <0-255>
// <i> Default: 16
// <i> BLE RAIL Pririty Mapping Range
#define EMBER_AF_PLUGIN_DMP_TUNING_BLE_RAIL_MAPPING_RANGE   16
//     </h>
// </h>
#endif
// <<< end of configuration section >>>
