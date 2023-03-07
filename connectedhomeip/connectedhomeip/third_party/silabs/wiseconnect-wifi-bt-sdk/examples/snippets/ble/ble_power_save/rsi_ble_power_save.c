/*******************************************************************************
 * @file  rsi_ble_power_save.c
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
#include <rsi_common_apis.h>
#include <rsi_driver.h>
#include <string.h>

#ifdef RSI_M4_INTERFACE
#include "rsi_rtc.h"
#include "rsi_board.h"
#include "rsi_m4.h"
#include "rsi_chip.h"
#include "rsi_driver.h"
#include "rsi_wisemcu_hardware_setup.h"
#include <string.h>
#include "rsi_ps_ram_func.h"
#include "rsi_ds_timer.h"
#endif

#ifdef FW_LOGGING_ENABLE
//! Firmware logging includes
#include "sl_fw_logging.h"
#endif

/*=======================================================================*/
//   ! MACROS
/*=======================================================================*/
//! Maximum number of advertise reports to hold
#define NO_OF_ADV_REPORTS 10

#ifdef FW_LOGGING_ENABLE
//! Memory length of driver updated for firmware logging
#define GLOBAL_BUFF_LEN (15000 + (FW_LOG_QUEUE_SIZE * MAX_FW_LOG_MSG_LEN))
#else
//! Memory length for driver
#define GLOBAL_BUFF_LEN 15000
#endif

#ifdef RSI_M4_INTERFACE
#define IVT_OFFSET_ADDR        0x8212000  /*<! Application IVT location !>*/
#define WKP_RAM_USAGE_LOCATION 0x24061000 /*<! Bootloader RAM usage location !>*/

#define WIRELESS_WAKEUP_IRQHandler NPSS_TO_MCU_WIRELESS_INTR_IRQn
#endif

#ifdef COMMON_FLASH_EN
#define NWPAON_MEM_HOST_ACCESS_CTRL_CLEAR_1 (*(volatile uint32_t *)(0x41300000 + 0x4))
#define M4SS_TASS_CTRL_SET_REG              (*(volatile uint32_t *)(0x24048400 + 0x34))
#define M4SS_TASS_CTRL_CLR_REG              (*(volatile uint32_t *)(0x24048400 + 0x38))

#endif

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
#ifdef FW_LOGGING_ENABLE
/*=======================================================================*/
//!    Firmware logging configurations
/*=======================================================================*/
//! Firmware logging task defines
#define RSI_FW_TASK_STACK_SIZE (512 * 2)
#define RSI_FW_TASK_PRIORITY   2
//! Firmware logging variables
extern rsi_semaphore_handle_t fw_log_app_sem;
rsi_task_handle_t fw_log_task_handle = NULL;
//! Firmware logging prototypes
void sl_fw_log_callback(uint8_t *log_message, uint16_t log_message_length);
void sl_fw_log_task(void);
#endif

void rsi_wireless_driver_task(void);
#endif

/*=======================================================================*/
//   ! GLOBAL VARIABLES
/*=======================================================================*/
//! Memory to initialize driver
uint8_t global_buf[GLOBAL_BUFF_LEN] = { 0 };
uint8_t wlan_radio_initialized = 0, powersave_cmd_given = 0;
uint8_t device_found          = 0;
uint8_t remote_dev_addr[18]   = { 0 };
uint8_t remote_dev_bd_addr[6] = { 0 };
static uint8_t remote_name[31];
static uint8_t remote_addr_type                                 = 0;
static rsi_bt_resp_get_local_name_t rsi_app_resp_get_local_name = { 0 };
static uint8_t rsi_app_resp_get_dev_addr[RSI_DEV_ADDR_LEN]      = { 0 };
static rsi_ble_event_conn_status_t rsi_app_connected_device     = { 0 };
static rsi_ble_event_disconnect_t rsi_app_disconnected_device   = { 0 };
uint8_t rsi_ble_states_bitmap;

