/*******************************************************************************
* @file  rsi_common.c
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
#include "rsi_hal.h"
#ifdef RSI_M4_INTERFACE
#include "rsi_m4.h"
#endif
#include "rsi_wlan_non_rom.h"
#ifdef RSI_BLE_ENABLE
#include "rsi_ble_common_config.h"
#endif
#ifdef RSI_BT_ENABLE
#include "rsi_bt_common_config.h"
#endif

extern rsi_socket_info_non_rom_t *rsi_socket_pool_non_rom;
extern rsi_socket_select_info_t *rsi_socket_select_info;
/** @addtogroup DRIVER5
* @{
*/
/*==============================================*/
/**
 * @fn         int8_t rsi_common_cb_init(rsi_common_cb_t *common_cb)
 * @brief      Initialize common control block structure
 * @param[in]  common_cb - pointer to common cb structure 
 * @return     0              - Success \n 
 *             Non-Zero Value - Failure
 */
/// @private
int8_t rsi_common_cb_init(rsi_common_cb_t *common_cb)
{
  int8_t retval = RSI_ERR_NONE;

  // validate input parameter
  if (common_cb == NULL) {
    return RSI_ERROR_INVALID_PARAM;
  }

  // Initialize common state
  common_cb->state = RSI_COMMON_STATE_NONE;

  // Initializes common status
  common_cb->status = RSI_SUCCESS;

  // Creates common mutex
  rsi_mutex_create(&common_cb->common_mutex);

  // Creates tx mutex
  rsi_mutex_create(&rsi_driver_cb_non_rom->tx_mutex);

#if defined(RSI_DEBUG_PRINTS) || defined(FW_LOGGING_ENABLE)
  // Creates debug prints mutex
  rsi_mutex_create(&rsi_driver_cb_non_rom->debug_prints_mutex);
#endif
  // Creates common cmd mutex
  retval = rsi_semaphore_create(&rsi_driver_cb_non_rom->common_cmd_send_sem, 0);
  if (retval != RSI_ERROR_NONE) {
    return RSI_ERROR_SEMAPHORE_CREATE_FAILED;
  }
  rsi_semaphore_post(&rsi_driver_cb_non_rom->common_cmd_send_sem);

  // Set expected_response to zero
  common_cb->expected_response = RSI_COMMON_RSP_CLEAR;

  retval = rsi_semaphore_create(&common_cb->common_sem, 0);
  if (retval != RSI_ERROR_NONE) {
    return RSI_ERROR_SEMAPHORE_CREATE_FAILED;
  }
  retval = rsi_semaphore_create(&rsi_driver_cb_non_rom->common_cmd_sem, 0);
  if (retval != RSI_ERROR_NONE) {
    return RSI_ERROR_SEMAPHORE_CREATE_FAILED;
  }
#ifdef RSI_WITH_OS
  retval = rsi_semaphore_create(&common_cb->common_card_ready_sem, 0);
  if (retval != RSI_ERROR_NONE) {
    return RSI_ERROR_SEMAPHORE_CREATE_FAILED;
  }
#endif
#ifdef WAKEUP_GPIO_INTERRUPT_METHOD
  retval = rsi_semaphore_create(&common_cb->wakeup_gpio_sem, 0);
  if (retval != RSI_ERROR_NONE) {
    return RSI_ERROR_SEMAPHORE_CREATE_FAILED;
  }
#endif
  return retval;
}
/*==============================================*/
/**
 * @fn          int32_t rsi_driver_common_send_cmd(rsi_common_cmd_request_t cmd, rsi_pkt_t *pkt)
 * @brief       Fill commands and places into wlan TX queue
 * @param[in]   cmd - type of the command to send 
 * @param[in]   pkt - pointer of packet to send
 * @return      0              - Success \n 
 *              Non-Zero Value - Failure
 *
 */
