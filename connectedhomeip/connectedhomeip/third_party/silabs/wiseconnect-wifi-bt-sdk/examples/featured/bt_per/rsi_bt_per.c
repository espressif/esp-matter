/*******************************************************************************
 * @file  rsi_bt_per.c
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
 * Include files
 * */

//! BT include file to refer BT APIs
#include <rsi_bt_apis.h>
#include <rsi_bt_common_apis.h>
#include <rsi_bt_common.h>
#include <rsi_bt_config.h>
#include <rsi_bt.h>
#include <stdio.h>

//! Common include file
#include <rsi_common_apis.h>
#include "rsi_driver.h"
//! application defines
#define RSI_BT_LOCAL_NAME (void *)"PER"

#define RSI_BT_PER_TRANSMIT_MODE 1
#define RSI_BT_PER_RECEIVE_MODE  2
#define RSI_PER_STATS            3
#define RSI_CONFIG_PER_MODE      RSI_BT_PER_RECEIVE_MODE

#define BT_PER_STATS_CMD_ID 0x08
#define BT_TRANSMIT_CMD_ID  0x15
#define BT_RECEIVE_CMD_ID   0x16

#define SEQUENCE_0    0
#define SEQUENCE_1    1
#define SEQUENCE_2    2
#define SEQUENCE_F0   3
#define SEQUENCE_PRBS 4

#define DISABLE 0
#define ENABLE  1

#define BR_MODE  1
#define EDR_MODE 2

#define SCO_LINK  0
#define ACL_LINK  1
#define ESCO_LINK 2

#define BURST_MODE     0
#define CONTIUOUS_MODE 1

#define NO_HOPPING     0
#define FIXED_HOPPING  1
#define RANDOM_HOPPING 2

#define ONBOARD_ANT_SEL 2
#define EXT_ANT_SEL     3

#define BT_EXTERNAL_RF 0
#define BT_INTERNAL_RF 1

#define NO_CHAIN_SEL      0
#define WLAN_HP_CHAIN_BIT 0
#define WLAN_LP_CHAIN_BIT 1
#define BT_HP_CHAIN_BIT   2
#define BT_LP_CHAIN_BIT   3

#define PLL_MODE_0 0
#define PLL_MODE_1 1

#define LOOP_BACK_MODE_DISABLE 0
#define LOOP_BACK_MODE_ENABLE  1

#define RSI_BT_BD_ADDR_1 0x12345678
#define RSI_BT_BD_ADDR_2 0x9012

#define PACKET_TYPE    15
#define PACKET_LEN     339
#define BT_RX_CHNL_NUM 10
#define BT_TX_CHNL_NUM 10

#define SCRAMBLER_SEED 0

#define PAYLOAD_TYPE      SEQUENCE_F0
#define BT_TX_POWER       10
#define RSI_INTER_PKT_GAP 0

//! Memory length for driver
#define BT_GLOBAL_BUFF_LEN 15000

#ifdef RSI_WITH_OS
//! BLE task stack size
#define RSI_BT_TASK_STACK_SIZE 3000

//! BT task priority
#define RSI_BT_TASK_PRIORITY 1

//! Number of packet to send or receive
#define NUMBER_OF_PACKETS 1000

//! Wireless driver task priority
#define RSI_DRIVER_TASK_PRIORITY 2

//! Wireless driver task stack size
#define RSI_DRIVER_TASK_STACK_SIZE 3000
#endif

#ifdef RSI_WITH_OS
//! BLE task stack size
#define RSI_BT_TASK_STACK_SIZE 3000

//! BT task priority
#define RSI_BT_TASK_PRIORITY 1

//! Number of packet to send or receive
#define NUMBER_OF_PACKETS 1000

//! Wireless driver task priority
#define RSI_DRIVER_TASK_PRIORITY 2

//! Wireless driver task stack size
#define RSI_DRIVER_TASK_STACK_SIZE 3000

void rsi_wireless_driver_task(void);

#endif

