/***************************************************************************/ /**
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
#include <stdbool.h>
#include <stddef.h>
#include "em_common.h"
#include "sl_bluetooth.h"
#include "gatt_db.h"

#include "btl_interface.h"
#include "btl_interface_storage.h"

#include "sl_bt_app_ota_dfu.h"
#include "sli_bt_app_ota_dfu.h"

/// Possible control words of OTA control characteristic
#define SL_BT_APP_OTA_DFU_BEGIN_PROGRESS       0u
#define SL_BT_APP_OTA_DFU_END_PROGRESS         3u
#define SL_BT_APP_OTA_DFU_CLOSE_CONNECTION     4u

#define ATT_ERR_WRITE_REQUEST_REJECTED         0x80u
#define ATT_ERR_NOT_READY_FOR_WRITE_REQUEST    0x81u
#define ATT_ERR_STORAGE_FULL                   0x82u
#define ATT_ERR_PACKAGE_LOST                   0x83u

#ifndef SL_BT_INVALID_CONNECTION_HANDLE
#define SL_BT_INVALID_CONNECTION_HANDLE ((uint8_t) 0xFF)
#endif // SL_BT_INVALID_CONNECTION_HANDLE

// Bootloader storage
static BootloaderInformation_t bootloader_info;
static BootloaderStorageSlot_t slot_info;

// OTA
static uint32_t ota_conn_hdl = SL_BT_INVALID_CONNECTION_HANDLE;
static sl_bt_app_ota_dfu_status_t ota_prev_sts = SL_BT_APP_OTA_DFU_UNINIT;
static sl_bt_app_ota_dfu_status_t ota_sts = SL_BT_APP_OTA_DFU_UNINIT;
static uint32_t ota_img_pos = 0u;

static sl_bt_app_ota_dfu_status_evt_t ota_event;

static uint8_t ota_buff[BOOTLOADER_STORAGE_VERIFICATION_CONTEXT_SIZE];

/**************************************************************************/ /**
 * Weak Application OTA DFU initialization.
 *****************************************************************************/
SL_WEAK void sl_bt_app_ota_dfu_init(void)
{
  int32_t boot_retv = BOOTLOADER_OK;
  sl_bt_app_ota_dfu_error_t ota_error = SL_BT_APP_OTA_DFU_NO_ERROR;

  // Bootloader init must be called before using bootloader_* API calls!
  boot_retv = bootloader_init();
  if (boot_retv == BOOTLOADER_OK) {
    sli_bt_app_ota_dfu_set_main_status(SL_BT_APP_OTA_DFU_INIT);
  } else {
    ota_error = SL_BT_APP_OTA_DFU_ERR_BOOTLOADER_API;
    sli_bt_app_ota_dfu_set_main_status(SL_BT_APP_OTA_DFU_ERROR);
  }

  // Forward state change information to application.
  ota_event.event_id = SL_BT_APP_OTA_DFU_EVT_STATE_CHANGE_ID;
  ota_event.ota_error_code = ota_error;
  ota_event.btl_api_retval = boot_retv;
  ota_event.evt_info.sts.status = ota_sts;
  ota_event.evt_info.sts.prev_status = ota_prev_sts;
  sl_bt_app_ota_dfu_on_status_event(&ota_event);
}

/**************************************************************************/ /**
 * Internal function to set Application OTA DFU main status.
 *****************************************************************************/
void sli_bt_app_ota_dfu_set_main_status(sl_bt_app_ota_dfu_status_t new_status)
{
  ota_prev_sts = ota_sts;
  ota_sts = new_status;
  // Continue execution.
  sli_bt_app_ota_dfu_proceed();
}

/**************************************************************************/ /**
 * Event handler.
 *****************************************************************************/
