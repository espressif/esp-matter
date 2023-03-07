/*******************************************************************************
* @file  rsi_common_apis.c
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
#ifdef RSI_M4_INTERFACE
#include "rsi_ipmu.h"
#endif
#include "rsi_timer.h"
#ifdef RSI_BT_ENABLE
#include "rsi_bt.h"
#include "rsi_bt_config.h"
#endif
#ifdef RSI_BLE_ENABLE
#include "rsi_ble.h"
#endif

#include "rsi_wlan_non_rom.h"
extern rsi_socket_info_non_rom_t *rsi_socket_pool_non_rom;
extern rsi_socket_select_info_t *rsi_socket_select_info;
#ifdef PROCESS_SCAN_RESULTS_AT_HOST
extern struct wpa_scan_results_arr *scan_results_array;
#endif
/*
  Global Variables
 * */
rsi_driver_cb_t *rsi_driver_cb = NULL;
#ifdef RSI_M4_INTERFACE
extern efuse_ipmu_t global_ipmu_calib_data;
#endif
extern global_cb_t *global_cb;
extern rom_apis_t *rom_apis;
extern void rom_init(void);
int32_t rsi_driver_memory_estimate(void);

/** @addtogroup COMMON 
* @{
*/
/*==============================================*/
/**
 * @brief       Return common block status. This is a non-blocking API.
 * @param[in]   Void
 * @return      0              -  Success \n
 *              Non-Zero Value -  Failure
 */

int32_t rsi_common_get_status(void)
{
  return rsi_driver_cb->common_cb->status;
}

/*==============================================*/
/**
 * @brief       Set the common block status. This is a non-blocking API.
 * @param[in]   status         -    Status of common control block
 * @return      0              - Success \n
 *              Non-Zero Value - Failure
 */
/// @private

void rsi_common_set_status(int32_t status)
{
  rsi_driver_cb->common_cb->status = status;
}

/*==============================================*/
/**
 *
 * @brief      Initialize WiSeConnect driver.  This is a non-blocking API.
 *             Designate memory to all driver components from the buffer provided by the application.
 *  ...........Initilize Scheduler, Events, and Queues needed.
 * @param[in]  buffer      -    Pointer to buffer from application. \n Driver uses this buffer to hold driver control for its operation.
 * @param[in]  length      -    Length of the buffer.
 * @return     **Success** -    Returns the memory used, which is less than or equal to buffer length provided. \n
 *             **Failure** -    Non-Zero values\n
 *
 *             			**RSI_ERROR_TIMEOUT**         -    If UART initialization fails in SPI / UART mode   \n
 *
 *             			**RSI_ERROR_INVALID_PARAM**   -    If maximum sockets is greater than 10
 */

/** @} */
uint8_t *buffer_addr = NULL;
int32_t rsi_driver_init(uint8_t *buffer, uint32_t length)
{
#if (defined RSI_WLAN_ENABLE) || (defined RSI_UART_INTERFACE) | (defined LINUX_PLATFORM) || (defined RSI_BLE_ENABLE) \
  || (defined RSI_BT_ENABLE)
  int32_t status = RSI_SUCCESS;
#endif

  uint32_t actual_length = 0;

  // If (((uint32_t)buffer & 3) != 0)
  if (((uintptr_t)buffer & 3) != 0) // To avoid compiler warning, replace uint32_t with uintptr_t
  {
    // Making buffer 4 byte aligned
    // Length -= (4 - ((uint32_t)buffer & 3));
    // To avoid compiler warning, replace uint32_t with uintptr_t
    length -= (4 - ((uintptr_t)buffer & 3));
    // Buffer = (uint8_t *)(((uint32_t)buffer + 3) & ~3);// To avoid compiler warning, replace uint32_t with uintptr_t
    buffer = (uint8_t *)(((uintptr_t)buffer + 3) & ~3);
  }

  // Memset user buffer
  memset(buffer, 0, length);

  actual_length += rsi_driver_memory_estimate();

  // If length is not sufficient
  if (length < actual_length) {
    return actual_length;
  }
  buffer_addr = buffer;

  // Store length minus any alignment bytes to first 32-bit address in buffer.
  *(uint32_t *)buffer = length;
  buffer += sizeof(uint32_t);

  // Designate memory for driver cb
  rsi_driver_cb = (rsi_driver_cb_t *)buffer;
  buffer += sizeof(rsi_driver_cb_t);
  global_cb = (global_cb_t *)buffer;
  buffer += sizeof(global_cb_t);
  rom_apis = (rom_apis_t *)buffer;
  buffer += sizeof(rom_apis_t);
#ifdef RSI_WLAN_ENABLE
  // Memory for sockets
  rsi_socket_pool = (rsi_socket_info_t *)buffer;
  buffer += RSI_SOCKET_INFO_POOL_SIZE;
  rsi_socket_pool_non_rom = (rsi_socket_info_non_rom_t *)buffer;
  buffer += RSI_SOCKET_INFO_POOL_ROM_SIZE;
  rsi_socket_select_info = (rsi_socket_select_info_t *)buffer;
  buffer += RSI_SOCKET_SELECT_INFO_POOL_SIZE;
  rsi_wlan_cb_non_rom = (rsi_wlan_cb_non_rom_t *)buffer;
  buffer += RSI_WLAN_CB_NON_ROM_POOL_SIZE;
#ifdef PROCESS_SCAN_RESULTS_AT_HOST
  scan_results_array = (struct wpa_scan_results_arr *)buffer;
  buffer += sizeof(struct wpa_scan_results_arr);
#endif

#ifdef SAPI_LOGGING_ENABLE
  buffer += sl_log_init(buffer);
#endif
#ifdef FW_LOGGING_ENABLE
  buffer += sl_fw_log_init(buffer);
#endif

  // Check for max no of sockets
  if (RSI_NUMBER_OF_SOCKETS > (10 + RSI_NUMBER_OF_LTCP_SOCKETS)) {
    status = RSI_ERROR_INVALID_PARAM;
    SL_PRINTF(SL_DRIVER_INIT_MAX_SOCKETS_ERROR, COMMON, LOG_ERROR, "status: %4x", status);
    return status;
  }
#endif

// Disabled in M4 A11 condition; Enabled in rest all condition.
#if !(defined(RSI_M4_INTERFACE) && !defined(A11_ROM))
  rsi_driver_cb->event_list = (rsi_event_cb_t *)buffer;
  buffer += RSI_EVENT_INFO_POOL_SIZE;
#endif

  rom_init();

  // Designate memory for rx_pool
#if !((defined RSI_SDIO_INTERFACE) && (!defined LINUX_PLATFORM))
  rsi_pkt_pool_init(&rsi_driver_cb->rx_pool, buffer, RSI_DRIVER_RX_POOL_SIZE, RSI_DRIVER_RX_PKT_LEN);
  buffer += RSI_DRIVER_RX_POOL_SIZE;
#endif
  // Designate memory for common_cb
  rsi_driver_cb_non_rom = (rsi_driver_cb_non_rom_t *)buffer;
  buffer += sizeof(rsi_driver_cb_non_rom_t);

  // Designate memory for common_cb
  rsi_driver_cb->common_cb = (rsi_common_cb_t *)buffer;
  buffer += sizeof(rsi_common_cb_t);

  // Initialize common cb
  rsi_common_cb_init(rsi_driver_cb->common_cb);

  // Designate pool for common block
  rsi_pkt_pool_init(&rsi_driver_cb->common_cb->common_tx_pool, buffer, RSI_COMMON_POOL_SIZE, RSI_COMMON_CMD_LEN);
  buffer += RSI_COMMON_POOL_SIZE;

  // Designate memory for wlan block
  rsi_driver_cb->wlan_cb = (rsi_wlan_cb_t *)buffer;
  buffer += sizeof(rsi_wlan_cb_t);

#ifdef RSI_M4_INTERFACE
  // Designate memory for efuse ipmu block
  rsi_driver_cb->common_cb->ipmu_calib_data_cb = (efuse_ipmu_t *)buffer;
  // ipmu_calib_data_cb = (efuse_ipmu_t *)buffer;
  // efuse_size = sizeof(efuse_ipmu_t);
  buffer += RSI_M4_MEMORY_POOL_SIZE;
#endif
#ifdef RSI_WLAN_ENABLE
  // Initialize wlan cb
  rsi_wlan_cb_init(rsi_driver_cb->wlan_cb);
#endif

  // Designate memory for wlan_cb pool
  rsi_pkt_pool_init(&rsi_driver_cb->wlan_cb->wlan_tx_pool, buffer, RSI_WLAN_POOL_SIZE, RSI_WLAN_CMD_LEN);
  buffer += RSI_WLAN_POOL_SIZE;

  // Initialize scheduler
  rsi_scheduler_init(&rsi_driver_cb->scheduler_cb);

  // Initialize events
  rsi_events_init();

  rsi_queues_init(&rsi_driver_cb->wlan_tx_q);

  rsi_queues_init(&rsi_driver_cb->common_tx_q);
#ifdef RSI_M4_INTERFACE
  rsi_queues_init(&rsi_driver_cb->m4_tx_q);

  rsi_queues_init(&rsi_driver_cb->m4_rx_q);

#endif
#ifdef RSI_ZB_ENABLE
  rsi_driver_cb->zigb_cb = (rsi_zigb_cb_t *)buffer;
  buffer += sizeof(rsi_zigb_cb_t);

  // Initialize zigb cb
  rsi_zigb_cb_init(rsi_driver_cb->zigb_cb);

  // Designate memory for zigb_cb buffer pool
  rsi_pkt_pool_init(&rsi_driver_cb->zigb_cb->zigb_tx_pool, buffer, RSI_ZIGB_POOL_SIZE, RSI_ZIGB_CMD_LEN);
  buffer += RSI_ZIGB_POOL_SIZE;

#ifdef ZB_MAC_API
  rsi_driver_cb->zigb_cb->zigb_global_mac_cb = (rsi_zigb_global_mac_cb_t *)buffer;
  buffer += sizeof(rsi_zigb_global_mac_cb_t);
  // Fill in zigb_global_cb
  buffer += rsi_zigb_global_mac_cb_init(buffer);
#else
  rsi_driver_cb->zigb_cb->zigb_global_cb = (rsi_zigb_global_cb_t *)buffer;
  buffer += sizeof(rsi_zigb_global_cb_t);

  // Fill in zigb_global_cb
  buffer += rsi_zigb_global_cb_init(buffer);
#endif
  rsi_queues_init(&rsi_driver_cb->zigb_tx_q);
#ifdef ZB_DEBUG
  printf("\n ZIGB POOL INIT \n");
#endif
#endif

#if defined(RSI_BT_ENABLE) || defined(RSI_BLE_ENABLE) || defined(RSI_PROP_PROTOCOL_ENABLE)
  // Designate memory for bt_common_cb
  rsi_driver_cb->bt_common_cb = (rsi_bt_cb_t *)buffer;
  buffer += ((sizeof(rsi_bt_cb_t) + 3) & ~3);

  // Initialize bt_common_cb
  rsi_bt_cb_init(rsi_driver_cb->bt_common_cb, RSI_PROTO_BT_COMMON);

  // Designate memory for bt_common_cb pool
  rsi_pkt_pool_init(&rsi_driver_cb->bt_common_cb->bt_tx_pool, buffer, RSI_BT_COMMON_POOL_SIZE, RSI_BT_COMMON_CMD_LEN);
  buffer += ((RSI_BT_COMMON_POOL_SIZE + 3) & ~3);

  rsi_queues_init(&rsi_driver_cb->bt_single_tx_q);
#endif

#ifdef RSI_BT_ENABLE
  // Designate memory for bt_classic_cb
  rsi_driver_cb->bt_classic_cb = (rsi_bt_cb_t *)buffer;
  buffer += ((sizeof(rsi_bt_cb_t) + 3) & ~3);

  // Initialize bt_classic_cb
  rsi_bt_cb_init(rsi_driver_cb->bt_classic_cb, RSI_PROTO_BT_CLASSIC);

  // Designate memory for bt_classic_cb pool
  rsi_pkt_pool_init(&rsi_driver_cb->bt_classic_cb->bt_tx_pool,
                    buffer,
                    RSI_BT_CLASSIC_POOL_SIZE,
                    RSI_BT_CLASSIC_CMD_LEN);
  buffer += ((RSI_BT_CLASSIC_POOL_SIZE + 3) & ~3);
#endif

#ifdef RSI_BLE_ENABLE
  // Designate memory for ble_cb
  rsi_driver_cb->ble_cb = (rsi_bt_cb_t *)buffer;
  buffer += ((sizeof(rsi_bt_cb_t) + 3) & ~3);

  // Initialize ble_cb
  rsi_bt_cb_init(rsi_driver_cb->ble_cb, RSI_PROTO_BLE);

  // Designate memory for ble_cb pool
  rsi_pkt_pool_init(&rsi_driver_cb->ble_cb->bt_tx_pool, buffer, RSI_BLE_POOL_SIZE, RSI_BLE_CMD_LEN);
  buffer += ((RSI_BLE_POOL_SIZE + 3) & ~3);
#endif

#ifdef RSI_PROP_PROTOCOL_ENABLE
  // Designate memory for prop_protocol_cb
  rsi_driver_cb->prop_protocol_cb = (rsi_bt_cb_t *)buffer;
  buffer += ((sizeof(rsi_bt_cb_t) + 3) & ~3);

  // Initialize prop_protocol_cb
  rsi_bt_cb_init(rsi_driver_cb->prop_protocol_cb, RSI_PROTO_PROP_PROTOCOL);

  // Designate memory for prop_protocol_cb pool
  rsi_pkt_pool_init(&rsi_driver_cb->prop_protocol_cb->bt_tx_pool,
                    buffer,
                    RSI_PROP_PROTOCOL_POOL_SIZE,
                    RSI_PROP_PROTOCOL_CMD_LEN);
  buffer += ((RSI_PROP_PROTOCOL_POOL_SIZE + 3) & ~3);

  rsi_queues_init(&rsi_driver_cb->prop_protocol_tx_q);
#endif

#ifdef SAPIS_BT_STACK_ON_HOST
  // Designate memory for bt_classic_cb
  rsi_driver_cb->bt_ble_stack_cb = (rsi_bt_cb_t *)buffer;
  buffer += sizeof(rsi_bt_cb_t);

  // Initialize bt_classic_cb
  rsi_bt_cb_init(rsi_driver_cb->bt_ble_stack_cb, RSI_PROTO_BT_BLE_STACK);

  // Designate memory for bt_classic_cb pool
  rsi_pkt_pool_init(&rsi_driver_cb->bt_ble_stack_cb->bt_tx_pool,
                    buffer,
                    RSI_BT_CLASSIC_POOL_SIZE,
                    RSI_BT_CLASSIC_CMD_LEN);
  buffer += RSI_BT_CLASSIC_POOL_SIZE;

  rsi_queues_init(&rsi_driver_cb->bt_ble_stack_tx_q);

#endif

#if defined(RSI_BT_ENABLE) || defined(RSI_BLE_ENABLE) || defined(RSI_PROP_PROTOCOL_ENABLE)
  // Designate memory for bt_common_cb
  rsi_driver_cb->bt_global_cb = (rsi_bt_global_cb_t *)buffer;
  buffer += sizeof(rsi_bt_global_cb_t);

  // Fill in bt_global_cb
  buffer += rsi_bt_global_cb_init(rsi_driver_cb, buffer);
#endif

  if (length < (uint32_t)(buffer - buffer_addr)) {
    SL_PRINTF(SL_DRIVER_INIT_INSUFFICIENT_BUFFER_2, COMMON, LOG_ERROR, "length: %4x", (uint32_t)(buffer - buffer_addr));
    return buffer - buffer_addr;
  }

#ifndef LINUX_PLATFORM
#ifdef RSI_SPI_INTERFACE
  rsi_timer_start(RSI_TIMER_NODE_0,
                  RSI_HAL_TIMER_TYPE_PERIODIC,
                  RSI_HAL_TIMER_MODE_MILLI,
                  1,
                  rsi_timer_expiry_interrupt_handler);
#endif

#if (defined(RSI_SPI_INTERFACE) || defined(RSI_SDIO_INTERFACE))
  // Configure power save GPIOs
  rsi_powersave_gpio_init();
#endif
#endif

#ifdef LINUX_PLATFORM
#if (defined(RSI_USB_INTERFACE) || defined(RSI_SDIO_INTERFACE))
  status = rsi_linux_app_init();
  if (status != RSI_SUCCESS) {
    return status;
  }

#endif
#endif

#ifdef RSI_UART_INTERFACE
  // UART initialization
  status = rsi_uart_init();
  if (status != RSI_SUCCESS) {
    SL_PRINTF(SL_DRIVER_INIT_UART_INIT_ERROR, COMMON, LOG_ERROR, "status: %4x", status);
    return status;
  }
#endif

  // Update state
  rsi_driver_cb_non_rom->device_state = RSI_DRIVER_INIT_DONE;

  SL_PRINTF(SL_DRIVER_INIT_EXIT, COMMON, LOG_INFO, "actual_length=%4x", actual_length);
  return actual_length;
}
/** @addtogroup COMMON 
* @{
*/

