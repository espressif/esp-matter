/*******************************************************************************
 * @file  rsi_ble_per.c
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

//! BLE include file to refer BLE APIs
#include <rsi_ble_apis.h>
#include <rsi_ble_config.h>
#include <rsi_bt_common_apis.h>
#include <rsi_bt_common.h>
#include <rsi_ble.h>
#include <rsi_bt_common.h>

//! Common include file
#include <rsi_common_apis.h>

#include <string.h>
#include "rsi_driver.h"

#define RSI_BLE_LOCAL_NAME (void *)"BLE_PERIPHERAL"

//! Memory length for driver
#define BT_GLOBAL_BUFF_LEN 15000

#define DISABLE 0
#define ENABLE  1

#define RSI_BLE_PER_TRANSMIT_MODE 1
#define RSI_BLE_PER_RECEIVE_MODE  2
#define RSI_PER_STATS             3

#define RSI_CONFIG_PER_MODE RSI_BLE_PER_RECEIVE_MODE

#define LE_ONE_MBPS       1
#define LE_TWO_MBPS       2
#define LE_125_KBPS_CODED 4
#define LE_500_KBPS_CODED 8

#define DATA_PRBS9                 0x00
#define DATA_FOUR_ONES_FOUR_ZEROES 0x01
#define DATA_ALT_ONES_AND_ZEROES   0x02
#define DATA_PRSB15                0x03
#define DATA_ALL_ONES              0x04
#define DATA_ALL_ZEROES            0x05
#define DATA_FOUR_ZEROES_FOUR_ONES 0x06
#define DATA_ALT_ZEROES_AND_ONES   0x07

#define LE_ADV_CHNL_TYPE  0
#define LE_DATA_CHNL_TYPE 1

#define BURST_MODE     0
#define CONTIUOUS_MODE 1

#define NO_HOPPING     0
#define FIXED_HOPPING  1
#define RANDOM_HOPPING 2

#define BT_PER_STATS_CMD_ID 0x08
#define BLE_TRANSMIT_CMD_ID 0x13
#define BLE_RECEIVE_CMD_ID  0x14

#define BLE_ACCESS_ADDR    0x71764129
#define BLE_TX_PKT_LEN     32
#define BLE_PHY_RATE       LE_ONE_MBPS
#define BLE_RX_CHNL_NUM    10
#define BLE_TX_CHNL_NUM    10
#define BLE_TX_POWER_INDEX 8
#define SCRAMBLER_SEED     0
#define NUM_PKTS           0
#define RSI_INTER_PKT_GAP  0

#define ONBOARD_ANT_SEL 2
#define EXT_ANT_SEL     3

#define BLE_EXTERNAL_RF 0
#define BLE_INTERNAL_RF 1

#define NO_CHAIN_SEL      0
#define WLAN_HP_CHAIN_BIT 0
#define WLAN_LP_CHAIN_BIT 1
#define BT_HP_CHAIN_BIT   2
#define BT_LP_CHAIN_BIT   3

#define PLL_MODE_0 0
#define PLL_MODE_1 1

#define LOOP_BACK_MODE_DISABLE 0
#define LOOP_BACK_MODE_ENABLE  1

#define EXT_DATA_LEN_IND 1

#define DUTY_CYCLING_DISABLE 0
#define DUTY_CYCLING_ENABLE  1

#ifdef RSI_WITH_OS
//! BLE task stack size
#define RSI_BT_TASK_STACK_SIZE 1000

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
static rsi_bt_resp_get_local_name_t rsi_app_resp_get_local_name = { 0 };
static uint8_t rsi_app_resp_get_dev_addr[RSI_DEV_ADDR_LEN]      = { 0 };
static rsi_ble_per_transmit_t rsi_ble_per_tx;
static rsi_ble_per_receive_t rsi_ble_per_rx;
static rsi_bt_per_stats_t per_stats;

/*==============================================*/
/**
 * @fn         rsi_ble_per
 * @brief      Tests the BLE PER Modes.
 * @param[in]  none
 * @return    none.
 * @section description
 * This function is used to test the different BLE PER Modes.
 */
