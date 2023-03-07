/***************************************************************************//**
 * @brief Zigbee GBCS Device Log component configuration header.
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

// <h>Zigbee GBCS Device Log configuration

// <o EMBER_AF_PLUGIN_GBCS_DEVICE_LOG_DEVICE_LOG_SIZE> Device log size <16-32>
// <i> Default: 16
// <i> Maximum number of devices stored in the device log.
#define EMBER_AF_PLUGIN_GBCS_DEVICE_LOG_DEVICE_LOG_SIZE   16

// <o EMBER_AF_PLUGIN_GBCS_DEVICE_LOG_DEVICE1_TYPE> Device1 Type
// <i> Device1 Type
// <GSME=> GSME
// <ESME=> ESME
// <CHF=> CHF
// <GPF=> GPF
// <HCALCS=> HCALCS
// <PPMID=> PPMID
// <TYPE2=> TYPE2
// <i> Default: GSME
// <i> The device type of the remote device
#define EMBER_AF_PLUGIN_GBCS_DEVICE_LOG_DEVICE1_TYPE   GSME

// The EUI64 address of a remote device that is allowed to communicate with this device.
//  A value of all 0's will be ignored.
// <a.8 EMBER_AF_PLUGIN_GBCS_DEVICE_LOG_DEVICE1_EUI> Device1 EUI64 <0..255> <f.h>
// <d> { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 }
#define EMBER_AF_PLUGIN_GBCS_DEVICE_LOG_DEVICE1_EUI    { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 }

// <o EMBER_AF_PLUGIN_GBCS_DEVICE_LOG_DEVICE2_TYPE> Device2 Type
// <i> Device2 Type
// <GSME=> GSME
// <ESME=> ESME
// <CHF=> CHF
// <GPF=> GPF
// <HCALCS=> HCALCS
// <PPMID=> PPMID
// <TYPE2=> TYPE2
// <i> Default: GSME
// <i> The device type of the remote device
#define EMBER_AF_PLUGIN_GBCS_DEVICE_LOG_DEVICE2_TYPE   GSME

// The EUI64 address of a remote device that is allowed to communicate with this device.
//  A value of all 0's will be ignored.
// <a.8 EMBER_AF_PLUGIN_GBCS_DEVICE_LOG_DEVICE2_EUI> Device2 EUI64 <0..255> <f.h>
// <d> { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 }
#define EMBER_AF_PLUGIN_GBCS_DEVICE_LOG_DEVICE2_EUI    { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 }

// <o EMBER_AF_PLUGIN_GBCS_DEVICE_LOG_DEVICE3_TYPE> Device3 Type
// <i> Device3 Type
// <GSME=> GSME
// <ESME=> ESME
// <CHF=> CHF
// <GPF=> GPF
// <HCALCS=> HCALCS
// <PPMID=> PPMID
// <TYPE2=> TYPE2
// <i> Default: GSME
// <i> The device type of the remote device
#define EMBER_AF_PLUGIN_GBCS_DEVICE_LOG_DEVICE3_TYPE   GSME

// The EUI64 address of a remote device that is allowed to communicate with this device.
//  A value of all 0's will be ignored.
// <a.8 EMBER_AF_PLUGIN_GBCS_DEVICE_LOG_DEVICE3_EUI> Device3 EUI64 <0..255> <f.h>
// <d> { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 }
#define EMBER_AF_PLUGIN_GBCS_DEVICE_LOG_DEVICE3_EUI    { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 }

// <o EMBER_AF_PLUGIN_GBCS_DEVICE_LOG_DEVICE4_TYPE> Device4 Type
// <i> Device4 Type
// <GSME=> GSME
// <ESME=> ESME
// <CHF=> CHF
// <GPF=> GPF
// <HCALCS=> HCALCS
// <PPMID=> PPMID
// <TYPE2=> TYPE2
// <i> Default: GSME
// <i> The device type of the remote device
#define EMBER_AF_PLUGIN_GBCS_DEVICE_LOG_DEVICE4_TYPE   GSME

// The EUI64 address of a remote device that is allowed to communicate with this device.
//  A value of all 0's will be ignored.
// <a.8 EMBER_AF_PLUGIN_GBCS_DEVICE_LOG_DEVICE4_EUI> Device4 EUI64 <0..255> <f.h>
// <d> { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 }
#define EMBER_AF_PLUGIN_GBCS_DEVICE_LOG_DEVICE4_EUI    { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 }

// </h>

// <<< end of configuration section >>>
