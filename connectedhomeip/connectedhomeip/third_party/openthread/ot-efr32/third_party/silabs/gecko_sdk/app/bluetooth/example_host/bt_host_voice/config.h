/***************************************************************************//**
 * @file
 * @brief Configuration header file
 *******************************************************************************
 * # License
 * <b>Copyright 2018 Silicon Laboratories Inc. www.silabs.com</b>
 *******************************************************************************
 *
 * SPDX-License-Identifier: Zlib
 *
 * The licensor of this software is Silicon Laboratories Inc.
 *
 * This software is provided 'as-is', without any express or implied
 * warranty. In no event will the authors be held liable for any damages
 * arising from the use of this software.
 *
 * Permission is granted to anyone to use this software for any purpose,
 * including commercial applications, and to alter it and redistribute it
 * freely, subject to the following restrictions:
 *
 * 1. The origin of this software must not be misrepresented; you must not
 *    claim that you wrote the original software. If you use this software
 *    in a product, an acknowledgment in the product documentation would be
 *    appreciated but is not required.
 * 2. Altered source versions must be plainly marked as such, and must not be
 *    misrepresented as being the original software.
 * 3. This notice may not be removed or altered from any source distribution.
 *
 ******************************************************************************/

#ifndef CONFIG_H_
#define CONFIG_H_

#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>

#include <stdlib.h>
#include <errno.h>
#include <unistd.h>

#include "app_log.h"
#include "sl_bt_api.h"

/***************************************************************************************************
 * Type Definitions
 **************************************************************************************************/

/** Supported sample rates */
typedef enum {
  sr_8k = 8,
  sr_16k = 16,
}adc_sample_rate_t;

/** Configuration structure */
typedef struct {
  uint32_t baud_rate;                 /**< UART baud rate */
  char *uart_port;                    /**< UART port */
  char *out_file_name;                /**< Output file name */
  bool output_to_stdout;              /**< Audio stream to stdout instead of file */
  char *transfer_status_file_name;    /**< Transfer status file name */
  bool audio_data_notification;       /**< Enable/Disable audio data notification*/
  adc_sample_rate_t adc_sample_rate;  /**< Sample rate*/
  bool filter_enabled;                /**< Enable/Disable filtering*/
  bool encoding_enabled;              /**< Enable/Disable encoding*/
  bool transfer_status;               /**< Enable/Disable transfet status flag*/
  bool remote_address_set;            /**< Remote Device address given as application parameter */
  bd_addr remote_address;             /**< Remote Device address */
}configuration_t;

#define DEFAULT_UART_BAUD_RATE   115200
#define DEFAULT_OUTPUT_FILE_NAME "-"
#define IMA_FILE_EXTENSION ".ima"
#define S8_FILE_EXTENSION ".s8"
#define S16_FILE_EXTENSION ".s16"
#define DEFAULT_TRANSFER_STATUS_FILE_NAME "transfer_status.log"

#define SERVICE_VOICE_OVER_BLE_UUID { 0x10, 0x0ad, 0xb3, 0x9e, 0x42, 0xdf, 0xd3, 0x93, 0x62, 0x43, 0x2e, 0xdc, 0x93, 0x11, 0xef, 0xb7 }

/*  UUIDs of the characteristics*/
#define CHAR_AUDIO_DATA_UUID      { 0xf2, 0xc5, 0xfd, 0x27, 0xec, 0x81, 0x3e, 0x94, 0x3d, 0x47, 0x08, 0xec, 0x72, 0x7a, 0xce, 0x00 }
#define CHAR_SAMPLE_RATE_UUID     { 0x01, 0xc6, 0xfd, 0x27, 0xec, 0x81, 0x3e, 0x94, 0x3d, 0x47, 0x08, 0xec, 0x72, 0x7a, 0xce, 0x00 }
#define CHAR_FILTER_ENABLE_UUID   { 0x02, 0xc6, 0xfd, 0x27, 0xec, 0x81, 0x3e, 0x94, 0x3d, 0x47, 0x08, 0xec, 0x72, 0x7a, 0xce, 0x00 }
#define CHAR_ENCODING_ENABLE_UUID { 0x03, 0xc6, 0xfd, 0x27, 0xec, 0x81, 0x3e, 0x94, 0x3d, 0x47, 0x08, 0xec, 0x72, 0x7a, 0xce, 0x00 }
#define CHAR_TRANSFER_STATUS_UUID { 0x04, 0xc6, 0xfd, 0x27, 0xec, 0x81, 0x3e, 0x94, 0x3d, 0x47, 0x08, 0xec, 0x72, 0x7a, 0xce, 0x00 }

/**  Default configuration*/
#define DEFAULT_CONFIGURATION                                                        \
  {                                                                                  \
    DEFAULT_UART_BAUD_RATE,            /** The default baud rate to use. */          \
    NULL,                              /** No default port */                        \
    DEFAULT_OUTPUT_FILE_NAME,          /** The default output file name. */          \
    false,                             /** Audio streaming to file. */               \
    DEFAULT_TRANSFER_STATUS_FILE_NAME, /** The default transfer status file name. */ \
    true,                              /** Audio Data notification enabled */        \
    sr_16k,                            /** The default ADC sample rate */            \
    false,                             /** Enable filtering */                       \
    true,                              /** Enable encoding */                        \
    true,                              /** Enable transfer status */                 \
  }

/** Error macro */
#define ERROR_EXIT(...) do { app_log(__VA_ARGS__); exit(EXIT_FAILURE); } while (0)

/** Debug macros */
#define DEBUG_INFO(...)     do { app_log("[Inf] "); app_log(__VA_ARGS__); } while (0)
#define DEBUG_WARNING(...)  do { app_log("[War] "); app_log(__VA_ARGS__); } while (0)
#define DEBUG_ERROR(...)    do { app_log("[Err] "); app_log(__VA_ARGS__); } while (0)
#define DEBUG_SUCCESS(...)  do { app_log("[Ok ] "); app_log(__VA_ARGS__); app_log("\n"); } while (0)

configuration_t *CONF_get(void);

#endif /* CONFIG_H_ */