rsi_semaphore_handle_t ble_slave_conn_sem;
rsi_semaphore_handle_t ble_main_task_sem;
static uint32_t ble_app_event_map;
static uint32_t ble_app_event_map1;
/*=======================================================================*/
//   ! EXTERN VARIABLES
/*=======================================================================*/

/*=======================================================================*/
//   ! EXTERN FUNCTIONS
/*=======================================================================*/

/*========================================================================*/
//!  CALLBACK FUNCTIONS
/*=======================================================================*/

/*=======================================================================*/
//   ! PROCEDURES
/*=======================================================================*/
/*==============================================*/
#ifdef RSI_M4_INTERFACE
/**
 * @fn         rsi_ble_only_Trigger_M4_Sleep
 * @brief      Keeps the M4 In the Sleep 
 * @param[in]  none
 * @return    none.
 * @section description
 * This function is used to trigger sleep in the M4 and in the case of the retention submitting the buffer valid
 * to the TA for the rx packets.
 */
void rsi_ble_only_Trigger_M4_Sleep(void)
{
  /* Configure Wakeup-Source */
  RSI_PS_SetWkpSources(WIRELESS_BASED_WAKEUP);

  NVIC_EnableIRQ(WIRELESS_WAKEUP_IRQHandler);

#ifndef FLASH_BASED_EXECUTION_ENABLE
  /* LDOSOC Default Mode needs to be disabled */
  RSI_PS_LdoSocDefaultModeDisable();

  /* bypass_ldorf_ctrl needs to be enabled */
  RSI_PS_BypassLdoRfEnable();

  RSI_PS_FlashLdoDisable();

  /* Configure RAM Usage and Retention Size */
  RSI_WISEMCU_ConfigRamRetention(WISEMCU_48KB_RAM_IN_USE, WISEMCU_RETAIN_DEFAULT_RAM_DURING_SLEEP);

  /* Trigger M4 Sleep */
  RSI_WISEMCU_TriggerSleep(SLEEP_WITH_RETENTION,
                           DISABLE_LF_MODE,
                           0,
                           (uint32_t)RSI_PS_RestoreCpuContext,
                           0,
                           RSI_WAKEUP_WITH_RETENTION_WO_ULPSS_RAM);

#else

#ifdef COMMON_FLASH_EN
  M4SS_P2P_INTR_SET_REG &= ~BIT(3);
#endif
  /* Configure RAM Usage and Retention Size */
  //  RSI_WISEMCU_ConfigRamRetention(WISEMCU_192KB_RAM_IN_USE, WISEMCU_RETAIN_DEFAULT_RAM_DURING_SLEEP);
  RSI_PS_SetRamRetention(M4ULP_RAM16K_RETENTION_MODE_EN | ULPSS_RAM_RETENTION_MODE_EN | M4ULP_RAM_RETENTION_MODE_EN
                         | M4SS_RAM_RETENTION_MODE_EN);

  RSI_WISEMCU_TriggerSleep(SLEEP_WITH_RETENTION,
                           DISABLE_LF_MODE,
                           WKP_RAM_USAGE_LOCATION,
                           (uint32_t)RSI_PS_RestoreCpuContext,
                           IVT_OFFSET_ADDR,
                           RSI_WAKEUP_FROM_FLASH_MODE);

#endif
#ifdef DEBUG_UART
  fpuInit();
  DEBUGINIT();
#endif
}
#endif
#ifdef RSI_M4_INTERFACE
void M4_sleep_wakeup()
{
  /* Configure Wakeup-Source */
  RSI_PS_SetWkpSources(WIRELESS_BASED_WAKEUP);

  /* Enable NVIC */
  NVIC_EnableIRQ(WIRELESS_WAKEUP_IRQHandler);

#ifndef FLASH_BASED_EXECUTION_ENABLE
  /* LDOSOC Default Mode needs to be disabled */
  RSI_PS_LdoSocDefaultModeDisable();

  /* bypass_ldorf_ctrl needs to be enabled */
  RSI_PS_BypassLdoRfEnable();

  /* Disable FlashLDO */
  RSI_PS_FlashLdoDisable();

  /* Configure RAM Usage and Retention Size */
  RSI_WISEMCU_ConfigRamRetention(WISEMCU_128KB_RAM_IN_USE, WISEMCU_RETAIN_DEFAULT_RAM_DURING_SLEEP);

  /* Trigger M4 Sleep */
  RSI_WISEMCU_TriggerSleep(SLEEP_WITH_RETENTION,
                           DISABLE_LF_MODE,
                           0,
                           (uint32_t)RSI_PS_RestoreCpuContext,
                           0,
                           RSI_WAKEUP_WITH_RETENTION_WO_ULPSS_RAM);
#else

  /* Configure RAM Usage and Retention Size */
  //RSI_WISEMCU_ConfigRamRetention(WISEMCU_192KB_RAM_IN_USE, WISEMCU_RETAIN_DEFAULT_RAM_DURING_SLEEP);

  RSI_PS_SetRamRetention(M4ULP_RAM16K_RETENTION_MODE_EN | ULPSS_RAM_RETENTION_MODE_EN | M4ULP_RAM_RETENTION_MODE_EN
                         | M4SS_RAM_RETENTION_MODE_EN);
#ifdef COMMON_FLASH_EN
  M4SS_P2P_INTR_SET_REG &= ~BIT(3);
#endif

  /* Trigger M4 Sleep*/
  RSI_WISEMCU_TriggerSleep(SLEEP_WITH_RETENTION,
                           DISABLE_LF_MODE,
                           WKP_RAM_USAGE_LOCATION,
                           (uint32_t)RSI_PS_RestoreCpuContext,
                           IVT_OFFSET_ADDR,
                           RSI_WAKEUP_FROM_FLASH_MODE);

#endif
}
#endif