/// @private
int32_t rsi_driver_common_send_cmd(rsi_common_cmd_request_t cmd, rsi_pkt_t *pkt)
{
  uint16_t payload_size = 0;
  int32_t status        = RSI_SUCCESS;
  // Get common control block structure pointer
  rsi_common_cb_t *rsi_common_cb = rsi_driver_cb->common_cb;
  // Get host descriptor pointer
  uint8_t *host_desc = (pkt->desc);
#ifdef RSI_PUF_ENABLE
  if (!(((cmd >= RSI_COMMON_REQ_PUF_ENROLL) && (cmd <= RSI_COMMON_REQ_AES_MAC))
        || (cmd == RSI_COMMON_REQ_PUF_INTR_KEY)))
#endif
#ifdef RSI_CRYPTO_ENABLE
    if (!(cmd == RSI_COMMON_REQ_ENCRYPT_CRYPTO))
#endif

    {
      // Memset host descriptor
      memset(host_desc, 0, RSI_HOST_DESC_LENGTH);
    }

  switch (cmd) {
    case RSI_COMMON_REQ_FW_VERSION: {

    } break;
    case RSI_COMMON_REQ_OPERMODE: {
      // opermode Parameters
      rsi_opermode_t *rsi_opermode = (rsi_opermode_t *)pkt->data;

      // fill other parameters from configuration file
#ifdef RSI_M4_INTERFACE
      rsi_uint32_to_4bytes(rsi_opermode->feature_bit_map, (FEAT_WPS_DISABLE | RSI_FEATURE_BIT_MAP));
#else
      rsi_uint32_to_4bytes(rsi_opermode->feature_bit_map, RSI_FEATURE_BIT_MAP);
#endif
#if RSI_TCP_IP_BYPASS
      rsi_uint32_to_4bytes(rsi_opermode->tcp_ip_feature_bit_map, RSI_TCP_IP_FEATURE_BIT_MAP);
#else
      rsi_uint32_to_4bytes(rsi_opermode->tcp_ip_feature_bit_map,
                           (RSI_TCP_IP_FEATURE_BIT_MAP | RSI_APP_TCP_IP_FEATURE_BITMAP));
#endif
      rsi_uint32_to_4bytes(rsi_opermode->custom_feature_bit_map,
                           (FEAT_CUSTOM_FEAT_EXTENTION_VALID | RSI_CUSTOM_FEATURE_BIT_MAP));

#ifdef CHIP_9117
#ifdef RSI_M4_INTERFACE
      /* To enable 384K memory for TA */
      rsi_uint32_to_4bytes(rsi_opermode->ext_custom_feature_bit_map,
                           (RSI_EXT_CUSTOM_FEATURE_BIT_MAP) & ~(BIT(20) | BIT(21)));
#else
      rsi_uint32_to_4bytes(rsi_opermode->ext_custom_feature_bit_map,
                           (EXT_FEAT_704K_M4SS_0K | RSI_EXT_CUSTOM_FEATURE_BIT_MAP));
#endif
#else //defaults
#ifdef RSI_M4_INTERFACE
      rsi_uint32_to_4bytes(rsi_opermode->ext_custom_feature_bit_map,
                           (EXT_FEAT_256K_MODE | RSI_EXT_CUSTOM_FEATURE_BIT_MAP));
#else
      rsi_uint32_to_4bytes(rsi_opermode->ext_custom_feature_bit_map,
                           (EXT_FEAT_384K_MODE | RSI_EXT_CUSTOM_FEATURE_BIT_MAP));
#endif
#endif

#ifdef RSI_PROCESS_MAX_RX_DATA
      rsi_uint32_to_4bytes(
        rsi_opermode->ext_tcp_ip_feature_bit_map,
        (RSI_EXT_TCPIP_FEATURE_BITMAP | RSI_APP_EXT_TCP_IP_FEATURE_BITMAP | EXT_TCP_MAX_RECV_LENGTH));
#else
      rsi_uint32_to_4bytes(rsi_opermode->ext_tcp_ip_feature_bit_map,
                           (RSI_EXT_TCPIP_FEATURE_BITMAP | RSI_APP_EXT_TCP_IP_FEATURE_BITMAP));
#endif
      rsi_uint32_to_4bytes(rsi_opermode->config_feature_bit_map,
                           (RSI_APP_CONFIG_FEATURE_BITMAP | RSI_CONFIG_FEATURE_BITMAP));

      rsi_uint32_to_4bytes(rsi_opermode->bt_feature_bit_map, RSI_BT_FEATURE_BITMAP);

#if (defined RSI_BLE_ENABLE || defined RSI_BT_ENABLE || defined RSI_PROP_PROTOCOL_ENABLE)
      if ((((rsi_bytes4R_to_uint32(rsi_opermode->opermode) >> 16) & 0xFFFF) == RSI_OPERMODE_WLAN_BLE)
          || (((rsi_bytes4R_to_uint32(rsi_opermode->opermode) >> 16) & 0xFFFF) == RSI_OPERMODE_WLAN_BT_CLASSIC)
          || (((rsi_bytes4R_to_uint32(rsi_opermode->opermode) >> 16) & 0xFFFF) == RSI_OPERMODE_WLAN_BT_DUAL_MODE)) {
        rsi_opermode->custom_feature_bit_map[3] |= 0x80;
        rsi_opermode->ext_custom_feature_bit_map[3] |= 0x80;
#if (defined A2DP_POWER_SAVE_ENABLE)
        rsi_opermode->ext_custom_feature_bit_map[2] |= 0x40;
#endif
#ifdef RSI_BLE_ENABLE
        //!ENABLE_BLE_PROTOCOL in bt_feature_bit_map
        rsi_opermode->bt_feature_bit_map[3] |= 0x80;
        rsi_uint32_to_4bytes(rsi_opermode->ble_feature_bit_map,
                             ((RSI_BLE_MAX_NBR_SLAVES << 12) | (RSI_BLE_MAX_NBR_MASTERS << 27)
                              | (RSI_BLE_MAX_NBR_ATT_SERV << 8) | RSI_BLE_MAX_NBR_ATT_REC));

        /*Enable BLE custom feature bitmap*/
        rsi_opermode->ble_feature_bit_map[3] |= 0x80;
        rsi_uint32_to_4bytes(rsi_opermode->ble_ext_feature_bit_map,
                             (RSI_BLE_NUM_CONN_EVENTS) | (RSI_BLE_NUM_REC_BYTES << 5));
        rsi_opermode->ble_ext_feature_bit_map[1] |=
          (RSI_BLE_INDICATE_CONFIRMATION_FROM_HOST << 6); //indication response from app
        rsi_opermode->ble_ext_feature_bit_map[1] |=
          (RSI_BLE_MTU_EXCHANGE_FROM_HOST << 7); //MTU Exchange request initiation from app
        rsi_opermode->ble_ext_feature_bit_map[2] |=
          (RSI_BLE_SET_SCAN_RESP_DATA_FROM_HOST); //Set SCAN Resp Data from app
        rsi_opermode->ble_ext_feature_bit_map[2] |=
          (RSI_BLE_DISABLE_CODED_PHY_FROM_HOST << 1); //Disable Coded PHY from app
#if BLE_SIMPLE_GATT
        rsi_opermode->ble_ext_feature_bit_map[1] |= (1 << 5);
#endif
        rsi_opermode->ble_feature_bit_map[2] |= RSI_BLE_PWR_INX;
        rsi_opermode->ble_feature_bit_map[3] |= RSI_BLE_PWR_SAVE_OPTIONS;
        rsi_opermode->ble_feature_bit_map[3] |= (1 << 6);
        rsi_opermode->ble_feature_bit_map[3] |= (RSI_BLE_GATT_ASYNC_ENABLE << 5);
#endif
#if (RSI_BT_ENABLE && RSI_BLE_ENABLE && RSI_BT_GATT_ON_CLASSIC)
        rsi_opermode->bt_feature_bit_map[3] |= (1 << 5); /* to support att over classic acl link */
#endif
      }
#endif

      // fill payload size
      payload_size = sizeof(rsi_opermode_t);
    } break;
    case RSI_COMMON_REQ_ANTENNA_SELECT: {
      // fill payload size
      payload_size = sizeof(rsi_antenna_select_t);
    } break;
    case RSI_COMMON_REQ_DEBUG_LOG: {
      // fill payload size
      payload_size = sizeof(rsi_debug_uart_print_t);
    } break;
    case RSI_COMMON_REQ_SWITCH_PROTO: {
      payload_size = sizeof(rsi_switch_proto_t);

    } break;
    case RSI_COMMON_REQ_GET_RAM_DUMP: {
      payload_size = sizeof(rsi_ram_dump_t);

    } break;

    case RSI_COMMON_REQ_SET_RTC_TIMER: {
      payload_size = sizeof(module_rtc_time_t);
    } break;
    case RSI_COMMON_REQ_GET_RTC_TIMER: {
      payload_size = sizeof(module_rtc_time_t);
    } break;
    case RSI_COMMON_REQ_FEATURE_FRAME: {
      // fill payload size
      payload_size = sizeof(rsi_feature_frame_t);
    } break;
    case RSI_COMMON_REQ_SOFT_RESET: {
      rsi_driver_cb->common_cb->state = RSI_COMMON_STATE_NONE;

    } break;

    case RSI_COMMON_REQ_UART_FLOW_CTRL_ENABLE: {
      payload_size = 1;
    } break;
    case RSI_COMMON_REQ_PWRMODE: {
      // Power mode setting
      rsi_power_save_req_t *power_mode = (rsi_power_save_req_t *)pkt->data;
      if (power_mode->power_mode != RSI_POWER_MODE_DISABLE) {
        if (rsi_common_cb->power_save.wlan_psp_mode == 0xA) {
          power_mode->ulp_mode_enable = RSI_ULP_WITHOUT_RAM_RET;
        } else if (rsi_common_cb->power_save.wlan_psp_mode == 0x8) {
          power_mode->ulp_mode_enable = RSI_ULP_WITH_RAM_RET;
        } else {
          // set LP/ULP/ULP-without RAM retention
          power_mode->ulp_mode_enable = RSI_SELECT_LP_OR_ULP_MODE;
        }

        // set DTIM aligment required
        // 0 - module wakes up at beacon which is just before or equal to listen_interval
        // 1 - module wakes up at DTIM beacon which is just after or equal to listen_interval
        power_mode->dtim_aligned_type = RSI_DTIM_ALIGNED_TYPE;

        // Monitor interval for the FAST PSP mode
        // default is 50 ms, and this parameter is valid for FAST PSP only
        power_mode->monitor_interval = RSI_MONITOR_INTERVAL;
        power_mode->num_of_dtim_skip = RSI_NUM_OF_DTIM_SKIP;
#ifdef RSI_WLAN_ENABLE
        power_mode->listen_interval = rsi_wlan_cb_non_rom->ps_listen_interval;
#endif
      }

      if (power_mode->ulp_mode_enable == RSI_ULP_WITHOUT_RAM_RET) {
        rsi_driver_cb->common_cb->state = RSI_COMMON_STATE_NONE;
      }

      // fill payload size
      payload_size = sizeof(rsi_power_save_req_t);
    } break;

#ifdef RSI_WAC_MFI_ENABLE
    case RSI_COMMON_REQ_IAP_INIT:
      break;
    case RSI_COMMON_REQ_IAP_GENERATE_SIGATURE: {
      payload_size = sizeof(rsi_mfi_auth_create_request_t);
    } break;
    case RSI_COMMON_REQ_IAP_GET_CERTIFICATE:
      break;
#endif

#ifdef RSI_PUF_ENABLE
    case RSI_COMMON_REQ_PUF_ENROLL: {
      payload_size = rsi_bytes2R_to_uint16(host_desc);
    } break;
    case RSI_COMMON_REQ_PUF_DIS_ENROLL: {
      payload_size = rsi_bytes2R_to_uint16(host_desc);
    } break;
    case RSI_COMMON_REQ_PUF_START: {
      payload_size = rsi_bytes2R_to_uint16(host_desc);
    } break;
    case RSI_COMMON_REQ_PUF_SET_KEY: {
      payload_size = rsi_bytes2R_to_uint16(host_desc);
    } break;
    case RSI_COMMON_REQ_PUF_DIS_SET_KEY: {
      payload_size = rsi_bytes2R_to_uint16(host_desc);
    } break;
    case RSI_COMMON_REQ_PUF_GET_KEY: {
      payload_size = rsi_bytes2R_to_uint16(host_desc);
    } break;
    case RSI_COMMON_REQ_PUF_DIS_GET_KEY: {
      payload_size = rsi_bytes2R_to_uint16(host_desc);
    } break;
    case RSI_COMMON_REQ_PUF_LOAD_KEY: {
      payload_size = rsi_bytes2R_to_uint16(host_desc);
    } break;
    case RSI_COMMON_REQ_AES_ENCRYPT: {
      payload_size = rsi_bytes2R_to_uint16(host_desc);
    } break;
    case RSI_COMMON_REQ_AES_DECRYPT: {
      payload_size = rsi_bytes2R_to_uint16(host_desc);
    } break;
    case RSI_COMMON_REQ_AES_MAC: {
      payload_size = rsi_bytes2R_to_uint16(host_desc);
    } break;
    case RSI_COMMON_REQ_PUF_INTR_KEY: {
      payload_size = rsi_bytes2R_to_uint16(host_desc);
    } break;
#endif
#ifdef RSI_CRYPTO_ENABLE
    case RSI_COMMON_REQ_ENCRYPT_CRYPTO: {
      payload_size = rsi_bytes2R_to_uint16(host_desc);
    } break;
#endif
    case RSI_COMMON_REQ_TA_M4_COMMANDS: {
#ifdef RSI_M4_INTERFACE
#ifdef CHIP_9117
      rsi_req_ta2m4_t *ta2m4 = (rsi_req_ta2m4_t *)pkt->data;
      if (ta2m4->sub_cmd == RSI_WRITE_TO_COMMON_FLASH) {
        payload_size = ta2m4->chunk_len + (sizeof(rsi_req_ta2m4_t) - RSI_MAX_CHUNK_SIZE);
      } else
#endif
      {
        payload_size = sizeof(ta_m4_handshake_param_t);
      }
#endif

    } break;
#ifdef CONFIGURE_GPIO_FROM_HOST
    case RSI_COMMON_REQ_GPIO_CONFIG: {
      payload_size = sizeof(rsi_gpio_pin_config_t);
    } break;
#endif
    case RSI_COMMON_REQ_SET_CONFIG: {
      payload_size = sizeof(rsi_set_config_t);
    } break;
#ifdef FW_LOGGING_ENABLE
    case RSI_COMMON_REQ_DEVICE_LOGGING_INIT: {
      payload_size = sizeof(sl_fw_logging_t);
    } break;
#endif
    default: {
    }
  }
  // Fill payload length
  rsi_uint16_to_2bytes(host_desc, (payload_size & 0xFFF));

  // Fill frame type
  host_desc[1] |= (RSI_WLAN_MGMT_Q << 4);

  // Fill frame type
  host_desc[2] = cmd;

  if ((cmd == RSI_COMMON_REQ_PWRMODE) || (cmd == RSI_COMMON_REQ_OPERMODE) || (cmd == RSI_COMMON_REQ_SOFT_RESET)) {
    // Block WLAN TX queue
    rsi_block_queue(&rsi_driver_cb->wlan_tx_q);
#ifdef RSI_ZB_ENABLE
    // Block ZB TX queue
    rsi_block_queue(&rsi_driver_cb->zigb_tx_q);
#endif
#if (defined RSI_BT_ENABLE || defined RSI_BLE_ENABLE || defined RSI_PROP_PROTOCOL_ENABLE)
    rsi_block_queue(&rsi_driver_cb->bt_single_tx_q);
#endif
#ifdef RSI_PROP_PROTOCOL_ENABLE
    rsi_block_queue(&rsi_driver_cb->prop_protocol_tx_q);
#endif
  }

  // Enqueue packet to common TX queue
  rsi_enqueue_pkt(&rsi_driver_cb->common_tx_q, pkt);

  // Set TX packet pending event
  rsi_set_event(RSI_TX_EVENT);

  // Return status
  return status;
}

