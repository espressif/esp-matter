/*******************************************************************************
* @file  rsi_driver_event_handlers.c
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

/**
 * Includes
 * */

#include "rsi_driver.h"
#ifdef RSI_UART_INTERFACE
#include "rsi_uart.h"
#endif
#ifdef RSI_M4_INTERFACE
#include "rsi_m4.h"
#endif
#ifdef LINUX_PLATFORM
#if (defined(RSI_USB_INTERFACE) || defined(RSI_SDIO_INTERFACE))
#include "rsi_linux_app_init.h"
#endif
#endif
#include "rsi_wlan_non_rom.h"
#include "rsi_sdio.h"
#include "rsi_pkt_mgmt.h"

// Sleep Ack frame
const uint8_t rsi_sleep_ack[RSI_FRAME_DESC_LEN] = { 0x00, 0x40, 0xDE, 0x00, 0x00, 0x00, 0x00, 0x00,
                                                    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };
#if (((defined RSI_SPI_INTERFACE) || (defined RSI_SDIO_INTERFACE)) && (RSI_ASSERT_ENABLE))
#endif
rsi_driver_cb_non_rom_t *rsi_driver_cb_non_rom = NULL;

#define COMMON_PKT          1
#define BT_BLE_STACK_PACKET 2
#define BT_PKT              3
#ifdef RSI_PROP_PROTOCOL_ENABLE
#define PROP_PROTOCOL_PKT 4
#endif
#define ZB_PKT   5
#define WLAN_PKT 6
uint8_t rsi_get_intr_status(void);
/** @addtogroup DRIVER8
* @{
*/
/*==============================================*/
/**
 * @fn          void rsi_tx_event_handler(void)
 * @brief       Retrieve the packet from protocol TX pending queue
 *              and forwards to the module.
 * @param[in]   void 
 * @return      void
 *
 */

void rsi_tx_event_handler(void)
{
  rsi_pkt_t *pkt = NULL;
  uint8_t *buf_ptr;
  uint16_t length = 0;

  //To remove the warning type condition always true, the status variable declared to int16_t.
  int16_t status = 0;
#if ((defined RSI_SPI_INTERFACE) || (defined RSI_M4_INTERFACE) || (defined RSI_SDIO_INTERFACE))
  int16_t ret_status = 0;
  uint8_t int_status = 0;
#endif

  //Get commmon cb pointer
  rsi_common_cb_t *rsi_common_cb = rsi_driver_cb->common_cb;

  uint8_t common_pkt_pending = 0;
  uint8_t bt_pkt_pending     = 0;
#ifdef SAPIS_BT_STACK_ON_HOST
  uint8_t bt_ble_stack_pkt_pending = 0;
#endif
  uint8_t wlan_pkt_pending = 0;
  uint8_t zb_pkt_pending   = 0;
#ifdef RSI_PROP_PROTOCOL_ENABLE
  uint8_t prop_protocol_pkt_pending = 0;
#endif
  uint8_t queueno      = 0xff;
  uint8_t frame_type   = 0x00;
  uint8_t pkt_dequeued = 0xff;

#if ((defined RSI_BT_ENABLE || defined RSI_BLE_ENABLE || defined RSI_PROP_PROTOCOL_ENABLE) \
     && (defined RSI_M4_INTERFACE))
  rsi_bt_cb_t *bt_cb;
#endif

#ifndef RSI_TX_EVENT_HANDLE_TIMER_DISABLE
  if (!rsi_driver_cb_non_rom->driver_timer_start) {
    rsi_init_timer(&rsi_driver_cb_non_rom->timer_start, RSI_TX_EVENT_WAIT_TIME);
    rsi_driver_cb_non_rom->driver_timer_start = 1;
  }
  if ((rsi_timer_expired(&rsi_driver_cb_non_rom->timer_start)) && (rsi_driver_cb_non_rom->driver_timer_start == 1)) {
    rsi_error_timeout_and_clear_events(RSI_ERROR_TX_BUFFER_FULL, TX_EVENT_CMD);
    return;
  }
#endif

#ifdef RSI_M4_INTERFACE

  if (rsi_driver_cb->rsi_tx_done_handler != NULL) {
    pkt = (rsi_pkt_t *)rsi_dequeue_pkt(&rsi_driver_cb->m4_tx_q);
    rsi_driver_cb->rsi_tx_done_handler(pkt);
    rsi_driver_cb->rsi_tx_done_handler = NULL;
    pkt                                = NULL;
  }
#endif
  /* First Check for Common Queue */
  common_pkt_pending = rsi_check_queue_status(&rsi_driver_cb->common_tx_q);
#ifdef SAPIS_BT_STACK_ON_HOST
  // check for packet pending in bt/ble stack common queue
  if ((&rsi_driver_cb->bt_single_tx_q)->head != NULL) {
    bt_ble_stack_pkt_pending = rsi_check_queue_status(&rsi_driver_cb->bt_single_tx_q);
  }

#else
#if (defined RSI_BT_ENABLE || defined RSI_BLE_ENABLE || defined RSI_PROP_PROTOCOL_ENABLE)
  // check for packet pending in bt/ble common queue
  if ((&rsi_driver_cb->bt_single_tx_q)->head != NULL) {
    bt_pkt_pending = rsi_check_queue_status(&rsi_driver_cb->bt_single_tx_q);
  }
#endif
#endif
#ifdef RSI_PROP_PROTOCOL_ENABLE
  // check for packet pending in prop_protocol queue
  if ((&rsi_driver_cb->prop_protocol_tx_q)->head != NULL) {
    prop_protocol_pkt_pending = rsi_check_queue_status(&rsi_driver_cb->prop_protocol_tx_q);
  }
#endif

#ifdef RSI_ZB_ENABLE
  // check for packet pending in zb queue
  zb_pkt_pending = rsi_check_queue_status(&rsi_driver_cb->zigb_tx_q);