/*==============================================*/

/**
 * @brief      Initialize WiSeConnect or Module features. This is a blocking API.
 *             RSI_SUCCESS return value indicates that the opermode and coex value were set successfully in the FW.
 *             In case of failure, appropriate error code is returned to the application. 
               Apart from setting WLAN/Coex Operating mode, this API also configures other features based on selected Feature Bitmaps. 
               In this API, configured feature bitmaps are internally processed and sent to firmware. These feature bitmaps are also called Opermode Command Parameters. 
               Default configurations (for reference) are available in rsi_wlan_common_config.h. 
               Based on the features required for a specific example, modify the rsi_wlan_config.h provided in the respective example folder.
               For more information about these feature bitmaps, refer \ref SP18.
 * @pre        \ref rsi_driver_init() followed by \ref rsi_device_init() API needs to be called before this API. 
 * @param[in]  opermode        -    WLAN Operating mode \n
 *			                        0 - Client mode \n
 *			                        2 - Enterprise security client mode \n
 *			                        6 - Access point mode \n
 *			                        8 - Transmit test mode \n
 *			                        9 - Concurrent mode
 * @note 			 Opermode WiFi-Direct(1) mode is not supported.
 * @param[in]  coex_mode       -    Coexistence mode
 *                                  0 - WLAN only mode \n
 *                                  1 - WLAN \n
 *                                  4 - Bluetooth \n
 *                                  5 - WLAN + Bluetooth \n
 *                                  8 - Dual Mode (Bluetooth and BLE) \n
 *                                  9 - WLAN + Dual Mode \n
 *                                  12- BLE mode \n
 *                                  13- WLAN + BLE \n
 *                                   
 * @note 			1. Coex modes are supported only in 384K memory configuration.
 * @note			2. Coex mode 4(Bluetooth classic), 8 (Dual mode), and 12(BLE mode) are not supported.
 * @note			3. To achieve the same functionality, use coexmode 5, 9, and 13 respectively instead of coexmode 4, 8, and 12.
 * @note			4. To achieve power save functionality, trigger 'rsi_wlan_radio_init()' API after rsi_wireless_init() API and also issue
 * both WLAN and BT power save commands. \n
 *
 * @return      **Success** - RSI_SUCCESS  \n
 *              **Failure** - Non-Zero Value \n
 *
 *                              `If return value is less than 0` \n
 *
 *	                         **RSI_ERROR_INVALID_PARAM** - Invalid parameters \n 
 *
 *				 **RSI_ERROR_COMMAND_GIVEN_IN_WRONG_STATE** - Command given in wrong state \n
 *
 *				 **RSI_ERROR_PKT_ALLOCATION_FAILURE** - Buffer not available to serve the command \n
 *
 *				`Other expected error codes are :` \n
 *
 *				**0x0021,0x0025,0xFF73,0x002C,0xFF6E,0xFF6F, 0xFF70,0xFFC5**
 * @note        Refer to Error Codes section for the above error codes \ref error-codes.
 */

int32_t rsi_wireless_init(uint16_t opermode, uint16_t coex_mode)
{
  rsi_pkt_t *pkt;
  rsi_opermode_t *rsi_opermode;
  int32_t status = RSI_SUCCESS;
  SL_PRINTF(SL_WIRELESS_INIT_ENTRY, COMMON, LOG_INFO);
#if !((defined(RSI_UART_INTERFACE) && defined(RSI_STM32)))
  rsi_timer_instance_t timer_instance;
#endif
#ifdef RSI_M4_INTERFACE
  // int32_t        ipmu_status = RSI_SUCCESS;

#endif
  // Get common cb pointer
  rsi_common_cb_t *common_cb = rsi_driver_cb->common_cb;
  rsi_wlan_cb_t *wlan_cb     = rsi_driver_cb->wlan_cb;

  if ((rsi_driver_cb_non_rom->device_state < RSI_DEVICE_INIT_DONE)) {
    // Command given in wrong state
    SL_PRINTF(SL_WIRELESS_INIT_COMMAND_GIVEN_IN_WRONG_STATE, COMMON, LOG_ERROR);
    return RSI_ERROR_COMMAND_GIVEN_IN_WRONG_STATE;
  }
  common_cb->ps_coex_mode = coex_mode;
  common_cb->ps_coex_mode &= ~BIT(0);

#if (defined(RSI_UART_INTERFACE) && defined(RSI_STM32))
  common_cb->state = RSI_COMMON_CARDREADY;
#else
  rsi_init_timer(&timer_instance, RSI_CARD_READY_WAIT_TIME);
  // If state is not in card ready received state
  while (common_cb->state == RSI_COMMON_STATE_NONE) {
#ifndef RSI_WITH_OS
    // Wait until receive card ready
    rsi_scheduler(&rsi_driver_cb->scheduler_cb);

    if (rsi_timer_expired(&timer_instance)) {
      SL_PRINTF(SL_WIRELESS_INIT_CARD_READY_TIMEOUT, COMMON, LOG_ERROR);
      return RSI_ERROR_CARD_READY_TIMEOUT;
    }
#else
#ifndef RSI_COMMON_SEM_BITMAP
    rsi_driver_cb_non_rom->common_wait_bitmap |= BIT(3);
#endif
#ifdef RSI_WITH_OS
    if (rsi_wait_on_common_semaphore(&common_cb->common_card_ready_sem, RSI_CARD_READY_WAIT_TIME) != RSI_ERROR_NONE) {
      SL_PRINTF(SL_WIRELESS_INIT_RESPONSE_TIMEOUT, COMMON, LOG_ERROR);
      return RSI_ERROR_RESPONSE_TIMEOUT;
    }
#endif
#endif
  }
#endif
  if (wlan_cb->auto_config_state != RSI_WLAN_STATE_NONE) {
    while (1) {
      // Check auto config state

      if ((wlan_cb->auto_config_state == RSI_WLAN_STATE_AUTO_CONFIG_DONE)
          || (wlan_cb->auto_config_state == RSI_WLAN_STATE_AUTO_CONFIG_FAILED)) {
        if (wlan_cb->state >= RSI_WLAN_STATE_INIT_DONE) {
          common_cb->ps_coex_mode |= BIT(0);
        }
        wlan_cb->auto_config_state = 0;
        break;
      }
#ifndef RSI_WLAN_SEM_BITMAP
      rsi_driver_cb_non_rom->wlan_wait_bitmap |= BIT(0);
#endif

#ifdef RSI_WLAN_ENABLE
      // Signal the WLAN semaphore
      if (rsi_wait_on_wlan_semaphore(&rsi_driver_cb_non_rom->wlan_cmd_sem, RSI_AUTO_JOIN_RESPONSE_WAIT_TIME)
          != RSI_ERROR_NONE) {
        SL_PRINTF(SL_WIRELESS_INIT_RESPONSE_TIMEOUT, COMMON, LOG_ERROR);
        return RSI_ERROR_RESPONSE_TIMEOUT;
      }
#endif
    }
#ifdef RSI_WLAN_ENABLE
    status = rsi_common_get_status();
    if (status) {
      // Return error
      SL_PRINTF(SL_WIRELESS_INIT_EXIT1, COMMON, LOG_ERROR, "status: %4x", status);
      return status;
    }
    status = rsi_wlan_get_status();
    if (status) {
      // Return error
      SL_PRINTF(SL_WIRELESS_INIT_EXIT2, COMMON, LOG_ERROR, "status: %4x", status);
      return status;
    }
#endif
    // Auto configuration status
    return RSI_USER_STORE_CFG_STATUS;
  } else if (common_cb->state != RSI_COMMON_CARDREADY) {
    // Command given in wrong state
    SL_PRINTF(SL_WIRELESS_INIT_COMMAND_GIVEN_IN_WRONG_STATE, COMMON, LOG_ERROR);
    return RSI_ERROR_COMMAND_GIVEN_IN_WRONG_STATE;
  }

  status = rsi_check_and_update_cmd_state(COMMON_CMD, IN_USE);
  if (status == RSI_SUCCESS) {

    // Allocate command buffer
    pkt = rsi_pkt_alloc(&common_cb->common_tx_pool);
    // If allocation of packet fails
    if (pkt == NULL) {
      //Change common state to allow state
      rsi_check_and_update_cmd_state(COMMON_CMD, ALLOW);
      // Return packet allocation failure
      SL_PRINTF(SL_WIRELESS_INIT_PKT_ALLOCATION_FAILURE, COMMON, LOG_ERROR);
      return RSI_ERROR_PKT_ALLOCATION_FAILURE;
    }

    // memset the packet data
    memset(&pkt->data, 0, sizeof(rsi_opermode_t));

    // Take the user provided data and fill it in opermode mode structure
    rsi_opermode = (rsi_opermode_t *)pkt->data;

    // Save opermode command
    wlan_cb->opermode = opermode;

    // Fill coex and opermode parameters
    rsi_uint32_to_4bytes(rsi_opermode->opermode, (coex_mode << 16 | opermode));

#ifndef RSI_COMMON_SEM_BITMAP
    rsi_driver_cb_non_rom->common_wait_bitmap |= BIT(0);
#endif
#if defined(RSI_BT_ENABLE) || defined(RSI_BLE_ENABLE) || defined(RSI_PROP_PROTOCOL_ENABLE)
    // Save expected response type
    rsi_driver_cb->bt_common_cb->expected_response_type = RSI_BT_EVENT_CARD_READY;
    rsi_driver_cb->bt_common_cb->sync_rsp               = 1;
#endif
    // Send opermode command to driver
    status = rsi_driver_common_send_cmd(RSI_COMMON_REQ_OPERMODE, pkt);

    // Wait on common semaphore
    rsi_wait_on_common_semaphore(&rsi_driver_cb_non_rom->common_cmd_sem, RSI_OPERMODE_RESPONSE_WAIT_TIME);

    // Change common state to allow state
    rsi_check_and_update_cmd_state(COMMON_CMD, ALLOW);

  }

  else {
    // Return common command error
    SL_PRINTF(SL_WIRELESS_INIT_COMMAND_ERROR, COMMON, LOG_ERROR, "status: %4x", status);
    return status;
  }

  // Get common command response status
  status = rsi_common_get_status();

#if defined(RSI_BT_ENABLE) || defined(RSI_BLE_ENABLE) || defined(RSI_PROP_PROTOCOL_ENABLE)
  if (!status) {
    if ((coex_mode == RSI_OPERMODE_WLAN_BLE) || (coex_mode == RSI_OPERMODE_WLAN_BT_CLASSIC)
        || (coex_mode == RSI_OPERMODE_WLAN_BT_DUAL_MODE)
#if defined(RSI_PROP_PROTOCOL_ENABLE)
        || (coex_mode == RSI_OPERMODE_PROP_PROTOCOL)
#endif
    ) {
      // WC waiting for BT Classic/ZB/BLE card ready
      rsi_bt_common_init();
    }
  }
#endif

#ifdef RSI_M4_INTERFACE
  // ipmu_status = rsi_cmd_m4_ta_secure_handshake(2,0,NULL,sizeof(efuse_ipmu_t),(uint8_t *)&global_ipmu_calib_data);
  // RSI_IPMU_UpdateIpmuCalibData(&global_ipmu_calib_data);
#endif

  // Return status
  SL_PRINTF(SL_WIRELESS_INIT_EXIT3, COMMON, LOG_INFO, "status: %4x", status);
  return status;
}
/** @} */
/*==============================================*/

/** @addtogroup COMMON 
* @{
*/
/**
 * @brief      Enable or disable UART flow control. This is a blocking API.
 * @param[in]  uartflow_en  -    Enable or Disable UART hardware flow control \n
 *                               1/2 - Enable and Pin set to be used to for RTS/CTS purpose. \n
 *                               0   - Disable  \n
 *                               If uart_hw_flowcontrol_enable parameter is 0, uart flow control is disabled. \n
 *                               If the parameter is given as 1 or 2, then UART hardware flow control is enabled and Pin set to be used \n
 *                               If parameter is given as 1: Pin set used for RTS/CTS functionality is: \n
 *                                     UART_CTS:    GPIO - 11 \n
 *                                     UART_RTS:    GPIO - 7 \n
 *                               If parameter is given as 2: Pin set used for RTS/CTS functionality is: \n
 *                                     UART_CTS:    GPIO - 15 \n
 *                                     UART_RTS:    GPIO - 12 
 * @return     0              - Success \n
 *             Negative Value - Failure
 * @note       Hardware flow control feature is not supported in Auto-Join/Auto-Create mode. \n
 *             In such a case, the feature has to be enabled separately.
 * 
 */