/**
 * @fn         rsi_ble_app_init_events
 * @brief      initializes the event parameter.
 * @param[in]  none.
 * @return     none.
 * @section description
 * This function is used during BLE initialization.
 */
static void rsi_ble_app_init_events()
{
  ble_app_event_map  = 0;
  ble_app_event_map1 = 0;
  return;
}

/*==============================================*/
/**
 * @fn         rsi_ble_app_set_event
 * @brief      set the specific event.
 * @param[in]  event_num, specific event number.
 * @return     none.
 * @section description
 * This function is used to set/raise the specific event.
 */
void rsi_ble_app_set_event(uint32_t event_num)
{

  if (event_num < 32) {
    ble_app_event_map |= BIT(event_num);
  } else {
    ble_app_event_map1 |= BIT((event_num - 32));
  }
  rsi_semaphore_post(&ble_main_task_sem);

  return;
}

/*==============================================*/
/**
 * @fn         rsi_ble_app_clear_event
 * @brief      clears the specific event.
 * @param[in]  event_num, specific event number.
 * @return     none.
 * @section description
 * This function is used to clear the specific event.
 */
static void rsi_ble_app_clear_event(uint32_t event_num)
{

  if (event_num < 32) {
    ble_app_event_map &= ~BIT(event_num);
  } else {
    ble_app_event_map1 &= ~BIT((event_num - 32));
  }

  return;
}

/*==============================================*/
/**
 * @fn         rsi_ble_app_get_event
 * @brief      returns the first set event based on priority
 * @param[in]  none.
 * @return     int32_t
 *             > 0  = event number
 *             -1   = not received any event
 * @section description
 * This function returns the highest priority event among all the set events
 */
static int32_t rsi_ble_app_get_event(void)
{
  uint32_t ix;

  for (ix = 0; ix < 64; ix++) {
    if (ix < 32) {
      if (ble_app_event_map & (1 << ix)) {
        return ix;
      }
    } else {
      if (ble_app_event_map1 & (1 << (ix - 32))) {
        return ix;
      }
    }
  }

  return (-1);
}

