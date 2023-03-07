/***************************************************************************//**
 * @brief Zigbee Stack Diagnostics component configuration header.
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

// <h>Zigbee Stack Diagnostics configuration

// <q EMBER_AF_PLUGIN_STACK_DIAGNOSTICS_RECORD_LQI_RSSI> (SoC only) Record LQI and RSSI for each neighbor
// <i> Default: FALSE
// <i> (SoC only) Record the last LQI and RSSI values received for each neighbor. Note: when this option is selected, the Packet Handoff plugin must be enabled and the emberAfIncomingPacketFilterCallback callback is consumed.
#define EMBER_AF_PLUGIN_STACK_DIAGNOSTICS_RECORD_LQI_RSSI   0

// </h>

// <<< end of configuration section >>>
