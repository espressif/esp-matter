/***************************************************************************//**
 * @brief Zigbee Network Find (Sub-GHz) component configuration header.
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

// TODO: EMZIGBEE-6962

// <<< Use Configuration Wizard in Context Menu >>>

// <h>Zigbee Network Find (Sub-GHz) configuration

// <o EMBER_AF_PLUGIN_NETWORK_FIND_SUB_GHZ_CUT_OFF_VALUE> Cut-off value (dBm) <-128..127>
// <i> Default: -48
// <i> The maximum noise allowed on a channel to consider for forming a network. For more details, see the "Cut-off value" and "Enable scanning all channels" options in the Network Find plugin.
#define EMBER_AF_PLUGIN_NETWORK_FIND_SUB_GHZ_CUT_OFF_VALUE   -48

// <o EMBER_AF_PLUGIN_NETWORK_FIND_SUB_GHZ_RADIO_TX_POWER> Sub-GHz radio output power <-30..20>
// <i> Default: 3
// <i> The radio output power (in dBm) at which the node will operate. Enabling getting the power from a callback in the Network Find plugin overrides this option.
#define EMBER_AF_PLUGIN_NETWORK_FIND_SUB_GHZ_RADIO_TX_POWER   3

#define EMBER_AF_PLUGIN_NETWORK_FIND_SUB_GHZ_CHANNEL_MASK     0x07FFFFFFUL

#define EMBER_AF_PLUGIN_NETWORK_FIND_SUB_GHZ_ALL_CHANNEL_MASK 0x07FFFFFFUL

// </h>

// <<< end of configuration section >>>