int32_t rsi_cmd_uart_flow_ctrl(uint8_t uartflow_en)
{
  rsi_pkt_t *pkt;

  int32_t status = RSI_SUCCESS;
  SL_PRINTF(SL_CMD_UART_FLOW_CTRL_ENTRY, COMMON, LOG_INFO);

  // Get common cb pointer
  rsi_common_cb_t *common_cb = rsi_driver_cb->common_cb;

  status = rsi_check_and_update_cmd_state(COMMON_CMD, IN_USE);
  if (status == RSI_SUCCESS) {
    // Allocate command buffer
    pkt = rsi_pkt_alloc(&common_cb->common_tx_pool);

    // If allocation of packet fails
    if (pkt == NULL) {
      //Change common state to allow state
      rsi_check_and_update_cmd_state(COMMON_CMD, ALLOW);
      // Return packet allocation failure error
      SL_PRINTF(SL_CMD_UART_FLOW_CTRL_PKT_ALLOCATION_FAILURE, COMMON, LOG_ERROR, "status: %4x", status);
      return RSI_ERROR_PKT_ALLOCATION_FAILURE;
    }

    pkt->data[0] = uartflow_en;
#ifndef RSI_COMMON_SEM_BITMAP
    rsi_driver_cb_non_rom->common_wait_bitmap |= BIT(0);
#endif
    // Send  antenna select command
    status = rsi_driver_common_send_cmd(RSI_COMMON_REQ_UART_FLOW_CTRL_ENABLE, pkt);

    //Wait on common semaphore
    rsi_wait_on_common_semaphore(&rsi_driver_cb_non_rom->common_cmd_sem, RSI_UART_FLOW_CTRL_RESPONSE_WAIT_TIME);

    // Change common state to allow state
    rsi_check_and_update_cmd_state(COMMON_CMD, ALLOW);

  }

  else {
    // Return common command error
    SL_PRINTF(SL_CMD_UART_FLOW_CTRL_COMMAND_ERROR, COMMON, LOG_ERROR, "status: %4x", status);
    return status;
  }

  // Get common command response status
  status = rsi_common_get_status();

  // Return status
  SL_PRINTF(SL_CMD_UART_FLOW_CTRL_EXIT, COMMON, LOG_INFO, "status: %4x", status);
  return status;
}

#ifdef RSI_M4_INTERFACE
#ifdef CHIP_9117
/*==============================================*/
/**
 * @brief      to write content on TA flash from M4. This is a blocking API.
 * @param[in]  wr_addr        - address at which data will be written on TA flash memory
 * @param[in]  write_data     - Input data
 * @param[in]  wr_data_len    - total length
 * @return     0              - Success \n
 *             Negative Value - Failure
 *
 *
 */
int32_t rsi_cmd_to_wr_comm_flash(uint32_t wr_addr, uint8_t *write_data, uint16_t wr_data_len)
{
  static uint32_t rem_len;
  uint16_t chunk_size = 0;
  static uint32_t offset;
  rsi_pkt_t *pkt;
  rsi_req_ta2m4_t *rsi_chunk_ptr;
  int32_t status = RSI_SUCCESS;

  SL_PRINTF(SL_CMD_M4_TA_SECURE_HANDSHAKE_ENTRY, COMMON, LOG_INFO);

  // Get common cb pointer
  rsi_common_cb_t *common_cb = rsi_driver_cb->common_cb;

  status = rsi_check_and_update_cmd_state(COMMON_CMD, IN_USE);

  if (status == RSI_SUCCESS) {

    // Get the chunk size
    chunk_size = RSI_MAX_CHUNK_SIZE - (sizeof(rsi_req_ta2m4_t) - RSI_MAX_CHUNK_SIZE);

    // Get input length
    rem_len = wr_data_len;

    while (rem_len) {

      // Allocate command buffer
      pkt = rsi_pkt_alloc(&common_cb->common_tx_pool);

      // If allocation of packet fails
      if (pkt == NULL) {

        //Change common state to allow state
        rsi_check_and_update_cmd_state(COMMON_CMD, ALLOW);

        // Return packet allocation failure error
        SL_PRINTF(SL_SI_CMD_M4_TA_SECURE_HANDSHAKE_PKT_ALLOCATION_FAILURE, COMMON, LOG_ERROR);

        return RSI_ERROR_PKT_ALLOCATION_FAILURE;
      }

      rsi_chunk_ptr = (rsi_req_ta2m4_t *)pkt->data;

      memset(&pkt->data, 0, RSI_MAX_CHUNK_SIZE);

      // Take the sub_cmd_type for TA and M4 commands
      rsi_chunk_ptr->sub_cmd = RSI_WRITE_TO_COMMON_FLASH;

      // Writes on which TA Flash location
      rsi_chunk_ptr->addr = wr_addr;

      // Total remaining length
      rsi_chunk_ptr->in_buf_len = rem_len;

      if (rem_len >= chunk_size) {

        //Total chunck length
        rsi_chunk_ptr->chunk_len = chunk_size;

        // More chunks to send
        rsi_chunk_ptr->more_chunks = 1;

        // Copy the chunk
        memcpy(rsi_chunk_ptr->input_data, write_data + offset, chunk_size);

        // Move the offset by chunk size
        offset += chunk_size;

        // Subtract the rem_len by the chunk size
        rem_len -= chunk_size;
      } else {

        rsi_chunk_ptr->chunk_len = rem_len;

        // last chunk to send
        rsi_chunk_ptr->more_chunks = 0;

        // Copy the chunk
        memcpy(rsi_chunk_ptr->input_data, write_data + offset, rem_len);

        // Reset rem_len and offset
        rem_len = 0;
        offset  = 0;
      }
#ifndef RSI_COMMON_SEM_BITMAP
      rsi_driver_cb_non_rom->common_wait_bitmap |= BIT(0);
#endif
      // Send  antenna select command
      status = rsi_driver_common_send_cmd(RSI_COMMON_REQ_TA_M4_COMMANDS, pkt);

      // Wait on common semaphore
      rsi_wait_on_common_semaphore(&rsi_driver_cb_non_rom->common_cmd_sem, RSI_TA_M4_COMMAND_RESPONSE_WAIT_TIME);

      // Get common command response status
      status = rsi_common_get_status();
    }

    // Change common state to allow state
    rsi_check_and_update_cmd_state(COMMON_CMD, ALLOW);
  } else {
    // Return common command error
    SL_PRINTF(SL_SI_CMD_M4_TA_SECURE_HANDSHAKE_COMMAND_ERROR, COMMON, LOG_ERROR, "status: %4x", status);
    return status;
  }

  // Get common command response status
  status = rsi_common_get_status();

  // Return status
  SL_PRINTF(SL_SI_CMD_M4_TA_SECURE_HANDSHAKE_EXIT, COMMON, LOG_INFO, "status: %4x", status);
  return status;
}
#endif
/*==============================================*/
/**
 * @brief      Secure handshake. This is a blocking API.
 * @param[in]  sub_cmd_type - Sub command
 * @param[in]  input_data  - Input data
 * @param[in]  input_len  - Length length
 * @param[in]  output_len  - Output length
 * @param[in]  output_data - Output data
 * @return     0              - Success \n
 *             Negative Value - Failure
 *
 *
 */

int32_t rsi_cmd_m4_ta_secure_handshake(uint8_t sub_cmd_type,
                                       uint8_t input_len,
                                       uint8_t *input_data,
                                       uint8_t output_len,
                                       uint8_t *output_data)
{
  rsi_pkt_t *pkt;

  int32_t status = RSI_SUCCESS;
  SL_PRINTF(SL_CMD_M4_TA_SECURE_HANDSHAKE_ENTRY, COMMON, LOG_INFO);

  // Get common cb pointer
  rsi_common_cb_t *common_cb = rsi_driver_cb->common_cb;

  status = rsi_check_and_update_cmd_state(COMMON_CMD, IN_USE);
  if (status == RSI_SUCCESS) {

    // Allocate command buffer
    pkt = rsi_pkt_alloc(&common_cb->common_tx_pool);

    // If allocation of packet fails
    if (pkt == NULL) {
      //Change common state to allow state
      rsi_check_and_update_cmd_state(COMMON_CMD, ALLOW);
      // Return packet allocation failure error
      SL_PRINTF(SL_SI_CMD_M4_TA_SECURE_HANDSHAKE_PKT_ALLOCATION_FAILURE, COMMON, LOG_ERROR);
      return RSI_ERROR_PKT_ALLOCATION_FAILURE;
    }

    // Take the sub_cmd_type for TA and M4 commands
    pkt->data[0] = sub_cmd_type;

    pkt->data[1] = input_len;

    memcpy(&pkt->data[2], input_data, input_len);

    // Attach the buffer given by user
    common_cb->app_buffer = output_data;

    // Length of buffer provided by user
    common_cb->app_buffer_length = output_len;

#ifndef RSI_COMMON_SEM_BITMAP
    rsi_driver_cb_non_rom->common_wait_bitmap |= BIT(0);
#endif
    // Send  antenna select command
    status = rsi_driver_common_send_cmd(RSI_COMMON_REQ_TA_M4_COMMANDS, pkt);

    // Wait on common semaphore
    rsi_wait_on_common_semaphore(&rsi_driver_cb_non_rom->common_cmd_sem, RSI_TA_M4_COMMAND_RESPONSE_WAIT_TIME);

    // Change common state to allow state
    rsi_check_and_update_cmd_state(COMMON_CMD, ALLOW);

  }

  else {
    // Return common command error
    SL_PRINTF(SL_SI_CMD_M4_TA_SECURE_HANDSHAKE_COMMAND_ERROR, COMMON, LOG_ERROR, "status: %4x", status);
    return status;
  }

  // Get common command response status
  status = rsi_common_get_status();

  // Return status
  SL_PRINTF(SL_SI_CMD_M4_TA_SECURE_HANDSHAKE_EXIT, COMMON, LOG_INFO, "status: %4x", status);
  return status;
}
#endif
/** @} */

/** @addtogroup COMMON
* @{
*/
/*==============================================*/
/**
 * @brief        Reset WiSeConnect module, load the firmware and restart the driver.This API should be called before
 *              \ref rsi_wireless_init API, if user wants to change  the previous configuration given through 
 *              \ref rsi_wireless_init. This is a blocking API.
 * @param[in]    Void
 * @return       0              - Success \n
 *               Negative Value - Failure \n
 *                         -2: Invalid parameters \n
 *                         -3: Command given in wrong state \n
 *                         -4: Buffer not available to serve the command 
 * @note        To restart RS9116W module from host, application needs to call \ref rsi_driver_deinit() followed by
 *              \ref rsi_driver_init() and \ref rsi_device_init(). For OS cases, additionally needs to call 
 *              \ref rsi_task_destroy(driver_task_handle) to delete the driver task before calling \ref rsi_driver_deinit() and should 
 *              create again after \ref rsi_device_init() using \ref rsi_task_create()
 */

int32_t rsi_wireless_deinit(void)
{
  int32_t status = RSI_SUCCESS;
  SL_PRINTF(SL_WIRELESS_DEINIT_ENTRY, COMMON, LOG_INFO);
#ifndef RSI_M4_INTERFACE
  int32_t length = 0;
#endif
#ifdef RSI_M4_INTERFACE
  rsi_pkt_t *pkt;
  // Get common cb structure pointer
  rsi_common_cb_t *common_cb = rsi_driver_cb->common_cb;

  status = rsi_check_and_update_cmd_state(COMMON_CMD, IN_USE);
  if (status == RSI_SUCCESS) {

    // Allocate command buffer  from WLAN pool
    pkt = rsi_pkt_alloc(&common_cb->common_tx_pool);

    // If allocation of packet fails
    if (pkt == NULL) {
      // Change common state to allow state
      rsi_check_and_update_cmd_state(COMMON_CMD, ALLOW);
      // Return packet allocation failure error
      SL_PRINTF(SL_WIRELESS_DEINIT_PKT_ALLOCATION_FAILURE, COMMON, LOG_ERROR);
      return RSI_ERROR_PKT_ALLOCATION_FAILURE;
    }

    // Send softreset command
    status = rsi_driver_common_send_cmd(RSI_COMMON_REQ_SOFT_RESET, pkt);

    // Change common state to allow state
    rsi_check_and_update_cmd_state(COMMON_CMD, ALLOW);

  }

  else {
    // Return common command error
    SL_PRINTF(SL_WIRELESS_DEINIT_COMMAND_ERROR, COMMON, LOG_ERROR, "status: %4x", status);
    return status;
  }
#ifdef SOFT_RESET_ENABLE
  // Mask interrupts
  rsi_hal_intr_mask();
#endif
#ifdef SOFT_RESET_ENABLE
  // Unmask interrupts
  rsi_hal_intr_unmask();
#endif

#ifdef RSI_UART_INTERFACE
  rsi_uart_deinit();

#endif
#ifndef RSI_COMMON_SEM_BITMAP
  rsi_driver_cb_non_rom->common_wait_bitmap |= BIT(2);
#endif

  // Wait on common semaphore
  if (rsi_wait_on_common_semaphore(&rsi_driver_cb_non_rom->common_cmd_sem, RSI_DEINIT_RESPONSE_WAIT_TIME)
      != RSI_ERROR_NONE) {
    return RSI_ERROR_RESPONSE_TIMEOUT;
  }

  // Get common status
  status = rsi_common_get_status();
#else

  if (buffer_addr != NULL) {
    length = *(uint32_t *)buffer_addr;
    // Driver initialization
    status = rsi_driver_init(buffer_addr, length);
    if ((status < 0) || (status > length)) {
      SL_PRINTF(SL_WIRELESS_DEINIT_DRIVER_INIT_ERROR_1, COMMON, LOG_ERROR, "status: %4x", status);
      return status;
    }
  } else {
    return -1;
  }

  // State update
  rsi_driver_cb->common_cb->state = RSI_COMMON_STATE_NONE;
#ifdef RSI_WLAN_ENABLE
  rsi_driver_cb->wlan_cb->state   = RSI_WLAN_STATE_NONE;
#endif

  // Deinitializing SDIO Interface
#if defined(RSI_SDIO_INTERFACE)
  rsi_sdio_deinit();
#endif
  // Initialize Device
  status = rsi_device_init(LOAD_NWP_FW);
  if (status != RSI_SUCCESS) {
    SL_PRINTF(SL_WIRELESS_DEINIT_DEVICE_INIT_ERROR_2, COMMON, LOG_ERROR, "status: %4x", status);
    return status;
  }

#endif
  SL_PRINTF(SL_WIRELESS_DEINIT_EXIT, COMMON, LOG_INFO, "status: %4x", status);
  return status;
}

/*==============================================*/
/**
 * @brief        Select antenna type on the device. This is a blocking API.
 * @pre 	 \ref rsi_wireless_antenna API must be given after \ref rsi_wlan_radio_init API.
 * @param[in]    type              -  0 : RF_OUT_2/Internal Antenna is selected \n
 * 				    	              1 : RF_OUT_1/uFL connector is selected.
 * @param[in]    gain_2g           -  Currently not supported 
 * @param[in]    gain_5g           -  Currently not supported 
 * @note         Currently ignore the gain_2g, gain_5g values\n
 * @return       0                 -  Success \n
 *               Non-Zero Value    -  Failure \n
 *                                    If return value is less than 0 \n
 *                                    -4: Buffer not available to serve the command \n
 *                                    If return value is greater than 0 \n
 *                                    0x0025, 0x002C
 * @note        Refer to Error Codes section for the above error codes \ref error-codes.
 */