#endif

  // check for packet pending in wlan queue
  wlan_pkt_pending = rsi_check_queue_status(&rsi_driver_cb->wlan_tx_q);

  if (rsi_common_cb->power_save.power_save_enable
      && (common_pkt_pending || wlan_pkt_pending || bt_pkt_pending
#ifdef RSI_PROP_PROTOCOL_ENABLE
          || prop_protocol_pkt_pending
#endif
          || zb_pkt_pending)) {
#ifdef RSI_M4_INTERFACE
    P2P_STATUS_REG |= M4_wakeup_TA;
    P2P_STATUS_REG;
    P2P_STATUS_REG;
    P2P_STATUS_REG;
    if (!(P2P_STATUS_REG & TA_is_active)) {
      while (!(P2P_STATUS_REG & TA_is_active))
        ;
    }
#endif
#if (RSI_HAND_SHAKE_TYPE == GPIO_BASED)
    // request wakeup if module is in GPIO_BASED handshake power save
#if (RSI_WMM_PS_ENABLE && RSI_WMM_PS_TYPE)
    status = rsi_wait4wakeup();
    if (status != 0) {
#ifndef RSI_TX_EVENT_HANDLE_TIMER_DISABLE
      rsi_error_timeout_and_clear_events(status, TX_EVENT_CMD);
#endif
      return;
    }
#else
    status = rsi_req_wakeup();
    if (status != 0) {
#ifndef RSI_TX_EVENT_HANDLE_TIMER_DISABLE
      rsi_error_timeout_and_clear_events(status, TX_EVENT_CMD);
#endif
      return;
    }
#endif
#else
#if (RSI_ULP_MODE == 1)
    rsi_ulp_wakeup_init();
#endif
#endif
  }

#if (defined RSI_BT_ENABLE || defined RSI_BLE_ENABLE || defined RSI_PROP_PROTOCOL_ENABLE)
#if ((defined RSI_SPI_INTERFACE) || (defined RSI_M4_INTERFACE) \
     || ((defined RSI_SDIO_INTERFACE) && (!defined LINUX_PLATFORM)))
  if (bt_pkt_pending
#ifdef RSI_PROP_PROTOCOL_ENABLE
      || prop_protocol_pkt_pending
#endif
  ) {
#ifdef RSI_BT_ENABLE
    // Read packet from BT/BLE Common Queue
    pkt = (rsi_pkt_t *)(rsi_driver_cb->bt_single_tx_q.head);

    buf_ptr = (uint8_t *)pkt->desc;

    // Get Frame type
    frame_type = buf_ptr[2];
#endif
    // Read interrupt status register to check buffer full condition
    ret_status = rsi_device_interrupt_status(&int_status);

    // if buffer full then return without clearing Tx event
    if ((ret_status != 0x0)
#ifdef RSI_BT_ENABLE
        || ((int_status & (BIT(1)))
            && (rsi_bt_get_ACL_type(frame_type) == RSI_BT_HCI_ACL_PKT)) /* Check for ACL Packet Buffer full Condition */
#endif
#if ((defined RSI_BT_ENABLE) || (defined RSI_BLE_ENABLE))
        || ((int_status & (BIT(4))) /* Check for Command Pool Buffer full Condition */
#ifdef RSI_BT_ENABLE
            // ACL Packets should not blocked by Command Buffer Full Condition
            && (!(frame_type == RSI_BT_REQ_A2DP_SBC_AAC_DATA) || (frame_type == RSI_BT_REQ_SPP_TRANSFER))
#endif
              )
#endif
#ifdef RSI_BLE_ENABLE
        || (int_status & (BIT(2)))
#endif
    ) {
      if (ret_status != 0x0) {
#ifndef RSI_TX_EVENT_HANDLE_TIMER_DISABLE
        rsi_error_timeout_and_clear_events(ret_status, TX_EVENT_CMD);
#endif
        return;
      }
      bt_pkt_pending = 0;
#ifdef RSI_PROP_PROTOCOL_ENABLE
      if (int_status & BIT(4)) {
        prop_protocol_pkt_pending = 0;
      }
#endif
      // Mask the TX event until RX event is served
      if (rsi_driver_cb->scheduler_cb.event_map & ~(BIT(RSI_TX_EVENT))) {
        // masking the tx event
        rsi_mask_event(RSI_TX_EVENT);
        // lock the mutex
        rsi_mutex_lock(&rsi_driver_cb_non_rom->tx_mutex);
        rsi_driver_cb_non_rom->tx_mask_event = 1;
        // unlock mutex
        rsi_mutex_unlock(&rsi_driver_cb_non_rom->tx_mutex);
      }
    }
  }
#endif
#endif

  if (common_pkt_pending) {
#if ((defined RSI_SPI_INTERFACE) || (defined RSI_M4_INTERFACE) \
     || ((defined RSI_SDIO_INTERFACE) && (!defined LINUX_PLATFORM)))
    rsi_driver_cb->rsi_tx_done_handler = &rsi_common_packet_transfer_done;
    // Read interrupt status register to check buffer full condition
    ret_status = rsi_device_interrupt_status(&int_status);

    // if buffer full then return without clearing Tx event
    if ((ret_status != 0x0) || (int_status & RSI_BUFFER_FULL)) {
      if (ret_status != 0x0) {
#ifndef RSI_TX_EVENT_HANDLE_TIMER_DISABLE
        rsi_error_timeout_and_clear_events(ret_status, TX_EVENT_CMD);
#endif
        return;
      }
      // Mask the TX event until RX event is served
      if (rsi_driver_cb->scheduler_cb.event_map & ~(BIT(RSI_TX_EVENT))) {
        // masking the tx event
        rsi_mask_event(RSI_TX_EVENT);
        // lock the mutex
        rsi_mutex_lock(&rsi_driver_cb_non_rom->tx_mutex);
        rsi_driver_cb_non_rom->tx_mask_event = 1;
        // unlock mutex
        rsi_mutex_unlock(&rsi_driver_cb_non_rom->tx_mutex);
      }
#if BUFFER_FULL_HANDLING
      pkt = (rsi_pkt_t *)rsi_dequeue_pkt(&rsi_driver_cb->common_tx_q);

      rsi_check_common_buffer_full(pkt);
#ifndef RSI_TX_EVENT_HANDLE_TIMER_DISABLE
      rsi_driver_cb_non_rom->driver_timer_start = 0;
#endif
      // if fail , return or
      // if buffer full then return without clearing Tx event
      return;
#else
      // if buffer full then return without clearing Tx event
      return;
#endif
    }
