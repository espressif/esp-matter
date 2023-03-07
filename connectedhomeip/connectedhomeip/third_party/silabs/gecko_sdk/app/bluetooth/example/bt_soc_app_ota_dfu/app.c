/***************************************************************************//**
 * @file
 * @brief Core application logic.
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
#include <stdbool.h>
#include "em_common.h"
#include "app_log.h"
#include "app_assert.h"

#include "sl_bluetooth.h"
#include "gatt_db.h"
#include "sl_simple_button_instances.h"
#include "btl_errorcode.h"
#ifdef SL_COMPONENT_CATALOG_PRESENT
#include "sl_component_catalog.h"
#endif // SL_COMPONENT_CATALOG_PRESENT
#include "app.h"
#include "sl_bt_app_ota_dfu.h"
#include "sl_simple_timer.h"

// Bootloader information
static uint8_t btl_type;
static uint32_t btl_version;
// Storage information
static uint32_t slot_startaddr;
static uint32_t slot_size;

// Connection verification
static uint8_t connected_handle;
static uint8_t write_request_handle;

// OTA Transfer
static sl_bt_app_ota_dfu_status_t app_ota_dfu_status = SL_BT_APP_OTA_DFU_UNINIT;
static sl_bt_app_ota_dfu_status_t app_ota_dfu_prev_status = SL_BT_APP_OTA_DFU_UNINIT;
static sl_bt_app_ota_dfu_error_t app_ota_dfu_error_code = SL_BT_APP_OTA_DFU_NO_ERROR;
static int32_t bootloader_api_error_code = BOOTLOADER_OK;
static uint16_t datablock_idx = 0u;
static uint32_t write_position = 0u;
static uint32_t verif_position = 0u;

sl_simple_timer_t progress_timer;

// The advertising set handle allocated from Bluetooth stack.
static uint8_t advertising_set_handle = 0xffu;

/**************************************************************************//**
 * Application Init.
 *****************************************************************************/
SL_WEAK void app_init(void)
{
  /////////////////////////////////////////////////////////////////////////////
  // Put your additional application init code here!                         //
  // This is called once during start-up.                                    //
  /////////////////////////////////////////////////////////////////////////////
}

/**************************************************************************//**
 * Application Process Action (baremetal).
 *****************************************************************************/
SL_WEAK void app_process_action(void)
{
  /////////////////////////////////////////////////////////////////////////////
  // Put your additional application code here!                              //
  // This is called infinitely.                                              //
  // Do not call blocking functions from here!                               //
  /////////////////////////////////////////////////////////////////////////////
}

/**************************************************************************//**
 * Private function to display download progress status:
 * transfer speed and used storage space.
 * @param[in] elapsed_time Elapsed time.
 *****************************************************************************/
static void app_ota_dfu_display_progress(uint16_t elapsed_time)
{
  if (app_ota_dfu_status == SL_BT_APP_OTA_DFU_DOWNLOAD_BEGIN) {
    app_log_info("Received packets: %u, storage used: %u%%.(%u kbps)" \
                 APP_LOG_NEW_LINE,
                 datablock_idx,
                 GET_DATA_PERCENTAGE(slot_size, write_position),
                 GET_TRANSFER_SPEED_KBPS(write_position, elapsed_time));
  } else {
    app_log_info("Verified %u%% of the new image.(%u block)" \
                 APP_LOG_NEW_LINE,
                 GET_DATA_PERCENTAGE(write_position, verif_position),
                 datablock_idx);
  }
}

/**************************************************************************//**
 * Bluetooth stack event handler.
 * This overrides the dummy weak implementation.
 *
 * @param[in] evt Event coming from the Bluetooth stack.
 *****************************************************************************/