/*==============================================*/
/**
 * @fn         rsi_ble_on_adv_report_event
 * @brief      invoked when advertise report event is received
 * @param[in]  adv_report, pointer to the received advertising report
 * @return     none.
 * @section description
 * This callback function updates the scanned remote devices list
 */
void rsi_ble_on_adv_report_event(rsi_ble_event_adv_report_t *adv_report)
{

  if (device_found == 1) {
    return;
  }

  memset(remote_name, 0, 31);
  BT_LE_ADPacketExtract(remote_name, adv_report->adv_data, adv_report->adv_data_len);

  remote_addr_type = adv_report->dev_addr_type;
  rsi_6byte_dev_address_to_ascii(remote_dev_addr, (uint8_t *)adv_report->dev_addr);
  memcpy(remote_dev_bd_addr, (uint8_t *)adv_report->dev_addr, 6);
  if (((device_found == 0) && ((strcmp((const char *)remote_name, RSI_REMOTE_DEVICE_NAME)) == 0))
      || ((remote_addr_type == RSI_BLE_DEV_ADDR_TYPE)
          && ((strcmp((const char *)remote_dev_addr, RSI_BLE_DEV_ADDR) == 0)))) {
    device_found = 1;
    rsi_ble_app_set_event(RSI_APP_EVENT_ADV_REPORT);
  }
}

/*==============================================*/
/**
 * @fn         rsi_ble_on_connect_event
 * @brief      invoked when connection complete event is received
 * @param[out] resp_conn, connected remote device information
 * @return     none.
 * @section description
 * This callback function indicates the status of the connection
 */
void rsi_ble_on_connect_event(rsi_ble_event_conn_status_t *resp_conn)
{
  memcpy(&rsi_app_connected_device, resp_conn, sizeof(rsi_ble_event_conn_status_t));
  rsi_ble_app_set_event(RSI_APP_EVENT_CONNECTED);
  //! unblock connection semaphore
  rsi_semaphore_post(&ble_slave_conn_sem);
}

/*==============================================*/
/**
 * @fn         rsi_ble_on_disconnect_event
 * @brief      invoked when disconnection event is received
 * @param[in]  resp_disconnect, disconnected remote device information
 * @param[in]  reason, reason for disconnection.
 * @return     none.
 * @section description
 * This callback function indicates disconnected device information and status
 */
void rsi_ble_on_disconnect_event(rsi_ble_event_disconnect_t *resp_disconnect, uint16_t reason)
{
  UNUSED_PARAMETER(reason); //This statement is added only to resolve compilation warning, value is unchanged
  memcpy(&rsi_app_disconnected_device, resp_disconnect, sizeof(rsi_ble_event_disconnect_t));
  //! Comparing Remote slave bd address to check the scan bitmap
  if (!(memcmp(remote_dev_bd_addr, (uint8_t *)resp_disconnect->dev_addr, 6))) {
    CLR_BIT1(rsi_ble_states_bitmap, RSI_SCAN_STATE);
  } else {
    CLR_BIT1(rsi_ble_states_bitmap, RSI_ADV_STATE);
  }
  rsi_ble_app_set_event(RSI_APP_EVENT_DISCONNECTED);
}

/*==============================================*/
/**
 * @fn         rsi_ble_on_enhance_conn_status_event
 * @brief      invoked when enhanced connection complete event is received
 * @param[out] resp_conn, connected remote device information
 * @return     none.
 * @section description
 * This callback function indicates the status of the connection
 */
