/***************************************************************************//**
 * @file sl_apploader_lib_api.h
 * @brief Bluetooth AppLoader library API
 *******************************************************************************
 * # License
 * <b>Copyright 2022 Silicon Laboratories Inc. www.silabs.com</b>
 *******************************************************************************
 *
 * The licensor of this software is Silicon Laboratories Inc.  Your use of this
 * software is governed by the terms of Silicon Labs Master Software License
 * Agreement (MSLA) available at
 * www.silabs.com/about-us/legal/master-software-license-agreement.  This
 * software is distributed to you in Source Code format and is governed by the
 * sections of the MSLA applicable to Source Code.
 *
 ******************************************************************************/

#ifndef SL_APPLOADER_LIB_API_H
#define SL_APPLOADER_LIB_API_H

#ifdef __cplusplus
extern "C" {
#endif

/***************************************************************************//**
 * @addtogroup apploader_lib
 * @{
 *
 * @brief AppLoader library
 *
 * The Apploader library encapsulates the functionality required to received
 * device firmware updates over Bluetooth. It is used by the AppLoader plugin
 * for the Bootloader component.
 *
 ******************************************************************************/

/** @} end apploader_lib */

#include <stdint.h>
#include <stdbool.h>
#include "sl_status.h"

typedef enum sl_apploader_address_type {
  sl_apploader_address_type_public = 0,
  sl_apploader_address_type_random = 1,
} sl_apploader_address_type_t;

typedef struct sl_apploader_address {
  uint8_t address[6];
  sl_apploader_address_type_t type : 1;
} sl_apploader_address_t;

typedef struct sl_apploader_config {
  int16_t txPower;   // (0 => 0 dBm, 15 = 1.5 dBm)
  sl_apploader_address_t *address;
} sl_apploader_config_t;

/**
 * Bluetooth initialisation and configuration functions
 */
void sl_apploader_init(const sl_apploader_config_t *config);
void sl_apploader_select_rf_path(uint8_t antenna);
void sl_apploader_set_database();
void sl_apploader_run();
void sl_apploader_get_device_address(sl_apploader_address_t *address);
void sl_apploader_get_static_random_device_address(sl_apploader_address_t *address);

/**
 * Bluetooth functions to control advertising and connections
 */
sl_status_t sl_apploader_set_advertising_data(const void *data, uint8_t length);
sl_status_t sl_apploader_set_scan_response_data(const void *data, uint8_t length);
sl_status_t sl_apploader_enable_advertising(bool enable);
sl_status_t sl_apploader_disconnect();

/**
 * GATT database functions
 */
void sl_apploader_reset_database();
void sl_apploader_database_add_gatt_service();
void sl_apploader_database_add_gap_service(const char *device_name, uint16_t device_name_len);
void sl_apploader_database_add_ota_service();

#ifdef __cplusplus
}
#endif

#endif // SL_APPLOADER_LIB_API_H
