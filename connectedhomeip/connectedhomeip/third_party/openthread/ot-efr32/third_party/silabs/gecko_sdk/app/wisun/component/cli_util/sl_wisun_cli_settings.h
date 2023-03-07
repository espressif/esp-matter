/***************************************************************************//**
 * @file
 * @brief Application commandline settings
 *******************************************************************************
 * # License
 * <b>Copyright 2021 Silicon Laboratories Inc. www.silabs.com</b>
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

#ifndef __SL_WISUN_CLI_SETTINGS_H__
#define __SL_WISUN_CLI_SETTINGS_H__

// -----------------------------------------------------------------------------
//                                   Includes
// -----------------------------------------------------------------------------

#include <stdint.h>
#include <stddef.h>
#include "sl_status.h"
#include "sl_wisun_types.h"
#include "sl_wisun_api.h"
#include "sl_wisun_cli_util.h"

/**************************************************************************//**
 * @brief App settings macro definitions
 *****************************************************************************/
#define APP_SETTINGS_INPUT_FLAG_NONE        (0U)
#define APP_SETTINGS_INPUT_FLAG_SIGNED      (1U << 0)
#define APP_SETTINGS_INPUT_FLAG_DEFAULT     (APP_SETTINGS_INPUT_FLAG_NONE)
#define APP_SETTINGS_OUTPUT_FLAG_NONE       (0U)
#define APP_SETTINGS_OUTPUT_FLAG_SIGNED     (1U << 0)
#define APP_SETTINGS_OUTPUT_FLAG_HEX        (1U << 1)
#define APP_SETTINGS_OUTPUT_FLAG_FIXEDSIZE  (1U << 2)
#define APP_SETTINGS_OUTPUT_FLAG_DEFAULT    (APP_SETTINGS_OUTPUT_FLAG_NONE)
#define APP_SETTINGS_VALUE_SIZE_NONE        (0U)
#define APP_SETTINGS_VALUE_SIZE_UINT8       (1U)
#define APP_SETTINGS_VALUE_SIZE_UINT16      (2U)
#define APP_SETTINGS_VALUE_SIZE_UINT32      (4U)

struct app_settings_entry_t;

/**************************************************************************//**
 * @brief Function pointer for setter function
 *****************************************************************************/
typedef sl_status_t (*app_settings_set_key_handler)(const char *value_str,
                                                    const char *key_str,
                                                    const struct app_settings_entry_t *entry);

/**************************************************************************//**
 * @brief Function pointer for getter function
 *****************************************************************************/
typedef sl_status_t (*app_settings_get_key_handler)(char *value_str,
                                                    const char *key_str,
                                                    const struct app_settings_entry_t *entry);

/**************************************************************************//**
 * @brief Structure for settings entry
 *****************************************************************************/
typedef struct app_settings_entry_t{
  char *key;
  uint8_t domain;
  uint8_t value_size;
  uint8_t input;
  uint8_t output;
  void *value;
  const app_enum_t *input_enum_list;
  const app_enum_t *output_enum_list;
  app_settings_set_key_handler set_handler;
  app_settings_get_key_handler get_handler;
  char *description;
} app_settings_entry_t;

/**************************************************************************//**
 * @brief Structure for saving items
 *****************************************************************************/
typedef struct {
  void *data;
  size_t data_size;
  const void *default_val;
} app_saving_item_t;

// -----------------------------------------------------------------------------
//                              Macros and Typedefs
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------
//                                Global Variables
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------
//                          Public Function Declarations
// -----------------------------------------------------------------------------

/**************************************************************************//**
 * @brief Initialize settings.
 * @details This function loads the settings from NVM.
 *****************************************************************************/
void app_settings_init(void);

/**************************************************************************//**
 * @brief Save settings.
 * @details This function saves the settings by using NVM.
 * @return SL_STATUS_OK if the saving is successful
 * @return SL_STATUS_FAIL saving failed
 *****************************************************************************/
sl_status_t app_settings_save(void);

/**************************************************************************//**
 * @brief Reset settings.
 * @details This function deletes settings by NVM and reverts their default value.
 *****************************************************************************/
void app_settings_reset(void);

/**************************************************************************//**
 * @brief Set app settings.
 * @details Set a value for a particular domain and key.
 * @param[in] domain_and_key Domain and key string
 * @param[in] value_str Value string
 * @return sl_status_t SL_STATUS_INVALID_KEY on error, otherwise the setter result
 *****************************************************************************/
sl_status_t app_settings_set(char *const domain_and_key, const char *const value_str);

/**************************************************************************//**
 * @brief Get app settings.
 * @details Get a value from app settings.
 * @param domain_and_key Domain and key string
 * @return sl_status_t SL_STATUS_OK on success, otherwise SL_STATUS_FAIL
 *****************************************************************************/
sl_status_t app_settings_get(char *const domain_and_key);

/**************************************************************************//**
 * @brief App settings help.
 * @details Get help from app setings
 * @param domain_and_key Domain and key string
 * @param get Function called by a Get method or a Set method
 * @return sl_status_t SL_STATUS_OK on success, otherwise SL_STATUS_FAIL
 *****************************************************************************/
sl_status_t app_settings_help(char *const domain_and_key, bool get);

/**************************************************************************//**
 * @brief Set a string in app settings.
 * @details Set a string value in app settings
 * @param[in] value_str Value string
 * @param[in] key_str Key string
 * @param[in] entry App settings entry
 * @return sl_status_t SL_STATUS_OK on success, SL_STATUS_FAIL on error
 *****************************************************************************/
sl_status_t app_settings_set_string(const char *value_str,
                                    const char *key_str,
                                    const app_settings_entry_t *entry);

/**************************************************************************//**
 * @brief Get string from app settings.
 * @details Get string from app settings.
 * @param[out] value_str Value string destination
 * @param[in] key_str Key string
 * @param[in] entry App settings entry
 * @return sl_status_t SL_STATUS_OK on success, SL_STATUS_FAIL on error
 *****************************************************************************/
sl_status_t app_settings_get_string(char *value_str,
                                    const char *key_str,
                                    const app_settings_entry_t *entry);

/**************************************************************************//**
 * @brief Set an integer in app settings.
 * @details Set integer
 * @param[in] value_str Value string
 * @param[in] key_str Key string
 * @param[in] entry App settings entry
 * @return sl_status_t SL_STATUS_OK on success, SL_STATUS_FAIL on error
 *****************************************************************************/
sl_status_t app_settings_set_integer(const char *value_str,
                                     const char *key_str,
                                     const app_settings_entry_t *entry);

/**************************************************************************//**
 * @brief Get an integer from app settings.
 * @details Get an integer from app settings.
 * @param[out] value_str Value string destination
 * @param[in] key_str Key string
 * @param[in] entry App settings entry
 * @return sl_status_t SL_STATUS_OK on success, SL_STATUS_FAIL on error
 *****************************************************************************/
sl_status_t app_settings_get_integer(char *value_str,
                                     const char *key_str,
                                     const app_settings_entry_t *entry);

/**************************************************************************//**
 * @brief Get IP address from app settings.
 * @details Get the IP address string.
 * @param[out] value_str Value string destination
 * @param[in] key_str Key string
 * @param[in] entry App settings entry
 * @return sl_status_t SL_STATUS_OK on success, SL_STATUS_FAIL on error
 *****************************************************************************/
sl_status_t app_settings_get_ip_address(char *value_str,
                                        const char *key_str,
                                        const app_settings_entry_t *entry);

#endif  // __SL_WISUN_CLI_SETTINGS_H__