void rsi_ble_on_enhance_conn_status_event(rsi_ble_event_enhance_conn_status_t *resp_enh_conn)
{
  rsi_app_connected_device.dev_addr_type = resp_enh_conn->dev_addr_type;
  memcpy(rsi_app_connected_device.dev_addr, resp_enh_conn->dev_addr, RSI_DEV_ADDR_LEN);
  rsi_app_connected_device.status = resp_enh_conn->status;
  rsi_ble_app_set_event(RSI_APP_EVENT_CONNECTED);
  //! unblock connection semaphore
  rsi_semaphore_post(&ble_slave_conn_sem);
}
/*==============================================*/
/**
 * @fn         rsi_initiate_power_save
 * @brief      send power save command to RS9116 module
 *
 * @param[out] none
 * @return     status of commands, success-> 0, failure ->-1
 * @section description
 * This function sends command to keep module in power save
 */
int32_t rsi_initiate_power_save(void)
{
  int32_t status = RSI_SUCCESS;

  //! enable wlan radio
  if (!wlan_radio_initialized) {
    status = rsi_wlan_radio_init();
    if (status != RSI_SUCCESS) {
      //LOG_PRINT("\n radio init failed,error = %ld\n", status);
      return status;
    } else {
      wlan_radio_initialized = 1;
    }
  }
  //! initiating power save in wlan mode
  status = rsi_wlan_power_save_profile(RSI_SLEEP_MODE_2, PSP_TYPE);
  if (status != RSI_SUCCESS) {
    //LOG_PRINT("\r\n Failed in initiating power save\r\n");
    return status;
  }

  status = rsi_bt_power_save_profile(RSI_SLEEP_MODE_2, PSP_TYPE);
  if (status != RSI_SUCCESS) {
    //LOG_PRINT("\r\n Failed in initiating power save\r\n");
    return status;
  }

  return status;
}
/*==============================================*/
/**
 * @fn         rsi_ble_app_task
 * @brief      Tests the BLE GAP peripheral role.
 * @param[in]  none
 * @return    none.
 * @section description
 * This function is used to test the BLE peripheral/central role and simple GAP API's.
 */