void sl_bt_on_event(sl_bt_msg_t *evt)
{
  sl_status_t sc;
  bd_addr address;
  uint8_t address_type;
  uint8_t system_id[8];

  switch (SL_BT_MSG_ID(evt->header)) {
    // -------------------------------
    // This event indicates the device has started and the radio is ready.
    // Do not call any stack command before receiving this boot event!
    case sl_bt_evt_system_boot_id:
      // Extract unique ID from BT Address.
      sc = sl_bt_system_get_identity_address(&address, &address_type);
      app_assert_status(sc);

      // Pad and reverse unique ID to get System ID.
      system_id[0] = address.addr[5];
      system_id[1] = address.addr[4];
      system_id[2] = address.addr[3];
      system_id[3] = 0xFF;
      system_id[4] = 0xFE;
      system_id[5] = address.addr[2];
      system_id[6] = address.addr[1];
      system_id[7] = address.addr[0];

      sc = sl_bt_gatt_server_write_attribute_value(gattdb_system_id,
                                                   0,
                                                   sizeof(system_id),
                                                   system_id);
      app_assert_status(sc);

      // Create an advertising set.
      sc = sl_bt_advertiser_create_set(&advertising_set_handle);
      app_assert_status(sc);

      // Generate data for advertising
      sc = sl_bt_legacy_advertiser_generate_data(advertising_set_handle,
                                                 sl_bt_advertiser_general_discoverable);
      app_assert_status(sc);

      // Set advertising interval to 100ms.
      sc = sl_bt_advertiser_set_timing(
        advertising_set_handle,
        160, // min. adv. interval (milliseconds * 1.6)
        160, // max. adv. interval (milliseconds * 1.6)
        0,   // adv. duration
        0);  // max. num. adv. events
      app_assert_status(sc);

      // Start advertising and enable connections.
      sc = sl_bt_legacy_advertiser_start(advertising_set_handle,
                                         sl_bt_advertiser_connectable_scannable);
      app_assert_status(sc);

      break;

    // -------------------------------
    // This event indicates that a connection was closed.
    case sl_bt_evt_connection_closed_id:
      // Generate data for advertising
      sc = sl_bt_legacy_advertiser_generate_data(advertising_set_handle,
                                                 sl_bt_advertiser_general_discoverable);
      app_assert_status(sc);

      // Restart advertising after client has disconnected.
      sc = sl_bt_legacy_advertiser_start(advertising_set_handle,
                                         sl_bt_advertiser_connectable_scannable);
      app_assert_status(sc);
      break;

    case sl_bt_evt_connection_opened_id:
      // Get connection handle for further checking.
      connected_handle = evt->data.evt_connection_opened.connection;
      app_log_info("Connection opened." APP_LOG_NEW_LINE);
      break;

    ///////////////////////////////////////////////////////////////////////////
    // Add additional event handlers here as your application requires!      //
    ///////////////////////////////////////////////////////////////////////////

    // -------------------------------
    // Default event handler.
    default:
      break;
  }
}

/***************************************************************************//**
 * Private bootloader API error handler function.
 * Parse bootloader API errors and display it to the user.
 * @param[in] btl_errcode Bootloader error code.
 ******************************************************************************/
