/*******************************************************************************
* @file  rsi_dhcp_user_class.c
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
#include "rsi_dhcp_user_class.h"

/** @addtogroup NETWORK1
* @{
*/
/*==============================================*/
/**
 * @brief      Enable DHCP user class. This is a non-blocking API.
 * @param[in]  mode - DHCP User Class mode \n
 *                    1- RFC Compatible mode \n
 *					  2- Windows Compatible mode
 * @param[in]  count - DHCP User Class count
 * @param[in]  user_class_data - The first byte of this buffer is user class data length and then user class data follows 
 * @return     0              -  Success  \n
 *             Negative Value - Failure \n
 *                         -3 - Command given in wrong state \n
 *                         -4 - Buffer not available to serve the command 
 * @note        Refer to Error Codes section for the description of the above error codes \ref error-codes.
 */

int32_t rsi_dhcp_user_class(uint8_t mode,
                            uint8_t count,
                            user_class_data_t *user_class_data,
                            void (*dhcp_usr_cls_rsp_handler)(uint16_t status))
{
  rsi_dhcp_user_class_t *user_class;
  rsi_pkt_t *pkt;
  int32_t status = RSI_SUCCESS;
  SL_PRINTF(SL_DHCP_USER_CLASS_ENTRY, NETWORK, LOG_INFO);
  uint8_t i = 0;

  // Get WLAN CB structure pointer
  rsi_wlan_cb_t *wlan_cb = rsi_driver_cb->wlan_cb;

  // If state is not in card ready received state
  if ((wlan_cb->state < RSI_WLAN_STATE_CONNECTED)) {
    // Command given in wrong state
    SL_PRINTF(SL_DHCP_USER_CLASS_COMMAND_GIVEN_IN_WRONG_STATE, NETWORK, LOG_ERROR);
    return RSI_ERROR_COMMAND_GIVEN_IN_WRONG_STATE;
  }

  status = rsi_check_and_update_cmd_state(NWK_CMD, IN_USE);
  if (status == RSI_SUCCESS) {
    if (dhcp_usr_cls_rsp_handler != NULL) {
      // Register DHCP client user class response notify callback handler
      rsi_wlan_cb_non_rom->nwk_callbacks.rsi_dhcp_usr_cls_rsp_handler = dhcp_usr_cls_rsp_handler;
    } else {
      // Change NWK state to allow
      rsi_check_and_update_cmd_state(NWK_CMD, ALLOW);
      // Return invalid command error
      SL_PRINTF(SL_DHCP_USER_CLASS_INVALID_PARAM, NETWORK, LOG_ERROR);
      return RSI_ERROR_INVALID_PARAM;
    }

    // Allocate command buffer from WLAN pool
    pkt = rsi_pkt_alloc(&wlan_cb->wlan_tx_pool);

    // If allocation of packet fails
    if (pkt == NULL) {
      // Change NWK state to allow
      rsi_check_and_update_cmd_state(NWK_CMD, ALLOW);
      // Return packet allocation failure error
      SL_PRINTF(SL_DHCP_USER_CLASS_PKT_ALLOCATION_FAILURE, NETWORK, LOG_ERROR);
      return RSI_ERROR_PKT_ALLOCATION_FAILURE;
    }

    user_class = (rsi_dhcp_user_class_t *)pkt->data;

    // Memset the packet data
    memset(&pkt->data, 0, sizeof(rsi_dhcp_user_class_t));

    if (count > RSI_DHCP_USER_CLASS_MAX_COUNT) {
      count = RSI_DHCP_USER_CLASS_MAX_COUNT;
    }
    // Fill user class mode
    user_class->mode = mode;

    // Fill user class count
    user_class->count = count;

    // Fill user class data
    for (i = 0; i < count; i++) {
      // Check for maximum user class data count

      // Copy user class data
      user_class->user_class_data[i].length = user_class_data[i].length;
      memcpy(&user_class->user_class_data[i].data[0], user_class_data[i].data, user_class_data[i].length);
    }

    // Send DHCP user class command
    status = rsi_driver_wlan_send_cmd(RSI_WLAN_REQ_DHCP_USER_CLASS, pkt);

  } else {
    // Return NWK command error
    SL_PRINTF(SL_DHCP_USER_CLASS_COMMAND_ERROR, NETWORK, LOG_ERROR, "status: %4x", status);
    return status;
  }

  // Return status
  SL_PRINTF(SL_DHCP_USER_CLASS_EXIT, NETWORK, LOG_ERROR, "status: %4x", status);
  return status;
}
/** @} */
