/***************************************************************************//**
 * @file
 * @brief
 *******************************************************************************
 * # License
 * <b>Copyright 2021 Silicon Laboratories Inc. www.silabs.com</b>
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

// <h>Zigbee Green Power Device Application Configuration

// <o EMBER_AF_PLUGIN_APPS_APP_DEVICE_ID> GPD Device Id <0-255>
// <i> Default: 2
// <i> The device id for type of green power device
#define EMBER_AF_PLUGIN_APPS_APP_DEVICE_ID   2

// <o EMBER_AF_PLUGIN_APPS_APPLICATION_ID> GPD Application Id <0-2>
// <i> Default: 0
// <i> The application id of green power device
#define EMBER_AF_PLUGIN_APPS_APPLICATION_ID   0

// <o EMBER_AF_PLUGIN_APPS_SRC_ID> GPD Source Id
// <i> Default: 0x12345678
// <i> Source Id for the GPD
#define EMBER_AF_PLUGIN_APPS_SRC_ID 0x12345678

// <o EMBER_AF_PLUGIN_APPS_GPD_ENDPOINT> GPD Endpoint <1-239>
// <i> Default: 99
// <i> GPD endpoint to be used with IEEE addressing
#define EMBER_AF_PLUGIN_APPS_GPD_ENDPOINT 99

// <o EMBER_AF_PLUGIN_APPS_MAC_SEQ> GPD MAC Seq Capability <0-1>
// <i> Default: 1
// <i> GPD MAC Sequenec capability : 0 - RANDOM - 1 INCREMENTAL
#define EMBER_AF_PLUGIN_APPS_MAC_SEQ   1

// <o EMBER_AF_PLUGIN_APPS_BIDIR_CAPABLE> GPD BiDirectional Capability <0-1>
// <i> Default: 1
// <i> GPD BiDirection Capability : true/false
#define EMBER_AF_PLUGIN_APPS_BIDIR_CAPABLE 1

// <o EMBER_AF_PLUGIN_APPS_RX_OFFSET> GPD BiDirectional Rx Offset <18-255>
// <i> Default: 20
// <i> GPD BiDirectional Rx Offset in msec.
#define EMBER_AF_PLUGIN_APPS_RX_OFFSET 20

// <o EMBER_AF_PLUGIN_APPS_RX_WINDOW> GGPD BiDirectional Rx Window <5-255>
// <i> Default: 80
// <i> GPD BiDirectional Rx Window in msec.
#define EMBER_AF_PLUGIN_APPS_RX_WINDOW 80

// <o EMBER_AF_PLUGIN_APPS_RX_CAPABILITY> GPD Rx capability in Operational Mode <0-1>
// <i> Default: 1
// <i> GPD Rx capability in Operational Mode.
#define EMBER_AF_PLUGIN_APPS_RX_CAPABILITY 1

// <o EMBER_AF_PLUGIN_APPS_PAN_ID_REQUEST> GPD PanId request flag <0-1>
// <i> Default: 0
// <i> GPD PanId request flag.
#define EMBER_AF_PLUGIN_APPS_PAN_ID_REQUEST 0

// <o EMBER_AF_PLUGIN_APPS_SECURITY_LEVEL> GPD Security Level <0-3>
// <i> Default: 3
// <i> GPD Security Level.
#define EMBER_AF_PLUGIN_APPS_SECURITY_LEVEL 3

// <o EMBER_AF_PLUGIN_APPS_SECURITY_KEY_TYPE> GPD Security Key Type <0-7>
// <i> Default: 4
// <i> GPD Security Key Type.
#define EMBER_AF_PLUGIN_APPS_SECURITY_KEY_TYPE 4

// <o EMBER_AF_PLUGIN_APPS_KEY_REQUEST> GPD Security Key Request <0-1>
// <i> Default: 1
// <i> GPD Security Key Request flag true/false.
#define EMBER_AF_PLUGIN_APPS_KEY_REQUEST 1

// <o EMBER_AF_PLUGIN_APPS_KEY_ENCRYPT> GPD Security Key Encryption <0-1>
// <i> Default: 1
// <i> GPD Security Key Encryption true/false.
#define EMBER_AF_PLUGIN_APPS_KEY_ENCRYPT 1

// <o EMBER_AF_PLUGIN_APPS_FIXED_LOCATION> GPD Fixed Location Flag <0-1>
// <i> Default: 1
// <i> GPD Fixed Location Flag true/false.
#define EMBER_AF_PLUGIN_APPS_FIXED_LOCATION 1

// <o EMBER_AF_PLUGIN_APPS_APP_INFO> GPD Application Information <0-1>
// <i> Default: 0
// <i> GPD Application Information true/false.
#define EMBER_AF_PLUGIN_APPS_APP_INFO 0

#if EMBER_AF_PLUGIN_APPS_APP_INFO

#define EMBER_AF_PLUGIN_APPS_GPD_APP_DESCRIPTION_FOLLOWS

// <o EMBER_AF_PLUGIN_APPS_APPS_GPD_MANUFACTURE_ID_PRESENT> ManufactureId presence <0-1>
// <i> Default: 0
// <i> ManufactureId presence true/false.
#define EMBER_AF_PLUGIN_APPS_APPS_GPD_MANUFACTURE_ID_PRESENT 0

#if EMBER_AF_PLUGIN_APPS_APPS_GPD_MANUFACTURE_ID_PRESENT
// <o EMBER_AF_PLUGIN_APPS_APPS_GPD_MANUFACTURE_ID>GPD Manufacture Id <0-65535>
// <i> Default: 4098
// <i> GPD Manufacture Id.
#define EMBER_AF_PLUGIN_APPS_APPS_GPD_MANUFACTURE_ID 4098
#endif //EMBER_AF_PLUGIN_APPS_APPS_GPD_MANUFACTURE_ID_PRESENT

// <o EMBER_AF_PLUGIN_APPS_APPS_GPD_MODEL_ID_PRESENT>Model Id presence<0-1>
// <i> Default: 0
// <i> Model Id presence
#define EMBER_AF_PLUGIN_APPS_APPS_GPD_MODEL_ID_PRESENT 0

#if EMBER_AF_PLUGIN_APPS_APPS_GPD_MODEL_ID_PRESENT
// <o EMBER_AF_PLUGIN_APPS_APPS_GPD_MODEL_ID>GPD ModelId<0-65535>
// <i> Default: 0
// <i> GPD ModelId
#define EMBER_AF_PLUGIN_APPS_APPS_GPD_MODEL_ID 0
#endif //EMBER_AF_PLUGIN_APPS_APPS_GPD_MODEL_ID_PRESENT

#endif //EMBER_AF_PLUGIN_APPS_APP_INFO

// <o EMBER_AF_PLUGIN_APPS_GPD_APP_NUMBER_OF_CHANNEL_REQUEST_PER_CHANNEL_WITH_AC_SET>GPD Application Number of Channel Requests per channel with Autocommissioning set <1-10>
// <i> Default: 4
// <i> GPD Application Number of Channel Requests per channel with Autocommissioning set
#define EMBER_AF_PLUGIN_APPS_GPD_APP_NUMBER_OF_CHANNEL_REQUEST_PER_CHANNEL_WITH_AC_SET 4

// <o EMBER_AF_PLUGIN_APPS_GPD_APP_NUMBER_OF_CHANNEL_REQUEST_PER_CHANNEL>GPD Application Number of Channel Requests per channel <1-10>
// <i> Default: 4
// <i> GPD Application Number of Channel Requests per channel
#define EMBER_AF_PLUGIN_APPS_GPD_APP_NUMBER_OF_CHANNEL_REQUEST_PER_CHANNEL 4

// <o EMBER_AF_PLUGIN_APPS_GPD_APP_NUMBER_OF_COMMISSIONING_REQUEST>GPD Application number of commissioning requests <1-10>
// <i> Default: 4
// <i> GPD Application number of commissioning requests
#define EMBER_AF_PLUGIN_APPS_GPD_APP_NUMBER_OF_COMMISSIONING_REQUEST 4

// <a.16 EMBER_AF_PLUGIN_APPS_APP_CHANNEL_SET> GPD Application Channel Set <0..255> <f.h>
// <d> { 11, 15, 20, 24, 25, 12, 13, 14, 16, 17, 18, 19, 21, 22, 23, 26 }
#define EMBER_AF_PLUGIN_APPS_APP_CHANNEL_SET { 11, 15, 20, 24, 25, 12, 13, 14, 16, 17, 18, 19, 21, 22, 23, 26 }
#define EMBER_AF_PLUGIN_APPS_APP_CHANNEL_SET_LENGTH 16

// <a.8 EMBER_AF_PLUGIN_APPS_IEEE> GPD IEEE address <0..255> <f.h>
// <d> { 0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77, 0x88 }
#define EMBER_AF_PLUGIN_APPS_IEEE { 0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77, 0x88 }

// <a.16 EMBER_AF_PLUGIN_APPS_KEY> GPD Application Key <0..255> <f.h>
// <d> { 0x00, 0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77, 0x88, 0x99, 0xaa, 0xbb, 0xcc, 0xdd, 0xee, 0xff }
#define EMBER_AF_PLUGIN_APPS_KEY { 0x00, 0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77, 0x88, 0x99, 0xaa, 0xbb, 0xcc, 0xdd, 0xee, 0xff }
#define EMBER_AF_PLUGIN_APPS_KEY_LENGTH (16)

// </h>

// <<< end of configuration section >>>

// Following configuration can take variable length array that is not supported at the moment.
#ifndef EMBER_AF_PLUGIN_APPS_APPS_GPD_CMD_LIST
#define EMBER_AF_PLUGIN_APPS_APPS_GPD_CMD_LIST {}
#define EMBER_AF_PLUGIN_APPS_APPS_GPD_CMD_LIST_LENGTH (0)
#endif //EMBER_AF_PLUGIN_APPS_APPS_GPD_CMD_LIST

#ifndef EMBER_AF_PLUGIN_APPS_APPS_IN_CLUS_LIST
#define EMBER_AF_PLUGIN_APPS_APPS_IN_CLUS_LIST {}
#define EMBER_AF_PLUGIN_APPS_APPS_IN_CLUS_LIST_LENGTH (0)
#endif //EMBER_AF_PLUGIN_APPS_APPS_IN_CLUS_LIST

#ifndef EMBER_AF_PLUGIN_APPS_APPS_OUT_CLUS_LIST
#define EMBER_AF_PLUGIN_APPS_APPS_OUT_CLUS_LIST {}
#define EMBER_AF_PLUGIN_APPS_APPS_OUT_CLUS_LIST_LENGTH (0)
#endif //EMBER_AF_PLUGIN_APPS_APPS_OUT_CLUS_LIST
