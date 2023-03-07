/***************************************************************************//**
 * @file
 * @brief Thunderboard advertising
 *******************************************************************************
 * # License
 * <b>Copyright 2022 Silicon Laboratories Inc. www.silabs.com</b>
 *******************************************************************************
 *
 * SPDX-License-Identifier: Zlib
 *
 * The licensor of this software is Silicon Laboratories Inc.
 *
 * This software is provided 'as-is', without any express or implied
 * warranty. In no event will the authors be held liable for any damages
 * arising from the use of this software.
 *
 * Permission is granted to anyone to use this software for any purpose,
 * including commercial applications, and to alter it and redistribute it
 * freely, subject to the following restrictions:
 *
 * 1. The origin of this software must not be misrepresented; you must not
 *    claim that you wrote the original software. If you use this software
 *    in a product, an acknowledgment in the product documentation would be
 *    appreciated but is not required.
 * 2. Altered source versions must be plainly marked as such, and must not be
 *    misrepresented as being the original software.
 * 3. This notice may not be removed or altered from any source distribution.
 *
 ******************************************************************************/

#include <stdio.h>
#include "sl_bluetooth.h"
#include "gatt_db.h"
#include "sl_simple_timer.h"
#include "app_assert.h"
#include "board.h"
#include "advertise.h"

typedef enum {
  ADV_TYPE_IBEACON,
  ADV_TYPE_SCAN_RESPONSE
} adv_type_t;

// -----------------------------------------------------------------------------
// Configuration

#define ADV_ALTERNATE_TIME_MS 1000
#define ADV_TYPE_DEFAULT      ADV_TYPE_SCAN_RESPONSE

// -----------------------------------------------------------------------------
// Private macros

// -------------------------------
// Advertising flags (common)

#define ADVERTISE_FLAGS_LENGTH                      2
#define ADVERTISE_FLAGS_TYPE                        0x01

/** Bit mask for flags advertising data type. */
#define ADVERTISE_FLAGS_LE_LIMITED_DISCOVERABLE     0x01
#define ADVERTISE_FLAGS_LE_GENERAL_DISCOVERABLE     0x02
#define ADVERTISE_FLAGS_BR_EDR_NOT_SUPPORTED        0x04

// -------------------------------
// Scan Response

#define ADVERTISE_MANDATORY_DATA_LENGTH             5
#define ADVERTISE_MANDATORY_DATA_TYPE_MANUFACTURER  0xFF

#define ADVERTISE_COMPANY_ID                        0x0047 /* Silicon Labs */
#define ADVERTISE_FIRMWARE_ID                       0x0002

/** Complete local name. */
#define ADVERTISE_TYPE_LOCAL_NAME                   0x09
#define ADVERTISE_DEVICE_NAME_LEN_MAX               20
#define ADVERTISE_DEVICE_NAME_DEFAULT_PREFIX        "Thunderboard "
#define ADVERTISE_DEVICE_NAME_DEFAULT_SUFFIX        "#00000"
#define ADVERTISE_DEVICE_NAME_FORMAT_STRING         "#%05d"

// -------------------------------
// iBeacon

#define ADVERTISE_IBEACON_PREAMBLE           0x004C /* Company ID reserved */
#define ADVERTISE_IBEACON_TYPE               0x1502 /* Beacon Type */
#define ADVERTISE_IBEACON_MAJOR              0x0000 /* Beacon major number */
#define ADVERTISE_IBEACON_MINOR              0x0000 /* Beacon minor number */

/** The Beacon's measured RSSI at 1 meter distance in dBm. */
#define ADVERTISE_IBEACON_RSSI               0xC3

/** Length of Manufacturer specific data field. */
#define ADVERTISE_IBEACON_MAN_SPEC_DATA_LEN  26

/** Universally Unique ID used in the Beacon.
 *  128-bit long ID. */
#define ADVERTISE_IBEACON_UUID                      \
  { 0xCE, 0xF7, 0x97, 0xDA, 0x2E, 0x91, 0x4E, 0xA4, \
    0xA4, 0x24, 0xF4, 0x50, 0x82, 0xAC, 0x06, 0x82 }

/** Helper macro */
#define UINT16_TO_BYTES(x) { (uint8_t)(x), (uint8_t)((x) >> 8) }

// -------------------------------
// Structure that holds iBeacon data