/*==============================================*/
/**
 * @fn          int32_t rsi_driver_process_common_recv_cmd(rsi_pkt_t *pkt)
 * @brief       Process commands related to common block
 * @param[in]   pkt - pointer to common block packet 
 * @return      0              - Success \n 
 *              Non-Zero Value - Failure 
 */
/// @private
int32_t rsi_driver_process_common_recv_cmd(rsi_pkt_t *pkt)
{
  uint8_t cmd_type;
  int32_t status     = RSI_SUCCESS;
  uint8_t *host_desc = pkt->desc;
  uint16_t copy_length;
  uint8_t *payload;
  uint16_t payload_length;
#ifdef RSI_WLAN_ENABLE
  uint8_t i = 0;
#endif
  // Get commmon cb pointer
  rsi_common_cb_t *rsi_common_cb = rsi_driver_cb->common_cb;

  // Get wlan cb pointer
  rsi_wlan_cb_t *rsi_wlan_cb = rsi_driver_cb->wlan_cb;

  // Get command type
  cmd_type = pkt->desc[2];

  // Get status
  status = rsi_bytes2R_to_uint16(host_desc + RSI_STATUS_OFFSET);

  // Get payload pointer
  payload = pkt->data;

  // Get payoad length
  payload_length = (rsi_bytes2R_to_uint16(host_desc) & 0xFFF);

  rsi_common_set_status(status);

  switch (cmd_type) {
    case RSI_COMMON_RSP_CARDREADY: {
      // if success, update state common_cb state
      if (status == RSI_SUCCESS) {
        rsi_common_cb->state = RSI_COMMON_CARDREADY;
        // Check for auto config status
        if (host_desc[15] & RSI_AUTO_CONFIG_GOING_ON) {
          // Set auto config failed status
          rsi_wlan_cb->auto_config_state = RSI_WLAN_STATE_AUTO_CONFIG_GOING_ON;

          // Check for auto config handler
          if (rsi_wlan_cb_non_rom->callback_list.auto_config_rsp_handler) {
            rsi_wlan_cb_non_rom->callback_list.auto_config_rsp_handler(status, RSI_AUTO_CONFIG_GOING_ON);
          }
        }
        // Check for auto config status
        else if (host_desc[15] & RSI_AUTO_CONFIG_FAILED) {
          // Set auto config failed status
          rsi_wlan_cb->auto_config_state = RSI_WLAN_STATE_AUTO_CONFIG_FAILED;

          // Check for auto config handler
          if (rsi_wlan_cb_non_rom->callback_list.auto_config_rsp_handler) {
            rsi_wlan_cb_non_rom->callback_list.auto_config_rsp_handler(status, RSI_AUTO_CONFIG_FAILED);
          }
        }
      }
    } break;

    case RSI_COMMON_RSP_OPERMODE: {
      // check status
      status = rsi_bytes2R_to_uint16(host_desc + RSI_STATUS_OFFSET);

      // if success update state common_cb state and wlan_cb state to OPERMODE done
      if (status == RSI_SUCCESS) {
        // Update wlan state to OPERMODE done
        rsi_wlan_cb->state = RSI_WLAN_STATE_OPERMODE_DONE;

        rsi_common_cb->state = RSI_COMMON_OPERMODE_DONE;

        // unblock protocol queues
        rsi_unblock_queue(&rsi_driver_cb->wlan_tx_q);

#ifdef RSI_ZB_ENABLE
        // unblock ZB TX queue
        rsi_unblock_queue(&rsi_driver_cb->zigb_tx_q);
#endif
#if (defined RSI_BT_ENABLE || defined RSI_BLE_ENABLE || defined RSI_PROP_PROTOCOL_ENABLE)
        // unblock BT common TX queue
        rsi_unblock_queue(&rsi_driver_cb->bt_single_tx_q);
#endif
#ifdef RSI_PROP_PROTOCOL_ENABLE
        // unblock PROP_PROTOCOL TX queue
        rsi_unblock_queue(&rsi_driver_cb->prop_protocol_tx_q);
#endif
      }
    } break;
    case RSI_COMMON_RSP_PWRMODE: {
      // check status
      status = rsi_bytes2R_to_uint16(host_desc + RSI_STATUS_OFFSET);

      // if success update state common_cb state and wlan_cb state to OPERMODE done
      if (status == RSI_SUCCESS) {
        if (rsi_common_cb->power_save.current_ps_mode != 0) {
          // set power_save_enable
          rsi_common_cb->power_save.power_save_enable = 1;
        }
      }
      // unblock protocol queues
      rsi_unblock_queue(&rsi_driver_cb->wlan_tx_q);

#ifdef RSI_ZB_ENABLE
      // unblock ZB TX queue
      rsi_unblock_queue(&rsi_driver_cb->zigb_tx_q);
#endif
#if (defined RSI_BT_ENABLE || defined RSI_BLE_ENABLE || defined RSI_PROP_PROTOCOL_ENABLE)
      // unblock BT TX queue
      rsi_unblock_queue(&rsi_driver_cb->bt_single_tx_q);
#endif
#ifdef RSI_PROP_PROTOCOL_ENABLE
      // unblock PROP_PROTOCOL TX queue
      rsi_unblock_queue(&rsi_driver_cb->prop_protocol_tx_q);
#endif
      // Flush module state
      rsi_common_cb->power_save.module_state = RSI_IDLE;
    } break;
    case RSI_COMMON_RSP_ANTENNA_SELECT: {
      // check status
      status = rsi_bytes2R_to_uint16(host_desc + RSI_STATUS_OFFSET);
    } break;
    case RSI_COMMON_RSP_DEBUG_LOG: {
      // check status
      status = rsi_bytes2R_to_uint16(host_desc + RSI_STATUS_OFFSET);
    } break;
    case RSI_COMMON_RSP_FEATURE_FRAME: {
      // check status
      status = rsi_bytes2R_to_uint16(host_desc + RSI_STATUS_OFFSET);
    } break;

    case RSI_COMMON_RSP_SOFT_RESET: {
      // check status
      status = rsi_bytes2R_to_uint16(host_desc + RSI_STATUS_OFFSET);

      // check common_cb for any task is waiting for response
      // No need to check for expected response as all variables will be reseted after reset command
      //if(rsi_common_cb->expected_response == cmd_type )
      {
        // state update
        rsi_common_cb->state = RSI_COMMON_CARDREADY;
        rsi_wlan_cb->state   = RSI_WLAN_STATE_NONE;
#ifdef RSI_WLAN_ENABLE
        // added this loop for socket pool not memset/clear sometime while deinit/reset. For more info.
        for (i = 0; i < NUMBER_OF_SOCKETS; i++) {
          // Memset socket info
          memset(&rsi_socket_pool[i], 0, sizeof(rsi_socket_info_t));
        }
#endif
        // Check for auto config status
        if (host_desc[15] & RSI_AUTO_CONFIG_GOING_ON) {
          // Set auto config failed status
          rsi_wlan_cb->auto_config_state = RSI_WLAN_STATE_AUTO_CONFIG_GOING_ON;

          // Check for auto config handler
          if (rsi_wlan_cb_non_rom->callback_list.auto_config_rsp_handler) {
            rsi_wlan_cb_non_rom->callback_list.auto_config_rsp_handler(status, RSI_AUTO_CONFIG_GOING_ON);
          }
        }
        // Check for auto config status
        else if (host_desc[15] & RSI_AUTO_CONFIG_FAILED) {
          // Set auto config failed status
          rsi_wlan_cb->auto_config_state = RSI_WLAN_STATE_AUTO_CONFIG_FAILED;

          // Check for auto config handler
          if (rsi_wlan_cb_non_rom->callback_list.auto_config_rsp_handler) {
            rsi_wlan_cb_non_rom->callback_list.auto_config_rsp_handler(status, RSI_AUTO_CONFIG_FAILED);
          }
        }
      }
    } break;
#ifdef RSI_CRYPTO_ENABLE
    case RSI_RSP_ENCRYPT_CRYPTO: {
      if (status == RSI_SUCCESS) {
        if ((rsi_common_cb->app_buffer != NULL) && (rsi_common_cb->app_buffer_length != 0)) {
          copy_length = (payload_length < rsi_common_cb->app_buffer_length) ? (payload_length)
                                                                            : (rsi_common_cb->app_buffer_length);
          memcpy(rsi_common_cb->app_buffer, payload, copy_length);
        }
      }

    } break;
#endif
#ifdef RSI_WAC_MFI_ENABLE
    case RSI_COMMON_RSP_IAP_INIT:
      break;
    case RSI_COMMON_RSP_IAP_GET_CERTIFICATE:
    case RSI_COMMON_RSP_IAP_GENERATE_SIGATURE: {
      // check status
      status = rsi_bytes2R_to_uint16(host_desc + RSI_STATUS_OFFSET);

      // Get payoad length
      payload_length = (rsi_bytes2R_to_uint16(host_desc) & 0xFFF);

      // Get payload pointer
      payload = pkt->data;

      // if success update state common_cb state and wlan_cb state to OPERMODE done
      if (status == RSI_SUCCESS) {

        if ((rsi_common_cb->app_buffer != NULL) && (rsi_common_cb->app_buffer_length != 0)) {
          copy_length = (payload_length < rsi_common_cb->app_buffer_length) ? (payload_length)
                                                                            : (rsi_common_cb->app_buffer_length);
          memcpy(rsi_common_cb->app_buffer, payload, copy_length);
          rsi_common_cb->app_buffer = NULL;
        }
      }

    } break;
#endif

#ifdef RSI_PUF_ENABLE
    case RSI_COMMON_RSP_PUF_ENROLL: {
      if (status == RSI_SUCCESS) {
        rsi_common_cb->puf_state = RSI_PUF_STATE_STARTED;
      }
    } break;
    case RSI_COMMON_RSP_PUF_DIS_ENROLL: {
      if (status == RSI_SUCCESS) {
      }
    } break;
    case RSI_COMMON_RSP_PUF_START: {
      if (status == RSI_SUCCESS) {
        rsi_common_cb->puf_state = RSI_PUF_STATE_STARTED;
        // check common_cb for any task is waiting for response
      }
    } break;
    case RSI_COMMON_RSP_PUF_SET_KEY: {
      if (status == RSI_SUCCESS) {
        rsi_common_cb->puf_state = RSI_PUF_STATE_SET_KEY;
        if ((rsi_common_cb->app_buffer != NULL) && (rsi_common_cb->app_buffer_length != 0)) {
          copy_length = (payload_length < rsi_common_cb->app_buffer_length) ? (payload_length)
                                                                            : (rsi_common_cb->app_buffer_length);
          memcpy(rsi_common_cb->app_buffer, payload, copy_length);
          rsi_common_cb->app_buffer = NULL;
        }
      }

    } break;
    case RSI_COMMON_RSP_PUF_DIS_SET_KEY: {
      if (status == RSI_SUCCESS) {
      }
    } break;

    case RSI_COMMON_RSP_PUF_GET_KEY: {
      if (status == RSI_SUCCESS) {
        if ((rsi_common_cb->app_buffer != NULL) && (rsi_common_cb->app_buffer_length != 0)) {
          copy_length = (payload_length < rsi_common_cb->app_buffer_length) ? (payload_length)
                                                                            : (rsi_common_cb->app_buffer_length);
          memcpy(rsi_common_cb->app_buffer, payload, copy_length);
          rsi_common_cb->app_buffer = NULL;
        }
      }
    } break;
    case RSI_COMMON_RSP_PUF_DIS_GET_KEY: {
      if (status == RSI_SUCCESS) {
      }
    } break;
    case RSI_COMMON_RSP_PUF_LOAD_KEY: {
      if (status == RSI_SUCCESS) {
        rsi_common_cb->puf_state = RSI_PUF_STATE_KEY_LOADED;
      }
    } break;
    case RSI_COMMON_RSP_AES_ENCRYPT: {
      if (status == RSI_SUCCESS) {
        if ((rsi_common_cb->app_buffer != NULL) && (rsi_common_cb->app_buffer_length != 0)) {
          copy_length = (payload_length < rsi_common_cb->app_buffer_length) ? (payload_length)
                                                                            : (rsi_common_cb->app_buffer_length);
          memcpy(rsi_common_cb->app_buffer, payload, copy_length);
          rsi_common_cb->app_buffer = NULL;
        }
      }
    } break;
    case RSI_COMMON_RSP_AES_DECRYPT: {
      if (status == RSI_SUCCESS) {
        if ((rsi_common_cb->app_buffer != NULL) && (rsi_common_cb->app_buffer_length != 0)) {
          copy_length = (payload_length < rsi_common_cb->app_buffer_length) ? (payload_length)
                                                                            : (rsi_common_cb->app_buffer_length);
          memcpy(rsi_common_cb->app_buffer, payload, copy_length);
          rsi_common_cb->app_buffer = NULL;
        }
      }
    } break;
    case RSI_COMMON_RSP_AES_MAC: {
      if (status == RSI_SUCCESS) {
        if ((rsi_common_cb->app_buffer != NULL) && (rsi_common_cb->app_buffer_length != 0)) {
          copy_length = (payload_length < rsi_common_cb->app_buffer_length) ? (payload_length)
                                                                            : (rsi_common_cb->app_buffer_length);
          memcpy(rsi_common_cb->app_buffer, payload, copy_length);
          rsi_common_cb->app_buffer = NULL;
        }
      }
    } break;
    case RSI_COMMON_RSP_PUF_INTR_KEY: {
      if (status == RSI_SUCCESS) {
        if ((rsi_common_cb->app_buffer != NULL) && (rsi_common_cb->app_buffer_length != 0)) {
          copy_length = (payload_length < rsi_common_cb->app_buffer_length) ? (payload_length)
                                                                            : (rsi_common_cb->app_buffer_length);
          memcpy(rsi_common_cb->app_buffer, payload, copy_length);
          rsi_common_cb->app_buffer = NULL;
        }
      }

    } break;

#endif

    case RSI_COMMON_RSP_FW_VERSION: {

      if (status == RSI_SUCCESS) {
        // check the length of application buffer and copy firmware version
        if ((rsi_common_cb->app_buffer != NULL) && (rsi_common_cb->app_buffer_length != 0)) {
          copy_length = (payload_length < rsi_common_cb->app_buffer_length) ? (payload_length)
                                                                            : (rsi_common_cb->app_buffer_length);
          memcpy(rsi_common_cb->app_buffer, payload, copy_length);
          rsi_common_cb->app_buffer = NULL;
        }
      }

    } break;
    case RSI_COMMON_RSP_GET_RTC_TIMER: {

      if (status == RSI_SUCCESS) {
        // check the length of application buffer and copy
        if ((rsi_common_cb->app_buffer != NULL) && (rsi_common_cb->app_buffer_length != 0)) {
          copy_length = (payload_length < rsi_common_cb->app_buffer_length) ? (payload_length)
                                                                            : (rsi_common_cb->app_buffer_length);
          memcpy(rsi_common_cb->app_buffer, payload, copy_length);
          rsi_common_cb->app_buffer = NULL;
        }
      }

    } break;

    case RSI_COMMON_RSP_TA_M4_COMMANDS: {
      // check status
      status = rsi_bytes2R_to_uint16(host_desc + RSI_STATUS_OFFSET);

      // Get payoad length
      payload_length = (rsi_bytes2R_to_uint16(host_desc) & 0xFFF);

      // Get payload pointer
      payload = pkt->data;

      // if success update state common_cb state and wlan_cb state to OPERMODE done
      if (status == RSI_SUCCESS) {

        if ((rsi_common_cb->app_buffer != NULL) && (rsi_common_cb->app_buffer_length != 0)) {
          copy_length = (payload_length < rsi_common_cb->app_buffer_length) ? (payload_length)
                                                                            : (rsi_common_cb->app_buffer_length);
          memcpy(rsi_common_cb->app_buffer, payload, copy_length);
          rsi_common_cb->app_buffer = NULL;
        }
      }
    } break;
    case RSI_COMMON_RSP_SWITCH_PROTO: {
      // check status
      status = rsi_bytes2R_to_uint16(host_desc + RSI_STATUS_OFFSET);

      // Get payoad length
      payload_length = (rsi_bytes2R_to_uint16(host_desc) & 0xFFF);

      // Get payload pointer
      payload = pkt->data;

#ifdef RSI_WLAN_ENABLE
      if (rsi_wlan_cb_non_rom->callback_list.rsi_max_available_rx_window) {
        rsi_wlan_cb_non_rom->callback_list.rsi_max_available_rx_window(status, payload, payload_length);
      }
#endif
    } break;
#ifdef CONFIGURE_GPIO_FROM_HOST
    case RSI_COMMON_RSP_GPIO_CONFIG: {
      status = rsi_bytes2R_to_uint16(host_desc + RSI_STATUS_OFFSET);

      // Get payoad length
      payload_length = (rsi_bytes2R_to_uint16(host_desc) & 0xFFF);

      // Get payload pointer
      payload = pkt->data;

      // if success update state common_cb state to OPERMODE done
      if (status == RSI_SUCCESS) {

        if ((rsi_common_cb->app_buffer != NULL) && (rsi_common_cb->app_buffer_length != 0)) {
          copy_length = (payload_length < rsi_common_cb->app_buffer_length) ? (payload_length)
                                                                            : (rsi_common_cb->app_buffer_length);
          memcpy(rsi_common_cb->app_buffer, payload, copy_length);
          rsi_common_cb->app_buffer = NULL;
        }
      }
    } break;
#endif
#ifdef FW_LOGGING_ENABLE
    case RSI_COMMON_RSP_DEVICE_LOGGING_INIT: {
      // Check status
      status = rsi_bytes2R_to_uint16(host_desc + RSI_STATUS_OFFSET);

      // Get payoad length
      payload_length = (rsi_bytes2R_to_uint16(host_desc) & 0xFFF);

      // Get payload pointer
      payload = pkt->data;

      // if SUCCESS, notify application using callback
      if (pkt->desc[15] == SL_LOG_DATA) {
        if (status == RSI_SUCCESS) {
          if (rsi_common_cb->sl_fw_log_callback != NULL) {
            rsi_common_cb->sl_fw_log_callback(&payload[0], payload_length);
            return RSI_SUCCESS;
          } else {
            return RSI_FAILURE;
          }
        } else {
          return RSI_FAILURE;
        }
      }
    } break;
#endif
    default: {
    }
  }
#ifdef RSI_WITH_OS
  if (cmd_type == RSI_COMMON_RSP_CARDREADY) {
#ifndef RSI_COMMON_SEM_BITMAP
    rsi_driver_cb_non_rom->common_wait_bitmap &= ~BIT(3);
#endif
    rsi_semaphore_post(&rsi_common_cb->common_card_ready_sem);
  }
#endif
  if ((cmd_type != RSI_COMMON_RSP_CARDREADY)) {
    // signal the common_cb semaphore
    if (cmd_type == RSI_COMMON_RSP_SWITCH_PROTO) {
      if (rsi_common_cb->sync_mode) {
#ifndef RSI_COMMON_SEM_BITMAP
        rsi_driver_cb_non_rom->common_wait_bitmap &= ~BIT(0);
#endif
        rsi_semaphore_post(&rsi_driver_cb_non_rom->common_cmd_sem);
      } else if ((rsi_wlan_cb_non_rom->switch_proto_callback != NULL) || (status != RSI_SUCCESS)) {
#ifndef RSI_COMMON_SEM_BITMAP
        rsi_driver_cb_non_rom->common_wait_bitmap &= ~BIT(0);
#endif
        rsi_semaphore_post(&rsi_driver_cb_non_rom->common_cmd_sem);
      }
    } else {
#ifndef RSI_COMMON_SEM_BITMAP
      rsi_driver_cb_non_rom->common_wait_bitmap &= ~BIT(0);
#endif
      rsi_semaphore_post(&rsi_driver_cb_non_rom->common_cmd_sem);
    }
  }

  return RSI_SUCCESS;
}