int32_t rsi_ble_app_task(void)
{
  int32_t status          = 0;
  int32_t temp_event_map  = 0;
  int32_t temp_event_map1 = 0;

#if ((BLE_ROLE == SLAVE_MODE) || (BLE_ROLE == DUAL_MODE))
  uint8_t adv[31] = { 2, 1, 6 };
#endif
#ifdef RSI_WITH_OS
  rsi_task_handle_t driver_task_handle = NULL;
#endif
#ifdef FW_LOGGING_ENABLE
  //Fw log component level
  sl_fw_log_level_t fw_component_log_level;
#endif
#ifndef RSI_WITH_OS
  //! Driver initialization
  status = rsi_driver_init(global_buf, GLOBAL_BUFF_LEN);
  if ((status < 0) || (status > GLOBAL_BUFF_LEN)) {
    return status;
  }
  //! SiLabs module intialisation
  status = rsi_device_init(LOAD_NWP_FW);
  if (status != RSI_SUCCESS) {
    LOG_PRINT("\r\nDevice Initialization Failed, Error Code : 0x%lX\r\n", status);
    return status;
  } else {
    LOG_PRINT("\r\nDevice Initialization Success\r\n");
  }
#ifdef COMMON_FLASH_EN
  NWPAON_MEM_HOST_ACCESS_CTRL_CLEAR_1                                           = (BIT(24) | BIT(25));
  BATT_FF->M4SS_TASS_CTRL_SET_REG_b.M4SS_CTRL_TASS_AON_PWRGATE_EN               = 1;
  BATT_FF->M4SS_TASS_CTRL_SET_REG_b.M4SS_CTRL_TASS_AON_DISABLE_ISOLATION_BYPASS = 1;
  M4SS_TASS_CTRL_CLR_REG                                                        = BIT(2);
#endif
#endif
#ifdef RSI_WITH_OS
  //! Silabs module initialisation
  status = rsi_device_init(LOAD_NWP_FW);
  if (status != RSI_SUCCESS) {
    LOG_PRINT("\r\nDevice Initialization Failed, Error Code : 0x%lX\r\n", status);
    return status;
  } else {
    LOG_PRINT("\r\nDevice Initialization Success\r\n");
  }

  //! Task created for Driver task
  rsi_task_create((rsi_task_function_t)rsi_wireless_driver_task,
                  (uint8_t *)"driver_task",
                  RSI_DRIVER_TASK_STACK_SIZE,
                  NULL,
                  RSI_DRIVER_TASK_PRIORITY,
                  &driver_task_handle);
#endif

#ifdef RSI_M4_INTERFACE

  RSI_PS_FlashLdoEnable();
  /* MCU Hardware Configuration for Low-Power Applications */
  RSI_WISEMCU_HardwareSetup();

#endif

  //! WC initialization
  status = rsi_wireless_init(0, RSI_OPERMODE_WLAN_BLE);
  if (status != RSI_SUCCESS) {
    LOG_PRINT("\r\nWireless Initialization Failed, Error Code : 0x%lX\r\n", status);
    return status;
  } else {
    LOG_PRINT("\r\nWireless Initialization Success\r\n");
  }
#ifdef FW_LOGGING_ENABLE
  //! Set log levels for firmware components
  sl_set_fw_component_log_levels(&fw_component_log_level);

  //! Configure firmware logging
  status = sl_fw_log_configure(FW_LOG_ENABLE,
                               FW_TSF_GRANULARITY_US,
                               &fw_component_log_level,
                               FW_LOG_BUFFER_SIZE,
                               sl_fw_log_callback);
  if (status != RSI_SUCCESS) {
    LOG_PRINT("\r\n Firmware Logging Init Failed\r\n");
  }
#ifdef RSI_WITH_OS
  //! Create firmware logging semaphore
  rsi_semaphore_create(&fw_log_app_sem, 0);
  //! Create firmware logging task
  rsi_task_create((rsi_task_function_t)sl_fw_log_task,
                  (uint8_t *)"fw_log_task",
                  RSI_FW_TASK_STACK_SIZE,
                  NULL,
                  RSI_FW_TASK_PRIORITY,
                  &fw_log_task_handle);
#endif
#endif
  //! BLE register GAP callbacks
  rsi_ble_gap_register_callbacks(rsi_ble_on_adv_report_event,
                                 rsi_ble_on_connect_event,
                                 rsi_ble_on_disconnect_event,
                                 NULL,
                                 NULL,
                                 NULL,
                                 rsi_ble_on_enhance_conn_status_event,
                                 NULL,
                                 NULL,
                                 NULL);
  //! create ble main task if ble protocol is selected
  rsi_semaphore_create(&ble_main_task_sem, 0);
  //! initialize the event map
  rsi_ble_app_init_events();

  //! get the local device address(MAC address).
  status = rsi_bt_get_local_device_address(rsi_app_resp_get_dev_addr);
  if (status != RSI_SUCCESS) {
    LOG_PRINT("\r\n ble get local device address cmd failed with reason code : %x \n", status);
    return status;
  }

  //! set the local device name
  status = rsi_bt_set_local_name((uint8_t *)RSI_BLE_LOCAL_NAME);
  if (status != RSI_SUCCESS) {
    LOG_PRINT("\r\n ble set local name cmd failed with reason code : %x \n", status);
    return status;
  }

  //! get the local device name
  status = rsi_bt_get_local_name(&rsi_app_resp_get_local_name);
  if (status != RSI_SUCCESS) {
    LOG_PRINT("\r\n ble get local name cmd failed with reason code : %x \n", status);
    return status;
  }

  rsi_semaphore_create(&ble_slave_conn_sem, 0);

#if ((BLE_ROLE == SLAVE_MODE) || (BLE_ROLE == DUAL_MODE))
  //! prepare advertise data //local/device name
  adv[3] = strlen(RSI_BLE_LOCAL_NAME) + 1;
  adv[4] = 9;
  strcpy((char *)&adv[5], RSI_BLE_LOCAL_NAME);

  //! set advertise data
  rsi_ble_set_advertise_data(adv, strlen(RSI_BLE_LOCAL_NAME) + 5);

  //! start the advertising
  LOG_PRINT("\n Start advertising \n");
  status = rsi_ble_start_advertising();
  if (status != RSI_SUCCESS) {
    LOG_PRINT("\r\n ble start advertising failed with reason code : %x \n", status);
    return status;
  }
  SET_BIT1(rsi_ble_states_bitmap, RSI_ADV_STATE);
#endif

#if ((BLE_ROLE == MASTER_MODE1) || (BLE_ROLE == DUAL_MODE))
  //! start scanning
  LOG_PRINT("\n Start scanning \n");
  status = rsi_ble_start_scanning();
  if (status != RSI_SUCCESS) {
    LOG_PRINT("\r\n ble start scanning failed with reason code : %x \n", status);
    return status;
  }
  SET_BIT1(rsi_ble_states_bitmap, RSI_SCAN_STATE);
#endif
#ifdef ENABLE_POWER_SAVE

  if (!powersave_cmd_given) {
    LOG_PRINT("\r\nInitiating PowerSave\r\n");
    status = rsi_initiate_power_save();
    if (status != RSI_SUCCESS) {
      LOG_PRINT("\r\n Failed to initiate power save in BLE mode \r\n");
      return status;
    }
    powersave_cmd_given = 1;
  }

#ifdef RSI_M4_INTERFACE
  P2P_STATUS_REG &= ~M4_wakeup_TA;
  // LOG_PRINT("\n RSI_BLE_REQ_PWRMODE\n ");
#endif
#endif
  while (1) {
    //! Application main loop
#ifndef RSI_WITH_OS
    //! run the non OS scheduler
    rsi_wireless_driver_task();
#endif
    //! checking for received events
    temp_event_map = rsi_ble_app_get_event();
    if (temp_event_map == RSI_FAILURE) {
      //! if events are not received loop will be continued.
      rsi_semaphore_wait(&ble_main_task_sem, 0);
#ifdef RSI_M4_INTERFACE
      //! if events are not received loop will be continued.
      if ((!(P2P_STATUS_REG & TA_wakeup_M4)) && (!rsi_driver_cb->scheduler_cb.event_map)) {
        P2P_STATUS_REG &= ~M4_wakeup_TA;
        rsi_ble_only_Trigger_M4_Sleep();
      }
#endif
      continue;
    }

    //! if any event is received, it will be served.
    switch (temp_event_map) {
      case RSI_APP_EVENT_ADV_REPORT: {
        //! clear the advertise report event.
        rsi_ble_app_clear_event(RSI_APP_EVENT_ADV_REPORT);
        //! advertise report event.
        //! initiate stop scanning command.
        status = rsi_ble_stop_scanning();
        if (status != RSI_SUCCESS) {
          LOG_PRINT("\r\n ble stop scanning failed with reason code : %x \n", status);
          return status;
        }

        //! initiating the connection with remote BLE device
        status = rsi_ble_connect(remote_addr_type, (int8_t *)remote_dev_bd_addr);
        if (status != RSI_SUCCESS) {
          LOG_PRINT("\r\n ble connect command failed with reason code : %x \n", status);
          return status;
        }

        rsi_semaphore_wait(&ble_slave_conn_sem, 10000);
#ifndef RSI_WITH_OS
        // need to give sufficient time to connect to remote device
        rsi_delay_ms(10000);
#endif

        temp_event_map1 = rsi_ble_app_get_event();

        if ((temp_event_map1 == -1) || (!(temp_event_map1 & RSI_APP_EVENT_CONNECTED))) {
          LOG_PRINT("\r\n Initiating connect cancel command \n");
          status = rsi_ble_connect_cancel((int8_t *)remote_dev_bd_addr);
          if (status != RSI_SUCCESS) {
            LOG_PRINT("\r\n ble connect cancel cmd status = %lx \n", status);
          } else {
            CLR_BIT1(rsi_ble_states_bitmap, RSI_SCAN_STATE);
            rsi_ble_app_set_event(RSI_APP_EVENT_DISCONNECTED);
          }
        }

      } break;
      case RSI_APP_EVENT_CONNECTED: {
        //! remote device connected event
        LOG_PRINT("\n connection is success .............\n");
        //! clear the connected event.
        rsi_ble_app_clear_event(RSI_APP_EVENT_CONNECTED);

      } break;
      case RSI_APP_EVENT_DISCONNECTED: {
        //! remote device disconnected event
        LOG_PRINT("\n In disconnect event............ \n ");
        //! clear the disconnected event.
        rsi_ble_app_clear_event(RSI_APP_EVENT_DISCONNECTED);

        LOG_PRINT("\n keep module in to active state \n");

        //! initiating Active mode in BT mode
        status = rsi_bt_power_save_profile(RSI_ACTIVE, PSP_TYPE);
        if (status != RSI_SUCCESS) {
          LOG_PRINT("\r\n Failed to keep Module in ACTIVE mode \r\n");
          return status;
        }

        //! initiating Active mode in WLAN mode
        status = rsi_wlan_power_save_profile(RSI_ACTIVE, PSP_TYPE);
        if (status != RSI_SUCCESS) {
          LOG_PRINT("\r\n Failed to keep Module in ACTIVE mode \r\n");
          return status;
        }
        if (((BLE_ROLE == SLAVE_MODE) || (BLE_ROLE == DUAL_MODE))
            && (!(CHK_BIT1(rsi_ble_states_bitmap, RSI_ADV_STATE)))) {
          //! set device in advertising mode.
          LOG_PRINT("\n Start advertising \n");
adv:
          status = rsi_ble_start_advertising();
          if (status != RSI_SUCCESS) {
            LOG_PRINT("\r\n ble start advertising failed with reason code : %x \n", status);
            goto adv;
          }
          SET_BIT1(rsi_ble_states_bitmap, RSI_ADV_STATE);
        }
        if (((BLE_ROLE == MASTER_MODE1) || (BLE_ROLE == DUAL_MODE))
            && (!(CHK_BIT1(rsi_ble_states_bitmap, RSI_SCAN_STATE)))) {
          device_found = 0;
          //! set device in scanning mode.
          LOG_PRINT("\n Start scanning \n");
scan:
          status = rsi_ble_start_scanning();
          if (status != RSI_SUCCESS) {
            LOG_PRINT("\r\n ble start scanning failed with reason code : %x \n", status);
            goto scan;
          }
          SET_BIT1(rsi_ble_states_bitmap, RSI_SCAN_STATE);
        }
        LOG_PRINT("\n keep module in to power save \n");
        status = rsi_bt_power_save_profile(PSP_MODE, PSP_TYPE);
        if (status != RSI_SUCCESS) {
          LOG_PRINT("\r\n Failed to keep Module in power save \r\n");
          return status;
        }

        status = rsi_wlan_power_save_profile(PSP_MODE, PSP_TYPE);
        if (status != RSI_SUCCESS) {
          LOG_PRINT("\r\n Failed to keep Module in power save \r\n");
          return status;
        }
        LOG_PRINT("\n Module is in power save \n");
      } break;
    }
  }
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
  rsi_task_handle_t ble_task_handle = NULL;
#endif

#ifndef RSI_WITH_OS

  //Start BT Stack
  intialize_bt_stack(STACK_BTLE_MODE);

  //! Call BLE Peripheral application
  status = rsi_ble_app_task();

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
  rsi_task_create((rsi_task_function_t)(int32_t)rsi_ble_app_task,
                  (uint8_t *)"ble_task",
                  RSI_BT_TASK_STACK_SIZE,
                  NULL,
                  RSI_BT_TASK_PRIORITY,
                  &ble_task_handle);

  //! OS TAsk Start the scheduler
  rsi_start_os_scheduler();

  return status;
#endif
}