//! Memory to initialize driver
uint8_t global_buf[BT_GLOBAL_BUFF_LEN] = { 0 };

//! Application global parameters.
static rsi_bt_resp_get_local_name_t local_name  = { 0 };
static uint8_t local_dev_addr[RSI_DEV_ADDR_LEN] = { 0 };
static rsi_bt_tx_per_params_t bt_tx_per;
static rsi_bt_rx_per_params_t bt_rx_per;
static rsi_bt_per_stats_t per_stats;

/*==============================================*/
/**
 * @fn         rsi_bt_per
 * @brief      Tests the BT Classic Per TX.
 * @param[in]  none
 * @return    none.
 * @section description
 * This function is used to test the  Classic Per TX.
 */
int32_t rsi_bt_per(void)
{
  int32_t status = 0;
#ifdef RSI_WITH_OS
  rsi_task_handle_t driver_task_handle = NULL;
#endif
#ifndef RSI_WITH_OS
  //! Driver initialization
  status = rsi_driver_init(global_buf, BT_GLOBAL_BUFF_LEN);
  if ((status < 0) || (status > BT_GLOBAL_BUFF_LEN)) {
    return status;
  }

  //! SiLabs module intialisation
  status = rsi_device_init(LOAD_NWP_FW);
  if (status != RSI_SUCCESS) {
    return status;
  }
#endif
#ifdef RSI_WITH_OS
  //! Silabs module intialisation
  status = rsi_device_init(LOAD_NWP_FW);
  if (status != RSI_SUCCESS) {
    return status;
  }

  //! Task created for Driver task
  rsi_task_create((rsi_task_function_t)rsi_wireless_driver_task,
                  (uint8_t *)"driver_task",
                  RSI_DRIVER_TASK_STACK_SIZE,
                  NULL,
                  RSI_DRIVER_TASK_PRIORITY,
                  &driver_task_handle);
#endif
  //! WC initialization
  status = rsi_wireless_init(0, RSI_OPERMODE_WLAN_BT_CLASSIC);
  if (status != RSI_SUCCESS) {
    LOG_PRINT("\r\nWireless Initialization Failed, Error Code : 0x%lX\r\n", status);
    return status;
  } else {
    LOG_PRINT("\r\nWireless Initialization Success\r\n");
  }

  //! get the local device address(MAC address).
  status = rsi_bt_get_local_device_address(local_dev_addr);
  if (status != RSI_SUCCESS) {
    LOG_PRINT("\r\n Get Local Device Address Failed = 0x%lX\r\n", status);
  }

  //! set the local device name
  status = rsi_bt_set_local_name(RSI_BT_LOCAL_NAME);
  if (status != RSI_SUCCESS) {
    LOG_PRINT("\r\n Set Local Name Failed = 0x%lX\r\n", status);
  }

  //! get the local device name
  status = rsi_bt_get_local_name(&local_name);
  if (status != RSI_SUCCESS) {
    LOG_PRINT("\r\n Get Local Name Failed =0x%lX\r\n", status);
  }

  if (RSI_CONFIG_PER_MODE == RSI_BT_PER_TRANSMIT_MODE) {
    bt_tx_per.cmd_id                       = BT_TRANSMIT_CMD_ID;
    bt_tx_per.transmit_enable              = ENABLE;
    *(uint32_t *)&bt_tx_per.device_Addr[0] = RSI_BT_BD_ADDR_1;
    *(uint16_t *)&bt_tx_per.device_Addr[4] = RSI_BT_BD_ADDR_2;
    *(uint16_t *)&bt_tx_per.pkt_len[0]     = PACKET_LEN;
    bt_tx_per.pkt_type                     = PACKET_TYPE;
    bt_tx_per.br_edr_mode                  = BR_MODE;
    bt_tx_per.rx_chnl_in                   = BT_RX_CHNL_NUM;
    bt_tx_per.tx_chnl_in                   = BT_TX_CHNL_NUM;
    bt_tx_per.link_type                    = ACL_LINK;
    bt_tx_per.scrambler_seed               = SCRAMBLER_SEED;
    bt_tx_per.payload_type                 = PAYLOAD_TYPE;
    bt_tx_per.tx_power                     = BT_TX_POWER;
    bt_tx_per.transmit_mode                = BURST_MODE;
    bt_tx_per.hopping_type                 = NO_HOPPING;
    bt_tx_per.ant_sel                      = ONBOARD_ANT_SEL;
    bt_tx_per.inter_pkt_gap                = RSI_INTER_PKT_GAP;
    bt_tx_per.pll_mode                     = PLL_MODE_0;
    bt_tx_per.rf_type                      = BT_INTERNAL_RF;
    bt_tx_per.rf_chain                     = BT_HP_CHAIN_BIT;
    status                                 = rsi_bt_per_tx((uint32_t *)&bt_tx_per);
    if (status != RSI_SUCCESS) {
      LOG_PRINT("\n per transmit cmd failed : 0x%X \n", status);
    } else {
      LOG_PRINT("\nRSI_BT_PER_TRANSMIT_MODE \n"
                "cmd id: 0x%X \n"
                "enable: %d \n"
                "pkt_len: %d \n"
                "pkt_type: %d \n"
                "rx_chnl_in: %d \n"
                "tx_chnl_in: %d \n"
                "link_type: %d \n"
                "scrambler_seed: %d \n"
                "payload_type: %d \n"
                "tx_power: %d \n"
                "transmit_mode: %d \n"
                "hopping_type: %d \n"
                "ant_sel: %d \n"
                "inter_pkt_gap: %d \n"
                "pll_mode: %d \n"
                "rf_type: %d \n"
                "rf_chain: %d \n",
                bt_tx_per.cmd_id,
                bt_tx_per.transmit_enable,
                *(uint16_t *)&bt_tx_per.pkt_len[0],
                bt_tx_per.pkt_type,
                bt_tx_per.br_edr_mode,
                bt_tx_per.rx_chnl_in,
                bt_tx_per.tx_chnl_in,
                bt_tx_per.link_type,
                bt_tx_per.scrambler_seed,
                bt_tx_per.payload_type,
                bt_tx_per.tx_power,
                bt_tx_per.transmit_mode,
                bt_tx_per.hopping_type,
                bt_tx_per.ant_sel,
                bt_tx_per.inter_pkt_gap,
                bt_tx_per.pll_mode,
                bt_tx_per.rf_type,
                bt_tx_per.rf_chain);
    }

  } else if (RSI_CONFIG_PER_MODE == RSI_BT_PER_RECEIVE_MODE) {
    bt_rx_per.cmd_id                       = BT_RECEIVE_CMD_ID;
    bt_rx_per.receive_enable               = ENABLE;
    *(uint32_t *)&bt_rx_per.device_Addr[0] = RSI_BT_BD_ADDR_1;
    *(uint16_t *)&bt_rx_per.device_Addr[4] = RSI_BT_BD_ADDR_2;
    bt_rx_per.link_type                    = ACL_LINK;
    bt_rx_per.pkt_type                     = PACKET_TYPE;
    *(uint16_t *)&bt_tx_per.pkt_len[0]     = PACKET_LEN;
    bt_rx_per.scrambler_seed               = SCRAMBLER_SEED;
    bt_rx_per.br_edr_mode                  = BR_MODE;
    bt_rx_per.rx_chnl_in                   = BT_RX_CHNL_NUM;
    bt_rx_per.tx_chnl_in                   = BT_TX_CHNL_NUM;
    bt_rx_per.hopping_type                 = NO_HOPPING;
    bt_rx_per.ant_sel                      = ONBOARD_ANT_SEL;
    bt_rx_per.loop_back_mode               = LOOP_BACK_MODE_DISABLE;
    bt_rx_per.pll_mode                     = PLL_MODE_0;
    bt_rx_per.rf_type                      = BT_INTERNAL_RF;
    bt_rx_per.rf_chain                     = BT_HP_CHAIN_BIT;
    //! start the Receive PER functionality
    status = rsi_bt_per_rx((uint32_t *)&bt_rx_per);
    if (status != RSI_SUCCESS) {
      LOG_PRINT("\n per receive cmd failed : 0x%lX \n", status);
    } else {
      LOG_PRINT("\nRSI_BT_PER_RECEIVE_MODE \n"
                "cmd id: 0x%X \n"
                "enable: %d \n"
                "link_type: %d \n"
                "pkt_type: %d \n"
                "pkt_len: %d \n"
                "scrambler_seed: %d \n"
                "br_edr_mode: %d \n"
                "rx_chnl_in: %d \n"
                "tx_chnl_in: %d \n"
                "hopping_type: %d \n"
                "ant_sel: %d \n"
                "loop_back_mode: %d \n"
                "pll_mode: %d \n"
                "rf_type: %d \n"
                "rf_chain: %d \n",
                bt_rx_per.cmd_id,
                bt_rx_per.receive_enable,
                bt_rx_per.link_type,
                bt_rx_per.pkt_type,
                *(uint16_t *)&bt_tx_per.pkt_len[0],
                bt_rx_per.scrambler_seed,
                bt_rx_per.br_edr_mode,
                bt_rx_per.rx_chnl_in,
                bt_rx_per.tx_chnl_in,
                bt_rx_per.hopping_type,
                bt_rx_per.ant_sel,
                bt_rx_per.loop_back_mode,
                bt_rx_per.pll_mode,
                bt_rx_per.rf_type,
                bt_rx_per.rf_chain);
    }
  }

  while (1) {
    status = rsi_bt_per_stats(BT_PER_STATS_CMD_ID, &per_stats);
    if (status != RSI_SUCCESS) {
      LOG_PRINT("\n per stats cmd failed : 0x%lX \n", status);
    } else {
      rsi_delay_ms(1000);
      LOG_PRINT("\nPER Stats \n"
                "crc_fail_cnt : %d \n"
                "crc_pass_cnt: %d \n"
                "tx_dones: %d \n"
                "rssi: %d \n"
                "id_pkts_rcvd :%d \n",
                per_stats.crc_fail_cnt,
                per_stats.crc_pass_cnt,
                per_stats.tx_dones,
                per_stats.rssi,
                per_stats.id_pkts_rcvd);
    }
  }

  return 0;
}