/*=================================================*/
/**
 * @fn          void rsi_handle_slp_wkp(uint8_t frame_type)
 * @brief       Handle sleep and wakeup.
 * @param[in]   frame_type - Frame type
 * @return      void
 */
/// @private
void rsi_handle_slp_wkp(uint8_t frame_type)
{
#ifdef RSI_WLAN_ENABLE
  uint8_t i = 0;
#endif
  //Get commmon cb pointer
  rsi_common_cb_t *rsi_common_cb = rsi_driver_cb->common_cb;

  // Get wlan cb pointer
  rsi_wlan_cb_t *rsi_wlan_cb = rsi_driver_cb->wlan_cb;

  // Handle them here based on power save state machine
  switch (frame_type) {
    case RSI_RSP_SLP: {
      rsi_common_cb->power_save.module_state = RSI_SLP_RECEIVED;
      rsi_set_event(RSI_TX_EVENT);
    } break;
    case RSI_RSP_WKP: {
      rsi_common_cb->power_save.module_state = RSI_WKUP_RECEIVED;
      rsi_unmask_event(RSI_TX_EVENT);
    } break;
    case RSI_COMMON_RSP_ULP_NO_RAM_RETENTION: {
      // state update
      rsi_common_cb->state = RSI_COMMON_CARDREADY;
      rsi_wlan_cb->state   = RSI_WLAN_STATE_NONE;
#ifdef RSI_WLAN_ENABLE
      // added this loop for socket pool not memset/clear sometime while deinit/reset. For more info (RSC-4333) .
      for (i = 0; i < NUMBER_OF_SOCKETS; i++) {
        // Memset socket info
        memset(&rsi_socket_pool[i], 0, sizeof(rsi_socket_info_t));
        memset(&rsi_socket_pool_non_rom[i], 0, sizeof(rsi_socket_info_non_rom_t));
      }
      rsi_wlan_cb_non_rom->wlan_radio    = 0;
      rsi_wlan_cb_non_rom->socket_bitmap = 0;
      // Set expected_response to zero
      rsi_wlan_cb->expected_response = RSI_WLAN_RSP_CLEAR;
#endif
      // Set expected_response to zero
      rsi_common_cb->expected_response = RSI_COMMON_RSP_CLEAR;
      rsi_allow_sleep();
      memset(&(rsi_common_cb->power_save), 0, sizeof(rsi_common_cb->power_save));
      rsi_unmask_event(RSI_TX_EVENT);
#ifdef RSI_WITH_OS
#ifndef RSI_COMMON_SEM_BITMAP
      rsi_driver_cb_non_rom->common_wait_bitmap &= ~BIT(3);
#endif
      rsi_semaphore_post(&rsi_common_cb->common_card_ready_sem);
#endif
    } break;
    default: {
    }
  }
}