int32_t rsi_wireless_antenna(uint8_t type, uint8_t gain_2g, uint8_t gain_5g)
{

  rsi_pkt_t *pkt;
  rsi_antenna_select_t *rsi_antenna;
  int32_t status = RSI_SUCCESS;
  SL_PRINTF(SL_WIRELESS_ANTENNA_ENTRY, COMMON, LOG_INFO);

  // Get wlan cb structure pointer
  rsi_wlan_cb_t *wlan_cb = rsi_driver_cb->wlan_cb;

  // Get common cb pointer
  rsi_common_cb_t *common_cb = rsi_driver_cb->common_cb;

  // Pre-condition for antenna selection
  if ((wlan_cb->state < RSI_WLAN_STATE_INIT_DONE)) {
    // Command given in wrong state
    SL_PRINTF(SL_WIRELESS_ANTENNA_COMMAND_GIVEN_IN_WRONG_STATE, COMMON, LOG_ERROR);
    return RSI_ERROR_COMMAND_GIVEN_IN_WRONG_STATE;
  }

  status = rsi_check_and_update_cmd_state(COMMON_CMD, IN_USE);
  if (status == RSI_SUCCESS) {

    // Allocate command buffer
    pkt = rsi_pkt_alloc(&common_cb->common_tx_pool);

    // If allocation of packet fails
    if (pkt == NULL) {
      // Change common state to allow state
      rsi_check_and_update_cmd_state(COMMON_CMD, ALLOW);
      // Return packet allocation failure error
      SL_PRINTF(SL_WIRELESS_ANTENNA_PKT_ALLOCATION_FAILURE, COMMON, LOG_ERROR);
      return RSI_ERROR_PKT_ALLOCATION_FAILURE;
    }

    // Take the user provided data and fill it in antenna select structure
    rsi_antenna = (rsi_antenna_select_t *)pkt->data;

    // Antenna type
    rsi_antenna->antenna_value = type;

    // Antenna gain in 2.4GHz
    rsi_antenna->gain_2g = gain_2g;

    // Antenna gain in 5GHz
    rsi_antenna->gain_5g = gain_5g;

#ifndef RSI_COMMON_SEM_BITMAP
    rsi_driver_cb_non_rom->common_wait_bitmap |= BIT(0);
#endif
    // Send  antenna select command
    status = rsi_driver_common_send_cmd(RSI_COMMON_REQ_ANTENNA_SELECT, pkt);

    // Wait on common semaphore
    rsi_wait_on_common_semaphore(&rsi_driver_cb_non_rom->common_cmd_sem, RSI_ANTENNA_SEL_RESPONSE_WAIT_TIME);

    // Change common state to allow state
    rsi_check_and_update_cmd_state(COMMON_CMD, ALLOW);

  }

  else {
    // Return common command error
    SL_PRINTF(SL_WIRELESS_ANTENNA_COMMAND_ERROR, COMMON, LOG_ERROR, "status: %4x", status);
    return status;
  }

  // Get common command response status
  status = rsi_common_get_status();

  // Return status
  SL_PRINTF(SL_WIRELESS_ANTENNA_EXIT, COMMON, LOG_INFO, "status: %4x", status);
  return status;
}
/*==============================================*/
/**
 * @brief        Select internal or external RF type and clock frequency for user to pass
 *               Feature enables dynamically. This is a blocking API.
 * @pre           \ref rsi_wireless_init() API needs to be called before this API 
 * @param[in]    feature_enables - Feature enables
 * @note 	  BIT[0] - Enable Preamble duty cycling. \n
 * @note	  BIT[4] - Enable LP chain for stand-by associate mode. \n
 * @note	  BIT[5] - Enable hardware beacon drop during power save. \n
 * @note	  Remaining bits are reserved.
 * @return       0              - Success \n 
 *               Negative Value - Failure 
 *                                 If return value is less than 0 \n
 *                                 -2 : Invalid parameters \n
 *                                 -3 : Command given in wrong state \n
 *                                 -4 : Buffer not available to serve the command \n
 *                                 If return value is greater than 0 \n
 *                                 0x0021, 0xFF74
 *               
 * @note Refer to Error Codes section for above error codes \ref error-codes.
 *
 */
int32_t rsi_send_feature_frame_dyn(uint32_t feature_enables)
{

  rsi_pkt_t *pkt;
  rsi_feature_frame_t *rsi_feature_frame;
  int32_t status = RSI_SUCCESS;
  SL_PRINTF(SL_SEND_FEATURE_FRAME_ENTRY, COMMON, LOG_INFO);

  // Get common cb pointer
  rsi_common_cb_t *common_cb = rsi_driver_cb->common_cb;

  if ((common_cb->state < RSI_COMMON_OPERMODE_DONE)) {
    // Command given in wrong state
    SL_PRINTF(SL_SEND_FEATURE_FRAME_COMMAND_GIVEN_IN_WRONG_STATE, COMMON, LOG_ERROR);
    return RSI_ERROR_COMMAND_GIVEN_IN_WRONG_STATE;
  }
  status = rsi_check_and_update_cmd_state(COMMON_CMD, IN_USE);
  if (status == RSI_SUCCESS) {

    // Allocate command buffer
    pkt = rsi_pkt_alloc(&common_cb->common_tx_pool);

    // If allocation of packet fails
    if (pkt == NULL) {
      // Change common state to allow state
      rsi_check_and_update_cmd_state(COMMON_CMD, ALLOW);
      // Return packet allocation failure error
      SL_PRINTF(SL_SEND_FEATURE_FRAME_PKT_ALLOCATION_FAILURE, COMMON, LOG_ERROR);
      return RSI_ERROR_PKT_ALLOCATION_FAILURE;
    }

    // Take the user provided data and fill it in antenna select structure
    rsi_feature_frame = (rsi_feature_frame_t *)pkt->data;

    // PLL mode value
    rsi_feature_frame->pll_mode = PLL_MODE;

    // RF type
    rsi_feature_frame->rf_type = RF_TYPE;

    // Wireless mode
    rsi_feature_frame->wireless_mode = WIRELESS_MODE;

    // Enable PPP
    rsi_feature_frame->enable_ppp = ENABLE_PPP;

    // AFE type
    rsi_feature_frame->afe_type = AFE_TYPE;

    // Feature enables
    rsi_feature_frame->feature_enables = feature_enables;

    // Send antenna select command
    status = rsi_driver_common_send_cmd(RSI_COMMON_REQ_FEATURE_FRAME, pkt);

    // Wait on common semaphore
    rsi_wait_on_common_semaphore(&rsi_driver_cb_non_rom->common_cmd_sem, RSI_FEATURE_FRAME_RESPONSE_WAIT_TIME);

    // Change common state to allow state
    rsi_check_and_update_cmd_state(COMMON_CMD, ALLOW);

  }

  else {
    // Return common command error
    SL_PRINTF(SL_SEND_FEATURE_FRAME_COMMAND_ERROR, COMMON, LOG_ERROR, "status: %4x", status);
    return status;
  }

  // Get common command response status
  status = rsi_common_get_status();

  // Return status
  SL_PRINTF(SL_SEND_FEATURE_FRAME_EXIT, COMMON, LOG_INFO, "status: %4x", status);
  return status;
}
/*==============================================*/
/**
 * @brief         Select internal or external RF type and clock frequency. This is a blocking API.
 * @pre           \ref rsi_wireless_init() API needs to be called before this API 
 * @return        0              - Success \n
 *                Non-Zero Value - Failure \n
 *                                 If return value is less than 0 \n
 *                                 -2 : Invalid parameters \n
 *                                 -3 : Command given in wrong state \n
 *                                 -4 : Buffer not available to serve the command \n
 *                                 If return value is greater than 0 \n
 *                                 0x0021, 0xFF74
 * @note         Refer to Error Codes section for above error codes \ref error-codes.
 */

int32_t rsi_send_feature_frame(void)
{

  rsi_pkt_t *pkt;
  rsi_feature_frame_t *rsi_feature_frame;
  int32_t status = RSI_SUCCESS;
  SL_PRINTF(SL_SEND_FEATURE_FRAME_ENTRY, COMMON, LOG_INFO);

  // Get common cb pointer
  rsi_common_cb_t *common_cb = rsi_driver_cb->common_cb;

  if ((common_cb->state < RSI_COMMON_OPERMODE_DONE)) {
    // Command given in wrong state
    SL_PRINTF(SL_SEND_FEATURE_FRAME_COMMAND_GIVEN_IN_WRONG_STATE, COMMON, LOG_ERROR);
    return RSI_ERROR_COMMAND_GIVEN_IN_WRONG_STATE;
  }
  status = rsi_check_and_update_cmd_state(COMMON_CMD, IN_USE);
  if (status == RSI_SUCCESS) {

    // Allocate command buffer
    pkt = rsi_pkt_alloc(&common_cb->common_tx_pool);

    // If allocation of packet fails
    if (pkt == NULL) {
      // Change common state to allow state
      rsi_check_and_update_cmd_state(COMMON_CMD, ALLOW);
      // Return packet allocation failure error
      SL_PRINTF(SL_SEND_FEATURE_FRAME_PKT_ALLOCATION_FAILURE, COMMON, LOG_ERROR);
      return RSI_ERROR_PKT_ALLOCATION_FAILURE;
    }

    // Take the user provided data and fill it in antenna select structure
    rsi_feature_frame = (rsi_feature_frame_t *)pkt->data;

    // PLL mode value
    rsi_feature_frame->pll_mode = PLL_MODE;

    // RF type
    rsi_feature_frame->rf_type = RF_TYPE;

    // Wireless mode
    rsi_feature_frame->wireless_mode = WIRELESS_MODE;

    // Enable PPP
    rsi_feature_frame->enable_ppp = ENABLE_PPP;

    // AFE type
    rsi_feature_frame->afe_type = AFE_TYPE;

    // Feature enables
    rsi_feature_frame->feature_enables =
      (RSI_WLAN_TRANSMIT_TEST_MODE == rsi_driver_cb->wlan_cb->opermode) ? 0 : FEATURE_ENABLES;

#ifndef RSI_COMMON_SEM_BITMAP
    rsi_driver_cb_non_rom->common_wait_bitmap |= BIT(0);
#endif
    // Send antenna select command
    status = rsi_driver_common_send_cmd(RSI_COMMON_REQ_FEATURE_FRAME, pkt);

    // Wait on common semaphore
    rsi_wait_on_common_semaphore(&rsi_driver_cb_non_rom->common_cmd_sem, RSI_FEATURE_FRAME_RESPONSE_WAIT_TIME);

    // Change common state to allow state
    rsi_check_and_update_cmd_state(COMMON_CMD, ALLOW);

  }

  else {
    // Return common command error
    SL_PRINTF(SL_SEND_FEATURE_FRAME_COMMAND_ERROR, COMMON, LOG_ERROR, "status: %4x", status);
    return status;
  }

  // Get common command response status
  status = rsi_common_get_status();

  // Return status
  SL_PRINTF(SL_SEND_FEATURE_FRAME_EXIT, COMMON, LOG_INFO, "status: %4x", status);
  return status;
}
/*==============================================*/
/**
 * @brief       Get firmware version present in the device. This is a blocking API.
 * @param[in]   length         - Length of the response buffer in bytes to hold result.
 * @param[out]  response       - Response of the requested command.
 * @return      0              - Success \n
 *				Non-Zero Value - Failure \n
 *                               If return value is less than 0 \n
 *                               -3: Command given in wrong state \n
 *                               -4: Buffer not available to serve the command \n
 *                               -6: Insufficient input buffer given \n
 *                               If return value is greater than 0 \n
 *                               0x0021, 0x0025, 0x002c
 * @note       Refer to Error Codes section for above error codes \ref error-codes .
 */

int32_t rsi_get_fw_version(uint8_t *response, uint16_t length)
{
  int32_t status = RSI_SUCCESS;
  SL_PRINTF(SL_GET_FW_VERSION_ENTRY, COMMON, LOG_INFO);
  rsi_pkt_t *pkt;

  // Get common cb structure pointer
  rsi_common_cb_t *common_cb = rsi_driver_cb->common_cb;

  // If state is not in card ready received state
  if (common_cb->state == RSI_COMMON_STATE_NONE) {
    while (common_cb->state != RSI_COMMON_CARDREADY) {
#ifndef RSI_WITH_OS
      rsi_scheduler(&rsi_driver_cb->scheduler_cb);
#endif
    }
  }

  status = rsi_check_and_update_cmd_state(COMMON_CMD, IN_USE);
  if (status == RSI_SUCCESS) {

    // Allocate command buffer  from common pool
    pkt = rsi_pkt_alloc(&common_cb->common_tx_pool);

    // If allocation of packet fails
    if (pkt == NULL) {
      // Change common state to allow state
      rsi_check_and_update_cmd_state(COMMON_CMD, ALLOW);

      // Return packet allocation failure error
      SL_PRINTF(SL_GET_FW_VERSION_PKT_ALLOCATION_FAILURE, COMMON, LOG_ERROR);
      return RSI_ERROR_PKT_ALLOCATION_FAILURE;
    }

    if (response != NULL) {
      // Attach the buffer given by user
      common_cb->app_buffer = response;

      // Length of the buffer provided by user
      common_cb->app_buffer_length = length;
    } else {
      // Assign NULL to the app_buffer to avoid junk
      common_cb->app_buffer = NULL;

      // Length of the buffer to 0
      common_cb->app_buffer_length = 0;
    }

#ifndef RSI_COMMON_SEM_BITMAP
    rsi_driver_cb_non_rom->common_wait_bitmap |= BIT(0);
#endif
    // Send firmware version query request
    status = rsi_driver_common_send_cmd(RSI_COMMON_REQ_FW_VERSION, pkt);

    // Wait on common semaphore
    rsi_wait_on_common_semaphore(&rsi_driver_cb_non_rom->common_cmd_sem, RSI_FWUP_RESPONSE_WAIT_TIME);

    // Change common state to allow state
    rsi_check_and_update_cmd_state(COMMON_CMD, ALLOW);

  }

  else {
    // Return common command error
    SL_PRINTF(SL_GET_FW_VERSION_COMMAND_ERROR, COMMON, LOG_ERROR, "status: %4x", status);
    return status;
  }

  // Get common command response status
  status = rsi_common_get_status();

  // Return status
  SL_PRINTF(SL_GET_FW_VERSION_EXIT, COMMON, LOG_INFO, "status: %4x", status);
  return status;
}

