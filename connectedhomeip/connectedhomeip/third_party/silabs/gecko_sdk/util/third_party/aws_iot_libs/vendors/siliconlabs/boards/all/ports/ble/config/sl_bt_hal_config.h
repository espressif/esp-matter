/***************************************************************************//**
 * @file
 * @brief Configuration for Silicon Labs implementation of FreeRTOS BLE library.
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

#include "sl_bt_api_compatibility.h"

#ifndef SL_BT_HAL_CONFIG_H
#define SL_BT_HAL_CONFIG_H

// <<< Use Configuration Wizard in Context Menu >>>

// <h> Silicon Labs Amazon-FreeRTOS BLE HAL configuration

// <o SL_BT_HAL_MAX_BLE_APPS> Maximum number of applications that can be concurrently
// registered with pxRegisterBleApp() function in BTBleAdapter_t. <1-255>
// <i> Default: 5
#ifndef SL_BT_HAL_MAX_BLE_APPS
#define SL_BT_HAL_MAX_BLE_APPS 5
#endif

// <o SL_BT_HAL_MAX_GATT_CLIENTS> Maximum number of clients that can be concurrently
// registered with pxRegisterClient() function in BTGattClientInterface_t. <1-255>
// <i> Default: 1
#ifndef SL_BT_HAL_MAX_GATT_CLIENTS
#define SL_BT_HAL_MAX_GATT_CLIENTS 1
#endif

// <o SL_BT_HAL_DEFAULT_IO_CAPABILITIES> Default IO capabilities
// <sm_io_capability_displayonly=> Display only
// <sm_io_capability_displayyesno=> Display Yes/No
// <sm_io_capability_keyboardonly=> Keyboard only
// <sm_io_capability_noinputnooutput=> No input, no output
// <sm_io_capability_keyboarddisplay=> Keyboard and display
// <i> Default: sm_io_capability_noinputnooutput
#define SL_BT_HAL_DEFAULT_IO_CAPABILITIES	sm_io_capability_noinputnooutput

// <h> Security Manager Default Configuration

// <q SL_BT_HAL_DEFAULT_SECURITY_CONFIG_BONDING_REQUIRES_MITM_PROTECTION> Require MITM protection
// <i> Default: 0
#define SL_BT_HAL_DEFAULT_SECURITY_CONFIG_BONDING_REQUIRES_MITM_PROTECTION	        0

// <q SL_BT_HAL_DEFAULT_SECURITY_CONFIG_ENCRYPTION_REQUIRES_BONDING> Encryption requires bonding
// <i> Default: 1
#define SL_BT_HAL_DEFAULT_SECURITY_CONFIG_ENCRYPTION_REQUIRES_BONDING	            1

// <q SL_BT_HAL_DEFAULT_SECURITY_CONFIG_SECURE_CONNECTIONS_ONLY> Secure connections only
// <i> Default: 1
#define SL_BT_HAL_DEFAULT_SECURITY_CONFIG_SECURE_CONNECTIONS_ONLY	                1

// <q SL_BT_HAL_DEFAULT_SECURITY_CONFIG_BONDING_REQUESTS_NEED_TO_BE_CONFIRMED> Bonding reqest requires confirmation
// <i> Default: 1
#define SL_BT_HAL_DEFAULT_SECURITY_CONFIG_BONDING_REQUESTS_NEED_TO_BE_CONFIRMED     1

// <q SL_BT_HAL_DEFAULT_SECURITY_CONFIG_CONNECTIONS_ONLY_FROM_BONDED_DEVICES> Connections from bonded devices only
// <i> Default: 1
#define SL_BT_HAL_DEFAULT_SECURITY_CONFIG_CONNECTIONS_ONLY_FROM_BONDED_DEVICES	    0

// </h> 

// </h> 

// <<< end of configuration section >>>

/*******************************************************************************
 *                              LOGGING
 ******************************************************************************/

/* No logging is in use in the default configuration. Define dummy macros. */

/* Tag to use in log prints */
#define SILABS_BLE_HAL ""

/* Log levels */
#define SILABS_BLE_LOG_ERROR 0
#define SILABS_BLE_LOG_WARN  1
#define SILABS_BLE_LOG_INFO  2
#define SILABS_BLE_LOG_DEBUG 3
#define SILABS_BLE_LOG_FATAL 4

/* Macro used for logging in the BLE HAL implementation */
#define SILABS_BLE_LOG(log_level, tag, fmt, ...)

#endif /* SL_BT_HAL_CONFIG_H */