/*====================================================*/
/**
 * @fn        int8_t rsi_req_wakeup(void)
 * @brief     Set wakeup GPIO high and wait for wakeup confirmation pin to get set.
 * @param[in] void 
 * @return    0              - Success \n 
 *            Non-Zero Value - Failure
 */

int8_t rsi_req_wakeup(void)
{

#ifndef RSI_M4_INTERFACE
  rsi_timer_instance_t timer_instance;
#if (RSI_SELECT_LP_OR_ULP_MODE != RSI_LP_MODE)
  rsi_hal_set_gpio(RSI_HAL_SLEEP_CONFIRM_PIN);
#else
  rsi_hal_set_gpio(RSI_HAL_LP_SLEEP_CONFIRM_PIN);
#endif
#ifdef WAKEUP_GPIO_INTERRUPT_METHOD
  if (rsi_hal_get_gpio(RSI_HAL_WAKEUP_INDICATION_PIN)) {
#ifdef RSI_SPI_INTERFACE
    rsi_ulp_wakeup_init();
#endif
  } else {
    rsi_hal_gpio_unmask();
#ifndef RSI_COMMON_SEM_BITMAP
    rsi_driver_cb_non_rom->common_wait_bitmap |= BIT(4);
#endif
    if (rsi_semaphore_wait(&rsi_driver_cb->common_cb->wakeup_gpio_sem, RSI_GPIO_WAIT_TIME) != RSI_ERROR_NONE) {
      return RSI_ERROR_GPIO_WAKEUP_TIMEOUT;
    }
#ifdef RSI_SPI_INTERFACE
    rsi_ulp_wakeup_init();
#endif
  }
#else
  rsi_init_timer(&timer_instance, RSI_GPIO_WAIT_TIME);
  do {
    if (rsi_hal_get_gpio(RSI_HAL_WAKEUP_INDICATION_PIN)) {
#if (RSI_SELECT_LP_OR_ULP_MODE != RSI_LP_MODE)
#ifdef RSI_SPI_INTERFACE
      rsi_ulp_wakeup_init();
#endif
#endif
      break;
    }
    if (rsi_timer_expired(&timer_instance)) {
      return RSI_ERROR_GPIO_WAKEUP_TIMEOUT;
    }
  } while (1);
#endif
#endif
  return RSI_SUCCESS;
}

/*====================================================*/
/**
 * @fn         int8_t rsi_wait4wakeup(void)
 * @brief      Set wakeup GPIO high and waits for wakeup confirmation pin to get set.
 * @param[in]  void
 * @return     0              - Success \n
 *             Non-Zero Value - Failure
 */
/// @private
int8_t rsi_wait4wakeup(void)
{
#ifndef RSI_M4_INTERFACE
  rsi_timer_instance_t timer_instance;
#ifdef WAKEUP_GPIO_INTERRUPT_METHOD
  if (rsi_hal_get_gpio(RSI_HAL_WAKEUP_INDICATION_PIN)) {
#ifdef RSI_SPI_INTERFACE
    rsi_ulp_wakeup_init();
#endif
  } else {
    rsi_hal_gpio_unmask();
#ifndef RSI_COMMON_SEM_BITMAP
    rsi_driver_cb_non_rom->common_wait_bitmap |= BIT(4);
#endif
    if (rsi_semaphore_wait(&rsi_driver_cb->common_cb->wakeup_gpio_sem, RSI_GPIO_WAIT_TIME) != RSI_ERROR_NONE) {
      return RSI_ERROR_GPIO_WAKEUP_TIMEOUT;
    }
#ifdef RSI_SPI_INTERFACE
    rsi_ulp_wakeup_init();
#endif
  }
#else
  rsi_init_timer(&timer_instance, RSI_GPIO_WAIT_TIME);
  if (rsi_driver_cb->wlan_cb->state < RSI_WLAN_STATE_CONNECTED) {
    rsi_hal_set_gpio(RSI_HAL_SLEEP_CONFIRM_PIN);
  }
  do {
    if (rsi_hal_get_gpio(RSI_HAL_WAKEUP_INDICATION_PIN)) {
#if (RSI_SELECT_LP_OR_ULP_MODE != RSI_LP_MODE)
      if (rsi_driver_cb->wlan_cb->state >= RSI_WLAN_STATE_CONNECTED) {
        rsi_hal_set_gpio(RSI_HAL_SLEEP_CONFIRM_PIN);
      }
#ifdef RSI_SPI_INTERFACE
      rsi_ulp_wakeup_init();
#endif
#else
      rsi_hal_set_gpio(RSI_HAL_LP_SLEEP_CONFIRM_PIN);
#endif
      break;
    }
    if (rsi_timer_expired(&timer_instance)) {
      return RSI_ERROR_GPIO_WAKEUP_TIMEOUT;
    }
  } while (1);
#endif
#endif
  return RSI_SUCCESS;
}

/*====================================================*/
/**
 * @fn        void rsi_allow_sleep(void)
 * @brief     Set wakeup GPIO low .
 * @param[in] void 
 * @return    void 
 */
/// @private
void rsi_allow_sleep(void)
{

#ifdef RSI_M4_INTERFACE
  P2P_STATUS_REG &= ~M4_wakeup_TA;
#else
#if (RSI_SELECT_LP_OR_ULP_MODE != RSI_LP_MODE)
  rsi_hal_clear_gpio(RSI_HAL_SLEEP_CONFIRM_PIN);
#else
  rsi_hal_clear_gpio(RSI_HAL_LP_SLEEP_CONFIRM_PIN);
#endif
#endif
}

/*====================================================*/
/**
 * @fn        void rsi_powersave_gpio_init(void)
 * @brief     Initialize GPIOs used in power save .
 * @param[in] void 
 * @return    void
 */
/// @private
void rsi_powersave_gpio_init(void)
{
  SL_PRINTF(SL_POWERSAVE_GPIO_INIT_ENTRY, DRIVER, LOG_INFO);
#ifndef RSI_M4_INTERFACE
  rsi_hal_config_gpio(RSI_HAL_WAKEUP_INDICATION_PIN, 0, 0);
#if (RSI_SELECT_LP_OR_ULP_MODE != RSI_LP_MODE)
  rsi_hal_config_gpio(RSI_HAL_SLEEP_CONFIRM_PIN, 1, 0);
  rsi_hal_clear_gpio(RSI_HAL_SLEEP_CONFIRM_PIN);
#else
  rsi_hal_config_gpio(RSI_HAL_LP_SLEEP_CONFIRM_PIN, 1, 0);
  rsi_hal_clear_gpio(RSI_HAL_LP_SLEEP_CONFIRM_PIN);
#endif
#endif
  SL_PRINTF(SL_POWERSAVE_GPIO_INIT_EXIT, DRIVER, LOG_INFO);
}
/*==============================================*/
/**
 * @fn          void rsi_common_packet_transfer_done(rsi_pkt_t *pkt)
 * @brief       Handle packet transfer completion which has an asyncronous response.
 * @param[in]   pkt - pointer to packet 
 * @return      void
 */