typedef struct {
  uint8_t flags_length;          /**< Length of the Flags field. */
  uint8_t flags_type;            /**< Type of the Flags field. */
  uint8_t flags;                 /**< Flags field. */
  uint8_t mandatory_data_length; /**< Length of the Manufacturer Data field. */
  uint8_t mandatory_data_type;   /**< Type of the Manufacturer Data field. */
  uint8_t company_id[2];         /**< Company ID field. */
  uint8_t beacon_type[2];        /**< Beacon Type field. */
  uint8_t uuid[16];              /**< 128-bit Universally Unique Identifier. */
  uint8_t major[2];              /**< Beacon major number. */
  uint8_t minor[2];              /**< Beacon minor number. */
  uint8_t rssi;                  /**< The Beacon's measured RSSI at 1 meter distance in dBm. */
} advertise_ibeacon_t;

#define ADVERTISE_IBEACON_DATA_DEFAULT                                    \
  {                                                                       \
    .flags_length          = ADVERTISE_FLAGS_LENGTH,                      \
    .flags_type            = ADVERTISE_FLAGS_TYPE,                        \
    .flags                 = ADVERTISE_FLAGS_LE_GENERAL_DISCOVERABLE      \
                             | ADVERTISE_FLAGS_BR_EDR_NOT_SUPPORTED,      \
    .mandatory_data_length = ADVERTISE_IBEACON_MAN_SPEC_DATA_LEN,         \
    .mandatory_data_type   = ADVERTISE_MANDATORY_DATA_TYPE_MANUFACTURER,  \
    .company_id            = UINT16_TO_BYTES(ADVERTISE_IBEACON_PREAMBLE), \
    .beacon_type           = UINT16_TO_BYTES(ADVERTISE_IBEACON_TYPE),     \
    .uuid                  = ADVERTISE_IBEACON_UUID,                      \
    .major                 = UINT16_TO_BYTES(ADVERTISE_IBEACON_MAJOR),    \
    .minor                 = UINT16_TO_BYTES(ADVERTISE_IBEACON_MINOR),    \
    .rssi                  = ADVERTISE_IBEACON_RSSI                       \
  }

// -------------------------------
// Structure that holds Scan Response data

typedef struct {
  uint8_t flags_length;          /**< Length of the Flags field. */
  uint8_t flags_type;            /**< Type of the Flags field. */
  uint8_t flags;                 /**< Flags field. */
  uint8_t mandatory_data_length; /**< Length of the mandata field. */
  uint8_t mandatory_data_type;   /**< Type of the mandata field. */
  uint8_t company_id[2];         /**< Company ID. */
  uint8_t firmware_id[2];        /**< Firmware ID */
  uint8_t local_name_length;     /**< Length of the local name field. */
  uint8_t local_name_type;       /**< Type of the local name field. */
  uint8_t local_name[ADVERTISE_DEVICE_NAME_LEN_MAX]; /**< Local name field. */
} advertise_scan_response_t;

#define ADVERTISE_SCAN_RESPONSE_DEFAULT                                  \
  {                                                                      \
    .flags_length          = ADVERTISE_FLAGS_LENGTH,                     \
    .flags_type            = ADVERTISE_FLAGS_TYPE,                       \
    .flags                 = ADVERTISE_FLAGS_LE_GENERAL_DISCOVERABLE     \
                             | ADVERTISE_FLAGS_BR_EDR_NOT_SUPPORTED,     \
    .mandatory_data_length = ADVERTISE_MANDATORY_DATA_LENGTH,            \
    .mandatory_data_type   = ADVERTISE_MANDATORY_DATA_TYPE_MANUFACTURER, \
    .company_id            = UINT16_TO_BYTES(ADVERTISE_COMPANY_ID),      \
    .firmware_id           = UINT16_TO_BYTES(ADVERTISE_FIRMWARE_ID),     \
    .local_name_length     = ADVERTISE_DEVICE_NAME_LEN_MAX + 1,          \
    .local_name_type       = ADVERTISE_TYPE_LOCAL_NAME,                  \
    .local_name            = ADVERTISE_DEVICE_NAME_DEFAULT_PREFIX        \
                             ADVERTISE_DEVICE_NAME_DEFAULT_SUFFIX        \
  }

// -----------------------------------------------------------------------------
// Private variables

// Advertising data structures
static advertise_ibeacon_t  adv_ibeacon = ADVERTISE_IBEACON_DATA_DEFAULT;
static advertise_scan_response_t adv_scan_response = ADVERTISE_SCAN_RESPONSE_DEFAULT;

// Advertising timer
static sl_simple_timer_t adv_timer;