void sli_bt_app_ota_dfu_on_event(sl_bt_msg_t *evt)
{
  uint8_t attr_status = SL_STATUS_OK;
  uint32_t connection;
  uint32_t characteristic;

  int32_t btl_ret_val = BOOTLOADER_OK;
  sl_bt_app_ota_dfu_error_t ota_error = SL_BT_APP_OTA_DFU_NO_ERROR;
  sl_bt_app_ota_dfu_status_t req_sts = SL_BT_APP_OTA_DFU_UNINIT;

  // Handle stack events
  switch (SL_BT_MSG_ID(evt->header)) {
    ///////////////////////////////////////////////////////////////////////////
    // OTA package write request event                                       //
    ///////////////////////////////////////////////////////////////////////////
    case sl_bt_evt_gatt_server_user_write_request_id:
      // Get characteristic and connection handle
      characteristic = evt->data.evt_gatt_server_user_write_request.characteristic;
      connection = evt->data.evt_gatt_server_user_write_request.connection;
      attr_status = SL_STATUS_OK;

      if (characteristic == gattdb_ota_control) {
        // OTA control characteristic detected.
        switch (evt->data.evt_gatt_server_user_write_request.value.data[0]) {
          case SL_BT_APP_OTA_DFU_BEGIN_PROGRESS:
            // Accept OTA begin request from single connection source, only!
            if (ota_sts == SL_BT_APP_OTA_DFU_READY
                && (ota_conn_hdl == SL_BT_INVALID_CONNECTION_HANDLE
                    || ota_conn_hdl == connection)) {
              // Application ready for the OTA DFU process.
              req_sts = SL_BT_APP_OTA_DFU_DOWNLOAD_BEGIN;
              ota_img_pos = 0;
              // save connection handle for later error checking / handling
              ota_conn_hdl = connection;
            } else {
              // Application not ready for the OTA DFU process.
              req_sts = SL_BT_APP_OTA_DFU_ERROR;
              ota_error = SL_BT_APP_OTA_DFU_ERR_UNEXPECTED_TRANSFER;
            }
            break;

          case SL_BT_APP_OTA_DFU_END_PROGRESS:
            // Wait for connection close and then reboot.
            req_sts = SL_BT_APP_OTA_DFU_DOWNLOAD_END;
            break;

          case SL_BT_APP_OTA_DFU_CLOSE_CONNECTION:
            // Requested to close connection.
            req_sts = SL_BT_APP_OTA_DFU_DISCONNECT;
            break;

          default:
            break;
        }
      } else if (characteristic == gattdb_ota_data && ota_conn_hdl == connection) {
        // OTA data characteristic detected - handle only if the connection is the initiator one.
        if (ota_sts == SL_BT_APP_OTA_DFU_DOWNLOAD_BEGIN) {
          if (ota_img_pos >= (slot_info.length)) {
            req_sts = SL_BT_APP_OTA_DFU_ERROR;
            ota_error = SL_BT_APP_OTA_DFU_ERR_STORAGE_FULL;
            attr_status = ATT_ERR_STORAGE_FULL;
          } else {
            //Additional buffer required to avoid passing unaligned data to the bootloader API.
            //The data element can be unaligned.
            memcpy(ota_buff,
                   evt->data.evt_gatt_server_user_write_request.value.data,
                   evt->data.evt_gatt_server_user_write_request.value.len);
            btl_ret_val =
              bootloader_writeStorage(SL_BT_APP_OTA_DFU_USED_SLOT,
                                      ota_img_pos,
                                      ota_buff,
                                      evt->data.evt_gatt_server_user_write_request.value.len);
            // Check write storage results
            if (btl_ret_val != BOOTLOADER_OK) {
              // Write failed.
              req_sts = SL_BT_APP_OTA_DFU_ERROR;
              ota_error = SL_BT_APP_OTA_DFU_ERR_BOOTLOADER_API;
              attr_status = ATT_ERR_PACKAGE_LOST;
            } else {
              // Stay in download state until process finished.
              req_sts = SL_BT_APP_OTA_DFU_DOWNLOAD_BEGIN;
              // Write successful. Update image position.
              ota_img_pos +=
                evt->data.evt_gatt_server_user_write_request.value.len;
              attr_status = SL_STATUS_OK;
            }
          }
        } else {
          // Unexpected download has to be aborted.
          // Application OTA not ready or error occurred during
          // ongoing download process.
          req_sts = SL_BT_APP_OTA_DFU_ERROR;
          attr_status = ATT_ERR_NOT_READY_FOR_WRITE_REQUEST;
        }
      }

      // Forward download information to application.
      ota_event.event_id = SL_BT_APP_OTA_DFU_EVT_DOWNLOAD_PACKET_ID;
      ota_event.ota_error_code = ota_error;
      ota_event.btl_api_retval = btl_ret_val;
      ota_event.evt_info.download_packet.connection_handle = connection;
      ota_event.evt_info.download_packet.write_image_position = ota_img_pos;
      sl_bt_app_ota_dfu_on_status_event(&ota_event);

      // Send write-response to GATT device.
      sl_bt_gatt_server_send_user_write_response(connection,
                                                 characteristic,
                                                 attr_status);
      // Update state if necessary.
      if (ota_sts != req_sts) {
        sli_bt_app_ota_dfu_set_main_status(req_sts);
        // Forward state change information to application.
        ota_event.event_id = SL_BT_APP_OTA_DFU_EVT_STATE_CHANGE_ID;
        ota_event.ota_error_code = ota_error;
        ota_event.btl_api_retval = btl_ret_val;
        ota_event.evt_info.sts.status = ota_sts;
        ota_event.evt_info.sts.prev_status = ota_prev_sts;
        sl_bt_app_ota_dfu_on_status_event(&ota_event);
        // Continue execution.
        sli_bt_app_ota_dfu_proceed();
      }
      break;

    ///////////////////////////////////////////////////////////////////////////
    // Bluetooth connection closed event                                     //
    ///////////////////////////////////////////////////////////////////////////
    case sl_bt_evt_connection_closed_id:
      connection = evt->data.evt_connection_closed.connection;

      // Ignore all close events other than the connection that did the update.
      if (ota_conn_hdl == connection) {
        if (ota_sts == SL_BT_APP_OTA_DFU_DOWNLOAD_END) {
          // Only If the download finished successfully, the image verification
          // will start automatically.

          // Set verify state - may be reverted if verify init fails below.
          req_sts = SL_BT_APP_OTA_DFU_VERIFY;
          sli_bt_app_ota_dfu_set_main_status(req_sts);

          // Try to prepare for verification.
          btl_ret_val = bootloader_initVerifyImage(SL_BT_APP_OTA_DFU_USED_SLOT,
                                                   ota_buff,
                                                   sizeof(ota_buff));
          if (btl_ret_val != BOOTLOADER_OK) {
            // Verification init failed.
            req_sts = SL_BT_APP_OTA_DFU_ERROR;
            ota_error = SL_BT_APP_OTA_DFU_ERR_BOOTLOADER_API;
          } else {
            // Reuse image position to store verification position.
            ota_img_pos = 0u;
            ota_event.ota_error_code = SL_BT_APP_OTA_DFU_NO_ERROR;
          }
        } else if (ota_sts == SL_BT_APP_OTA_DFU_DOWNLOAD_BEGIN) {
          // Connection should not be closed during an update!
          req_sts = SL_BT_APP_OTA_DFU_ERROR;
          ota_event.ota_error_code = SL_BT_APP_OTA_DFU_ERR_UNEXPECTED_CLOSE;
        } else {
          // Restart handler just in case.
          req_sts = SL_BT_APP_OTA_DFU_INIT;
          ota_event.ota_error_code = SL_BT_APP_OTA_DFU_NO_ERROR;
        }

        // Don't call status update again if verify init was already successful.
        if (ota_sts != req_sts) {
          sli_bt_app_ota_dfu_set_main_status(req_sts);
        }

        // Forward state change information to application, always.
        ota_event.event_id = SL_BT_APP_OTA_DFU_EVT_STATE_CHANGE_ID;
        ota_event.btl_api_retval = btl_ret_val;
        ota_event.evt_info.sts.status = ota_sts;
        ota_event.evt_info.sts.prev_status = ota_prev_sts;
        sl_bt_app_ota_dfu_on_status_event(&ota_event);
        // Forget the OTA connection handle
        ota_conn_hdl = SL_BT_INVALID_CONNECTION_HANDLE;
        // Continue execution.
        sli_bt_app_ota_dfu_proceed();
      }
      break;

    default:
      break;
  }
}