#endif

    // if packet pending dequeue the packet from common queue
    pkt = (rsi_pkt_t *)rsi_dequeue_pkt(&rsi_driver_cb->common_tx_q);

    pkt_dequeued = COMMON_PKT;
  }
#ifdef BT_STACK_ON_FREERTOS
  else if (bt_ble_stack_pkt_pending) {
#ifdef RSI_M4_INTERFACE
    rsi_driver_cb->rsi_tx_done_handler = &bt_tx_pkt_free;
#endif
    // if packet pending dequeue the packet from bt/ble common queue
    pkt          = (rsi_pkt_t *)rsi_dequeue_pkt(&rsi_driver_cb->bt_single_tx_q);
    pkt_dequeued = BT_BLE_STACK_PACKET;
  }
#else
#if (defined RSI_BT_ENABLE || defined RSI_BLE_ENABLE || defined RSI_PROP_PROTOCOL_ENABLE)
  else if (bt_pkt_pending) {
    // if packet pending dequeue the packet from bt/ble common queue
    pkt          = (rsi_pkt_t *)rsi_dequeue_pkt(&rsi_driver_cb->bt_single_tx_q);
    pkt_dequeued = BT_PKT;

#ifdef RSI_M4_INTERFACE
    rsi_bt_get_proto_type(rsi_bytes2R_to_uint16(((uint8_t *)(pkt->desc)) + RSI_BT_RSP_TYPE_OFFSET), &bt_cb);
    rsi_driver_cb->rsi_tx_done_handler = &rsi_bt_common_tx_done;
#endif
  }
#endif
#endif
#ifdef RSI_PROP_PROTOCOL_ENABLE
  else if (prop_protocol_pkt_pending) {
    // if packet pending dequeue the packet from prop_protocol queue
    pkt          = (rsi_pkt_t *)rsi_dequeue_pkt(&rsi_driver_cb->prop_protocol_tx_q);
    pkt_dequeued = PROP_PROTOCOL_PKT;
#ifdef RSI_M4_INTERFACE
    rsi_bt_get_proto_type(rsi_bytes2R_to_uint16(((uint8_t *)(pkt->desc)) + RSI_BT_RSP_TYPE_OFFSET), &bt_cb);
    rsi_driver_cb->rsi_tx_done_handler = &rsi_prop_protocol_common_tx_done;
#endif
  }
#endif
#ifdef RSI_ZB_ENABLE
  else if (zb_pkt_pending) {
#ifdef RSI_M4_INTERFACE
    rsi_driver_cb->rsi_tx_done_handler = &rsi_zb_tx_done;
#endif
    // if packet pending dequeue the packet from zb queue
    pkt          = (rsi_pkt_t *)rsi_dequeue_pkt(&rsi_driver_cb->zigb_tx_q);
    pkt_dequeued = ZB_PKT;
  }
#endif
  else if (wlan_pkt_pending) {
#ifdef RSI_WLAN_ENABLE
#if ((defined RSI_SPI_INTERFACE) || (defined RSI_M4_INTERFACE) \
     || ((defined RSI_SDIO_INTERFACE) && (!defined LINUX_PLATFORM)))
    // Read interrupt status register to check buffer full condition
    ret_status = rsi_device_interrupt_status(&int_status);

    // if buffer full then return without clearing Tx event
    if ((ret_status != 0x0) || (int_status & RSI_BUFFER_FULL)) {
      if (ret_status != 0x0) {
#ifndef RSI_TX_EVENT_HANDLE_TIMER_DISABLE
        rsi_error_timeout_and_clear_events(ret_status, TX_EVENT_CMD);
#endif
        return;
      }
      // Mask the TX event until RX event is served
      if (rsi_driver_cb->scheduler_cb.event_map & ~(BIT(RSI_TX_EVENT))) {
        // masking the tx event
        rsi_mask_event(RSI_TX_EVENT);
        // lock the mutex
        rsi_mutex_lock(&rsi_driver_cb_non_rom->tx_mutex);
        rsi_driver_cb_non_rom->tx_mask_event = 1;
        // unlock mutex
        rsi_mutex_unlock(&rsi_driver_cb_non_rom->tx_mutex);
      }

#if BUFFER_FULL_HANDLING
      pkt = (rsi_pkt_t *)rsi_dequeue_pkt(&rsi_driver_cb->wlan_tx_q);
#ifndef RSI_TX_EVENT_HANDLE_TIMER_DISABLE
      rsi_driver_cb_non_rom->driver_timer_start = 0;
#endif
      rsi_check_wlan_buffer_full(pkt);
      // if fail , return or
      // if buffer full then return without clearing Tx event
      return;
#else
      // if buffer full then return without clearing Tx event
      return;
#endif
    }

#endif
    // dequeue the packet from wlan queue
    pkt          = (rsi_pkt_t *)rsi_dequeue_pkt(&rsi_driver_cb->wlan_tx_q);
    pkt_dequeued = WLAN_PKT;
#ifdef RSI_M4_INTERFACE
    rsi_driver_cb->rsi_tx_done_handler = &rsi_wlan_packet_transfer_done;
#endif
#endif
  }

  // Check any packet pending in protocol queue
#ifdef BT_STACK_ON_FREERTOS
  if (common_pkt_pending || wlan_pkt_pending || zb_pkt_pending || bt_ble_stack_pkt_pending)
#else
  if (common_pkt_pending || wlan_pkt_pending || bt_pkt_pending
#ifdef RSI_PROP_PROTOCOL_ENABLE
      || prop_protocol_pkt_pending
#endif
      || zb_pkt_pending)