// Advertising set handle allocated by Bluetooth stack
static uint8_t adv_set_handle = 0xff;

// -----------------------------------------------------------------------------
// Private function declarations

static void adv_set_data(adv_type_t type);
static void adv_timer_cb(sl_simple_timer_t *timer, void *data);

// -----------------------------------------------------------------------------
// Private function definitions

static void adv_set_data(adv_type_t type)
{
  sl_status_t sc;
  uint8_t *data = (type == ADV_TYPE_IBEACON)
                  ? (uint8_t*)(&adv_ibeacon)
                  : (uint8_t*)(&adv_scan_response);
  size_t length = (type == ADV_TYPE_IBEACON)
                  ? sizeof(adv_ibeacon)
                  : sizeof(adv_scan_response);

  sc = sl_bt_legacy_advertiser_set_data(adv_set_handle, 0, length, data);
  app_assert_status(sc);
}

static void adv_timer_cb(sl_simple_timer_t *timer, void *data)
{
  (void)timer;
  (void)data;
  // Initial advertising type
  static adv_type_t advType = ADV_TYPE_DEFAULT;
  // Swap advertising message
  advType = (advType == ADV_TYPE_IBEACON)
            ? ADV_TYPE_SCAN_RESPONSE
            : ADV_TYPE_IBEACON;
  adv_set_data(advType);
  // Toggle advertising LED state
  adv_led_toggle();
}

// -----------------------------------------------------------------------------
// Public function definitions

void advertise_init(uint32_t unique_id)
{
  sl_status_t sc;

  // Read device name from the local GATT table
  size_t local_name_length;
  // Helper buffer to hold device name string and the terminating null character
  uint8_t local_name[ADVERTISE_DEVICE_NAME_LEN_MAX + 1];
  sc = sl_bt_gatt_server_read_attribute_value(gattdb_device_name,
                                              0,
                                              sizeof(local_name) - 1,
                                              &local_name_length,
                                              local_name);
  app_log_status_error(sc);

  // Update iBeacon data
  adv_ibeacon.minor[1] = (uint8_t)unique_id;
  adv_ibeacon.minor[0] = (uint8_t)(unique_id >> 8);
  adv_ibeacon.major[1] = (uint8_t)(unique_id >> 16);

  // Search for device name suffix
  char *suffix;
  suffix = strstr((char *)local_name, ADVERTISE_DEVICE_NAME_DEFAULT_SUFFIX);
  app_assert(suffix != NULL,
             "Device name substring cannot be found: %s\n",
             (char *)local_name);

  // Update Scan Response data
  (void)snprintf(suffix,
                 strlen(ADVERTISE_DEVICE_NAME_DEFAULT_SUFFIX) + 1,
                 ADVERTISE_DEVICE_NAME_FORMAT_STRING,
                 (int)(unique_id & 0xFFFF));
  (void)memcpy(adv_scan_response.local_name,
               local_name,
               local_name_length + 1);
  adv_scan_response.local_name_length = local_name_length + 1;

  // Create an advertising set
  sc = sl_bt_advertiser_create_set(&adv_set_handle);
  app_assert_status(sc);

  // Set advertising interval to 100ms
  sc = sl_bt_advertiser_set_timing(
    adv_set_handle, // advertising set handle
    160, // min. adv. interval (milliseconds * 1.6)
    160, // max. adv. interval (milliseconds * 1.6)
    0,   // adv. duration
    0);  // max. num. adv. events
  app_assert_status(sc);
  // Start advertising
  advertise_start();
}

void advertise_start(void)
{
  sl_status_t sc;

  // Set initial advertising data type
  adv_set_data(ADV_TYPE_DEFAULT);

  // Turn on advertising LED
  adv_led_turn_on();

  // Start advertising and enable connections
  sc = sl_bt_legacy_advertiser_start(adv_set_handle,
                                     sl_bt_advertiser_connectable_scannable);
  app_assert_status(sc);

  // Start timer to alternate advertising data
  sc = sl_simple_timer_start(&adv_timer,
                             ADV_ALTERNATE_TIME_MS,
                             adv_timer_cb,
                             NULL,
                             true);
  app_assert_status(sc);
}

void advertise_stop(void)
{
  sl_status_t sc;
  // Stop advertising timer
  sc = sl_simple_timer_stop(&adv_timer);
  app_assert_status(sc);
  // Stop advertising
  sc = sl_bt_advertiser_stop(adv_set_handle);
  app_assert_status(sc);

  // Turn off advertising LED
  adv_led_turn_off();
}