/*==============================================*/
/**
 * @brief       Debug prints on UART interfaces 1 and 2. Host can get 5 types of debug prints based on
 *              the assertion level and assertion type. This is a blocking API.
 * @param[in]   assertion_type          - Assertion_type (Possible values 0 - 15) \n
 * 				                          0000 - LMAC core, 0001 - SME, 0010 - UMAC, 0100 - NETX, 1000 - Enables assertion indication and \n
 *				                          provides ram dump in critical assertion. \n
 * @param[in]   assertion_level         - Assertion_level (Possible values 0 - 15). value 1 is least value/only specific prints, 15 is the highest \n
 *				                          level/Enable all prints.  \n
 *				                          0000 - Assertion required, 0010 - Recoverable, 0100 - Information \n
 * @return      0                       - Success \n
 *				Non-Zero value          - Failure \n
 *				-2                      - Parameters invalid
 * @note        1. To minimize the debug prints host is supposed to give the same command with assertion type and assertion level as 0.
 *              2. Baud rate for UART 2 on host application side should be 460800.
 *              3. Enable few Feature bit map for getting debug logs on UART:
 *                 // To set custom feature select bit map
 *                 #define RSI_CUSTOM_FEATURE_BIT_MAP   FEAT_CUSTOM_FEAT_EXTENTION_VALID
 *                 // To set Extended custom feature select bit map   
 *                 #define RSI_EXT_CUSTOM_FEATURE_BIT_MAP    EXT_FEAT_UART_SEL_FOR_DEBUG_PRINTS
 */

int32_t rsi_common_debug_log(int32_t assertion_type, int32_t assertion_level)
{
  int32_t status = RSI_SUCCESS;
  SL_PRINTF(SL_COMMON_DEBUG_LOG_ENTRY, COMMON, LOG_INFO);
  rsi_pkt_t *pkt;
  rsi_debug_uart_print_t *debug_uart_print;
  // Get common cb structure pointer
  rsi_common_cb_t *common_cb = rsi_driver_cb->common_cb;
  if ((common_cb->state < RSI_COMMON_OPERMODE_DONE)) {
    // Command given in wrong state
    SL_PRINTF(SL_COMMON_DEBUG_LOG_COMMAND_GIVEN_IN_WRONG_STATE, COMMON, LOG_ERROR);
    return RSI_ERROR_COMMAND_GIVEN_IN_WRONG_STATE;
  }
  if (((assertion_type > 15) || (assertion_type < 0)) || ((assertion_level < 0) || (assertion_level > 15))) {
    SL_PRINTF(SL_COMMON_DEBUG_LOG_INVALID_PARAM, COMMON, LOG_ERROR);
    return RSI_ERROR_INVALID_PARAM;
  }

  status = rsi_check_and_update_cmd_state(COMMON_CMD, IN_USE);
  if (status == RSI_SUCCESS) {

    // Allocate command buffer  from common pool
    pkt = rsi_pkt_alloc(&common_cb->common_tx_pool);

    // If allocation of packet fails
    if (pkt == NULL) {
      // Change common state to allow state
      rsi_check_and_update_cmd_state(COMMON_CMD, ALLOW);
      // Return packet allocation failure error
      SL_PRINTF(SL_COMMON_DEBUG_LOG_PKT_ALLOCATION_FAILURE, COMMON, LOG_ERROR);
      return RSI_ERROR_PKT_ALLOCATION_FAILURE;
    }

    // Take the user provided data and fill it in debug uart print select structure
    debug_uart_print = (rsi_debug_uart_print_t *)pkt->data;

    // Assertion_type
    debug_uart_print->assertion_type = assertion_type;

    // RF type
    debug_uart_print->assertion_level = assertion_level;

#ifndef RSI_COMMON_SEM_BITMAP
    rsi_driver_cb_non_rom->common_wait_bitmap |= BIT(0);
#endif
    // Send firmware version query request
    status = rsi_driver_common_send_cmd(RSI_COMMON_REQ_DEBUG_LOG, pkt);

    // Wait on common semaphore
    rsi_wait_on_common_semaphore(&rsi_driver_cb_non_rom->common_cmd_sem, RSI_DEBUG_LOG_RESPONSE_WAIT_TIME);

    // Change common state to allow state
    rsi_check_and_update_cmd_state(COMMON_CMD, ALLOW);

  }

  else {
    // Return common command error
    SL_PRINTF(SL_COMMON_DEBUG_LOG_COMMAND_ERROR, COMMON, LOG_ERROR, "status: %4x", status);
    return status;
  }

  // Get common command response status
  status = rsi_common_get_status();

  // Return status
  SL_PRINTF(SL_COMMON_DEBUG_LOG_EXIT, COMMON, LOG_INFO, "status: %4x", status);
  return status;
}

/*==============================================*/
/**
 * @brief        To enable or disable BT. This is a blocking API.
 * @param[in]    type               - Type
 * @param[in]    callback           - Indicate BT enable or disable based on mode value
 * @param[out]   mode               - Indicate BT enable or disable
 * @param[out]   bt_disabled_status - BT disabled status either success or failure to host
 * @return        0              - Success \n
 *                Negative Value - Failure
 *                0xFF - BT_ACTIVITY_PENDING
 *
 * @note         Refer Error Codes section for above error codes \ref error-codes .
 */

int32_t rsi_switch_proto(uint8_t type, void (*callback)(uint16_t mode, uint8_t *bt_disabled_status))
{
  rsi_pkt_t *pkt;
  rsi_switch_proto_t *ptr;
  int32_t status = RSI_SUCCESS;
  SL_PRINTF(SL_SWITCH_PROTO_ENTRY, COMMON, LOG_INFO);

  // Get common cb pointer
  rsi_common_cb_t *common_cb = rsi_driver_cb->common_cb;

  if ((type == 1) && (callback != NULL)) {
    /* In ENABLE case, callback must be NULL */
    SL_PRINTF(SL_SWITCH_PROTO_INVALID_PARAM, COMMON, LOG_ERROR);
    return RSI_ERROR_INVALID_PARAM;
  }
  if ((common_cb->state < RSI_COMMON_OPERMODE_DONE)) {
    // Command given in wrong state
    SL_PRINTF(SL_SWITCH_PROTO_COMMAND_GIVEN_IN_WRONG_STATE, COMMON, LOG_ERROR);
    return RSI_ERROR_COMMAND_GIVEN_IN_WRONG_STATE;
  }

  status = rsi_check_and_update_cmd_state(COMMON_CMD, IN_USE);
  if (status == RSI_SUCCESS) {

    // Allocate command buffer
    pkt = rsi_pkt_alloc(&common_cb->common_tx_pool);

    // If allocation of packet fails
    if (pkt == NULL) {
      // Change common state to allow state
      rsi_check_and_update_cmd_state(COMMON_CMD, ALLOW);
      // Return packet allocation failure error
      SL_PRINTF(SL_SWITCH_PROTO_PKT_ALLOCATION_FAILURE, COMMON, LOG_ERROR);
      return RSI_ERROR_PKT_ALLOCATION_FAILURE;
    }
    // Take the user provided data and fill it in switch proto structure
    ptr = (rsi_switch_proto_t *)pkt->data;

    // Type
    ptr->mode_value = type;

    common_cb->sync_mode = type;
#ifdef RSI_WLAN_ENABLE
    if (callback != NULL) {
      // Set socket asynchronous receive callback
      rsi_wlan_cb_non_rom->switch_proto_callback = callback;

    } else {
      // Set socket asynchronous receive callback
      rsi_wlan_cb_non_rom->switch_proto_callback = NULL;
    }
#endif
#ifndef RSI_COMMON_SEM_BITMAP
    rsi_driver_cb_non_rom->common_wait_bitmap |= BIT(0);
#endif

    // Send switch proto command
    status = rsi_driver_common_send_cmd(RSI_COMMON_REQ_SWITCH_PROTO, pkt);

    // Wait on common semaphore
    rsi_wait_on_common_semaphore(&rsi_driver_cb_non_rom->common_cmd_sem, RSI_SWITCH_PROTO_RESPONSE_WAIT_TIME);

    // Change common state to allow state
    rsi_check_and_update_cmd_state(COMMON_CMD, ALLOW);

  }

  else {
    // Return common command error
    SL_PRINTF(SL_SWITCH_PROTO_COMMAND_ERROR, COMMON, LOG_ERROR, "status: %4x", status);
    return status;
  }

  // Get common command response status
  status = rsi_common_get_status();

  // Return status
  SL_PRINTF(SL_SWITCH_PROTO_EXIT, COMMON, LOG_INFO, "status: %4x", status);
  return status;
}
/** @} */
/** @addtogroup COMMON
* @{
*/
/*==============================================*/
/**
 *
 * @brief       Handle driver events. Called in application main loop for non-OS platforms. /n
 *              With OS, this API is blocking and with baremetal this API is non-blocking.
 * @param[in]   Void
 * @return      Void
 */

void rsi_wireless_driver_task(void)
{
  SL_PRINTF(SL_WIRELESS_DRIVER_TASK_ENTRY, COMMON, LOG_INFO);
#ifdef RSI_WITH_OS
  while (1)
#endif
  {
    rsi_scheduler(&rsi_driver_cb->scheduler_cb);
  }
  SL_PRINTF(SL_WIRELESS_DRIVER_TASK_EXIT, COMMON, LOG_INFO);
}
//======================================================
/**
 *
 * @brief       De-Initialize driver components. Clear all the memory given for driver operations in \ref rsi_driver_init() API.
 * In OS case,  User need to take care of OS variables initialized in \ref rsi_driver_init(). This is a non-blocking API.
 * This API must be called by the thread/task/Master thread that it is not dependent on.
 * OS variables allocated/initialized in \ref rsi_driver_init() API.
 * @pre 		Need to call after the driver initialization
 * @param[in]   Void
 * @return      0              - Success \n
 *              Non-Zero Value - Failure
 */