static void app_ota_dfu_bootloader_api_error_handler(int32_t btl_errcode)
{
  switch (btl_errcode) {
    // Initialization errors.
    case BOOTLOADER_ERROR_INIT_STORAGE:
      app_log_error("Storage init error!");
      break;
    case BOOTLOADER_ERROR_INIT_TABLE:
      app_log_error("Bootloader table invalid!");
      break;
    case BOOTLOADER_ERROR_INIT_SFDP:
      app_log_error("Bootloader SFDP not supported!");
      break;

    // Storage errors.
    case BOOTLOADER_ERROR_STORAGE_INVALID_SLOT:
      app_log_error("Invalid storage!");
      break;
    case BOOTLOADER_ERROR_STORAGE_INVALID_ADDRESS:
      app_log_error("Storage address not aligned or out of range!");
      break;
    case BOOTLOADER_ERROR_STORAGE_NEEDS_ERASE:
      app_log_error("Storage area needs to be erased before use!");
      break;
    case BOOTLOADER_ERROR_STORAGE_NEEDS_ALIGN:
      app_log_error("Storage address or length must be aligned!");
      break;
    case BOOTLOADER_ERROR_STORAGE_BOOTLOAD:
      app_log_error("Bootload from storage failed!");
      break;
    case BOOTLOADER_ERROR_STORAGE_NO_IMAGE:
      app_log_error("No image in this storage!");
      break;
    case BOOTLOADER_ERROR_STORAGE_GENERIC:
      app_log_error("Generic storage error!");
      break;

    // Bootloading errors.
    case  BOOTLOADER_ERROR_BOOTLOAD_LIST_EMPTY:
      app_log_error("Bootload list empty!");
      break;
    case  BOOTLOADER_ERROR_BOOTLOAD_LIST_FULL:
      app_log_error("Bootload list full!");
      break;
    case  BOOTLOADER_ERROR_BOOTLOAD_LIST_OVERFLOW:
      app_log_error("Bootload list overflow!");
      break;
    case BOOTLOADER_ERROR_BOOTLOAD_LIST_NO_LIST:
      app_log_error("No bootload list found at the base storage!");
      break;

    // UART peripheral bootloader errors.
    case BOOTLOADER_ERROR_UART_UNINIT:
      app_log_error("UART peripheral not initialized!");
      break;
    case BOOTLOADER_ERROR_UART_INIT:
      app_log_error("UART peripheral error during init!");
      break;
    case BOOTLOADER_ERROR_UART_ARGUMENT:
      app_log_error("UART peripheral invalid argument!");
      break;
    case BOOTLOADER_ERROR_UART_TIMEOUT:
      app_log_error("UART peripheral operation timeout!");
      break;
    case BOOTLOADER_ERROR_UART_OVERFLOW:
      app_log_error("UART peripheral buffer overflow!");
      break;
    case BOOTLOADER_ERROR_UART_BUSY:
      app_log_error("UART peripheral busy!");
      break;

    // Everything else displayed as bootloader API error.
    default:
      app_log_error("Bootloader API error!" APP_LOG_NEW_LINE);
      break;
  }
  app_log_error("Error code: 0x%x ." APP_LOG_NEW_LINE, btl_errcode);
}

/***************************************************************************//**
 * Private Application OTA DFU error handling function.
 * Display actual error message to user.
 * @param[in] last_state Last state before error.
 * @param[in] ota_err_code Last state before error.
 * @param[in] btl_retval Last state before error.
 ******************************************************************************/
static void app_ota_dfu_error_handler(sl_bt_app_ota_dfu_status_t last_state,
                                      sl_bt_app_ota_dfu_error_t  ota_err_code,
                                      int32_t btl_retval)
{
  switch (ota_err_code) {
    case SL_BT_APP_OTA_DFU_ERR_BOOTLOADER_API:
      app_ota_dfu_bootloader_api_error_handler(btl_retval);
      break;

    case SL_BT_APP_OTA_DFU_ERR_UNEXPECTED_TRANSFER:
      app_log_error("Unexpected OTA transfer rejected!" APP_LOG_NEW_LINE);
      app_log_error("Wait until OTA function is ready!" APP_LOG_NEW_LINE);
      break;

    case SL_BT_APP_OTA_DFU_ERR_UNEXPECTED_CLOSE:
      app_log_error("Unexpected connection close!" APP_LOG_NEW_LINE);
      app_log_error("Hdl: 0x%x | Write req. hdl: 0x%x." APP_LOG_NEW_LINE,
                    connected_handle,
                    write_request_handle);
      break;

    case SL_BT_APP_OTA_DFU_ERR_STORAGE_FULL:
      app_log_error("Storage full!" APP_LOG_NEW_LINE);
      app_log_error("Consecutive packets will be ignored!" APP_LOG_NEW_LINE);
      break;

    case SL_BT_APP_OTA_DFU_NO_ERROR:
      app_log_debug("State transition to: %u." APP_LOG_NEW_LINE, last_state);
      break;
  }
}

/***************************************************************************//**
 * Button state changed callback strong implementation.
 * @param[in] handle Button event handle
 ******************************************************************************/