#endif
  {
#ifdef RSI_M4_INTERFACE
    if (rsi_driver_cb->rsi_tx_done_handler) {
      rsi_enqueue_pkt(&rsi_driver_cb->m4_tx_q, pkt);
    }
#endif
    buf_ptr = (uint8_t *)pkt->desc;

    // Get Frame type
    frame_type = buf_ptr[2];

    // Get protocol type
    queueno = (buf_ptr[1] & 0xf0) >> 4;

    // Get packet length
    length = (*(uint16_t *)buf_ptr & 0x0FFF);
    if ((frame_type == RSI_COMMON_REQ_PWRMODE) && (queueno == RSI_WLAN_MGMT_Q)) {
      // Mask Tx events while sending pwr mode req
      rsi_mask_event(RSI_TX_EVENT);

      // Adding small delay
      rsi_delay_ms(3);
    }
#ifdef RSI_M4_INTERFACE
    rsi_mask_event(RSI_TX_EVENT);
#endif
    // Writing to Module
    status = rsi_frame_write((rsi_frame_desc_t *)buf_ptr, &buf_ptr[RSI_HOST_DESC_LENGTH], length);
    if (status < 0x0) {
#ifndef RSI_FREE_QUEUE_PKT
      rsi_free_queue_pkt(pkt_dequeued, pkt);
#endif
#ifndef RSI_TX_EVENT_HANDLE_TIMER_DISABLE
      rsi_error_timeout_and_clear_events(status, TX_EVENT_CMD);

#endif
      SL_PRINTF(SL_RSI_ERROR_TIMEOUT,
                BLUETOOTH,
                LOG_ERROR,
                "STATUS: %4x, length: %2x, queue_number: %1x",
                status,
                length,
                queueno);
      SL_PRINTF(SL_RSI_ERROR_TIMEOUT_FRAME_TYPE, BLUETOOTH, LOG_ERROR, "Frame_type: %1x", frame_type);

      return;
    }
#ifndef RSI_TX_EVENT_HANDLE_TIMER_DISABLE
    rsi_driver_cb_non_rom->driver_timer_start = 0;
#endif
#ifdef RSI_M4_INTERFACE
    if ((queueno == RSI_WLAN_MGMT_Q) && (frame_type == RSI_COMMON_REQ_SOFT_RESET)) {
      rsi_config_m4_dma_desc_on_reset();
    }
    if (pkt_dequeued == COMMON_PKT) {
      // free the packet
      rsi_pkt_free(&rsi_driver_cb->common_cb->common_tx_pool, pkt);
      // signal semaphore incase of packet having async response
      rsi_common_packet_transfer_done(pkt);
    }
    return;
#endif
#ifndef RSI_M4_INTERFACE
    if (pkt_dequeued == COMMON_PKT) {
      // free the packet
      rsi_pkt_free(&rsi_driver_cb->common_cb->common_tx_pool, pkt);
      // signal semaphore incase of packet having async response
      rsi_common_packet_transfer_done(pkt);
    } else
#endif
      if (pkt_dequeued == WLAN_PKT) {
#ifdef RSI_WLAN_ENABLE
      if ((queueno == RSI_WLAN_MGMT_Q) || (queueno == RSI_WLAN_DATA_Q)) {
        // signal semaphore incase of packet having async response
        rsi_wlan_packet_transfer_done(pkt);
      }
#endif
    } else if (pkt_dequeued == BT_PKT) {
#if (defined RSI_BT_ENABLE || defined RSI_BLE_ENABLE || defined RSI_PROP_PROTOCOL_ENABLE)
      if (queueno == RSI_BT_Q) {
        rsi_bt_common_tx_done(pkt);
      }
#endif
    }
#ifdef RSI_PROP_PROTOCOL_ENABLE
    else if (pkt_dequeued == PROP_PROTOCOL_PKT) {
      if (queueno == RSI_BT_Q) {
        rsi_prop_protocol_common_tx_done(pkt);
      }
    }
#endif
#ifdef BT_STACK_ON_FREERTOS
    else if (pkt_dequeued == BT_BLE_STACK_PACKET) {
      if ((queueno == RSI_BT_INT_MGMT_Q) || (queueno == RSI_BT_HCI_Q)) {
        int_status = bt_tx_pkt_free(pkt);

        if (int_status != 0)
          breakpt();
      }
    }
#endif
    else if (pkt_dequeued == ZB_PKT) {
#ifdef RSI_ZB_ENABLE
      if (queueno == RSI_ZB_Q) {
        rsi_zb_tx_done(pkt);
      }
#endif
    }
  } else {

#if (defined RSI_BT_ENABLE || defined RSI_BLE_ENABLE || defined RSI_PROP_PROTOCOL_ENABLE)
    /* REVIEW : reviewed by siva. */
    if ((&rsi_driver_cb->bt_single_tx_q)->head != NULL) {
      bt_pkt_pending = rsi_check_queue_status(&rsi_driver_cb->bt_single_tx_q);

      if (bt_pkt_pending) {
#if ((defined RSI_SPI_INTERFACE) || (defined RSI_M4_INTERFACE) \
     || ((defined RSI_SDIO_INTERFACE) && (!defined LINUX_PLATFORM)))
        // Read interrupt status register to check buffer full condition
        ret_status = rsi_device_interrupt_status(&int_status);

        // if buffer full then return without clearing Tx event
        if ((ret_status != 0x0)
#ifdef RSI_BT_ENABLE
            || ((int_status & (BIT(1)))
                && (rsi_bt_get_ACL_type(frame_type)
                    == RSI_BT_HCI_ACL_PKT)) /* Check for ACL Packet Buffer full Condition */
#endif
#if ((defined RSI_BT_ENABLE) || (defined RSI_BLE_ENABLE))
            || ((int_status & (BIT(4))) /* Check for Command Pool Buffer full Condition */
#ifdef RSI_BT_ENABLE
                // ACL Packets should not blocked by Command Buffer Full Condition
                && (!(frame_type == RSI_BT_REQ_A2DP_SBC_AAC_DATA) || (frame_type == RSI_BT_REQ_SPP_TRANSFER))
#endif
                  )
#endif
#ifdef RSI_BLE_ENABLE
            || (int_status & (BIT(2)))
#endif
        ) {
          //do nothing
        } else
#endif
        {
          return;
        }
      }
    }