int32_t rsi_driver_deinit(void)
{
  int32_t status = RSI_SUCCESS;
  SL_PRINTF(SL_DRIVER_DEINIT_ENTRY, COMMON, LOG_INFO);
  uint32_t actual_length = 0;

  if ((rsi_driver_cb_non_rom->device_state < RSI_DRIVER_INIT_DONE)) {
    // Command given in wrong state
    return RSI_ERROR_COMMAND_GIVEN_IN_WRONG_STATE;
  }
  // Check if buffer is enough for driver components
  actual_length += rsi_driver_memory_estimate();
  if (buffer_addr == NULL) {
    return RSI_FAILURE;
  }
#ifndef RSI_M4_INTERFACE
  rsi_hal_intr_mask();
#else
  mask_ta_interrupt(RX_PKT_TRANSFER_DONE_INTERRUPT | TX_PKT_TRANSFER_DONE_INTERRUPT);
#endif
#ifndef RSI_CHECK_PKT_QUEUE
  rsi_check_pkt_queue_and_dequeue();
#endif
#ifdef RSI_WITH_OS
  rsi_vport_enter_critical();
#endif
#ifndef RSI_RELEASE_SEMAPHORE
  rsi_release_waiting_semaphore();
#endif
  rsi_semaphore_destroy(&rsi_driver_cb->scheduler_cb.scheduler_sem);
#ifdef WAKEUP_GPIO_INTERRUPT_METHOD
  rsi_semaphore_destroy(&rsi_driver_cb->common_cb->wakeup_gpio_sem);
#endif
#ifdef RSI_WITH_OS
  rsi_semaphore_destroy(&rsi_driver_cb->common_cb->common_card_ready_sem);
#endif
  status = rsi_semaphore_destroy(&rsi_driver_cb->common_cb->common_sem);
  if (status != RSI_ERROR_NONE) {
    SL_PRINTF(SL_DRIVER_DEINIT_SEMAPHORE_DESTROY_FAILED_1, COMMON, LOG_ERROR, "status: %4x", status);
    return RSI_ERROR_SEMAPHORE_DESTROY_FAILED;
  }
  rsi_mutex_destroy(&rsi_driver_cb->common_cb->common_mutex);
  rsi_mutex_destroy(&rsi_driver_cb_non_rom->tx_mutex);
#ifdef SAPI_LOGGING_ENABLE
  sl_log_deinit();
#endif
#ifdef FW_LOGGING_ENABLE
  sl_fw_log_deinit();
#endif
#if defined(RSI_DEBUG_PRINTS) || defined(FW_LOGGING_ENABLE)
  rsi_mutex_destroy(&rsi_driver_cb_non_rom->debug_prints_mutex);
#endif
#ifdef RSI_ZB_ENABLE
  rsi_mutex_destroy(&rsi_driver_cb->zigb_tx_q.queue_mutex);
#endif
#ifdef RSI_PROP_PROTOCOL_ENABLE
  rsi_mutex_destroy(&rsi_driver_cb->prop_protocol_tx_q.queue_mutex);
#endif
#if (defined RSI_BT_ENABLE || defined RSI_BLE_ENABLE || defined RSI_PROP_PROTOCOL_ENABLE)
  rsi_mutex_destroy(&rsi_driver_cb->bt_single_tx_q.queue_mutex);
#endif
#if (defined RSI_BLE_ENABLE || defined RSI_BT_ENABLE)
  rsi_mutex_destroy(&rsi_driver_cb->bt_common_tx_q.queue_mutex);
#endif
// Added WLAN define
#ifdef RSI_WLAN_ENABLE
  rsi_mutex_destroy(&rsi_driver_cb->wlan_tx_q.queue_mutex);
#endif
  rsi_mutex_destroy(&rsi_driver_cb->common_tx_q.queue_mutex);
// Added WLAN define
#ifdef RSI_WLAN_ENABLE
  status = rsi_semaphore_destroy(&rsi_driver_cb_non_rom->send_data_sem);
  if (status != RSI_ERROR_NONE) {
    SL_PRINTF(SL_DRIVER_DEINIT_SEMAPHORE_DESTROY_FAILED_2, COMMON, LOG_ERROR, "status: %4x", status);
    return RSI_ERROR_SEMAPHORE_DESTROY_FAILED;
  }
#endif
  if (rsi_driver_cb->rx_pool.pkt_sem != (uint32_t)NULL) {
    status = rsi_semaphore_destroy(&rsi_driver_cb->rx_pool.pkt_sem);
    if (status != RSI_ERROR_NONE) {
      SL_PRINTF(SL_DRIVER_DEINIT_SEMAPHORE_DESTROY_FAILED_3, COMMON, LOG_ERROR, "status: %4x", status);
      return RSI_ERROR_SEMAPHORE_DESTROY_FAILED;
    }
  }
  status = rsi_semaphore_destroy(&rsi_driver_cb->common_cb->common_tx_pool.pkt_sem);
  if (status != RSI_ERROR_NONE) {
    SL_PRINTF(SL_DRIVER_DEINIT_SEMAPHORE_DESTROY_FAILED_4, COMMON, LOG_ERROR, "status: %4x", status);
    return RSI_ERROR_SEMAPHORE_DESTROY_FAILED;
  }
  status = rsi_semaphore_destroy(&rsi_driver_cb_non_rom->common_cmd_send_sem);
  if (status != RSI_ERROR_NONE) {
    return RSI_ERROR_SEMAPHORE_DESTROY_FAILED;
  }
  status = rsi_semaphore_destroy(&rsi_driver_cb_non_rom->common_cmd_sem);
  if (status != RSI_ERROR_NONE) {
    SL_PRINTF(SL_DRIVER_DEINIT_SEMAPHORE_DESTROY_FAILED_5, COMMON, LOG_ERROR, "status: %4x", status);
    return RSI_ERROR_SEMAPHORE_DESTROY_FAILED;
  }
#ifdef RSI_WLAN_ENABLE
  // Create WLAN semaphore
  status = rsi_semaphore_destroy(&rsi_driver_cb_non_rom->nwk_cmd_send_sem);
  if (status != RSI_ERROR_NONE) {
    SL_PRINTF(SL_DRIVER_DEINIT_SEMAPHORE_DESTROY_FAILED_6, COMMON, LOG_ERROR, "status: %4x", status);
    return RSI_ERROR_SEMAPHORE_DESTROY_FAILED;
  }
  status = rsi_semaphore_destroy(&rsi_driver_cb_non_rom->nwk_sem);
  if (status != RSI_ERROR_NONE) {
    SL_PRINTF(SL_DRIVER_DEINIT_SEMAPHORE_DESTROY_FAILED_7, COMMON, LOG_ERROR, "status: %4x", status);
    return RSI_ERROR_SEMAPHORE_DESTROY_FAILED;
  }
  status = rsi_semaphore_destroy(&rsi_driver_cb_non_rom->wlan_cmd_send_sem);
  if (status != RSI_ERROR_NONE) {
    SL_PRINTF(SL_DRIVER_DEINIT_SEMAPHORE_DESTROY_FAILED_8, COMMON, LOG_ERROR, "status: %4x", status);
    return RSI_ERROR_SEMAPHORE_DESTROY_FAILED;
  }
  status = rsi_semaphore_destroy(&rsi_driver_cb_non_rom->wlan_cmd_sem);
  if (status != RSI_ERROR_NONE) {
    SL_PRINTF(SL_DRIVER_DEINIT_SEMAPHORE_DESTROY_FAILED_9, COMMON, LOG_ERROR, "status: %4x", status);
    return RSI_ERROR_SEMAPHORE_DESTROY_FAILED;
  }
  status = rsi_semaphore_destroy(&rsi_driver_cb->wlan_cb->wlan_sem);
  if (status != RSI_ERROR_NONE) {
    SL_PRINTF(SL_DRIVER_DEINIT_SEMAPHORE_DESTROY_FAILED_10, COMMON, LOG_ERROR, "status: %4x", status);
    return RSI_ERROR_SEMAPHORE_DESTROY_FAILED;
  }
  status = rsi_semaphore_destroy(&rsi_driver_cb->wlan_cb->wlan_tx_pool.pkt_sem);
  if (status != RSI_ERROR_NONE) {
    SL_PRINTF(SL_DRIVER_DEINIT_SEMAPHORE_DESTROY_FAILED_11, COMMON, LOG_ERROR, "status: %4x", status);
    return RSI_ERROR_SEMAPHORE_DESTROY_FAILED;
  }
  rsi_mutex_destroy(&rsi_driver_cb->wlan_cb->wlan_mutex);
  rsi_socket_pool         = NULL;
  rsi_socket_pool_non_rom = NULL;
#endif
#if (defined RSI_BT_ENABLE || defined RSI_BLE_ENABLE || defined RSI_PROP_PROTOCOL_ENABLE)
  // Create BT semaphore
  status = rsi_semaphore_destroy(&rsi_driver_cb->bt_common_cb->bt_cmd_sem);
  if (status != RSI_ERROR_NONE) {
    SL_PRINTF(SL_DRIVER_DEINIT_SEMAPHORE_DESTROY_FAILED_12, COMMON, LOG_ERROR, "status: %4x", status);
    return RSI_ERROR_SEMAPHORE_DESTROY_FAILED;
  }
  status = rsi_semaphore_destroy(&rsi_driver_cb->bt_common_cb->bt_sem);
  if (status != RSI_ERROR_NONE) {
    SL_PRINTF(SL_DRIVER_DEINIT_SEMAPHORE_DESTROY_FAILED_13, COMMON, LOG_ERROR, "status: %4x", status);
    return RSI_ERROR_SEMAPHORE_DESTROY_FAILED;
  }
  status = rsi_semaphore_destroy(&rsi_driver_cb->bt_common_cb->bt_tx_pool.pkt_sem);
  if (status != RSI_ERROR_NONE) {
    SL_PRINTF(SL_DRIVER_DEINIT_SEMAPHORE_DESTROY_FAILED_14, COMMON, LOG_ERROR, "status: %4x", status);
    return RSI_ERROR_SEMAPHORE_DESTROY_FAILED;
  }
#ifdef SAPIS_BT_STACK_ON_HOST
  status = rsi_semaphore_destroy(&rsi_driver_cb->bt_ble_stack_cb->bt_tx_pool.pkt_sem);
  if (status != RSI_ERROR_NONE) {
    SL_PRINTF(SL_DRIVER_DEINIT_SEMAPHORE_DESTROY_FAILED_15, COMMON, LOG_ERROR, "status: %4x", status);
    return RSI_ERROR_SEMAPHORE_DESTROY_FAILED;
  }
#endif
#ifdef RSI_BT_ENABLE
  status = rsi_semaphore_destroy(&rsi_driver_cb->bt_classic_cb->bt_cmd_sem);
  if (status != RSI_ERROR_NONE) {
    SL_PRINTF(SL_DRIVER_DEINIT_SEMAPHORE_DESTROY_FAILED_16, COMMON, LOG_ERROR, "status: %4x", status);
    return RSI_ERROR_SEMAPHORE_DESTROY_FAILED;
  }
  status = rsi_semaphore_destroy(&rsi_driver_cb->bt_classic_cb->bt_sem);
  if (status != RSI_ERROR_NONE) {
    SL_PRINTF(SL_DRIVER_DEINIT_SEMAPHORE_DESTROY_FAILED_17, COMMON, LOG_ERROR, "status: %4x", status);
    return RSI_ERROR_SEMAPHORE_DESTROY_FAILED;
  }
  status = rsi_semaphore_destroy(&rsi_driver_cb->bt_classic_cb->bt_tx_pool.pkt_sem);
  if (status != RSI_ERROR_NONE) {
    SL_PRINTF(SL_DRIVER_DEINIT_SEMAPHORE_DESTROY_FAILED_18, COMMON, LOG_ERROR, "status: %4x", status);
    return RSI_ERROR_SEMAPHORE_DESTROY_FAILED;
  }
#endif
#ifdef RSI_BLE_ENABLE
  status = rsi_semaphore_destroy(&rsi_driver_cb->ble_cb->bt_cmd_sem);
  if (status != RSI_ERROR_NONE) {
    SL_PRINTF(SL_DRIVER_DEINIT_SEMAPHORE_DESTROY_FAILED_19, COMMON, LOG_ERROR, "status: %4x", status);
    return RSI_ERROR_SEMAPHORE_DESTROY_FAILED;
  }
  status = rsi_semaphore_destroy(&rsi_driver_cb->ble_cb->bt_sem);
  if (status != RSI_ERROR_NONE) {
    SL_PRINTF(SL_DRIVER_DEINIT_SEMAPHORE_DESTROY_FAILED_20, COMMON, LOG_ERROR, "status: %4x", status);
    return RSI_ERROR_SEMAPHORE_DESTROY_FAILED;
  }
  status = rsi_semaphore_destroy(&rsi_driver_cb->ble_cb->bt_tx_pool.pkt_sem);
  if (status != RSI_ERROR_NONE) {
    SL_PRINTF(SL_DRIVER_DEINIT_SEMAPHORE_DESTROY_FAILED_21, COMMON, LOG_ERROR, "status: %4x", status);
    return RSI_ERROR_SEMAPHORE_DESTROY_FAILED;
  }
#endif
#ifdef RSI_PROP_PROTOCOL_ENABLE
  status = rsi_semaphore_destroy(&rsi_driver_cb->prop_protocol_cb->bt_cmd_sem);
  if (status != RSI_ERROR_NONE) {
    SL_PRINTF(SL_DRIVER_DEINIT_SEMAPHORE_DESTROY_FAILED_22, COMMON, LOG_ERROR, "status: %4x", status);
    return RSI_ERROR_SEMAPHORE_DESTROY_FAILED;
  }
  status = rsi_semaphore_destroy(&rsi_driver_cb->prop_protocol_cb->bt_tx_pool.pkt_sem);
  if (status != RSI_ERROR_NONE) {
    SL_PRINTF(SL_DRIVER_DEINIT_SEMAPHORE_DESTROY_FAILED_23, COMMON, LOG_ERROR, "status: %4x", status);
    return RSI_ERROR_SEMAPHORE_DESTROY_FAILED;
  }
#endif
#endif
#ifdef RSI_ZB_ENABLE
  status = rsi_semaphore_destroy(&rsi_driver_cb->zigb_cb->zigb_tx_pool.pkt_sem);
  if (status != RSI_ERROR_NONE) {
    SL_PRINTF(SL_DRIVER_DEINIT_SEMAPHORE_DESTROY_FAILED_24, COMMON, LOG_ERROR, "status: %4x", status);
    return RSI_ERROR_SEMAPHORE_DESTROY_FAILED;
  }
  status = rsi_semaphore_destroy(&rsi_driver_cb->zigb_cb->zigb_sem);
  if (status != RSI_ERROR_NONE) {
    SL_PRINTF(SL_DRIVER_DEINIT_SEMAPHORE_DESTROY_FAILED_25, COMMON, LOG_ERROR, "status: %4x", status);
    return RSI_ERROR_SEMAPHORE_DESTROY_FAILED;
  }
  rsi_mutex_destroy(&rsi_driver_cb->zigb_cb->zigb_mutex);
#endif
#ifdef RSI_WITH_OS
  rsi_vport_exit_critical();
#endif
  // state update
  rsi_driver_cb->common_cb->state = RSI_COMMON_STATE_NONE;
#ifdef RSI_WLAN_ENABLE
  rsi_driver_cb->wlan_cb->state = RSI_WLAN_STATE_NONE;
#endif
#ifndef LINUX_PLATFORM
#ifdef RSI_SPI_INTERFACE
  rsi_timer_stop(RSI_TIMER_NODE_0);
#endif
#endif
  rsi_driver_cb_non_rom->device_state = RSI_DEVICE_STATE_NONE;
  SL_PRINTF(SL_DRIVER_DEINIT_SEMAPHORE_DESTROY_FAILED_26, COMMON, LOG_INFO);
  return RSI_SUCCESS;
}

/*======================================================*/
/**
 *
 * @brief       In OS case, delete driver task before calling rsi_driver_deinit() API.
 * @pre         Need to call after the driver initialization
 * @param[in]   task_handle - Task handle/instance to be deleted
 * @return      0              - Success \n
 *              Non-Zero Value - Failure
 */

int32_t rsi_destroy_driver_task_and_driver_deinit(rsi_task_handle_t *task_handle)
{
#ifndef RSI_WITH_OS
  UNUSED_PARAMETER(task_handle); //This statement is added only to resolve compilation warning, value is unchanged
#endif
  SL_PRINTF(SL_DRIVER_DEINIT_TASK_DESTROY_ENTRY, COMMON, LOG_INFO);
  int32_t status = RSI_SUCCESS;

#ifdef RSI_WITH_OS
  rsi_task_destroy(task_handle);
#endif

  status = rsi_driver_deinit();
  SL_PRINTF(SL_DRIVER_DEINIT_TASK_DESTROY_EXIT, COMMON, LOG_INFO, "status: %4x", status);
  return status;
}

/*=============================================================================*/
/**
 * @brief       Get current driver version. This is a non-blocking API.
 * @param[in]   Request - pointer to fill driver version
 * @return      0              - Success \n
 * 				Non-Zero Value - Failure
 */

#define RSI_DRIVER_VERSION "2.6.0.38"
int32_t rsi_driver_version(uint8_t *request)
{
  SL_PRINTF(SL_DRIVER_VERSION_ENTRY, COMMON, LOG_INFO);
  memcpy(request, (uint8_t *)RSI_DRIVER_VERSION, sizeof(RSI_DRIVER_VERSION));
  SL_PRINTF(SL_DRIVER_VERSION_EXIT, COMMON, LOG_INFO);
  return RSI_SUCCESS;
}
/** @} */
/** @addtogroup COMMON 
* @{
*/
/*==============================================*/
/**
 * @brief       Set the host rtc timer. This is a blocking API.
 * @pre 		\ref rsi_wireless_init() API needs to be called before this API
 * @param[in]   timer          - Pointer to fill RTC time.
 *	  		                        second -->  seconds [0-59] \n
 *	 		                        minute -->  minutes [0-59] \n
 *	  		                        hour   -->  hours since midnight [0-23] \n
 *	  		                        day    -->  day of the month [1-31] \n
 *	  		                        month  -->  months since January [0-11] \n
 *	  		                        year   -->  year since 1990. \n
 *	 		                        Weekday-->  Weekday from Sunday to Saturday [1-7]. \n
 * @return      0              - Success \n
 * 				Non-Zero Value - Failure \n
 * @note        Hour is 24-hour format only (valid values are 0 to 23). Valid values for Month are 0 to 11 (January to December).
 */

int32_t rsi_set_rtc_timer(module_rtc_time_t *timer)
{
  int32_t status = RSI_SUCCESS;
  SL_PRINTF(SL_SET_RTC_TIMER_ENTRY, COMMON, LOG_INFO);
  rsi_pkt_t *pkt;
  module_rtc_time_t *rtc;
  rsi_common_cb_t *common_cb = rsi_driver_cb->common_cb;

  if ((common_cb->state < RSI_COMMON_OPERMODE_DONE)) {
    // Command given in wrong state
    SL_PRINTF(SL_SET_RTC_TIMER_COMMAND_GIVEN_IN_WRONG_STATE, COMMON, LOG_ERROR, "status: %4x", common_cb->state);
    return RSI_ERROR_COMMAND_GIVEN_IN_WRONG_STATE;
  }

  if ((timer->tm_sec[0] > 59) || (timer->tm_min[0] > 59) || (timer->tm_hour[0] > 23)
      || ((timer->tm_mday[0] < 1) || (timer->tm_mday[0] > 31)) || (timer->tm_mon[0] > 11)
      || ((timer->tm_wday[0] < 1) || (timer->tm_wday[0] > 7))) {

    // Checking Invalid Parameters
    return RSI_ERROR_INVALID_PARAM;
  }

  status = rsi_check_and_update_cmd_state(COMMON_CMD, IN_USE);
  if (status == RSI_SUCCESS) {

    // Allocate command buffer from common pool
    pkt = rsi_pkt_alloc(&common_cb->common_tx_pool);

    // If allocation of packet fails
    if (pkt == NULL) {
      // Change common state to allow state
      rsi_check_and_update_cmd_state(COMMON_CMD, ALLOW);
      // Return packet allocation failure error
      SL_PRINTF(SL_SET_RTC_TIMER_PKT_ALLOCATION_FAILURE, COMMON, LOG_ERROR);
      return RSI_ERROR_PKT_ALLOCATION_FAILURE;
    }

    // Take the user provided data and fill it in module RTC time structure
    rtc = (module_rtc_time_t *)pkt->data;

    memcpy(&rtc->tm_sec, &timer->tm_sec, 4);
    memcpy(&rtc->tm_min, &timer->tm_min, 4);
    memcpy(&rtc->tm_hour, &timer->tm_hour, 4);
    memcpy(&rtc->tm_mday, &timer->tm_mday, 4);
    memcpy(&rtc->tm_mon, &timer->tm_mon, 4);
    memcpy(&rtc->tm_year, &timer->tm_year, 4);
    memcpy(&rtc->tm_wday, &timer->tm_wday, 4);

#ifndef RSI_COMMON_SEM_BITMAP
    rsi_driver_cb_non_rom->common_wait_bitmap |= BIT(0);
#endif
    // Send set RTC timer request
    status = rsi_driver_common_send_cmd(RSI_COMMON_REQ_SET_RTC_TIMER, pkt);

    // Wait on common semaphore
    rsi_wait_on_common_semaphore(&rsi_driver_cb_non_rom->common_cmd_sem, RSI_SET_RTC_TIMER_RESPONSE_WAIT_TIME);

    // Change common state to allow state
    rsi_check_and_update_cmd_state(COMMON_CMD, ALLOW);

  }

  else {
    // Return common command error
    SL_PRINTF(SL_SET_RTC_TIMER_COMMAND_ERROR, COMMON, LOG_ERROR, "status: %4x", status);
    return status;
  }

  // Get common command response status
  status = rsi_common_get_status();

  // Return status
  SL_PRINTF(SL_SET_RTC_TIMER_EXIT, COMMON, LOG_INFO, "status: %4x", status);
  return status;
}

