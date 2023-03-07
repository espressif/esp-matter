/*******************************************************************************
* @file  rsi_firmware_upgradation.c
* @brief
*******************************************************************************
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

#include "rsi_driver.h"
#include "rsi_firmware_upgradation.h"

/** @addtogroup FIRMWARE
* @{
*/
/*==============================================*/
/**
 * @brief       Helper function for actual APIs
 * @param[in]   type    - Firmware upgrade chunk type
 * @param[in]   content - Firmware content
 * @param[in]   length  - Length of the content
 * @return      Zero    - Success \n
 *              3       -  Firmware upgrade completed successfully \n
 *              Negative Value -  Failure
 */
/// @private
static int32_t rsi_fwup(uint8_t type, uint8_t *content, uint16_t length)
{
  int32_t status = RSI_SUCCESS;
  SL_PRINTF(SL_FWUP_ENTRY, FW_UPDATE, LOG_INFO);
  rsi_pkt_t *pkt;

  // Get WLAN CB structure pointer
  rsi_wlan_cb_t *wlan_cb = rsi_driver_cb->wlan_cb;

  rsi_req_fwup_t *fwup = NULL;

  // Check if length exceeds
  if (length > RSI_MAX_FWUP_CHUNK_SIZE) {
    SL_PRINTF(SL_FWUP_INVALID_PARAM, FW_UPDATE, LOG_ERROR);
    return RSI_ERROR_INVALID_PARAM;
  }

  status = rsi_check_and_update_cmd_state(NWK_CMD, IN_USE);
  if (status == RSI_SUCCESS) {

    // Allocate command buffer from WLAN pool
    pkt = rsi_pkt_alloc(&wlan_cb->wlan_tx_pool);

    // If allocation of packet fails
    if (pkt == NULL) {
      // Change NWK state to allow
      rsi_check_and_update_cmd_state(NWK_CMD, ALLOW);
      // Return packet allocation failure error
      SL_PRINTF(SL_FWUP_PKT_ALLOCATION_FAILURE, FW_UPDATE, LOG_ERROR);
      return RSI_ERROR_PKT_ALLOCATION_FAILURE;
    }

    fwup = (rsi_req_fwup_t *)pkt->data;

    // Fill packet type
    rsi_uint16_to_2bytes(fwup->type, type);

    // Fill packet length
    rsi_uint16_to_2bytes(fwup->length, length);

    // Fill packet content
    memcpy(fwup->content, content, length);

    if (rsi_wlan_cb_non_rom->nwk_callbacks.rsi_wireless_fw_upgrade_handler == NULL) {
#ifndef RSI_NWK_SEM_BITMAP
      rsi_driver_cb_non_rom->nwk_wait_bitmap |= BIT(0);
#endif
    }
    // Send set FTP Create command
    status = rsi_driver_wlan_send_cmd(RSI_WLAN_REQ_FWUP, pkt);

    if (rsi_wlan_cb_non_rom->nwk_callbacks.rsi_wireless_fw_upgrade_handler == NULL) {
      // Wait on NWK semaphore
      rsi_wait_on_nwk_semaphore(&rsi_driver_cb_non_rom->nwk_sem, RSI_FWUP_RESPONSE_WAIT_TIME);

      // Get WLAN/network command response status
      status = rsi_wlan_get_nwk_status();
      // Change NWK state to allow
      rsi_check_and_update_cmd_state(NWK_CMD, ALLOW);
    }
  } else {
    // Return NWK command error
    SL_PRINTF(SL_FWUP_COMMAND_ERROR, FW_UPDATE, LOG_ERROR, "status: %4x", status);
    return status;
  }

  // Return status if error in sending command occurs
  SL_PRINTF(SL_FWUP_EXIT, FW_UPDATE, LOG_INFO, "status: %4x", status);
  return status;
}

/** @} */

/** @addtogroup FIRMWARE
* @{
*/
/*==============================================*/
/**
 * @brief       Send the RPS header content of firmware file. This is a blocking API.
 * @pre         \ref rsi_wlan_radio_init() API needs to be called before this API
 * @param[in]   rps_header     - Pointer to the RPS header content
 * @return      0              - Success \n
 *              Negative Value - Failure \n
 *                          -2 - Invalid Parameters \n
 *                          -4 - Buffer not available to serve the command
 * @note        Refer to Error Codes section for the description of the above error codes \ref error-codes.
 */
int32_t rsi_fwup_start(uint8_t *rps_header)
{
  int32_t status = RSI_SUCCESS;
  SL_PRINTF(SL_FWUP_START_ENTRY, FW_UPDATE, LOG_INFO);

  status = rsi_fwup(RSI_FWUP_RPS_HEADER, rps_header, RSI_RPS_HEADER_SIZE);
  SL_PRINTF(SL_FWUP_START_EXIT, FW_UPDATE, LOG_INFO, "status: %4x", status);
  return status;
}

/** @} */

/** @addtogroup FIRMWARE
* @{
*/
/*==============================================*/
/**
 * @brief      Send the firmware file content. This is a blocking API.
 * @pre        \ref rsi_wlan_radio_init() API needs to be called before this API
 * @param[in]  content         -  Pointer to the firmware file content
 * @param[in]  length          -  Length of the content
 * @return      0              -  Success \n
 *              Negative Value -  Failure \n
 *                          3  -  Firmware upgradate is completed successfully \n
 *                         -2  -  Invalid parameters \n
 *			   -4  -  Buffer not availableto serve the command
 * @note        Refer to Error Codes section for the description of the above error codes \ref error-codes.
 *
 */
int32_t rsi_fwup_load(uint8_t *content, uint16_t length)
{
  int32_t status = RSI_SUCCESS;
  SL_PRINTF(SL_FWUP_LOAD_ENTRY, FW_UPDATE, LOG_INFO);

  status = rsi_fwup(RSI_FWUP_RPS_CONTENT, content, length);
  SL_PRINTF(SL_FWUP_LOAD_EXIT, FW_UPDATE, LOG_INFO, "status: %4x", status);
  return status;
}
/** @} */