#ifdef RSI_PROP_PROTOCOL_ENABLE
    if ((&rsi_driver_cb->prop_protocol_tx_q)->head != NULL) {
      prop_protocol_pkt_pending = rsi_check_queue_status(&rsi_driver_cb->prop_protocol_tx_q);

      if (prop_protocol_pkt_pending) {
        return;
      }
    }
#endif
#endif
#ifndef RSI_TX_EVENT_HANDLE_TIMER_DISABLE
    rsi_driver_cb_non_rom->driver_timer_start = 0;
#endif

    if (rsi_common_cb->power_save.power_save_enable) {
#if (RSI_HAND_SHAKE_TYPE == GPIO_BASED)

      // Keep Sleep confirm GPIO low
      rsi_allow_sleep();

#elif defined(RSI_M4_INTERFACE) && (RSI_HAND_SHAKE_TYPE == M4_BASED)
      rsi_allow_sleep();
#elif (RSI_HAND_SHAKE_TYPE == MSG_BASED)

      if (rsi_common_cb->power_save.module_state == RSI_SLP_RECEIVED) {
        // Send ACK if POWERMODE 3 and 9,incase of powermode 2 and 8 make GPIO low
        if (rsi_frame_write((rsi_frame_desc_t *)rsi_sleep_ack, NULL, 0)) {
          // Handle failure
        }
        rsi_mask_event(RSI_TX_EVENT);
        rsi_common_cb->power_save.module_state = RSI_SLP_ACK_SENT;
      }
#endif
    }

#ifndef RSI_M4_INTERFACE
    // signal semaphore incase of packet having async response
    rsi_common_packet_transfer_done(pkt);
#endif
    rsi_clear_event(RSI_TX_EVENT);
  }
}
/** @} */

/** @addtogroup DRIVER5
* @{
*/

/*==============================================*/
/**
 * @fn         void rsi_rx_event_handler(void)
 * @brief      Read the packet from module and process RX packet.
 * @param[in]  void  
 * @return     void 
 *
 */