//*==============================================*/
/**
 * @brief       Get ram log on UART/UART2. This is a blocking API.
 * @pre 		\ref rsi_wireless_init() API needs to be called before this API.
 * @param[in]   addr           - Address in RS9116 module
 * @param[in]   length         - Chunk length to read from RS9116 module
 * @return 		0              - Success \n
 *				Non-Zero Value - Failure \n
 *				                 If return value is less than 0  \n
 *				                 -2: Invalid parameters  \n
 *				                 -3: Command given in wrong state \n
 *				                 If return value is greater than 0 \n
 *								 0x0021,0x003E
 * @note       Refer to Error Codes section for above error codes \ref error-codes.
 */

int32_t rsi_get_ram_log(uint32_t addr, uint32_t length)
{
  int32_t status = RSI_SUCCESS;
  SL_PRINTF(SL_GET_RAM_LOG_ENTRY, COMMON, LOG_INFO);
  rsi_pkt_t *pkt;
  rsi_ram_dump_t *ram;
  rsi_common_cb_t *common_cb = rsi_driver_cb->common_cb;

  if ((common_cb->state < RSI_COMMON_OPERMODE_DONE)) {
    // Command given in wrong state
    SL_PRINTF(SL_GET_RAM_LOG_EXIT1, COMMON, LOG_ERROR, "status: %4x", common_cb->state);
    return RSI_ERROR_COMMAND_GIVEN_IN_WRONG_STATE;
  }
  status = rsi_check_and_update_cmd_state(COMMON_CMD, IN_USE);
  if (status == RSI_SUCCESS) {

    // Allocate command buffer from common pool
    pkt = rsi_pkt_alloc(&common_cb->common_tx_pool);

    // If allocation of packet fails
    if (pkt == NULL) {
      // Change common state to allow state
      rsi_check_and_update_cmd_state(COMMON_CMD, ALLOW);
      // Return packet allocation failure error
      SL_PRINTF(SL_GET_RAM_LOG_PKT_ALLOCATION_FAILURE, COMMON, LOG_ERROR);
      return RSI_ERROR_PKT_ALLOCATION_FAILURE;
    }

    // Take the user provided data and fill it in ram_dump structure
    ram = (rsi_ram_dump_t *)pkt->data;
    // Address
    ram->addr = addr;
    // Length
    ram->length = length;
#ifndef RSI_COMMON_SEM_BITMAP
    rsi_driver_cb_non_rom->common_wait_bitmap |= BIT(0);
#endif
    // Send RAM dump request
    status = rsi_driver_common_send_cmd(RSI_COMMON_REQ_GET_RAM_DUMP, pkt);

    // Wait on common semaphore
    rsi_wait_on_common_semaphore(&rsi_driver_cb_non_rom->common_cmd_sem, RSI_GET_RAM_DUMP_RESPONSE_WAIT_TIME);

    // Change common state to allow state
    rsi_check_and_update_cmd_state(COMMON_CMD, ALLOW);

  }

  else {
    // Return common command error
    SL_PRINTF(SL_GET_RAM_LOG_COMMAND_ERROR, COMMON, LOG_ERROR, "status: %4x", status);
    return status;
  }

  // Get common command response status
  status = rsi_common_get_status();

  // Return status
  SL_PRINTF(SL_GET_RAM_LOG_EXIT, COMMON, LOG_INFO, "status: %4x", status);
  return status;
}

/*==============================================*/
/**
 *
 * @brief      Provide the memory required by the application. This is a non-blocking API.
 * @param[in]  Void
 * @return     Driver pool size
 *        	   
 */

int32_t rsi_driver_memory_estimate(void)
{
  uint32_t actual_length = 0;

  // Calculate the Memory length of the application
  actual_length += RSI_DRIVER_POOL_SIZE;
  return actual_length;
}

/*==============================================*/
/**
 *
 * @brief       De-register event handler for the given event. This is a non-blocking API.
 * @param[out]  event_num     - Event number
 * @return      0             - Success \n
 *              Non-Zero Value- Failure \n
 */
void rsi_uregister_events_callbacks(void (*callback_handler_ptr)(uint32_t event_num))
{
  SL_PRINTF(SL_UNREGISTER_EVENTS_CALLBACK_ENTRY, COMMON, LOG_INFO);
  global_cb_p->rsi_driver_cb->unregistered_event_callback = callback_handler_ptr;
  SL_PRINTF(SL_UNREGISTER_EVENTS_CALLBACK_EXIT, COMMON, LOG_INFO);
}
#ifndef RSI_WAIT_TIMEOUT_EVENT_HANDLE_TIMER_DISABLE
/*==============================================*/
/**
 *
 * @brief       Register SAPI wait timeout handler. This is a non-blocking API.
 * @param[out]  status   - Status
 * @param[out]  cmd_type - Command
 * @return      0        - Success \n
 *              Non-Zero - Failure \n
 */
void rsi_register_wait_timeout_error_callbacks(void (*callback_handler_ptr)(int32_t status, uint32_t cmd_type))
{
  SL_PRINTF(SL_REGISTER_WAIT_TIMEOUT_ERROR_CALLBACKS_ENTRY, COMMON, LOG_INFO);
  rsi_driver_cb_non_rom->rsi_wait_timeout_handler_error_cb = callback_handler_ptr;
  SL_PRINTF(SL_REGISTER_WAIT_TIMEOUT_ERROR_CALLBACKS_EXIT, COMMON, LOG_INFO);
}
#endif
/** @} */
/** @addtogroup COMMON
* @{
*/
//*==============================================*/
/**
 * @brief       Fetch current time from hardware Real Time Clock. This is a blocking API.
 * @pre         \ref rsi_set_rtc_timer() API needs to be called before this API.
 *              Also this API needs to be called only after \ref rsi_wlan_scan() API or \ref rsi_wlan_radio_init() API.
 * @param[out]  response       - Response of the requested command. \n
 * @note	Response parameters: \n
 *	  		second -->  Current real time clock seconds. \n
 *	 		minute -->  Current real time clock minute. \n
 *	  		hour   -->  Current real time clock hour. \n
 *	  		day    -->  Current real time clock day. \n
 *	  		month  -->  Current real time clock month. \n
 *	  		year   -->  Current real time clock year. \n
 *	 		Weekday-->  Current real time clock Weekday. \n
 * @return     0              - Success \n
 *             Non-Zero Value - Failure \n
 *                              0x0021, 0x0025
 * @note       Hour is 24-hour format only (valid values are 0 to 23). Valid values for Month are 0 to 11 (January to December).
 */

int32_t rsi_get_rtc_timer(module_rtc_time_t *response)
{
  int32_t status = RSI_SUCCESS;
  SL_PRINTF(SL_GET_RTC_TIMER_ENTRY, COMMON, LOG_INFO);
  rsi_pkt_t *pkt;
  rsi_common_cb_t *common_cb = rsi_driver_cb->common_cb;
  if ((common_cb->state < RSI_COMMON_OPERMODE_DONE)) {
    // Command given in wrong state
    SL_PRINTF(SL_GET_RTC_TIMER_COMMAND_GIVEN_IN_WRONG_STATE, COMMON, LOG_ERROR, "status: %4x", common_cb->state);
    return RSI_ERROR_COMMAND_GIVEN_IN_WRONG_STATE;
  }
  status = rsi_check_and_update_cmd_state(COMMON_CMD, IN_USE);
  if (status == RSI_SUCCESS) {
    // Allocate command buffer from common pool
    pkt = rsi_pkt_alloc(&common_cb->common_tx_pool);

    // If allocation of packet fails
    if (pkt == NULL) {
      // Unlock mutex
      rsi_mutex_unlock(&common_cb->common_mutex);
      // Return packet allocation failure error
      SL_PRINTF(SL_GET_RTC_TIMER_PKT_ALLOCATION_FAILURE, COMMON, LOG_ERROR);
      return RSI_ERROR_PKT_ALLOCATION_FAILURE;
    }

    if (response != NULL) {
      // Attach the buffer given by user
      common_cb->app_buffer = (uint8_t *)response;

      // Length of the buffer provided by user
      common_cb->app_buffer_length = sizeof(module_rtc_time_t);
    } else {
      // Assign NULL to the app_buffer to avoid junk
      common_cb->app_buffer = NULL;

      // Length of the buffer to 0
      common_cb->app_buffer_length = 0;
    }
#ifndef RSI_COMMON_SEM_BITMAP
    rsi_driver_cb_non_rom->common_wait_bitmap |= BIT(0);
#endif

    // Send get RTC timer request
    status = rsi_driver_common_send_cmd(RSI_COMMON_REQ_GET_RTC_TIMER, pkt);
    // Wait on common semaphore
    rsi_wait_on_common_semaphore(&rsi_driver_cb_non_rom->common_cmd_sem, RSI_GET_RTC_TIMER_RESPONSE_WAIT_TIME);

    // Change common state to allow state
    rsi_check_and_update_cmd_state(COMMON_CMD, ALLOW);

  }

  else {
    // Return common command error
    SL_PRINTF(SL_GET_RTC_TIMER_COMMAND_ERROR, COMMON, LOG_ERROR, "status: %4x", status);
    return status;
  }

  // Get common command response status
  status = rsi_common_get_status();

  // Return status
  SL_PRINTF(SL_GET_RTC_TIMER_EXIT, COMMON, LOG_INFO, "status: %4x", status);
  return status;
}

/*==============================================*/
/**
 * @fn                int32_t rsi_set_config( uint32_t code, uint8_t value)
 * @brief             Configure XO Ctune value from host
 * @param[in]         Code - XO_CTUNE_FROM_HOST
 * @param[in]         value - Value to be configured
 * @return            0 		- Success \n
 *                    Non-Zero Value  - Failure
 */

int32_t rsi_set_config(uint32_t code, uint8_t value)
{
  rsi_pkt_t *pkt;
  rsi_set_config_t *set_config = NULL;
  int32_t status               = RSI_SUCCESS;

  // Get common cb structure pointer
  rsi_common_cb_t *common_cb = rsi_driver_cb->common_cb;

  if (common_cb->state < RSI_COMMON_OPERMODE_DONE) {
    // Command given in wrong state
    return RSI_ERROR_COMMAND_GIVEN_IN_WRONG_STATE;
  }

  if (code != XO_CTUNE_FROM_HOST) {
    // if flag set is other than xo_ctune from host
    return RSI_ERROR_INVALID_SET_CONFIG_FLAG;
  }
  status = rsi_check_and_update_cmd_state(COMMON_CMD, IN_USE);
  if (status == RSI_SUCCESS) {
    // allocate command buffer  from common pool
    pkt = rsi_pkt_alloc(&common_cb->common_tx_pool);

    // If allocation of packet fails
    if (pkt == NULL) {
      // Changing the common cmd state to allow
      rsi_check_and_update_cmd_state(COMMON_CMD, ALLOW);
      // return packet allocation failure error
      return RSI_ERROR_PKT_ALLOCATION_FAILURE;
    }

    set_config = (rsi_set_config_t *)pkt->data;

    //code
    set_config->code = code;

    //xo ctune value from host
    set_config->values.xo_ctune = value;

#ifndef RSI_COMMON_SEM_BITMAP
    rsi_driver_cb_non_rom->common_wait_bitmap |= BIT(0);
#endif
    // send  set config request command
    status = rsi_driver_common_send_cmd(RSI_COMMON_REQ_SET_CONFIG, pkt);

    // wait on common semaphore
    rsi_wait_on_common_semaphore(&rsi_driver_cb_non_rom->common_cmd_sem, RSI_SET_CONFIG_RESPONSE_WAIT_TIME);

    // Changing the common cmd state to allow
    rsi_check_and_update_cmd_state(COMMON_CMD, ALLOW);

  } else {
    // return common command error
    return status;
  }
  // get common command response status
  status = rsi_common_get_status();

  // Return the status
  return status;
}
#ifdef RSI_ASSERT_API
/*==============================================*/
/**
 * @brief       Trigger an assert. This is a blocking API.
 * @param[in]   Void
 * @return      0              - Success \n
 *              Non-Zero Value - Failure
 */

int32_t rsi_assert(void)
{
  int32_t status = RSI_SUCCESS;
  SL_PRINTF(SL_ASSERT_ENTRY, COMMON, LOG_INFO);
  rsi_pkt_t *pkt;
  rsi_common_cb_t *common_cb = rsi_driver_cb->common_cb;
  if ((common_cb->state < RSI_COMMON_OPERMODE_DONE)) {
    // Command given in wrong state
    SL_PRINTF(SL_ASSERT_COMMAND_GIVEN_IN_WRONG_STATE, COMMON, LOG_ERROR, "status: %4x", common_cb->state);
    return RSI_ERROR_COMMAND_GIVEN_IN_WRONG_STATE;
  }
  status = rsi_check_and_update_cmd_state(COMMON_CMD, IN_USE);
  if (status == RSI_SUCCESS) {
    // Command given in wrong state
    pkt = rsi_pkt_alloc(&common_cb->common_tx_pool);

    // If allocation of packet fails
    if (pkt == NULL) {
      // Change common state to allow state
      rsi_check_and_update_cmd_state(COMMON_CMD, ALLOW);
      // Return packet allocation failure error
      SL_PRINTF(SL_ASSERT_PKT_ALLOCATION_FAILURE, COMMON, LOG_ERROR);
      return RSI_ERROR_PKT_ALLOCATION_FAILURE;
    }
#ifndef RSI_COMMON_SEM_BITMAP
    rsi_driver_cb_non_rom->common_wait_bitmap |= BIT(0);
#endif

    // Send assert request
    status = rsi_driver_common_send_cmd(RSI_COMMON_REQ_ASSERT, pkt);

    // Wait on common semaphore
    rsi_wait_on_common_semaphore(&rsi_driver_cb_non_rom->common_cmd_sem, RSI_ASSERT_RESPONSE_WAIT_TIME);

    // Change common state to allow state
    rsi_check_and_update_cmd_state(COMMON_CMD, ALLOW);

  }

  else {
    // Return common command error
    SL_PRINTF(SL_ASSERT_COMMAND_ERROR, COMMON, LOG_ERROR, "status: %4x", status);
    return status;
  }

  // Get common command response status
  status = rsi_common_get_status();

  // Return status
  SL_PRINTF(SL_ASSERT_EXIT, COMMON, LOG_INFO, "status: %4x", status);
  return status;
}
#endif
#ifdef CONFIGURE_GPIO_FROM_HOST

