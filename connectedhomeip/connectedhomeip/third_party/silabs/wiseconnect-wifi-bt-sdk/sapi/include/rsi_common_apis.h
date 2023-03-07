/*******************************************************************************
* @file  rsi_common_apis.h
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

#ifndef RSI_COMMON_APIS_H
#define RSI_COMMON_APIS_H
#ifdef RSI_M4_INTERFACE
#include "rsi_ccp_user_config.h"
#endif
#if defined(RSI_DEBUG_PRINTS) || (defined(RSI_ENABLE_DEBUG_PRINT) && defined(LINUX_PLATFORM))
#include <stdio.h>
#endif
#ifdef RSI_WITH_OS
#include "rsi_os.h"
#endif
#ifdef RSI_DEBUG_PRINTS
#include "rsi_common.h"
#endif
/******************************************************
 * *                      Macros
 * ******************************************************/
#if !(defined(FRDM_K28F) || defined(MXRT_595s))
#if defined(RSI_DEBUG_PRINTS)
#define LOG_PRINT(...)                                            \
  {                                                               \
    rsi_mutex_lock(&rsi_driver_cb_non_rom->debug_prints_mutex);   \
    printf(__VA_ARGS__);                                          \
    rsi_mutex_unlock(&rsi_driver_cb_non_rom->debug_prints_mutex); \
  }
#elif (defined(RSI_M4_INTERFACE) && defined(DEBUG_UART))
#define LOG_PRINT(...) DEBUGOUT(__VA_ARGS__)
#elif defined(RSI_ENABLE_DEBUG_PRINT) && defined(LINUX_PLATFORM)
#define LOG_PRINT(...) printf(__VA_ARGS__)
#else
#define LOG_PRINT(...)
#endif
#endif

// To select antenna type , internal antenna for 301 modules RFOUT2 for 03 modules
#define RSI_INTERNAL_OR_RFOUT2 0

// To select antenna type , ufl connector for 301 modules RFOUT1 for 03 modules
#define RSI_UFL_OR_RFOUT1 1

// success return value
#define RSI_SUCCESS 0

// failure return value
#define RSI_FAILURE -1

//Load Image types
#define LOAD_NWP_FW                    '1'
#define LOAD_DEFAULT_NWP_FW_ACTIVE_LOW 0x71

// Upgrade images
#define BURN_NWP_FW 'B'

// Upgrade images
#define RSI_JUMP_TO_PC 'J'

#define RSI_ACTIVE_LOW_INTR  0x2
#define RSI_ACTIVE_HIGH_INTR 0x0
#define RSI_RX_BUFFER_CHECK  0x15
// TX buffer full macro
#define RSI_TX_BUFFER_FULL -2
#define RSI_SOFT_RESET     0
#define RSI_HARD_RESET     1

#ifdef CONFIGURE_GPIO_FROM_HOST
typedef struct rsi_gpio_pin_config_val_s {

  uint8_t drive_strength;
  uint8_t input_mode;
  uint8_t default_gpio_state;

} rsi_gpio_pin_config_val_t;

typedef struct rsi_gpio_pin_config_s {
  uint8_t gpio_type;
  uint8_t config_mode;
  uint8_t pin_num;
  uint8_t config_values;
  uint8_t output_value;

} rsi_gpio_pin_config_t;
#endif
/******************************************************
 * *                    Constants
 * ******************************************************/
/******************************************************
 * *                   Enumerations
 * ******************************************************/

// enumerations for power save profile modes
typedef enum rsi_power_save_profile_mode_e {
  RSI_ACTIVE        = 0,
  RSI_SLEEP_MODE_1  = 1,
  RSI_SLEEP_MODE_2  = 2,
  RSI_SLEEP_MODE_8  = 8,
  RSI_SLEEP_MODE_10 = 10,
} rsi_power_save_profile_mode_t;

// enumerations for power save profile types
typedef enum rsi_power_save_profile_type_e {
  RSI_MAX_PSP  = 0,
  RSI_FAST_PSP = 1,
  RSI_UAPSD    = 2
} rsi_power_save_profile_type_t;

// enumerations for differnt power mode exists for SiLabs module
typedef enum rsi_power_save_mode_e {
  RSI_POWER_MODE_DISABLE      = 0,
  RSI_CONNECTED_SLEEP_PS      = 1,
  RSI_CONNECTED_GPIO_BASED_PS = 2,
  RSI_CONNECTED_MSG_BASED_PS  = 3,
#ifdef RSI_M4_INTERFACE
  RSI_CONNECTED_M4_BASED_PS = 4,
#endif
  RSI_GPIO_BASED_DEEP_SLEEP = 8,
  RSI_MSG_BASED_DEEP_SLEEP  = 9,
#ifdef RSI_M4_INTERFACE
  RSI_M4_BASED_DEEP_SLEEP = 10
#endif
} rsi_power_save_mode_t;

#ifdef CONFIGURE_GPIO_FROM_HOST
typedef enum rsi_gpio_config_mode_e { RSI_CONFIG_GPIO = 0, RSI_SET_GPIO = 1, RSI_GET_GPIO = 2 } rsi_gpio_config_mode_t;
#endif

#ifdef RSI_M4_INTERFACE
#define NO_OF_HOURS_IN_A_DAY      24
#define NO_OF_MINUTES_IN_AN_HOUR  60
#define NO_OF_SECONDS_IN_A_MINUTE 60
#define BASE_YEAR                 2000
#define SOC_OPER_FREQUENCY        36000
#endif

/******************************************************
 * *                 Type Definitions
 * ******************************************************/
/******************************************************
 * *                    Structures
 * ******************************************************/
/******************************************************
 * *                 Global Variables
 * ******************************************************/
/******************************************************
 * *               Function Declarations
 * ******************************************************/
#include <stdint.h>
extern int32_t rsi_driver_init(uint8_t *buffer, uint32_t length);
extern int32_t rsi_driver_deinit(void);
extern int32_t rsi_wireless_init(uint16_t opermode, uint16_t coex_mode);
extern int32_t rsi_wireless_antenna(uint8_t type, uint8_t gain_2g, uint8_t gain_5g);
extern int32_t rsi_send_feature_frame(void);
extern int32_t rsi_get_fw_version(uint8_t *response, uint16_t length);
extern int32_t rsi_common_debug_log(int32_t assertion_type, int32_t assertion_level);
extern int32_t rsi_socket_config(void);
extern int32_t rsi_wireless_deinit(void);
extern int32_t rsi_common_get_status(void);
extern void rsi_wireless_driver_task(void);
extern int16_t rsi_check_assertion(void);
extern int32_t rsi_get_ram_log(uint32_t addr, uint32_t length);
extern int32_t rsi_driver_version(uint8_t *request);
#ifdef RSI_ASSERT_API
int32_t rsi_assert(void);
#endif
#ifdef RSI_WITH_OS
extern int32_t rsi_destroy_driver_task_and_driver_deinit(rsi_task_handle_t *task_handle);
#endif
#endif