void sl_button_on_change(const sl_button_t *handle)
{
  if (SL_SIMPLE_BUTTON_INSTANCE(0) == handle) {
    if (sl_button_get_state(SL_SIMPLE_BUTTON_INSTANCE(0)) == SL_SIMPLE_BUTTON_PRESSED) {
      if (app_ota_dfu_status == SL_BT_APP_OTA_DFU_ERROR) {
        // Restart the application OTA DFU progress without resetting the device.
        // This way the user application may continue in foreground, while
        // application OTA DFU progress restart in the background.
        app_log_info("Reinitialize application OTA DFU progress..." \
                     APP_LOG_NEW_LINE);
        sl_bt_app_ota_dfu_restart_progress();
      } else if (app_ota_dfu_status == SL_BT_APP_OTA_DFU_WAIT_FOR_REBOOT) {
        app_log_info("Reboot..." APP_LOG_NEW_LINE);
        sl_bt_app_ota_dfu_reboot();
      }
    }
  }
}

/**************************************************************************//**
 * Timer callback function.
 *****************************************************************************/
static void app_ota_dfu_progress_timer_cb(sl_simple_timer_t *timer, void *dat)
{
  (void) timer;
  *(uint16_t *)dat += 1u;
  app_ota_dfu_display_progress(*(uint16_t *)dat);
}

/**************************************************************************//**
 * Private function to handle the Application OTA DFU states.
 * @param[in] curr_sts Current state.
 * @param[in] prev_sts Previous state for error handling.
 * @param[in] app_ota_dfu_error_code Application OTA DFU error code.
 * @param[in] boot_api_err_code Bootloader error code.
 *****************************************************************************/
static void app_ota_dfu_on_status_change(sl_bt_app_ota_dfu_status_t curr_sts,
                                         sl_bt_app_ota_dfu_status_t prev_sts,
                                         sl_bt_app_ota_dfu_error_t app_ota_dfu_error_code,
                                         int32_t boot_api_err_code)
{
  sl_status_t sc;
  static uint16_t time_elapsed = 0;

  switch (curr_sts) {
    case SL_BT_APP_OTA_DFU_UNINIT:
      // Intentionally left blank.
      break;

    case SL_BT_APP_OTA_DFU_INIT:
      // Disable button by default.
      sl_button_disable(SL_SIMPLE_BUTTON_INSTANCE(0));
      app_log_info("Bootloader initialized." APP_LOG_NEW_LINE);
      break;

    case SL_BT_APP_OTA_DFU_ERASE:
      app_log_info("Erase storage slot..." APP_LOG_NEW_LINE);
      break;

    case SL_BT_APP_OTA_DFU_READY:
      app_log_info("Erase done." APP_LOG_NEW_LINE);
      app_log_info("Application OTA DFU ready." APP_LOG_NEW_LINE);
      break;

    case SL_BT_APP_OTA_DFU_DISCONNECT:
      // Indicates that disconnect from current device initiated.
      // Happens in case of write request or transfer issues.
      app_log_error("Disconnected by the target device." APP_LOG_NEW_LINE);
      break;

    case SL_BT_APP_OTA_DFU_DOWNLOAD_BEGIN:
      datablock_idx = 0;
      time_elapsed = 0;
      app_log_info("Download started." APP_LOG_NEW_LINE);
      sc = sl_simple_timer_start(&progress_timer,
                                 DOWNLOAD_TIMER_CYCLE,
                                 app_ota_dfu_progress_timer_cb,
                                 (void *)&time_elapsed,
                                 true);
      app_assert_status(sc);
      break;

    case SL_BT_APP_OTA_DFU_DOWNLOAD_END:
      sc = sl_simple_timer_stop(&progress_timer);
      time_elapsed = 0;
      app_assert_status(sc);
      app_log_info("Download finished. Received %u bytes." APP_LOG_NEW_LINE,
                   write_position);
      app_log_info("Press END button in EFR Connect app!" APP_LOG_NEW_LINE);
      break;

    case SL_BT_APP_OTA_DFU_VERIFY:
      datablock_idx = 0u;
      app_log_info("Connection closed." APP_LOG_NEW_LINE);
      app_log_info("Verify downloaded image..." APP_LOG_NEW_LINE);
      sc = sl_simple_timer_start(&progress_timer,
                                 VERIFICATION_TIMER_CYCLE,
                                 app_ota_dfu_progress_timer_cb,
                                 (void *)&time_elapsed,
                                 true);
      break;

    case SL_BT_APP_OTA_DFU_FINALIZE:
      sc = sl_simple_timer_stop(&progress_timer);
      app_log_info("Verified %u%% of the new image." APP_LOG_NEW_LINE,
                   GET_DATA_PERCENTAGE(write_position, verif_position),
                   datablock_idx);
      app_log_info("Set image to bootload." APP_LOG_NEW_LINE);
      break;

    case SL_BT_APP_OTA_DFU_WAIT_FOR_REBOOT:
      app_log_info("Press button 0 to reboot!" APP_LOG_NEW_LINE);
      sl_button_enable(SL_SIMPLE_BUTTON_INSTANCE(0));
      break;

    case SL_BT_APP_OTA_DFU_ERROR:
      //In case the error handler reached, progress timer has to be stopped.
      sc = sl_simple_timer_stop(&progress_timer);
      time_elapsed = 0;
      app_ota_dfu_error_handler(prev_sts, app_ota_dfu_error_code, boot_api_err_code);
      app_log_error("Press button 0 to reinitialize progress, without reboot." \
                    APP_LOG_NEW_LINE);
      break;
  }
}

