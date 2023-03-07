/***************************************************************************//**
 * @brief Zigbee GBCS Gas Smart Metering Equipment (GSME) component configuration header.
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

// <h>Zigbee GBCS Gas Smart Metering Equipment (GSME) configuration

// <o EMBER_AF_PLUGIN_GBCS_GAS_METER_GSME_ENDPOINT> GSME Endpoint <1-255>
// <i> Default: 1
// <i> This is a local endpoint representing the GSME.
#define EMBER_AF_PLUGIN_GBCS_GAS_METER_GSME_ENDPOINT   1

// <o EMBER_AF_PLUGIN_GBCS_GAS_METER_CHF_ENDPOINT> CHF Remote CommsHub Endpoint <1-255>
// <i> Default: 1
// <i> This is the remote endpoint representing the Remote CommsHub which is part of the CHF.
#define EMBER_AF_PLUGIN_GBCS_GAS_METER_CHF_ENDPOINT   1

// <o EMBER_AF_PLUGIN_GBCS_GAS_METER_REPORT_INTERVAL> Meter Report Interval <1-65535>
// <i> Default: 20
// <i> The GBCS spec recommends this value at 30 minutes (1800 seconds).  For testing purposes we default to 20 seconds.
#define EMBER_AF_PLUGIN_GBCS_GAS_METER_REPORT_INTERVAL   20

// <o EMBER_AF_PLUGIN_GBCS_GAS_METER_GBZ_OFFSET> GBZ Offset <0-255>
// <i> Default: 8
// <i> Offset from the begin of a tunneled remote party message where the GBZ packet begins
#define EMBER_AF_PLUGIN_GBCS_GAS_METER_GBZ_OFFSET   8

// </h>

// <<< end of configuration section >>>
