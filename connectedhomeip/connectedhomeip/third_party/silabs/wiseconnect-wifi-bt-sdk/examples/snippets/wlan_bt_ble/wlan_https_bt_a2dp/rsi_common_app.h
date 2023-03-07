/*
 * rsi_common_app.h
 *
 *  Created on: 03-Nov-2018
 *     
 */

#ifndef RSI_COMMON_APP_H_
#define RSI_COMMON_APP_H_

#include "stdint.h"
#include "stdio.h"
#include "fsl_debug_console.h"

#define ENABLE_1P8V 1 //! Disable this when using 3.3v

#define SPI_DMA //! Enable SPI with DMA
#define RSI_ENABLE_DEMOS
/*********************Demo Macros*******************************/

//#define COEX_MAX_APP                           1

/***************************************************************/

//! WLAN Mode
#define RSI_WLAN_MODE RSI_WLAN_CLIENT_MODE

//#if COEX_MAX_APP // Major
//! By default this macro is disabled
//! Enable this macro to take run time inputs
#define RUN_TIME_CONFIG_ENABLE 0
//! compile time configurations
#if !RUN_TIME_CONFIG_ENABLE
/***********************************************************************************************************************************************/
//! Macros to enable/disable individual protocol activities Minors
/***********************************************************************************************************************************************/
#define RSI_ENABLE_BLE_TEST  0
#define RSI_ENABLE_BT_TEST   1
#define RSI_ENABLE_ANT_TEST  0
#define RSI_ENABLE_WIFI_TEST 1
#endif
//#endif

//! CO-EX Mode configuration
#define RSI_COEX_MODE (0x109)

#define TICK_1_MSEC 1 //!	0 - Disable, 1 - Enable
#ifndef RSI_WITH_OS
//! systick timer configurations
#define TICK_100_USEC 0 //!	0 - Disable, 1 - Enable
#define TICK_1_MSEC   1 //!	0 - Disable, 1 - Enable
#define T_NOP         0 //! Use NOP for delay
#endif

#define RSI_DEBUG_EN 0

#define LOG_PRINT PRINTF
#if RSI_DEBUG_EN
#define LOG_PRINT_D PRINTF
#else
#define LOG_PRINT_D
#endif

//! Function declarations
void rsi_demo_app(void);

//#if COEX_MAX_APP
int32_t rsi_wlan_app_task(void);
int32_t rsi_bt_app_task();
void rsi_ble_main_app_task(void);
void rsi_ble_task_on_conn(void *parameters);
void rsi_ble_slave_app_task(void);
void rsi_ant_app_task(void);
void rsi_ui_app_task(void);
void rsi_common_app_task(void);
void rsi_window_reset_notify_app_task(void);
int32_t rsi_certificate_valid(uint16_t valid, uint16_t socket_id);
void rsi_app_task_send_certificates(void);
#endif