/// @private
void rsi_common_packet_transfer_done(rsi_pkt_t *pkt)
{
  UNUSED_PARAMETER(pkt); //This statement is added only to resolve compilation warning, value is unchanged
#ifdef RSI_WLAN_ENABLE
  // Set wlan status as success
  rsi_wlan_set_status(RSI_SUCCESS);
#endif
}

/*====================================================*/
/**
 * @fn          int32_t rsi_sleep_mode_decision(rsi_common_cb_t *rsi_common_cb)
 * @brief       A handler which decides the power mode to program and change 
 * @param[in]   rsi_common_cb - pointer to the common control block
 * @return      0              - Success \n 
 *              Non-Zero Value - Failure
 */
/// @private
int32_t rsi_sleep_mode_decision(rsi_common_cb_t *rsi_common_cb)
{
  int32_t status           = RSI_SUCCESS;
  uint8_t selected_ps_mode = 0;
  uint8_t selected_ps_type = 0;
  uint8_t mode_decision    = 0;

  switch ((rsi_common_cb->ps_coex_mode & 0xF)) {
    case 1: {
      mode_decision = ((rsi_common_cb->power_save.wlan_psp_mode << 4) | (rsi_common_cb->power_save.wlan_psp_mode));
    } break;
    case 4:
    case 12:
    case 8: {
      mode_decision = ((rsi_common_cb->power_save.bt_psp_mode << 4) | (rsi_common_cb->power_save.bt_psp_mode));
    } break;
    case 5:
    case 9:
    case 13: {
      mode_decision = ((rsi_common_cb->power_save.wlan_psp_mode << 4) | (rsi_common_cb->power_save.bt_psp_mode));
    } break;
    case 3: {
      mode_decision = ((rsi_common_cb->power_save.wlan_psp_mode << 4) | (rsi_common_cb->power_save.zb_psp_mode));
    } break;
    default:
      break;
  }

  switch (mode_decision) {
    case 0x00:
    case 0x10:
    case 0x01:
    case 0x20:
    case 0x02:
    case 0x80:
    case 0x08: {
      selected_ps_mode = RSI_ACTIVE;
    } break;
    case 0x88:
    case 0xAA: {
      // Update power save profile mode
#if (RSI_HAND_SHAKE_TYPE == GPIO_BASED)

      selected_ps_mode = RSI_GPIO_BASED_DEEP_SLEEP;

#elif defined(RSI_M4_INTERFACE) && (RSI_HAND_SHAKE_TYPE == M4_BASED)

      selected_ps_mode = RSI_M4_BASED_DEEP_SLEEP;
#else
      selected_ps_mode = RSI_MSG_BASED_DEEP_SLEEP;
#endif
    } break;
    case 0x11:
    case 0x81:
    case 0x18:
    case 0x12:
    case 0x21: {
      // Update power save profile mode
      selected_ps_mode = RSI_CONNECTED_SLEEP_PS;

      // Update power save profile type
      selected_ps_type = rsi_common_cb->power_save.wlan_psp_type;
    } break;
    case 0x22:
    case 0x82:
    case 0x28: {
      // Update power save profile mode
#if (RSI_HAND_SHAKE_TYPE == GPIO_BASED)
      selected_ps_mode = RSI_CONNECTED_GPIO_BASED_PS;

#elif defined(RSI_M4_INTERFACE) && (RSI_HAND_SHAKE_TYPE == M4_BASED)
      selected_ps_mode = RSI_CONNECTED_M4_BASED_PS;
#else
      selected_ps_mode = RSI_CONNECTED_MSG_BASED_PS;
#endif
      // Update power save profile type
      selected_ps_type = rsi_common_cb->power_save.wlan_psp_type;
    } break;
    default: {
      // Do nothing
    } break;
  }

  if (rsi_common_cb->power_save.current_ps_mode != selected_ps_mode) {
    status = rsi_send_ps_mode_to_module(selected_ps_mode, selected_ps_type);
  }

  return status;
}
/*====================================================*/
/**
 * @fn          int32_t rsi_send_ps_mode_to_module(uint8_t selected_ps_mode, uint8_t selected_ps_type)
 * @brief       Send the power save command to module  
 * @param[in]   selected_ps_mode - Select power save mode
 * @param[in]   selected_ps_type - Select power save type 
 * @return      0              - Success \n 
 *              Non-Zero Value - Failure
 */
/// @private
int32_t rsi_send_ps_mode_to_module(uint8_t selected_ps_mode, uint8_t selected_ps_type)
{
  rsi_pkt_t *pkt;
  rsi_power_save_req_t *ps_req;
  int32_t status = 0;

  rsi_common_cb_t *rsi_common_cb = rsi_driver_cb->common_cb;

  status = rsi_check_and_update_cmd_state(COMMON_CMD, IN_USE);
  if (status == RSI_SUCCESS) {
    // allocate command buffer  from common pool
    pkt = rsi_pkt_alloc(&rsi_common_cb->common_tx_pool);

    // If allocation of packet fails
    if (pkt == NULL) {
      //Changing the common cmd state to progress
      rsi_check_and_update_cmd_state(COMMON_CMD, ALLOW);
      // return packet allocation failure error
      return RSI_ERROR_PKT_ALLOCATION_FAILURE;
    }

    // Power mode setting
    ps_req = (rsi_power_save_req_t *)pkt->data;

    // reset to 0
    memset(&pkt->data, 0, sizeof(rsi_power_save_req_t));

    //take backup of powe save mode
    rsi_common_cb->power_save.current_ps_mode = 0;

    // send power save command
    status = rsi_driver_common_send_cmd(RSI_COMMON_REQ_PWRMODE, pkt);

#ifndef RSI_COMMON_SEM_BITMAP
    rsi_driver_cb_non_rom->common_wait_bitmap |= BIT(0);
#endif
    // wait on common semaphore
    rsi_wait_on_common_semaphore(&rsi_driver_cb_non_rom->common_cmd_sem, RSI_PWRMODE_RESPONSE_WAIT_TIME);

    // unmask Tx event
    rsi_unmask_event(RSI_TX_EVENT);

    // get common command response stattus
    status = rsi_common_get_status();

    if (status != RSI_SUCCESS) {
      //Changing the common state to allow state
      rsi_check_and_update_cmd_state(COMMON_CMD, ALLOW);
      // return status
      return status;
    }

    rsi_common_cb->power_save.power_save_enable = 0;

    if (selected_ps_mode == 0) {
      //Changing the common state to allow state
      rsi_check_and_update_cmd_state(COMMON_CMD, ALLOW);
      // return status
      return RSI_SUCCESS;
    }
    // allocate command buffer  from common pool
    pkt = rsi_pkt_alloc(&rsi_common_cb->common_tx_pool);

    // If allocation of packet fails
    if (pkt == NULL) {
      //Changing the common cmd state to progress
      rsi_check_and_update_cmd_state(COMMON_CMD, ALLOW);
      // return packet allocation failure error
      return RSI_ERROR_PKT_ALLOCATION_FAILURE;
    }

    memset(&pkt->data, 0, sizeof(rsi_power_save_req_t));

    //take backup of powe save mode
    rsi_common_cb->power_save.current_ps_mode = selected_ps_mode;

    // Update power save profile mode
    ps_req->power_mode = selected_ps_mode;

    // Update power save profile type
    ps_req->psp_type = selected_ps_type;

#ifndef RSI_COMMON_SEM_BITMAP
    rsi_driver_cb_non_rom->common_wait_bitmap |= BIT(0);
#endif
    // send power save command
    status = rsi_driver_common_send_cmd(RSI_COMMON_REQ_PWRMODE, pkt);

    // wait on common semaphore
    rsi_wait_on_common_semaphore(&rsi_driver_cb_non_rom->common_cmd_sem, RSI_PWRMODE_RESPONSE_WAIT_TIME);

    //Changing the common state to allow state
    rsi_check_and_update_cmd_state(COMMON_CMD, ALLOW);

  }

  else {
    //return common command error
    return status;
  }

  // get common command response stattus
  status = rsi_common_get_status();

  if (status == RSI_SUCCESS) {
    // Enable power save
    rsi_common_cb->power_save.power_save_enable = 1;
  }

  // unmask Tx event
  rsi_unmask_event(RSI_TX_EVENT);
  return status;
}
/*==============================================*/
/**
 * @fn          rsi_error_t rsi_wait_on_common_semaphore(rsi_semaphore_handle_t *semaphore, uint32_t timeout_ms ) 
 * @brief       Wireless library to acquire or wait for common semaphore.
 * @param[in]   semaphore  - Semaphore handle pointer  
 * @param[in]   timeout_ms - Maximum time to wait to acquire semaphore. If timeout_ms is 0 then wait
                till acquire semaphore.
 * @return      0              - Success \n 
 *              Non-Zero Value - Failure
 */
/// @private
rsi_error_t rsi_wait_on_common_semaphore(rsi_semaphore_handle_t *semaphore, uint32_t timeout_ms)
{
  // Wait on wlan semaphore
  if (rsi_semaphore_wait(semaphore, timeout_ms) != RSI_ERROR_NONE) {
    rsi_common_set_status(RSI_ERROR_RESPONSE_TIMEOUT);
#ifndef RSI_WAIT_TIMEOUT_EVENT_HANDLE_TIMER_DISABLE
    if (rsi_driver_cb_non_rom->rsi_wait_timeout_handler_error_cb != NULL) {
      rsi_driver_cb_non_rom->rsi_wait_timeout_handler_error_cb(RSI_ERROR_RESPONSE_TIMEOUT, COMMON_CMD);
    }
#endif
    return RSI_ERROR_RESPONSE_TIMEOUT;
  }
  return RSI_ERROR_NONE;
}
/*==============================================*/
/**
 * @fn          int32_t rsi_check_and_update_cmd_state(uint8_t cmd_type,uint8_t cmd_state)
 * @brief       This API is used by wireless library to check and update the command state to progress state.
 * @param[in]   cmd_type - Command type \n
 *              1 - COMMON_CMD \n
 *              2 - WLAN_CMD \n
 *              3 - NWK_CMD 
 * @param[in]   cmd_state - command state \n
 *              1 - IN_USE \n
 *              2 - ALLOW 
 * @return      0              - Success \n 
 *              Non-Zero Value - Failure
 */
