/***************************************************************************//**
 * @file
 * @brief Application Over-the-Air Device Firmware Update
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

#ifndef SL_BT_APP_OTA_DFU_H
#define SL_BT_APP_OTA_DFU_H

#include "sl_bgapi.h"
#include "sl_power_manager.h"

// Bootloader internal storage slot index.
#define SL_BT_APP_OTA_DFU_USED_SLOT                0u

// Bootloader internal storage check.
#define SL_BT_APP_OTA_DFU_READ_STORAGE_CONTEXT_SIZE    256u
#define SL_BT_APP_OTA_DFU_EMPTY_FLASH_CONTENT          0xFFu
#define SL_BT_APP_OTA_DFU_VERIFICATION_BLOCK_SIZE      128u

// Application OTA DFU state enumerator.
typedef enum {
  SL_BT_APP_OTA_DFU_UNINIT = 0,
  SL_BT_APP_OTA_DFU_INIT,
  SL_BT_APP_OTA_DFU_ERASE,
  SL_BT_APP_OTA_DFU_READY,
  SL_BT_APP_OTA_DFU_DISCONNECT,
  SL_BT_APP_OTA_DFU_DOWNLOAD_BEGIN,
  SL_BT_APP_OTA_DFU_DOWNLOAD_END,
  SL_BT_APP_OTA_DFU_VERIFY,
  SL_BT_APP_OTA_DFU_FINALIZE,
  SL_BT_APP_OTA_DFU_WAIT_FOR_REBOOT,
  SL_BT_APP_OTA_DFU_ERROR,
} sl_bt_app_ota_dfu_status_t;

// Application OTA DFU error state enumerator.
typedef enum {
  SL_BT_APP_OTA_DFU_NO_ERROR = 0,
  SL_BT_APP_OTA_DFU_ERR_UNEXPECTED_TRANSFER,
  SL_BT_APP_OTA_DFU_ERR_UNEXPECTED_CLOSE,
  SL_BT_APP_OTA_DFU_ERR_BOOTLOADER_API,
  SL_BT_APP_OTA_DFU_ERR_STORAGE_FULL,
} sl_bt_app_ota_dfu_error_t;

// Application OTA DFU event identifier enumerator.
typedef enum {
  SL_BT_APP_OTA_DFU_EVT_BTL_STORAGE_INFO_ID = 0u,
  SL_BT_APP_OTA_DFU_EVT_STATE_CHANGE_ID,
  SL_BT_APP_OTA_DFU_EVT_DOWNLOAD_PACKET_ID,
  SL_BT_APP_OTA_DFU_EVT_VERIFY_IMAGE_ID,
} sl_bt_app_ota_dfu_event_id_t;

// State structure for Application OTA DFU states.
typedef struct  {
  sl_bt_app_ota_dfu_status_t status;
  sl_bt_app_ota_dfu_status_t prev_status;
} sl_bt_app_ota_dfu_state_t;

// Bootloader storage info structure for
// bootloader and storage initialization event.
typedef struct {
  uint8_t bootloader_type;
  uint32_t bootloader_ver;
  uint32_t storage_start_addr;
  uint32_t storage_size_bytes;
} sl_bt_app_ota_dfu_btl_storage_info_t;

// Download package structure for
// Application OTA DFU packet download event.
typedef struct {
  uint8_t connection_handle;
  uint32_t write_image_position;
} sl_bt_app_ota_dfu_download_package_t;

// Application OTA DFU status message structure.
// Provides the state indication structure types for every event:
// - bootloader and storage information
// - state machine status updates
// - downloaded packet information
// - downloaded image verification progress status
PACKSTRUCT(struct sl_bt_app_ota_dfu_msg_t {
  // Static event elements
  sl_bt_app_ota_dfu_event_id_t event_id;
  sl_bt_app_ota_dfu_error_t ota_error_code;
  int32_t btl_api_retval;
  // Dynamic event elements
  union {
    sl_bt_app_ota_dfu_state_t sts;
    sl_bt_app_ota_dfu_btl_storage_info_t btl_storage;
    sl_bt_app_ota_dfu_download_package_t download_packet;
    uint32_t verified_bytes;
  } evt_info;
});

// Application OTA DFU event type definition.
typedef struct sl_bt_app_ota_dfu_msg_t sl_bt_app_ota_dfu_status_evt_t;

/**************************************************************************//**
 * Application OTA DFU initialization.
 *****************************************************************************/
void sl_bt_app_ota_dfu_init(void);

/**************************************************************************/ /**
 * Function to check if it is okay if the device goes to sleep now.
 *****************************************************************************/
bool sl_bt_app_ota_dfu_is_ok_to_sleep(void);

/**************************************************************************//**
 * Routine to notify power manager handler.
 *****************************************************************************/
sl_power_manager_on_isr_exit_t sl_bt_app_ota_dfu_sleep_on_isr_exit(void);

/**************************************************************************/ /**
 * Function to restart application OTA DFU progress without rebooting in
 * case of any error.
 *****************************************************************************/
void sl_bt_app_ota_dfu_restart_progress(void);

/**************************************************************************//**
 * Function to reboot only when the application OTA DFU process is already
 * in the SL_BT_APP_OTA_DFU_WAIT_FOR_REBOOT state.
 *****************************************************************************/
void sl_bt_app_ota_dfu_reboot(void);

/**************************************************************************//**
 * Function to indicate Application OTA DFU status and in case of error
 * the error codes for assertion.
 * @param[in] event Actual app ota dfu event address.
 * @note To be implemented in user code.
 *****************************************************************************/
void sl_bt_app_ota_dfu_on_status_event(sl_bt_app_ota_dfu_status_evt_t *evt);

#endif // SL_BT_APP_OTA_DFU_H