/**************************************************************************/ /**
 * Function to check if it is okay if the device goes to sleep now.
 *****************************************************************************/
bool sl_bt_app_ota_dfu_is_ok_to_sleep(void)
{
  bool ret_val = true;
  // Device shall go to sleep if:
  // - no verification is in progress
  // - no storage checking for erase is in progress
  if ((ota_sts == SL_BT_APP_OTA_DFU_VERIFY) \
      || (ota_sts == SL_BT_APP_OTA_DFU_ERASE)) {
    ret_val = false;
  }
  return ret_val;
}

/**************************************************************************//**
 * Routine to notify power manager handler.
 *****************************************************************************/
sl_power_manager_on_isr_exit_t sl_bt_app_ota_dfu_sleep_on_isr_exit(void)
{
  sl_power_manager_on_isr_exit_t ret_val = SL_POWER_MANAGER_IGNORE;
  // Device shall wake up for sure if:
  // - verification is in progress
  // - storage checking for erase is in progress
  if ((ota_sts == SL_BT_APP_OTA_DFU_VERIFY) \
      || (ota_sts == SL_BT_APP_OTA_DFU_ERASE)) {
    ret_val = SL_POWER_MANAGER_WAKEUP;
  }
  return ret_val;
}

/**************************************************************************//**
 * Weak function to proceed cyclic action in case of xRTOS kernel projects.
 *****************************************************************************/