void rsi_rx_event_handler(void)
{
  uint8_t queue_no;
  uint8_t frame_type;
  uint16_t status    = 0;
  uint8_t *buf_ptr   = NULL;
  rsi_pkt_t *rx_pkt  = NULL;
  uint8_t int_status = 0;

#if RSI_ASSERT_ENABLE
#if ((defined RSI_SPI_INTERFACE) || ((defined RSI_SDIO_INTERFACE) && (!defined LINUX_PLATFORM)))
  uint32_t assert_val        = 0;
  uint32_t assert_intr_clear = 0;
#endif
#endif
#if ((defined RSI_SDIO_INTERFACE) && (!defined LINUX_PLATFORM))
  uint32_t actual_offset = 0;
#endif
#ifndef RSI_M4_INTERFACE
  uint16_t ret_status = 0;
#endif

  //Get commmon cb pointer
  rsi_common_cb_t *rsi_common_cb = rsi_driver_cb->common_cb;

  //rsi_wlan_cb variable is not so to avoid unused variable warning the below code is commented

  //#if (((defined RSI_SPI_INTERFACE) || (defined RSI_SDIO_INTERFACE)))
  // Get wlan cb structure pointer
  //  rsi_wlan_cb_t *rsi_wlan_cb = rsi_driver_cb->wlan_cb;
  //#endif

#ifndef RSI_RX_EVENT_HANDLE_TIMER_DISABLE
  if (!rsi_driver_cb_non_rom->driver_rx_timer_start) {
    rsi_init_timer(&rsi_driver_cb_non_rom->rx_timer_start, RSI_RX_EVENT_WAIT_TIME);
    rsi_driver_cb_non_rom->driver_rx_timer_start = 1;
  }
  if ((rsi_timer_expired(&rsi_driver_cb_non_rom->rx_timer_start))
      && (rsi_driver_cb_non_rom->driver_rx_timer_start == 1)) {
    rsi_error_timeout_and_clear_events(RSI_ERROR_RESPONSE_TIMEOUT, RX_EVENT_CMD);
    return;
  }
#endif
#if ((defined RSI_SPI_INTERFACE) || ((defined RSI_SDIO_INTERFACE) && (!defined LINUX_PLATFORM)))
#if ((defined RSI_SPI_INTERFACE) || (defined RSI_SDIO_INTERFACE))
  if (!rsi_get_intr_status()) {

    rsi_clear_event(RSI_RX_EVENT);
    rsi_hal_intr_unmask();
    if (rsi_get_intr_status()) {
      // Set RX event
      rsi_set_event(RSI_RX_EVENT);
    }
    if (rsi_driver_cb_non_rom->tx_mask_event == 1) {
      rsi_unmask_event(RSI_TX_EVENT);
      // lock the mutex
      rsi_mutex_lock(&rsi_driver_cb_non_rom->tx_mutex);
      rsi_driver_cb_non_rom->tx_mask_event = 0;
      // unlock mutex
      rsi_mutex_unlock(&rsi_driver_cb_non_rom->tx_mutex);
    }
#ifndef RSI_RX_EVENT_HANDLE_TIMER_DISABLE
    rsi_driver_cb_non_rom->driver_rx_timer_start = 0;
#endif
    return;
  }
#endif
  if (rsi_common_cb->power_save.power_save_enable) {
#if ((defined RSI_SPI_INTERFACE) || (defined RSI_SDIO_INTERFACE))

#if (RSI_HAND_SHAKE_TYPE == GPIO_BASED)
    // request wakeup if module is in GPIO_BASED handshake power save
#if (RSI_WMM_PS_ENABLE && RSI_WMM_PS_TYPE)
    status = rsi_wait4wakeup();
    if (status != 0) {
#ifndef RSI_RX_EVENT_HANDLE_TIMER_DISABLE
      rsi_error_timeout_and_clear_events(status, RX_EVENT_CMD);
#endif
      return;
    }
#else
    status = rsi_req_wakeup();
    if (status != 0) {
#ifndef RSI_RX_EVENT_HANDLE_TIMER_DISABLE
      rsi_error_timeout_and_clear_events(status, RX_EVENT_CMD);
#endif
      return;
    }
    //rsi_delay_ms(5);
#endif
#elif (RSI_HAND_SHAKE_TYPE == MSG_BASED)
#if (RSI_SELECT_LP_OR_ULP_MODE != RSI_LP_MODE)
#ifdef RSI_SPI_INTERFACE
    rsi_ulp_wakeup_init();
#endif
#endif
#endif
#endif
  }
#if (RSI_SPI_DUP_INTR_HANDLE || RSI_ASSERT_ENABLE)
  ret_status = rsi_device_interrupt_status(&int_status);
  if (ret_status != 0x0) {
    // if SPI fail then return
#ifndef RSI_RX_EVENT_HANDLE_TIMER_DISABLE
    rsi_error_timeout_and_clear_events(ret_status, RX_EVENT_CMD);
#endif
    return;
  }
#endif //! (RSI_SPI_DUP_INTR_HANDLE || RSI_ASSERT_ENABLE)
#if RSI_ASSERT_ENABLE
  // Check for assertion interrupt
  if (int_status & RSI_ASSERT_INTR) {
    int_status &= (~RSI_ASSERT_INTR);
    ret_status = rsi_mem_rd(RSI_ASSERT_VAL_RD_REG, 4, (uint8_t *)&assert_val);
    if (ret_status == RSI_SUCCESS) {
      if (rsi_wlan_cb_non_rom->callback_list.rsi_assertion_cb != NULL) {
        // Call asynchronous response handler to indicate to host
        rsi_wlan_cb_non_rom->callback_list.rsi_assertion_cb(assert_val, NULL, 0);
      }
      assert_intr_clear = RSI_ASSERT_INTR;
      ret_status        = rsi_mem_wr(RSI_ASSERT_INTR_CLR_REG, 4, (uint8_t *)&assert_intr_clear);
    }
    if (ret_status != 0x0) {
      // if SPI fail then return
#ifndef RSI_RX_EVENT_HANDLE_TIMER_DISABLE
      rsi_error_timeout_and_clear_events(ret_status, RX_EVENT_CMD);
#endif
      return;
    }
  }
#endif //! (RSI_ASSERT_ENABLE)
  if (!(int_status & RSI_RX_PKT_PENDING)) {
    rsi_clear_event(RSI_RX_EVENT);
    rsi_hal_intr_unmask();
#if ((defined RSI_SPI_INTERFACE) || (defined RSI_SDIO_INTERFACE))
    if (rsi_get_intr_status()) {
      // Set RX event
      rsi_set_event(RSI_RX_EVENT);
    }
#endif

#if ((defined RSI_SPI_INTERFACE) || (defined RSI_SDIO_INTERFACE))
    if (rsi_driver_cb_non_rom->tx_mask_event == 1) {
      rsi_unmask_event(RSI_TX_EVENT);
      // lock the mutex
      rsi_mutex_lock(&rsi_driver_cb_non_rom->tx_mutex);
      rsi_driver_cb_non_rom->tx_mask_event = 0;
      // unlock mutex
      rsi_mutex_unlock(&rsi_driver_cb_non_rom->tx_mutex);
    }
#endif
    if (rsi_common_cb->power_save.power_save_enable) {
#if (RSI_HAND_SHAKE_TYPE == GPIO_BASED)
      rsi_allow_sleep();
#endif
    }
#ifndef RSI_RX_EVENT_HANDLE_TIMER_DISABLE
    rsi_driver_cb_non_rom->driver_rx_timer_start = 0;
#endif
    return;
  }
#endif
#if ((defined RSI_SDIO_INTERFACE) && (!defined LINUX_PLATFORM))
#else
#ifndef RSI_M4_INTERFACE
  // Allocate packet to receive packet from module
  rx_pkt = rsi_pkt_alloc(&rsi_driver_cb->rx_pool);
  if (rx_pkt == NULL) {
    // rx_pkt not available mask rx event
#ifndef RSI_ENABLE_DEMOS
    rsi_mask_event(RSI_RX_EVENT);
#endif
    return;
  }

  buf_ptr = (uint8_t *)rx_pkt->desc;

#endif
#endif

#ifdef RSI_M4_INTERFACE
  rx_pkt  = (rsi_pkt_t *)rsi_frame_read();
  buf_ptr = (uint8_t *)&rx_pkt->desc[0];
#elif ((defined RSI_SDIO_INTERFACE) && (!defined LINUX_PLATFORM))
  status  = (uint16_t)rsi_frame_read(&rsi_driver_cb_non_rom->sdio_read_buff[SIZE_OF_HEADROOM]);
#else
  // Read packet from module
  status = rsi_frame_read(buf_ptr);
#endif
  // Allow Power save again
  if (rsi_common_cb->power_save.power_save_enable) {
#if (RSI_HAND_SHAKE_TYPE == GPIO_BASED)
    rsi_allow_sleep();
#endif
  }
  if (status) {
#ifdef LINUX_PLATFORM
#ifdef RSI_UART_INTERFACE
    if (!rsi_check_queue_status(&rsi_linux_app_cb.rcv_queue))
#endif
#if (defined(RSI_USB_INTERFACE) || defined(RSI_SDIO_INTERFACE))
      if (rsi_linux_driver_app_cb.rcv_queue.pending_pkt_count == 0)
#endif
#endif
        rsi_clear_event(RSI_RX_EVENT);
#if ((defined RSI_SDIO_INTERFACE) && (!defined LINUX_PLATFORM))
// rx_pkt is not from pool, so not freeing
#else
    rsi_pkt_free(&rsi_driver_cb->rx_pool, rx_pkt);
#endif
#ifndef RSI_M4_INTERFACE
    rsi_hal_intr_unmask();
#if ((defined RSI_SPI_INTERFACE) || (defined RSI_SDIO_INTERFACE))
    if (rsi_get_intr_status()) {
      // Set RX event
      rsi_set_event(RSI_RX_EVENT);
    }
#endif
#endif
#if ((defined RSI_SPI_INTERFACE) || (defined RSI_M4_INTERFACE) || (defined RSI_SDIO_INTERFACE))
    if (rsi_driver_cb_non_rom->tx_mask_event == 1) {
      rsi_unmask_event(RSI_TX_EVENT);
      // lock the mutex
      rsi_mutex_lock(&rsi_driver_cb_non_rom->tx_mutex);
      rsi_driver_cb_non_rom->tx_mask_event = 0;
      // unlock mutex
      rsi_mutex_unlock(&rsi_driver_cb_non_rom->tx_mutex);
    }
#endif
#ifndef RSI_RX_EVENT_HANDLE_TIMER_DISABLE
    rsi_driver_cb_non_rom->driver_rx_timer_start = 0;
#endif
    SL_PRINTF(SL_RSI_ERROR_TIMEOUT_READ, BLUETOOTH, LOG_ERROR, "STATUS: %4x", status);

    return;
  }

#if ((defined RSI_SDIO_INTERFACE) && (!defined LINUX_PLATFORM))
  actual_offset = rsi_driver_cb_non_rom->sdio_read_buff[2 + SIZE_OF_HEADROOM];
  actual_offset |= (rsi_driver_cb_non_rom->sdio_read_buff[3 + SIZE_OF_HEADROOM] << 8);
  buf_ptr = (uint8_t *)&rsi_driver_cb_non_rom->sdio_read_buff[actual_offset + SIZE_OF_HEADROOM];
  rx_pkt  = (rsi_pkt_t *)(buf_ptr - 4 - SIZE_OF_HEADROOM);
#endif
#ifndef RSI_RX_EVENT_HANDLE_TIMER_DISABLE
  rsi_driver_cb_non_rom->driver_rx_timer_start = 0;
#endif
  if (rsi_driver_cb_non_rom->rx_driver_flag) {
    rsi_driver_cb_non_rom->rx_driver_flag = 0;
  }
  // Extract the queue number from the receivec frame
  queue_no = ((buf_ptr[1] & 0xF0) >> 4);

  // Extract the frame type from the received frame
  frame_type = *(uint8_t *)(buf_ptr + RSI_RESP_OFFSET);
#if (RSI_HAND_SHAKE_TYPE == GPIO_BASED)
  rsi_handle_slp_wkp(frame_type);
#endif
#if (RSI_HAND_SHAKE_TYPE != GPIO_BASED)
  if ((queue_no == RSI_WLAN_MGMT_Q)
      && ((frame_type == RSI_COMMON_RSP_ULP_NO_RAM_RETENTION) || (frame_type == RSI_RSP_SLP)
          || (frame_type == RSI_RSP_WKP))) {
    rsi_handle_slp_wkp(frame_type);
#ifndef RSI_M4_INTERFACE
#if ((defined RSI_SDIO_INTERFACE) && (!defined LINUX_PLATFORM))
#else
    rsi_pkt_free(&rsi_driver_cb->rx_pool, rx_pkt);
#endif
#ifdef LINUX_PLATFORM
#if RSI_UART_INTERFACE
    if (!rsi_check_queue_status(&rsi_linux_app_cb.rcv_queue))
#endif
#if (defined(RSI_USB_INTERFACE) || defined(RSI_SDIO_INTERFACE))
      if (rsi_linux_driver_app_cb.rcv_queue.pending_pkt_count == 0)
#endif
#endif
        rsi_clear_event(RSI_RX_EVENT);
    // Unmask RX event
    rsi_unmask_event(RSI_RX_EVENT);

    rsi_hal_intr_unmask();
#if ((defined RSI_SPI_INTERFACE) || (defined RSI_M4_INTERFACE) || (defined RSI_SDIO_INTERFACE))
    if (rsi_driver_cb_non_rom->tx_mask_event == 1) {
      rsi_unmask_event(RSI_TX_EVENT);
      // lock the mutex
      rsi_mutex_lock(&rsi_driver_cb_non_rom->tx_mutex);
      rsi_driver_cb_non_rom->tx_mask_event = 0;
      // unlock mutex
      rsi_mutex_unlock(&rsi_driver_cb_non_rom->tx_mutex);
    }
#endif
    return;
#endif
  }
#endif
  if (((queue_no == RSI_WLAN_MGMT_Q)
         && ((frame_type == RSI_COMMON_RSP_CARDREADY) || (frame_type == RSI_COMMON_RSP_OPERMODE)
             || (frame_type == RSI_COMMON_RSP_PWRMODE) || (frame_type == RSI_COMMON_RSP_ANTENNA_SELECT)
             || (frame_type == RSI_COMMON_RSP_FEATURE_FRAME) || (frame_type == RSI_COMMON_RSP_SOFT_RESET)
             || (frame_type == RSI_COMMON_REQ_UART_FLOW_CTRL_ENABLE) || (frame_type == RSI_COMMON_RSP_FW_VERSION)
             || (frame_type == RSI_COMMON_RSP_DEBUG_LOG) || (frame_type == RSI_COMMON_RSP_SWITCH_PROTO)
             || (frame_type == RSI_COMMON_RSP_GET_RAM_DUMP) || (frame_type == RSI_COMMON_RSP_SET_RTC_TIMER)
             || (frame_type == RSI_COMMON_RSP_GET_RTC_TIMER) || (frame_type == RSI_COMMON_REQ_SET_CONFIG)
#ifdef FW_LOGGING_ENABLE
             || (frame_type == RSI_COMMON_RSP_DEVICE_LOGGING_INIT)
#endif
#ifdef RSI_ASSERT_API
             || (frame_type == RSI_COMMON_RSP_ASSERT)
#endif
#ifdef RSI_M4_INTERFACE
             || (frame_type == RSI_COMMON_RSP_TA_M4_COMMANDS)
#endif
#ifdef RSI_WAC_MFI_ENABLE
             || (frame_type == RSI_COMMON_RSP_IAP_INIT) || (frame_type == RSI_COMMON_RSP_IAP_GET_CERTIFICATE)
             || (frame_type == RSI_COMMON_RSP_IAP_GENERATE_SIGATURE)
#endif
#ifdef RSI_PUF_ENABLE
             || (frame_type == RSI_COMMON_RSP_PUF_DIS_ENROLL) || (frame_type == RSI_COMMON_RSP_PUF_DIS_GET_KEY)
             || (frame_type == RSI_COMMON_RSP_PUF_DIS_SET_KEY) || (frame_type == RSI_COMMON_RSP_PUF_ENROLL)
             || (frame_type == RSI_COMMON_RSP_PUF_GET_KEY) || (frame_type == RSI_COMMON_RSP_PUF_LOAD_KEY)
             || (frame_type == RSI_COMMON_RSP_PUF_SET_KEY) || (frame_type == RSI_COMMON_RSP_PUF_START)
             || (frame_type == RSI_COMMON_RSP_AES_DECRYPT) || (frame_type == RSI_COMMON_RSP_AES_ENCRYPT)
             || (frame_type == RSI_COMMON_RSP_AES_MAC) || (frame_type == RSI_COMMON_RSP_PUF_INTR_KEY)
#endif
#ifdef RSI_CRYPTO_ENABLE
             || (frame_type == RSI_RSP_ENCRYPT_CRYPTO)
#endif
               )
#ifdef CONFIGURE_GPIO_FROM_HOST
       || (frame_type == RSI_COMMON_RSP_GPIO_CONFIG)
#endif
         )
#ifdef FW_LOGGING_ENABLE
      || ((queue_no == RSI_SL_LOG_DATA_Q) && (frame_type == RSI_COMMON_RSP_DEVICE_LOGGING_INIT))
#endif
  ) {
    // Process common packet
    rsi_driver_process_common_recv_cmd(rx_pkt);
  }
#ifdef SAPIS_BT_STACK_ON_HOST
  else if ((queue_no == RSI_BT_INT_MGMT_Q) || (queue_no == RSI_BT_HCI_Q) || (queue_no == RSI_ZB_Q))
#else
  else if ((queue_no == RSI_BT_Q) || (queue_no == RSI_ZB_Q))
#endif
  {
    // Process BT/ZB packet
#ifdef RSI_ZB_ENABLE
    rsi_driver_process_zigb_recv_cmd(rx_pkt);
#endif
#if (defined RSI_BT_ENABLE || defined RSI_BLE_ENABLE || defined RSI_PROP_PROTOCOL_ENABLE)
#ifdef SAPIS_BT_STACK_ON_HOST
    if ((queue_no == RSI_BT_INT_MGMT_Q) || (queue_no == RSI_BT_HCI_Q)) {

      //Post Packet to BT Stack
      frame_from_module_to_bt_stack(rx_pkt);
    } else
#endif
    {
      rsi_driver_process_bt_resp_handler(rx_pkt);
    }
#endif
  }
#ifdef RSI_WLAN_ENABLE
  else if (queue_no == RSI_WLAN_MGMT_Q) {
    // Process WLAN packet
    rsi_driver_process_wlan_recv_cmd(rx_pkt);
  } else if (queue_no == RSI_WLAN_DATA_Q) {
#if (RSI_TCP_IP_BYPASS || TCP_IP_ETHERNET_BRIDGE)
    // Process Raw DATA packet
    rsi_wlan_process_raw_data(rx_pkt);
#else
    // Process DATA packet
    rsi_driver_process_recv_data(rx_pkt);
#endif
  }
#endif

#if ((defined RSI_SDIO_INTERFACE) && (!defined LINUX_PLATFORM))
#else
  // Free the packet after processing
  rsi_pkt_free(&rsi_driver_cb->rx_pool, rx_pkt);
#endif
#ifdef LINUX_PLATFORM
#ifdef RSI_UART_INTERFACE
  if (!rsi_check_queue_status(&rsi_linux_app_cb.rcv_queue))
#endif
#if (defined(RSI_USB_INTERFACE) || defined(RSI_SDIO_INTERFACE))
    if (rsi_linux_driver_app_cb.rcv_queue.pending_pkt_count == 0)
#endif
#endif
#ifdef RSI_M4_INTERFACE

      mask_ta_interrupt(RX_PKT_TRANSFER_DONE_INTERRUPT);

  if (!rsi_check_queue_status(&global_cb_p->rsi_driver_cb->m4_rx_q))
#endif
#if ((defined RSI_SPI_INTERFACE) || (defined RSI_SDIO_INTERFACE))
    if (!rsi_get_intr_status())
#endif
    {
      // Clear RX event
      rsi_clear_event(RSI_RX_EVENT);
    }
#if !THROUGHPUT_EN
#if RSI_SPI_DUP_INTR_HANDLE
  if (!rsi_common_cb->power_save.power_save_enable) {
    // Read interrupt status register
    ret_status = rsi_device_interrupt_status(&int_status);
    // Check packet pending in interrupt status register
    if (int_status & RSI_RX_PKT_PENDING) {
      rsi_set_event(RSI_RX_EVENT);
      return;
    }
  }
#endif
#endif
  // Unmask RX event
  rsi_unmask_event(RSI_RX_EVENT);
#ifdef RSI_M4_INTERFACE
  // submit to TA submit packet
  rsi_submit_rx_pkt();

  unmask_ta_interrupt(RX_PKT_TRANSFER_DONE_INTERRUPT);
#endif
#ifndef RSI_M4_INTERFACE
  rsi_hal_intr_unmask();
#endif

#if ((defined RSI_SPI_INTERFACE) || (defined RSI_SDIO_INTERFACE))
  if (rsi_get_intr_status()) {
    // Set RX event
    rsi_set_event(RSI_RX_EVENT);
  }
#endif
#if ((defined RSI_SPI_INTERFACE) || (defined RSI_M4_INTERFACE) || (defined RSI_SDIO_INTERFACE))
  if (rsi_driver_cb_non_rom->tx_mask_event == 1) {
    rsi_unmask_event(RSI_TX_EVENT);
    // lock the mutex
    rsi_mutex_lock(&rsi_driver_cb_non_rom->tx_mutex);
    rsi_driver_cb_non_rom->tx_mask_event = 0;
    // unlock mutex
    rsi_mutex_unlock(&rsi_driver_cb_non_rom->tx_mutex);
  }
#endif

  return;
}
/** @} */