/**************************************************************************//**
 * Strong implementation of the callback for application OTA DFU status change.
 * The whole process can be tracked with this function from the storage erase
 * until the verification process.
 *****************************************************************************/
void sl_bt_app_ota_dfu_on_status_event(sl_bt_app_ota_dfu_status_evt_t* evt)
{
  // Get information that is up-to-date at every trigger
  app_ota_dfu_error_code = evt->ota_error_code;
  bootloader_api_error_code = evt->btl_api_retval;

  switch (evt->event_id) {
    case SL_BT_APP_OTA_DFU_EVT_BTL_STORAGE_INFO_ID:
      // Get bootloader and storage information.
      btl_type = evt->evt_info.btl_storage.bootloader_type;
      btl_version = evt->evt_info.btl_storage.bootloader_ver;
      slot_startaddr = evt->evt_info.btl_storage.storage_start_addr;
      slot_size = evt->evt_info.btl_storage.storage_size_bytes;

      app_log_info("Gecko bootloader version: %u.%u ." APP_LOG_NEW_LINE,
                   (btl_version & 0xFF000000) >> 24,
                   (btl_version & 0x00FF0000) >> 16);
      app_log_info("Slot %d starts @ 0x%8.8x, size %u bytes ." APP_LOG_NEW_LINE,
                   SL_BT_APP_OTA_DFU_USED_SLOT,
                   slot_startaddr,
                   slot_size);
      break;

    case SL_BT_APP_OTA_DFU_EVT_STATE_CHANGE_ID:
      // Get application OTA DFU state machine status.
      app_ota_dfu_status = evt->evt_info.sts.status;
      app_ota_dfu_prev_status = evt->evt_info.sts.prev_status;
      // Process state transtions, display information to the user or
      // start/stop timers for download, etc.
      app_ota_dfu_on_status_change(app_ota_dfu_status,
                                   app_ota_dfu_prev_status,
                                   app_ota_dfu_error_code,
                                   bootloader_api_error_code);
      break;

    case SL_BT_APP_OTA_DFU_EVT_DOWNLOAD_PACKET_ID:
      // Get download information.
      datablock_idx += 1;
      write_request_handle = evt->evt_info.download_packet.connection_handle;
      write_position = evt->evt_info.download_packet.write_image_position;
      break;

    case SL_BT_APP_OTA_DFU_EVT_VERIFY_IMAGE_ID:
      // Get verification information.
      datablock_idx += 1;
      verif_position = evt->evt_info.verified_bytes;
      break;
  }
}