SL_WEAK void sli_bt_app_ota_dfu_proceed(void)
{
  ///////////////////////////////////////////////////////////////////////////
  // Let the cyclic process executed, in case of xRTOS scheduling.         //
  // This is to avoid unwanted power-consumption when the application OTA  //
  // cyclic process not required, e.g. application awaits for user action. //
  ///////////////////////////////////////////////////////////////////////////
}

/**************************************************************************/ /**
 * Cyclic process of Application OTA DFU.
 *****************************************************************************/
void sli_bt_app_ota_dfu_step(void)
{
  int32_t btl_ret_val = BOOTLOADER_OK;
  sl_bt_app_ota_dfu_error_t ota_error = SL_BT_APP_OTA_DFU_NO_ERROR;
  sl_bt_app_ota_dfu_status_t req_sts = SL_BT_APP_OTA_DFU_UNINIT;

  uint32_t offset = 0, num_blocks = 0, i = 0;
  bool dirty = false;

  switch (ota_sts) {
    ///////////////////////////////////////////////////////////////////////////
    // Pre initialization state.                                             //
    ///////////////////////////////////////////////////////////////////////////
    case SL_BT_APP_OTA_DFU_UNINIT:
      // Intentionally left blank.
      break;

    ///////////////////////////////////////////////////////////////////////////
    // Bootloader API initialized, check storage content.                    //
    ///////////////////////////////////////////////////////////////////////////
    case SL_BT_APP_OTA_DFU_INIT:
      // Get bootloader information.
      bootloader_getInfo(&bootloader_info);
      // Get storage slot information.
      btl_ret_val =
        bootloader_getStorageSlotInfo(SL_BT_APP_OTA_DFU_USED_SLOT, &slot_info);

      if (btl_ret_val == BOOTLOADER_OK) {
        // Forward bootloader and storage information to application.
        ota_event.event_id = SL_BT_APP_OTA_DFU_EVT_BTL_STORAGE_INFO_ID;
        ota_event.ota_error_code = SL_BT_APP_OTA_DFU_NO_ERROR;
        ota_event.btl_api_retval = btl_ret_val;
        ota_event.evt_info.btl_storage.bootloader_type = bootloader_info.type;
        ota_event.evt_info.btl_storage.bootloader_ver = bootloader_info.version;
        ota_event.evt_info.btl_storage.storage_size_bytes = slot_info.length;
        ota_event.evt_info.btl_storage.storage_start_addr = slot_info.address;
        sl_bt_app_ota_dfu_on_status_event(&ota_event);
        // Proceed to erase state to erase the storage.
        req_sts = SL_BT_APP_OTA_DFU_ERASE;
      } else {
        // Getting bootloader and storage information failed.
        req_sts = SL_BT_APP_OTA_DFU_ERROR;
        ota_error = SL_BT_APP_OTA_DFU_ERR_BOOTLOADER_API;
      }
      // Change to next state.
      sli_bt_app_ota_dfu_set_main_status(req_sts);
      // Forward state change information to application.
      ota_event.event_id = SL_BT_APP_OTA_DFU_EVT_STATE_CHANGE_ID;
      ota_event.ota_error_code = ota_error;
      ota_event.btl_api_retval = btl_ret_val;
      ota_event.evt_info.sts.status = ota_sts;
      ota_event.evt_info.sts.prev_status = ota_prev_sts;
      sl_bt_app_ota_dfu_on_status_event(&ota_event);
      // Continue execution.
      sli_bt_app_ota_dfu_proceed();
      break;
    ///////////////////////////////////////////////////////////////////////////
    // Erase storage slot if neccessary.                                     //
    ///////////////////////////////////////////////////////////////////////////
    case SL_BT_APP_OTA_DFU_ERASE:
      // Check the download area content by reading it in byte blocks.
      num_blocks = slot_info.length / SL_BT_APP_OTA_DFU_READ_STORAGE_CONTEXT_SIZE;
      // Run through the full storage and read the context in chunks.
      while ((dirty == false) && (offset < SL_BT_APP_OTA_DFU_READ_STORAGE_CONTEXT_SIZE * num_blocks) && (btl_ret_val == BOOTLOADER_OK)) {
        // Actual storage content read.
        btl_ret_val =
          bootloader_readStorage(SL_BT_APP_OTA_DFU_USED_SLOT,
                                 offset,
                                 ota_buff,
                                 SL_BT_APP_OTA_DFU_READ_STORAGE_CONTEXT_SIZE);

        if (btl_ret_val == BOOTLOADER_OK) {
          // Run through the chunk byte-by-byte and check if its empty or not.
          for (i = 0; i < SL_BT_APP_OTA_DFU_READ_STORAGE_CONTEXT_SIZE && !dirty; i++) {
            if (ota_buff[i] != SL_BT_APP_OTA_DFU_EMPTY_FLASH_CONTENT) {
              dirty = true;
            }
          }
          // Proceed with next data chunk.
          offset += SL_BT_APP_OTA_DFU_READ_STORAGE_CONTEXT_SIZE;
        }
      }
      // Check results.
      if (btl_ret_val != BOOTLOADER_OK) {
        // Bootloader API issue during reading flash content.
        ota_error = SL_BT_APP_OTA_DFU_ERR_BOOTLOADER_API;
        req_sts = SL_BT_APP_OTA_DFU_ERROR;
      } else if (dirty) {
        // Storage space is not empty proceed to erase.
        btl_ret_val = bootloader_eraseStorageSlot(SL_BT_APP_OTA_DFU_USED_SLOT);
        if (btl_ret_val != BOOTLOADER_OK) {
          // Failed to erase storage slot.
          ota_error = SL_BT_APP_OTA_DFU_ERR_BOOTLOADER_API;
          req_sts = SL_BT_APP_OTA_DFU_ERROR;
        } else {
          // Storage slot erased successfully.
          // Application OTA DFU component is ready for an OTA process.
          req_sts = SL_BT_APP_OTA_DFU_READY;
        }
      } else {
        // Storage space was originally empty, proceed to next
        req_sts = SL_BT_APP_OTA_DFU_READY;
      }
      // Change to next state.
      sli_bt_app_ota_dfu_set_main_status(req_sts);
      // Forward state change information to application.
      ota_event.event_id = SL_BT_APP_OTA_DFU_EVT_STATE_CHANGE_ID;
      ota_event.ota_error_code = ota_error;
      ota_event.btl_api_retval = btl_ret_val;
      ota_event.evt_info.sts.status = ota_sts;
      ota_event.evt_info.sts.prev_status = ota_prev_sts;
      sl_bt_app_ota_dfu_on_status_event(&ota_event);
      // Continue execution.
      sli_bt_app_ota_dfu_proceed();
      break;
    ///////////////////////////////////////////////////////////////////////////
    // Ready for OTA DFU process.                                            //
    ///////////////////////////////////////////////////////////////////////////
    case SL_BT_APP_OTA_DFU_READY:
      // In ready state the usual business can happen on the application side.
      break;

    ///////////////////////////////////////////////////////////////////////////
    // Disconnected state.                                                   //
    ///////////////////////////////////////////////////////////////////////////
    case SL_BT_APP_OTA_DFU_DISCONNECT:
      // Intentionally left blank.
      break;

    ///////////////////////////////////////////////////////////////////////////
    // Download started state.                                               //
    ///////////////////////////////////////////////////////////////////////////
    case SL_BT_APP_OTA_DFU_DOWNLOAD_BEGIN:
      // Covered in the event handler.
      break;

    ///////////////////////////////////////////////////////////////////////////
    // Download finished state.                                               //
    ///////////////////////////////////////////////////////////////////////////
    case SL_BT_APP_OTA_DFU_DOWNLOAD_END:
      // Covered in the event handler.
      break;

    ///////////////////////////////////////////////////////////////////////////
    // Verify downloaded image.                                              //
    ///////////////////////////////////////////////////////////////////////////
    case SL_BT_APP_OTA_DFU_VERIFY:
      // Verify downloaded bytes of the image.
      btl_ret_val = bootloader_continueVerifyImage(ota_buff, NULL);
      if (btl_ret_val == BOOTLOADER_ERROR_PARSE_CONTINUE) {
        // Verification may continue.
        ota_img_pos += SL_BT_APP_OTA_DFU_VERIFICATION_BLOCK_SIZE;
        // Avoid state change in this case.
        req_sts = ota_sts;
      } else if (btl_ret_val == BOOTLOADER_ERROR_PARSE_SUCCESS) {
        // Last chunk of bytes checked.
        ota_img_pos += SL_BT_APP_OTA_DFU_VERIFICATION_BLOCK_SIZE;
        // Verification finished successfully.
        // This means state change as well.
        req_sts = SL_BT_APP_OTA_DFU_FINALIZE;
        ota_error = SL_BT_APP_OTA_DFU_NO_ERROR;
      } else {
        // Verification process aborted.
        req_sts = SL_BT_APP_OTA_DFU_ERROR;
        ota_error = SL_BT_APP_OTA_DFU_ERR_BOOTLOADER_API;
      }

      // Update mandatory event fields anyways.
      ota_event.event_id = SL_BT_APP_OTA_DFU_EVT_VERIFY_IMAGE_ID;
      ota_event.ota_error_code = ota_error;
      ota_event.btl_api_retval = btl_ret_val;
      // Forward verification status to application.
      ota_event.evt_info.verified_bytes = ota_img_pos;
      sl_bt_app_ota_dfu_on_status_event(&ota_event);

      if (ota_sts != req_sts) {
        // State change is also necessary because
        // error occured or verification finished successfully.
        sli_bt_app_ota_dfu_set_main_status(req_sts);
        ota_event.event_id = SL_BT_APP_OTA_DFU_EVT_STATE_CHANGE_ID;
        ota_event.evt_info.sts.status = req_sts;
        ota_event.evt_info.sts.prev_status = ota_prev_sts;
        sl_bt_app_ota_dfu_on_status_event(&ota_event);
      }
      // Continue execution.
      sli_bt_app_ota_dfu_proceed();
      break;

    ///////////////////////////////////////////////////////////////////////////
    // Finalize Application OTA DFU process and reboot.                      //
    ///////////////////////////////////////////////////////////////////////////
    case SL_BT_APP_OTA_DFU_FINALIZE:
      // Set image for next boot.
      btl_ret_val = bootloader_setImageToBootload(SL_BT_APP_OTA_DFU_USED_SLOT);
      if (btl_ret_val != BOOTLOADER_OK) {
        // Setting Image to boot failed.
        req_sts = SL_BT_APP_OTA_DFU_ERROR;
        ota_error = SL_BT_APP_OTA_DFU_ERR_BOOTLOADER_API;
      } else {
        // Get ready for reboot trigger from user code.
        req_sts = SL_BT_APP_OTA_DFU_WAIT_FOR_REBOOT;
      }
      // Change to next state.
      sli_bt_app_ota_dfu_set_main_status(req_sts);
      // Forward state change information to application.
      ota_event.event_id = SL_BT_APP_OTA_DFU_EVT_STATE_CHANGE_ID;
      ota_event.ota_error_code = ota_error;
      ota_event.btl_api_retval = btl_ret_val;
      ota_event.evt_info.sts.status = ota_sts;
      ota_event.evt_info.sts.prev_status = ota_prev_sts;
      sl_bt_app_ota_dfu_on_status_event(&ota_event);
      break;

    ///////////////////////////////////////////////////////////////////////////
    // Wait for reboot triggering state.                                     //
    ///////////////////////////////////////////////////////////////////////////
    case SL_BT_APP_OTA_DFU_WAIT_FOR_REBOOT:
      // Intentionally left blank.
      break;

    ///////////////////////////////////////////////////////////////////////////
    // Error state.                                                          //
    ///////////////////////////////////////////////////////////////////////////
    case SL_BT_APP_OTA_DFU_ERROR:
      // In error state the device awaits for application intervention.
      break;
  }
}

