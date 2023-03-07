/*******************************************************************************
* @file  rsi_raw_data.c
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

/** @addtogroup NETWORK3
* @{
*/
/*==============================================*/
/**
 * @brief      Send RAW data to Module. This is a blocking API.
 * @param[in]  buffer - Pointer to the buffer to send
 * @param[in]  length - Length of the buffer to send
 * @return     0              -  Success \n
 *             Negative Value -  Failure
 */

#include "rsi_driver.h"
int32_t rsi_send_raw_data(uint8_t *buffer, uint32_t length)
{
  int32_t status = RSI_SUCCESS;
  SL_PRINTF(SL_SEND_RAW_DATA_ENTRY, NETWORK, LOG_INFO);
  uint8_t *host_desc;
  rsi_pkt_t *pkt;

  // If buffer is invalid
  if ((buffer == NULL) || (length == 0)) {
    // Return packet allocation failure error
    SL_PRINTF(SL_SEND_RAW_DATA_INVALID_PARAM, NETWORK, LOG_ERROR);
    return RSI_ERROR_INVALID_PARAM;
  }

  // Allocate packet to send data
  pkt = rsi_pkt_alloc(&rsi_driver_cb->wlan_cb->wlan_tx_pool);

  if (pkt == NULL) {
    // Return packet allocation failure error
    SL_PRINTF(SL_SEND_RAW_DATA_PKT_ALLOCATION_FAILURE, NETWORK, LOG_ERROR);
    return RSI_ERROR_PKT_ALLOCATION_FAILURE;
  }

  // Get host descriptor pointer
  host_desc = pkt->desc;

  // Memset host descriptor
  memset(host_desc, 0, RSI_HOST_DESC_LENGTH);

  // Fill host descriptor
  rsi_uint16_to_2bytes(host_desc, (length & 0xFFF));

  // Fill packet type
  host_desc[1] |= (RSI_WLAN_DATA_Q << 4);

  // Fill frame type
  host_desc[2] = 0x1;

  // Copy data to be sent
  memcpy(pkt->data, buffer, length);

  // Enqueue packet to WLAN TX queue
  rsi_enqueue_pkt(&rsi_driver_cb->wlan_tx_q, pkt);

#ifndef RSI_SEND_SEM_BITMAP
  rsi_driver_cb_non_rom->send_wait_bitmap |= BIT(0);
#endif
  // Set TX packet pending event
  rsi_set_event(RSI_TX_EVENT);

  if (rsi_wait_on_wlan_semaphore(&rsi_driver_cb_non_rom->send_data_sem, RSI_RAW_DATA_RESPONSE_WAIT_TIME)
      != RSI_ERROR_NONE) {
    rsi_wlan_set_status(RSI_ERROR_RESPONSE_TIMEOUT);
    return RSI_ERROR_RESPONSE_TIMEOUT;
  }
  status = rsi_wlan_get_status();

  // Return status
  SL_PRINTF(SL_SEND_RAW_DATA_EXIT, NETWORK, LOG_INFO, "status: %4x", status);
  return status;
}
/** @} */