//*==============================================*/
/**
 * @brief      Configure TA GPIOs using Command from host.This is a non-blocking API.
 * @param[in]  gpio_type     - GPIO types :  \n 
 *                             0 - TA_GPIO   \n
 *                             1 - ULP_GPIO  \n
 *                             2 - UULP_GPIO \n
 * @param[in]  pin_num       - GPIO Number : \n
 *                             Valid values  0 - 63, if gpio_type is TA_GPIO. \n
 *                             Valid values  0 - 15, if gpio_type is ULP_GPIO. \n
 *                             Valid values  0 and 2, if gpio_type is UULP_GPIO. \n
 * @param[in]  configuration - BIT[4]     -  1 - Input mode  \n
 *                                           0 - Output mode \n
 *                             BIT[0 - 1] : Drive strength \n
 *                                          0 - 2mA  \n
 *                                          1 - 4mA  \n
 *                                          2 - 8mA  \n
 *                                          3 - 12mA \n
 *                             BIT[6 - 7] : \n
 *                             0 - Hi-Z     \n
 *                             1 - Pull-up  \n 
 *                             2 - Pull-down \n
 * @return 	   0              - Success \n
 *             Non-Zero Value - Failure
 * @note       It is not recommended to configure ULP_GPIO6 as it is used for WOWLAN feature.
 */

int32_t rsi_gpio_pininit(uint8_t gpio_type, uint8_t pin_num, uint8_t configuration)
{
  int32_t status = RSI_SUCCESS;
  SL_PRINTF(SL_GPIO_PININIT_ENTRY, COMMON, LOG_INFO);
  rsi_pkt_t *pkt;
  rsi_common_cb_t *common_cb = rsi_driver_cb->common_cb;
  rsi_gpio_pin_config_t *config_gpio;
  // Take lock on common control block
  if ((common_cb->state < RSI_COMMON_OPERMODE_DONE)) {
    // Command given in wrong state
    SL_PRINTF(SL_GPIO_PININIT_COMMAND_GIVEN_IN_WRONG_STATE, COMMON, LOG_ERROR, "status: %4x", common_cb->state);
    return RSI_ERROR_COMMAND_GIVEN_IN_WRONG_STATE;
  }
  status = rsi_check_and_update_cmd_state(COMMON_CMD, IN_USE);
  if (status == RSI_SUCCESS) {

    // Allocate command buffer from common pool
    pkt = rsi_pkt_alloc(&common_cb->common_tx_pool);

    // If allocation of packet fails
    if (pkt == NULL) {
      // Change common state to allow state
      rsi_check_and_update_cmd_state(COMMON_CMD, ALLOW);
      // Return packet allocation failure error
      SL_PRINTF(SL_GPIO_PININIT_PKT_ALLOCATION_FAILURE, COMMON, LOG_ERROR);
      return RSI_ERROR_PKT_ALLOCATION_FAILURE;
    }

    config_gpio                = (rsi_gpio_pin_config_t *)pkt->data;
    config_gpio->gpio_type     = gpio_type;
    config_gpio->config_mode   = RSI_CONFIG_GPIO;
    config_gpio->pin_num       = pin_num;
    config_gpio->output_value  = 0;
    config_gpio->config_values = configuration;

#ifndef RSI_COMMON_SEM_BITMAP
    rsi_driver_cb_non_rom->common_wait_bitmap |= BIT(0);
#endif
    // Send GPIO configuration request
    status = rsi_driver_common_send_cmd(RSI_COMMON_REQ_GPIO_CONFIG, pkt);
    // Wait on common semaphore
    rsi_wait_on_common_semaphore(&rsi_driver_cb_non_rom->common_cmd_sem, RSI_GPIO_CONFIG_RESP_WAIT_TIME);
    // Change common state to allow state
    rsi_check_and_update_cmd_state(COMMON_CMD, ALLOW);
  } else {
    // Return common command error
    SL_PRINTF(SL_GPIO_PININIT_COMMAND_ERROR, COMMON, LOG_ERROR, "status: %4x", status);
    return status;
  }
  // Get common command response status
  status = rsi_common_get_status();
  // Return status
  SL_PRINTF(SL_GPIO_PININIT_EXIT, COMMON, LOG_INFO, "status: %4x", status);
  return status;
}

//*==============================================*/
/**
 * @brief       Drive the Module GPIOs high or low using command from host. This is a non-blocking API.
 * @param[in]  gpio_type     - GPIO types :  \n 
 *                             0 - TA_GPIO   \n
 *                             1 - ULP_GPIO  \n
 *                             2 - UULP_GPIO \n
 * @param[in]  pin_num       - GPIO Number : \n
 *                             Valid values  0 - 63, if gpio_type is TA_GPIO. \n
 *                             Valid values  0 - 15, if gpio_type is ULP_GPIO. \n
 *                             Valid values  0 and 2, if gpio_type is UULP_GPIO. \n
 *@param[in]    value        - Value to be driven on GPIO \n
 *                                1 - Drive high \n
 *                                0 - Drive Low  \n
 *@return 	    0              -  Success \n
 *              Non-Zero Value -  Failure	
 * @note        It is not recommended to configure ULP_GPIO6 as it is used for WOWLAN feature.
 */

int32_t rsi_gpio_writepin(uint8_t gpio_type, uint8_t pin_num, uint8_t value)
{
  int32_t status = RSI_SUCCESS;
  SL_PRINTF(SL_GPIO_WRITEPIN_ENTRY, COMMON, LOG_INFO);
  rsi_pkt_t *pkt;
  rsi_common_cb_t *common_cb = rsi_driver_cb->common_cb;
  rsi_gpio_pin_config_t *config_gpio;

  if ((common_cb->state < RSI_COMMON_OPERMODE_DONE)) {
    // Command given in wrong state
    SL_PRINTF(SL_GPIO_WRITEPIN_COMMAND_GIVEN_IN_WRONG_STATE, COMMON, LOG_ERROR, "status: %4x", common_cb->state);
    return RSI_ERROR_COMMAND_GIVEN_IN_WRONG_STATE;
  }
  status = rsi_check_and_update_cmd_state(COMMON_CMD, IN_USE);
  if (status == RSI_SUCCESS) {

    // Allocate command buffer from common pool
    pkt = rsi_pkt_alloc(&common_cb->common_tx_pool);

    // If allocation of packet fails
    if (pkt == NULL) {
      // Change common state to allow state
      rsi_check_and_update_cmd_state(COMMON_CMD, ALLOW);
      // Return packet allocation failure error
      SL_PRINTF(SL_GPIO_WRITEPIN_PKT_ALLOCATION_FAILURE, COMMON, LOG_ERROR);
      return RSI_ERROR_PKT_ALLOCATION_FAILURE;
    }
    config_gpio                = (rsi_gpio_pin_config_t *)pkt->data;
    config_gpio->gpio_type     = gpio_type;
    config_gpio->config_mode   = RSI_SET_GPIO;
    config_gpio->pin_num       = pin_num;
    config_gpio->output_value  = value;
    config_gpio->config_values = 0;

#ifndef RSI_COMMON_SEM_BITMAP
    rsi_driver_cb_non_rom->common_wait_bitmap |= BIT(0);
#endif
    // Send GPIO configuration request
    status = rsi_driver_common_send_cmd(RSI_COMMON_REQ_GPIO_CONFIG, pkt);
    // Wait on common semaphore
    rsi_wait_on_common_semaphore(&rsi_driver_cb_non_rom->common_cmd_sem, RSI_GPIO_CONFIG_RESP_WAIT_TIME);
    // Change common state to allow state
    rsi_check_and_update_cmd_state(COMMON_CMD, ALLOW);
  } else {
    // Return common command error
    SL_PRINTF(SL_GPIO_WRITEPIN_COMMAND_ERROR, COMMON, LOG_ERROR, "status: %4x", status);
    return status;
  }
  // Get common command response status
  status = rsi_common_get_status();
  // Return status
  SL_PRINTF(SL_GPIO_WRITEPIN_EXIT, COMMON, LOG_INFO, "status: %4x", status);
  return status;
}

//*==============================================*/
/**
 * @brief       Read status of TA GPIOs using Command from host. This is a non-blocking API.
 * @param[in]  gpio_type     - GPIO types :  \n 
 *                             0 - TA_GPIO   \n
 *                             1 - ULP_GPIO  \n
 *                             2 - UULP_GPIO \n
 * @param[in]  pin_num       - GPIO Number : \n
 *                             Valid values  0 - 63, if gpio_type is TA_GPIO. \n
 *                             Valid values  0 - 15, if gpio_type is ULP_GPIO. \n
 *                             Valid values  0 and 2, if gpio_type is UULP_GPIO. \n
 * @param[in]   gpio_value   - Address of variable to store the value
 * @return 		  0              - Success \n 
 *				      Non-Zero Value - Failure        
 * @note        It is not recommended to configure ULP_GPIO6 as it is used for WOWLAN feature.
 */

int32_t rsi_gpio_readpin(uint8_t gpio_type, uint8_t pin_num, uint8_t *gpio_value)
{
  int32_t status = RSI_SUCCESS;
  SL_PRINTF(SL_GPIO_READPIN_ENTRY, COMMON, LOG_INFO);
  rsi_pkt_t *pkt;
  rsi_common_cb_t *common_cb = rsi_driver_cb->common_cb;
  rsi_gpio_pin_config_t *config_gpio;

  if ((common_cb->state < RSI_COMMON_OPERMODE_DONE)) {
    // Command given in wrong state
    SL_PRINTF(SL_GPIO_READPIN_COMMAND_GIVEN_IN_WRONG_STATE, COMMON, LOG_ERROR, "status: %4x", common_cb->state);
    return RSI_ERROR_COMMAND_GIVEN_IN_WRONG_STATE;
  }
  status = rsi_check_and_update_cmd_state(COMMON_CMD, IN_USE);
  if (status == RSI_SUCCESS) {

    // Allocate command buffer from common pool
    pkt = rsi_pkt_alloc(&common_cb->common_tx_pool);
    // If allocation of packet fails
    if (pkt == NULL) {
      // Change common state to allow state
      rsi_check_and_update_cmd_state(COMMON_CMD, ALLOW);
      // Return packet allocation failure error
      SL_PRINTF(SL_GPIO_READPIN_PKT_ALLOCATION_FAILURE, COMMON, LOG_ERROR);
      return RSI_ERROR_PKT_ALLOCATION_FAILURE;
    }
    config_gpio                = (rsi_gpio_pin_config_t *)pkt->data;
    config_gpio->gpio_type     = gpio_type;
    config_gpio->config_mode   = RSI_GET_GPIO;
    config_gpio->pin_num       = pin_num;
    config_gpio->output_value  = 0;
    config_gpio->config_values = 0;

    // Attach the buffer given by user
    common_cb->app_buffer = (uint8_t *)gpio_value;
    // Length of the buffer provided by user
    common_cb->app_buffer_length = 1;

#ifndef RSI_COMMON_SEM_BITMAP
    rsi_driver_cb_non_rom->common_wait_bitmap |= BIT(0);
#endif
    // Send GPIO configuration request
    status = rsi_driver_common_send_cmd(RSI_COMMON_REQ_GPIO_CONFIG, pkt);
    // Wait on common semaphore
    rsi_wait_on_common_semaphore(&rsi_driver_cb_non_rom->common_cmd_sem, RSI_GPIO_CONFIG_RESP_WAIT_TIME);
    // Change common state to allow state
    rsi_check_and_update_cmd_state(COMMON_CMD, ALLOW);
  } else {
    // Return common command error
    SL_PRINTF(SL_GPIO_READPIN_COMMAND_ERROR, COMMON, LOG_ERROR, "status: %4x", status);
    return status;
  }
  // Get common command response status
  status = rsi_common_get_status();
  // Return status
  SL_PRINTF(SL_GPIO_READPIN_EXIT, COMMON, LOG_INFO, "status: %4x", status);
  return status;
}

//*==============================================*/
/**
 * @brief       Read status of NWP GPIOs using Command from host. This is a non-blocking API.
 * @param[in]   gpio_num   -  GPIO Number : Valid values  0 - 15 for ULP \n
 *              GPIO Number : Valid values  0 - 63 for SOC \n
 *				GPIO Number : Valid values  0 - 3 for UULP 
 * @return 		0  - status of NWP GPIO is LOW \n 
 *                      1  - status of NWP GPIO is HIGH \n
 *		        < 0  Non-Zero Value - Failure        
 */

int32_t rsi_gpio_read(uint8_t gpio_num)
{
  int32_t status = 0;
  int32_t buf    = 0;
  status         = rsi_mem_rd((PAD_REN + (4 * gpio_num)), 4, (uint8_t *)&buf);
  if (status < 0) {
    return status;
  }
  buf |= 0x10;
  status = rsi_mem_wr((PAD_REN + (4 * gpio_num)), 4, (uint8_t *)&buf);
  if (status < 0) {
    return status;
  }
  buf    = 0x20;
  status = rsi_mem_wr((RSI_GPIO_ADDR + (gpio_num * 2)), 2, (uint8_t *)&buf);
  if (status < 0) {
    return status;
  }
  status = rsi_mem_rd((RSI_GPIO_ADDR + (gpio_num * 2)), 2, (uint8_t *)&buf);
  if (status < 0) {
    return status;
  }
  return (buf >> 13);
}

//*==============================================*/
/**
 *@brief        Drive the NWP GPIOs high or low using command from host. This is a non-blocking API.
 *@param[in]   	gpio_num       - GPIO Number : Valid values  0 - 15 for ULP \n
 *              GPIO Number : Valid values  0 - 63 for SOC \n
 *				GPIO Number : Valid values  0 - 3 for UULP
 *@param[in]    write          -  Value to be driven on GPIO \n
 *                                0x10 - Drive high \n
 *                                0 - Drive Low  \n
 *@return 	     0              -  Success \n
 *                   Non-Zero Value -  Failure	
 */
int32_t rsi_gpio_write(uint8_t gpio_num, uint8_t write)
{
  int32_t status = 0;

  status = rsi_mem_wr((RSI_GPIO_ADDR + (gpio_num * 2)), 2, (uint8_t *)&write);
  if (status < 0) {
    return status;
  }
  return status;
}

#endif
/** @} */
