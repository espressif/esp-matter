/*******************************************************************************
* @file  sl_logging.h
* @brief
*******************************************************************************
* # License
* <b>Copyright 2021 Silicon Laboratories Inc. www.silabs.com</b>
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
#ifndef __SL_APP_LOGGING_H__
#define __SL_APP_LOGGING_H__
#ifdef SAPI_LOGGING_ENABLE
//Includes
#include "rsi_common.h"
#include "rsi_driver.h"

extern rsi_driver_cb_non_rom_t *rsi_driver_cb_non_rom;

typedef enum { LOG_OFF, LOG_ERROR, LOG_WARNING, LOG_INFO } log_levels_t;
typedef enum { BYTE_LEN, WORD_LEN, VARIABLE_LEN, DWORD_LEN } log_data_len_t;
typedef enum { BLE, BLUETOOTH, COMMON, CRYPTO, DRIVER, FW_UPDATE, NETWORK, WLAN } log_component_t;

typedef struct log_flags_s {
  uint32_t signal_lost : 1;
  uint32_t tsf_present : 1;
  uint32_t hdid_present : 1;
  uint32_t did : 12;
} __attribute__((packed)) log_flags_t;

typedef struct log_tsf_s {
  uint16_t tsf_format : 1;
  uint16_t tsf : 15;
} __attribute__((packed)) log_tsf_t;

typedef struct debug_info_s {
  uint32_t did : 12;
  uint32_t arg1_len : 2;
  uint32_t arg2_len : 2;
  uint32_t arg3_len : 2;
  uint32_t component : 3;
  uint32_t level : 3;
  uint32_t reserved : 8;
} debug_info_t;

extern uint8_t *logging_buffer;
#define LOG_BUFFER_SIZE 128
// LDID is 5 bits wide
#define MAX_LDID 0x1F
// LTSF is 7 bits wide
#define MAX_LTSF 0x3F
// Maximum number of arguments
#define MAX_NUM_ARGS 3

// Function prototypes
uint32_t sl_log_init(uint8_t *buffer);
uint32_t sl_log_deinit();
void sl_log_no_args(uint32_t diagnostic_info);
void sl_log_1_args(uint32_t diagnostic_info, uint32_t arg1);
void sl_log_2_args(uint32_t diagnostic_info, uint32_t arg1, uint32_t arg2);
void sl_log_3_args(uint32_t diagnostic_info, uint32_t arg1, uint32_t arg2, uint32_t arg3);

// HAL function to send the logging buffer
int32_t sl_hal_send_log(uint8_t *buffer, uint32_t buffer_length);
#endif

#endif