/// @private
int32_t rsi_check_and_update_cmd_state(uint8_t cmd_type, uint8_t cmd_state)
{
  int32_t status = RSI_SUCCESS;
  if (rsi_driver_cb_non_rom->device_state < RSI_DEVICE_INIT_DONE) {
    //command given in wrong state
    return RSI_ERROR_COMMAND_GIVEN_IN_WRONG_STATE;
  }
  switch (cmd_type) {
    case COMMON_CMD: {
      if (cmd_state == IN_USE) {
#ifndef RSI_COMMON_SEM_BITMAP
        rsi_driver_cb_non_rom->common_wait_bitmap |= BIT(1);
#endif
        // common semaphore
        status = rsi_wait_on_common_semaphore(&rsi_driver_cb_non_rom->common_cmd_send_sem,
                                              RSI_COMMON_SEND_CMD_RESPONSE_WAIT_TIME);
        if (status != RSI_ERROR_NONE) {
          return RSI_ERROR_COMMON_CMD_IN_PROGRESS;
        }
      } else if (cmd_state == ALLOW) {
#ifndef RSI_COMMON_SEM_BITMAP
        rsi_driver_cb_non_rom->common_wait_bitmap &= ~BIT(1);
#endif
        // common semaphore post
        rsi_semaphore_post(&rsi_driver_cb_non_rom->common_cmd_send_sem);
      }
    } break;
#ifdef RSI_WLAN_ENABLE
    case WLAN_CMD: {
      if (cmd_state == IN_USE) {
#ifndef RSI_WLAN_SEM_BITMAP
        rsi_driver_cb_non_rom->wlan_wait_bitmap |= BIT(1);
#endif
        // wlan semaphore
        status =
          rsi_wait_on_wlan_semaphore(&rsi_driver_cb_non_rom->wlan_cmd_send_sem, RSI_WLAN_SEND_CMD_RESPONSE_WAIT_TIME);
        if (status != RSI_ERROR_NONE) {
          return RSI_ERROR_WLAN_CMD_IN_PROGRESS;
        }
      } else if (cmd_state == ALLOW) {
#ifndef RSI_WLAN_SEM_BITMAP
        rsi_driver_cb_non_rom->wlan_wait_bitmap &= ~BIT(1);
#endif
        // wlan semaphore post
        rsi_semaphore_post(&rsi_driver_cb_non_rom->wlan_cmd_send_sem);
      }
    } break;
    case NWK_CMD: {
      if (cmd_state == IN_USE) {
#ifndef RSI_NWK_SEM_BITMAP
        rsi_driver_cb_non_rom->nwk_wait_bitmap |= BIT(1);
#endif
        // nwk semaphore
        status =
          rsi_wait_on_nwk_semaphore(&rsi_driver_cb_non_rom->nwk_cmd_send_sem, RSI_NWK_SEND_CMD_RESPONSE_WAIT_TIME);
        if (status != RSI_ERROR_NONE) {
          return RSI_ERROR_NWK_CMD_IN_PROGRESS;
        }
      } else if (cmd_state == ALLOW) {
#ifndef RSI_NWK_SEM_BITMAP
        rsi_driver_cb_non_rom->nwk_wait_bitmap &= ~BIT(1);
#endif
        // nwk semaphore post
        rsi_semaphore_post(&rsi_driver_cb_non_rom->nwk_cmd_send_sem);
      }
    } break;
#endif
    default:
      status = RSI_ERROR_INVALID_PARAM;
      break;
  }
  return status;
}
/*==============================================*/
/**
 * @fn          void rsi_post_waiting_common_semaphore(void)
 * @brief       releases all the common semaphores which are on semaphore wait 
 * @param[in]   void  
 * @return      void
 *
 *
 */
#ifndef RSI_COMMON_SEM_BITMAP
/// @private
void rsi_post_waiting_common_semaphore(void)
{
  if (rsi_driver_cb_non_rom->common_wait_bitmap & BIT(0)) {
    rsi_semaphore_post(&rsi_driver_cb_non_rom->common_cmd_sem);
  }
  if (rsi_driver_cb_non_rom->common_wait_bitmap & BIT(1)) {
    rsi_semaphore_post(&rsi_driver_cb_non_rom->common_cmd_send_sem);
  }
  if (rsi_driver_cb_non_rom->common_wait_bitmap & BIT(2)) {
    rsi_semaphore_post(&rsi_driver_cb->common_cb->common_sem);
  }

  if (rsi_driver_cb_non_rom->common_wait_bitmap & BIT(3)) {
#ifdef RSI_WITH_OS
    rsi_semaphore_post(&rsi_driver_cb->common_cb->common_card_ready_sem);
#endif
  }
  if (rsi_driver_cb_non_rom->common_wait_bitmap & BIT(4)) {
#ifdef WAKEUP_GPIO_INTERRUPT_METHOD
    rsi_semaphore_post(&rsi_driver_cb->common_cb->wakeup_gpio_sem);
#endif
  }
  rsi_driver_cb_non_rom->common_wait_bitmap = 0;
}
#endif
/*==============================================*/
/**
 * @fn          void rsi_post_waiting_bt_semaphore(void)
 * @brief       Releases the all bt ,prop_protocol ,ble semaphores
 * @param[in]   void  
 * @return      void
 *
 *
 */
/// @private
void rsi_post_waiting_bt_semaphore(void)
{
#if (defined RSI_BT_ENABLE || defined RSI_BLE_ENABLE || defined RSI_PROP_PROTOCOL_ENABLE)
  if (rsi_driver_cb_non_rom->bt_wait_bitmap & BIT(1)) {
    rsi_semaphore_post(&rsi_driver_cb->bt_common_cb->bt_sem);
  }
#endif
#ifdef RSI_BT_ENABLE

  if (rsi_driver_cb_non_rom->bt_wait_bitmap & BIT(2)) {
    rsi_semaphore_post(&rsi_driver_cb->bt_classic_cb->bt_sem);
  }
#endif
#ifdef RSI_BLE_ENABLE

  if (rsi_driver_cb_non_rom->bt_wait_bitmap & BIT(3)) {
    rsi_semaphore_post(&rsi_driver_cb->ble_cb->bt_sem);
  }
#endif
#ifdef RSI_PROP_PROTOCOL_ENABLE

  if (rsi_driver_cb_non_rom->bt_wait_bitmap & BIT(4)) {
    rsi_semaphore_post(&rsi_driver_cb->prop_protocol_cb->bt_sem);
  }
#endif
#if (defined RSI_BT_ENABLE || defined RSI_BLE_ENABLE || defined RSI_PROP_PROTOCOL_ENABLE)

  if (rsi_driver_cb_non_rom->bt_cmd_wait_bitmap & BIT(1)) {
    rsi_semaphore_post(&rsi_driver_cb->bt_common_cb->bt_cmd_sem);
  }
#endif
#ifdef RSI_BT_ENABLE

  if (rsi_driver_cb_non_rom->bt_cmd_wait_bitmap & BIT(2)) {
    rsi_semaphore_post(&rsi_driver_cb->bt_classic_cb->bt_cmd_sem);
  }
#endif
#ifdef RSI_BLE_ENABLE

  if (rsi_driver_cb_non_rom->bt_cmd_wait_bitmap & BIT(3)) {
    rsi_semaphore_post(&rsi_driver_cb->ble_cb->bt_cmd_sem);
  }
#endif
#ifdef RSI_PROP_PROTOCOL_ENABLE

  if (rsi_driver_cb_non_rom->bt_cmd_wait_bitmap & BIT(4)) {
    rsi_semaphore_post(&rsi_driver_cb->prop_protocol_cb->bt_cmd_sem);
  }
#endif
}
/*=============================================================================*/
/**
 * @fn          int32_t rsi_release_waiting_semaphore(void)
 * @brief       Releases all common ,wlan and nwk semaphores which are on waiting 
 * @param[in]   void
 * @return      0              - Success \n 
 *              
 */
/// @private
int32_t rsi_release_waiting_semaphore(void)
{

#ifdef RSI_WLAN_ENABLE
  rsi_wlan_set_status(RSI_ERROR_IN_WLAN_CMD);
  rsi_post_waiting_wlan_semaphore();
  rsi_post_waiting_semaphore();

#endif
  rsi_common_set_status(RSI_ERROR_IN_COMMON_CMD);
  rsi_post_waiting_common_semaphore();

  rsi_post_waiting_bt_semaphore();
  if (rsi_driver_cb_non_rom->send_wait_bitmap & BIT(0)) {
#ifndef RSI_SEND_SEM_BITMAP
    rsi_driver_cb_non_rom->send_wait_bitmap &= ~BIT(0);
#endif
    rsi_semaphore_post(&rsi_driver_cb_non_rom->send_data_sem);
  }
#ifdef RSI_ZB_ENABLE
  if (rsi_driver_cb_non_rom->zigb_wait_bitmap & BIT(0)) {
#ifndef RSI_ZIGB_SEM_BITMAP
    rsi_driver_cb_non_rom->zigb_wait_bitmap &= ~BIT(0);
#endif
    rsi_semaphore_post(&rsi_driver_cb->zigb_cb->zigb_sem);
  }
#endif
  return RSI_SUCCESS;
}

/*==============================================*/
/**
 * @fn          void rsi_check_pkt_queue_and_dequeue(void)
 * @brief       Check the packet queues and dequeues the packet.
 * @param[in]   void 
 * @return      void
 *
 */
