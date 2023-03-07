/***************************************************************************//**
 * @brief Zigbee MAC Address Filtering component configuration header.
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

// <h>Zigbee MAC Address Filtering configuration

// <o EMBER_AF_PLUGIN_MAC_ADDRESS_FILTERING_SHORT_ADDRESS_FILTER_LENGTH> Short Address Filter Length <1-10>
// <i> Default: 10
// <i> The length of the list used for filtering short addresses.  This may be configured at runtime with specific addresses, and whether the list is a whitelist or blacklist.
#define EMBER_AF_PLUGIN_MAC_ADDRESS_FILTERING_SHORT_ADDRESS_FILTER_LENGTH   10

// <o EMBER_AF_PLUGIN_MAC_ADDRESS_FILTERING_LONG_ADDRESS_FILTER_LENGTH> Long Address Filter Length <1-10>
// <i> Default: 10
// <i> The length of the list used for filtering long addresses.  This may be configured at runtime with specific addresses, and whether the list is a whitelist or blacklist.
#define EMBER_AF_PLUGIN_MAC_ADDRESS_FILTERING_LONG_ADDRESS_FILTER_LENGTH   10

// <o EMBER_AF_PLUGIN_MAC_ADDRESS_FILTERING_PAN_ID_FILTER_LENGTH> PAN ID Filter Length <1-10>
// <i> Default: 10
// <i> The length of the list used for filtering PAN IDs.  This may be configured at runtime with specific PANs, and whether the list is a whitelist or blaclist.
#define EMBER_AF_PLUGIN_MAC_ADDRESS_FILTERING_PAN_ID_FILTER_LENGTH   10

// </h>

// <<< end of configuration section >>>