/**************************************************************************/ /**
 * Weak function to restart application OTA DFU progress without rebooting in
 * case of any error.
 *****************************************************************************/
SL_WEAK void sl_bt_app_ota_dfu_restart_progress(void)
{
  // Re-run init function.
  sl_bt_app_ota_dfu_init();
}

/**************************************************************************/ /**
 * Function to reboot only when the application OTA DFU process finished
 * completely.
 *****************************************************************************/
void sl_bt_app_ota_dfu_reboot(void)
{
  if (ota_sts == SL_BT_APP_OTA_DFU_WAIT_FOR_REBOOT) {
    bootloader_rebootAndInstall();
  }
}

/**************************************************************************/ /**
 * Weak function to indicate Application OTA DFU status and in case of error
 * the error codes for assertion.
 * @param[in] evt Actual app ota dfu event.
 * @note To be implemented in user code.
 *****************************************************************************/
SL_WEAK void sl_bt_app_ota_dfu_on_status_event(sl_bt_app_ota_dfu_status_evt_t *evt)
{
  // Default error handling: trying to recover.
  if (evt->evt_info.sts.status == SL_BT_APP_OTA_DFU_ERROR) {
    // Restart the system, install the image.
    sl_bt_app_ota_dfu_restart_progress();
  } else if (evt->evt_info.sts.status == SL_BT_APP_OTA_DFU_WAIT_FOR_REBOOT) {
    // Default finish action: auto-reboot.
    sl_bt_app_ota_dfu_reboot();
  }
}