/// @private
void rsi_check_pkt_queue_and_dequeue(void)
{
  uint8_t common_pkt_pending = 0;
  uint8_t wlan_pkt_pending   = 0;
#if (defined(RSI_BT_ENABLE) || defined(RSI_BLE_ENABLE) || defined(RSI_PROP_PROTOCOL_ENABLE))
  uint8_t bt_pkt_pending = 0;
#endif
#ifdef RSI_PROP_PROTOCOL_ENABLE
  uint8_t prop_protocol_pkt_pending = 0;
#endif
  rsi_pkt_t *pkt = NULL;

  /*First Check for Common Queue */
  common_pkt_pending = rsi_check_queue_status(&rsi_driver_cb->common_tx_q);

  // check for packet pending in wlan queue

  if (common_pkt_pending) {
    // if packet pending dequeue the packet from common queue
    pkt = (rsi_pkt_t *)rsi_dequeue_pkt(&rsi_driver_cb->common_tx_q);

    // free the packet
    rsi_pkt_free(&rsi_driver_cb->common_cb->common_tx_pool, pkt);
  }

  // check for packet pending in wlan queue
  wlan_pkt_pending = rsi_check_queue_status(&rsi_driver_cb->wlan_tx_q);

  if (wlan_pkt_pending) {
    // dequeue the packet from wlan queue
    pkt = (rsi_pkt_t *)rsi_dequeue_pkt(&rsi_driver_cb->wlan_tx_q);
    // free the packet
    rsi_pkt_free(&rsi_driver_cb->wlan_cb->wlan_tx_pool, pkt);
  }
#if (defined(RSI_BT_ENABLE) || defined(RSI_BLE_ENABLE) || defined(RSI_PROP_PROTOCOL_ENABLE))
  // check for packet pending in bt/ble queue
  bt_pkt_pending = rsi_check_queue_status(&rsi_driver_cb->bt_single_tx_q);

  if (bt_pkt_pending) {
    // dequeue the packet from bt/ble queue
    pkt = (rsi_pkt_t *)rsi_dequeue_pkt(&rsi_driver_cb->bt_single_tx_q);
    // free the packet
#if (defined(RSI_BT_ENABLE) || defined(RSI_BLE_ENABLE) || defined(RSI_PROP_PROTOCOL_ENABLE))
    rsi_pkt_free(&rsi_driver_cb->bt_common_cb->bt_tx_pool, pkt);
#endif
#ifdef RSI_BT_ENABLE
    rsi_pkt_free(&rsi_driver_cb->bt_classic_cb->bt_tx_pool, pkt);
#endif
#ifdef RSI_BLE_ENABLE
    rsi_pkt_free(&rsi_driver_cb->ble_cb->bt_tx_pool, pkt);
#endif
  }
#endif
#ifdef RSI_PROP_PROTOCOL_ENABLE
  // check for packet pending in prop_protocol queue
  prop_protocol_pkt_pending = rsi_check_queue_status(&rsi_driver_cb->prop_protocol_tx_q);

  if (prop_protocol_pkt_pending) {
    // dequeue the packet from prop_protocol queue
    pkt = (rsi_pkt_t *)rsi_dequeue_pkt(&rsi_driver_cb->prop_protocol_tx_q);
    // free the packet
    rsi_pkt_free(&rsi_driver_cb->prop_protocol_cb->bt_tx_pool, pkt);
  }
#endif
}
/*==============================================*/
/**
 * @fn          void rsi_free_queue_pkt(uint8_t pkt_dequeued,rsi_pkt_t *pkt)
 * @brief       free the queued packet.
 * @param[in]   pkt_dequeued - pkt to be free
 * @param[in]   pkt - pointer of the packet to be free
 * @return      void
 *
 */
void rsi_free_queue_pkt(uint8_t pkt_dequeued, rsi_pkt_t *pkt)
{
  if (pkt_dequeued == COMMON_PKT) {
    rsi_pkt_free(&rsi_driver_cb->common_cb->common_tx_pool, pkt);
  }
#ifdef RSI_WLAN_ENABLE
  if (pkt_dequeued == WLAN_PKT) {
    rsi_pkt_free(&rsi_driver_cb->wlan_cb->wlan_tx_pool, pkt);
  }
#endif
#if (defined(RSI_BT_ENABLE) || defined(RSI_BLE_ENABLE) || defined(RSI_PROP_PROTOCOL_ENABLE))
  if (pkt_dequeued == BT_PKT) {
#if (defined(RSI_BT_ENABLE) || defined(RSI_BLE_ENABLE) || defined(RSI_PROP_PROTOCOL_ENABLE))
    rsi_pkt_free(&rsi_driver_cb->bt_common_cb->bt_tx_pool, pkt);
#endif
#ifdef RSI_BT_ENABLE
    rsi_pkt_free(&rsi_driver_cb->bt_classic_cb->bt_tx_pool, pkt);
#endif
#ifdef RSI_BLE_ENABLE
    rsi_pkt_free(&rsi_driver_cb->ble_cb->bt_tx_pool, pkt);
#endif
  }
#endif
#ifdef RSI_PROP_PROTOCOL_ENABLE
  if (pkt_dequeued == PROP_PROTOCOL_PKT) {
    rsi_pkt_free(&rsi_driver_cb->prop_protocol_cb->bt_tx_pool, pkt);
  }
#endif
}
/*==============================================*/
/**
 * @fn          void rsi_error_timeout_and_clear_events(int32_t error, uint32_t cmd_type)
 * @brief       Clear the event and gives timeout error
 * @param[in]   error - Error 
 * @param[in]   cmd_type - command type    
 * @return      void
 */
/// @private
void rsi_error_timeout_and_clear_events(int32_t error, uint32_t cmd_type)
{
  rsi_clear_event(RSI_TX_EVENT);
  rsi_clear_event(RSI_RX_EVENT);
  if (cmd_type == TX_EVENT_CMD) {
#ifndef RSI_TX_EVENT_HANDLE_TIMER_DISABLE
    rsi_driver_cb_non_rom->driver_timer_start = 0;
#endif
#ifndef RSI_CHECK_PKT_QUEUE
    rsi_check_pkt_queue_and_dequeue();
#endif
  } else if (cmd_type == RX_EVENT_CMD) {
#ifndef RSI_RX_EVENT_HANDLE_TIMER_DISABLE
    rsi_driver_cb_non_rom->driver_rx_timer_start = 0;
#endif
  }
#ifndef RSI_RELEASE_SEMAPHORE
  rsi_release_waiting_semaphore();
#endif
#ifndef RSI_WAIT_TIMEOUT_EVENT_HANDLE_TIMER_DISABLE
  if (rsi_driver_cb_non_rom->rsi_wait_timeout_handler_error_cb != NULL) {
    rsi_driver_cb_non_rom->rsi_wait_timeout_handler_error_cb(error, cmd_type);
  }
#endif
  return;
}

/*==============================================*/
/**
 * @fn          void rsi_check_waiting_cmds(rsi_rsp_waiting_cmds_t *reponse)
 * @brief       Check the commands which are waiting for the response and sets the flag
 * @param[in]   buffer - Buffer 
 * @return      void 
 *
 *
 */
int32_t rsi_check_waiting_cmds(rsi_rsp_waiting_cmds_t *response)
{
#ifdef RSI_WLAN_ENABLE
  int32_t i = 0;
#endif
  if (response == NULL) {
    return RSI_FAILURE;
  } else {
    memset(response, 0, sizeof(rsi_rsp_waiting_cmds_t));
    if (rsi_driver_cb_non_rom->common_wait_bitmap & BIT(0)) {
      response->waiting_cmds |= BIT(0);
    }
#ifdef RSI_WLAN_ENABLE
    if (rsi_driver_cb_non_rom->wlan_wait_bitmap & BIT(1)) {
      response->waiting_cmds |= BIT(1);
    }
    if (rsi_driver_cb_non_rom->nwk_wait_bitmap & BIT(0)) {
      response->waiting_cmds |= BIT(2);
    }
    for (i = 0; i < NUMBER_OF_SOCKETS; i++) {
      if (rsi_socket_pool[i].sock_state > RSI_SOCKET_STATE_INIT) {
        if (rsi_socket_pool_non_rom[i].socket_wait_bitmap) {
          response->waiting_cmds |= BIT(3);
          break;
        }
      }
    }
    for (i = 0; i < RSI_NUMBER_OF_SELECTS; i++) {
      if (rsi_socket_select_info[i].select_state == RSI_SOCKET_SELECT_STATE_CREATE) {
        response->waiting_cmds |= BIT(4);
        break;
      }
    }
    if (rsi_driver_cb_non_rom->send_wait_bitmap & BIT(0)) {
      response->waiting_cmds |= BIT(5);
    }
#endif
#if (defined RSI_BT_ENABLE || defined RSI_BLE_ENABLE || defined RSI_PROP_PROTOCOL_ENABLE)
    if (rsi_driver_cb_non_rom->bt_wait_bitmap & BIT(1)) {
      response->waiting_cmds |= BIT(6);
    }
#endif
#ifdef RSI_BT_ENABLE
    if (rsi_driver_cb_non_rom->bt_wait_bitmap & BIT(2)) {
      response->waiting_cmds |= BIT(7);
    }
#endif
#ifdef RSI_BLE_ENABLE
    if (rsi_driver_cb_non_rom->bt_wait_bitmap & BIT(3)) {
      response->waiting_cmds |= BIT(8);
    }
#endif
#ifdef RSI_PROP_PROTOCOL_ENABLE
    if (rsi_driver_cb_non_rom->bt_wait_bitmap & BIT(4)) {
      response->waiting_cmds |= BIT(9);
    }
#endif
    response->rx_driver_flag              = rsi_driver_cb_non_rom->rx_driver_flag;
    rsi_driver_cb_non_rom->rx_driver_flag = 1;
  }
  return RSI_SUCCESS;
}