/*==============================================*/
/**
 * @fn         main_loop
 * @brief      Schedules the Driver task.
 * @param[in]  none.
 * @return     none.
 * @section description
 * This function schedules the Driver task.
 */
void main_loop(void)
{
  while (1) {
    rsi_wireless_driver_task();
  }
}

/*==============================================*/
/**
 * @fn         main
 * @brief      main function
 * @param[in]  none.
 * @return     none.
 * @section description
 * This is the main function to call Application
 */
int main(void)
{

  int32_t status;
#ifdef RSI_WITH_OS
  rsi_task_handle_t bt_task_handle = NULL;
#endif

#ifndef RSI_WITH_OS
  //Start BT-BLE Stack
  intialize_bt_stack(STACK_BT_MODE);

  //! Call BLE Peripheral application
  status = rsi_bt_per();

  //! Application main loop
  main_loop();

  return status;
#endif

#ifdef RSI_WITH_OS
  //! Driver initialization
  status = rsi_driver_init(global_buf, BT_GLOBAL_BUFF_LEN);
  if ((status < 0) || (status > BT_GLOBAL_BUFF_LEN)) {
    return status;
  }

  //Start BT-BLE Stack
  intialize_bt_stack(STACK_BT_MODE);

  //! OS case
  //! Task created for BLE task
  rsi_task_create((rsi_task_function_t)rsi_bt_per,
                  (uint8_t *)"bt_task",
                  RSI_BT_TASK_STACK_SIZE,
                  NULL,
                  RSI_BT_TASK_PRIORITY,
                  &bt_task_handle);

  rsi_start_os_scheduler();
  return status;
#endif
}