int32_t rsi_ble_per(void)
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

  //! Silabs module intialisation
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
  status = rsi_wireless_init(0, RSI_OPERMODE_WLAN_BLE);
  if (status != RSI_SUCCESS) {
    LOG_PRINT("\r\nWireless Initialization Failed, Error Code : 0x%lX\r\n", status);
    return status;
  } else {
    LOG_PRINT("\r\nWireless Initialization Success\r\n");
  }

  //! get the local device address(MAC address).
  status = rsi_bt_get_local_device_address(rsi_app_resp_get_dev_addr);
  if (status != RSI_SUCCESS) {
    LOG_PRINT("\r\n Get Local Device Address Failed = %x\r\n", status);
  }

  //! set the local device name
  status = rsi_bt_set_local_name(RSI_BLE_LOCAL_NAME);
  if (status != RSI_SUCCESS) {
    LOG_PRINT("\r\n Set Local Name Failed = %x\r\n", status);
  }

  //! get the local device name
  status = rsi_bt_get_local_name(&rsi_app_resp_get_local_name);
  if (status != RSI_SUCCESS) {
    LOG_PRINT("\r\n Get Local Name Failed = %x\r\n", status);
  }

  if (RSI_CONFIG_PER_MODE == RSI_BLE_PER_TRANSMIT_MODE) {
    rsi_ble_per_tx.cmd_ix                       = BLE_TRANSMIT_CMD_ID;
    rsi_ble_per_tx.transmit_enable              = ENABLE;
    *(uint32_t *)&rsi_ble_per_tx.access_addr[0] = BLE_ACCESS_ADDR;
    *(uint16_t *)&rsi_ble_per_tx.pkt_len[0]     = BLE_TX_PKT_LEN;
    rsi_ble_per_tx.phy_rate                     = BLE_PHY_RATE;
    rsi_ble_per_tx.rx_chnl_num                  = BLE_RX_CHNL_NUM;
    rsi_ble_per_tx.tx_chnl_num                  = BLE_TX_CHNL_NUM;
    rsi_ble_per_tx.scrambler_seed               = SCRAMBLER_SEED;
    rsi_ble_per_tx.payload_type                 = DATA_FOUR_ZEROES_FOUR_ONES;
    rsi_ble_per_tx.le_chnl_type                 = LE_DATA_CHNL_TYPE;
    rsi_ble_per_tx.tx_power                     = BLE_TX_POWER_INDEX;
    rsi_ble_per_tx.transmit_mode                = BURST_MODE;
    rsi_ble_per_tx.freq_hop_en                  = NO_HOPPING;
    rsi_ble_per_tx.ant_sel                      = ONBOARD_ANT_SEL;
    rsi_ble_per_tx.inter_pkt_gap                = RSI_INTER_PKT_GAP;
    rsi_ble_per_tx.pll_mode                     = PLL_MODE_0;
    rsi_ble_per_tx.rf_type                      = BLE_INTERNAL_RF;
    rsi_ble_per_tx.rf_chain                     = BT_HP_CHAIN_BIT;
    //! start the Transmit PER functionality
    status = rsi_ble_per_transmit(&rsi_ble_per_tx);
    if (status != RSI_SUCCESS) {
      LOG_PRINT("\n per transmit cmd failed : 0x%X \n", status);
    } else {
      LOG_PRINT("\nRSI_BLE_PER_TRANSMIT_MODE \n"
                "cmd id: 0x%X \n"
                "enable: %d \n"
                "access_addr: 0x%X \n"
                "pkt_len: %d \n"
                "phy_rate: %d \n"
                "rx_chnl_num: %d \n"
                "tx_chnl_num: %d \n"
                "scrambler_seed: %d \n"
                "payload_type: %d \n"
                "le_chnl_type: %d \n"
                "tx_power: %d \n"
                "transmit_mode: %d \n"
                "freq_hop_en: %d \n"
                "ant_sel: %d \n"
                "inter_pkt_gap: %d \n"
                "pll_mode: %d \n"
                "rf_type: %d \n"
                "rf_chain: %d \n",
                rsi_ble_per_tx.cmd_ix,
                rsi_ble_per_tx.transmit_enable,
                *(uint32_t *)&rsi_ble_per_tx.access_addr[0],
                (*(uint16_t *)&rsi_ble_per_tx.pkt_len[0]),
                rsi_ble_per_tx.phy_rate,
                rsi_ble_per_tx.rx_chnl_num,
                rsi_ble_per_tx.tx_chnl_num,
                rsi_ble_per_tx.scrambler_seed,
                rsi_ble_per_tx.payload_type,
                rsi_ble_per_tx.le_chnl_type,
                rsi_ble_per_tx.tx_power,
                rsi_ble_per_tx.transmit_mode,
                rsi_ble_per_tx.freq_hop_en,
                rsi_ble_per_tx.ant_sel,
                rsi_ble_per_tx.inter_pkt_gap,
                rsi_ble_per_tx.inter_pkt_gap,
                rsi_ble_per_tx.pll_mode,
                rsi_ble_per_tx.rf_type,
                rsi_ble_per_tx.rf_chain);
    }
  } else if (RSI_CONFIG_PER_MODE == RSI_BLE_PER_RECEIVE_MODE) {
    rsi_ble_per_rx.cmd_ix                       = BLE_RECEIVE_CMD_ID;
    rsi_ble_per_rx.receive_enable               = ENABLE;
    *(uint32_t *)&rsi_ble_per_rx.access_addr[0] = BLE_ACCESS_ADDR;
    rsi_ble_per_rx.ext_data_len_indication      = EXT_DATA_LEN_IND;
    rsi_ble_per_rx.phy_rate                     = BLE_PHY_RATE;
    rsi_ble_per_rx.rx_chnl_num                  = BLE_RX_CHNL_NUM;
    rsi_ble_per_rx.tx_chnl_num                  = BLE_TX_CHNL_NUM;
    rsi_ble_per_rx.scrambler_seed               = SCRAMBLER_SEED;
    rsi_ble_per_rx.le_chnl_type                 = LE_DATA_CHNL_TYPE;
    rsi_ble_per_rx.loop_back_mode               = LOOP_BACK_MODE_DISABLE;
    rsi_ble_per_rx.freq_hop_en                  = NO_HOPPING;
    rsi_ble_per_rx.ant_sel                      = ONBOARD_ANT_SEL;
    rsi_ble_per_rx.duty_cycling_en              = DUTY_CYCLING_DISABLE;
    rsi_ble_per_rx.pll_mode                     = PLL_MODE_0;
    rsi_ble_per_rx.rf_type                      = BLE_INTERNAL_RF;
    rsi_ble_per_rx.rf_chain                     = BT_HP_CHAIN_BIT;
    //! start the Receive PER functionality
    rsi_ble_per_receive(&rsi_ble_per_rx);
    if (status != RSI_SUCCESS) {
      LOG_PRINT("\n per receive cmd failed : %x \n", status);
    } else {
      LOG_PRINT("\nRSI_BLE_PER_RECEIVE_MODE \n"
                "cmd id: 0x%X \n"
                "enable: %d \n"
                "access_addr: 0x%X \n"
                "ext_data_len_indication: %d \n"
                "phy_rate: %d \n"
                "rx_chnl_num: %d \n"
                "tx_chnl_num: %d \n"
                "scrambler_seed: %d \n"
                "le_chnl_type: %d \n"
                "loop_back_mode: %d \n"
                "freq_hop_en: %d \n"
                "ant_sel: %d \n"
                "duty_cycling_en: %d \n"
                "pll_mode: %d \n"
                "rf_type: %d \n"
                "rf_chain: %d \n",
                rsi_ble_per_rx.cmd_ix,
                rsi_ble_per_rx.receive_enable,
                *(uint32_t *)&rsi_ble_per_rx.access_addr[0],
                rsi_ble_per_rx.ext_data_len_indication,
                rsi_ble_per_rx.phy_rate,
                rsi_ble_per_rx.rx_chnl_num,
                rsi_ble_per_rx.tx_chnl_num,
                rsi_ble_per_rx.scrambler_seed,
                rsi_ble_per_rx.le_chnl_type,
                rsi_ble_per_rx.loop_back_mode,
                rsi_ble_per_rx.freq_hop_en,
                rsi_ble_per_rx.ant_sel,
                rsi_ble_per_rx.duty_cycling_en,
                rsi_ble_per_rx.pll_mode,
                rsi_ble_per_rx.rf_type,
                rsi_ble_per_rx.rf_chain);
    }
  }

  while (1) {
    status = rsi_bt_per_stats(BT_PER_STATS_CMD_ID, &per_stats);
    if (status != RSI_SUCCESS) {
      LOG_PRINT("\n per stats cmd failed : %x \n", status);
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

  //Start BT Stack
  intialize_bt_stack(STACK_BTLE_MODE);

  //! Call BLE Peripheral application
  status = rsi_ble_per();

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

  //Start BT Stack
  intialize_bt_stack(STACK_BTLE_MODE);

  //! OS case
  //! Task created for BLE task
  rsi_task_create((rsi_task_function_t)rsi_ble_per,
                  (uint8_t *)"ble_task",
                  RSI_BT_TASK_STACK_SIZE,
                  NULL,
                  RSI_BT_TASK_PRIORITY,
                  &bt_task_handle);

  //! OS TAsk Start the scheduler
  rsi_start_os_scheduler();

  return status;
#endif
}
