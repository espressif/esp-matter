/***************************************************************************//**
 * @file
 * @brief
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

// -----------------------------------------------------------------------------
//                                   Includes
// -----------------------------------------------------------------------------
#include <stdio.h>
#include <string.h>
#include <assert.h>
#include "sl_component_catalog.h"
#include "sl_cli.h"
#include "sl_wisun_api.h"
#include "sl_wisun_util.h"
#include "sl_wisun_trace_util.h"
#include "sl_wisun_cli_settings.h"
#include "sl_wisun_app_setting.h"
#include "sl_wisun_cli_util.h"
#include "sl_wisun_cli_core.h"
#include "sl_wisun_app_core.h"
#include "sl_wisun_app_core_util_config.h"

// -----------------------------------------------------------------------------
//                              Macros and Typedefs
// -----------------------------------------------------------------------------

/**************************************************************************//**
 * @brief Wisun CLI domain id
 *****************************************************************************/
#define WISUN_CLI_DOMAIN_ID               (0U)

#define APP_CLI_INPUT_FLAG_NONE           APP_SETTINGS_INPUT_FLAG_NONE
#define APP_CLI_INPUT_FLAG_SIGNED         APP_SETTINGS_INPUT_FLAG_SIGNED
#define APP_CLI_INPUT_FLAG_DEFAULT        APP_SETTINGS_INPUT_FLAG_DEFAULT
#define APP_CLI_OUTPUT_FLAG_NONE          APP_SETTINGS_OUTPUT_FLAG_NONE
#define APP_CLI_OUTPUT_FLAG_SIGNED        APP_SETTINGS_OUTPUT_FLAG_SIGNED
#define APP_CLI_OUTPUT_FLAG_HEX           APP_SETTINGS_OUTPUT_FLAG_HEX
#define APP_CLI_OUTPUT_FLAG_FIXEDSIZE     APP_SETTINGS_OUTPUT_FLAG_FIXEDSIZE
#define APP_CLI_OUTPUT_FLAG_DEFAULT       APP_SETTINGS_OUTPUT_FLAG_DEFAULT
#define APP_CLI_VALUE_SIZE_NONE           APP_SETTINGS_VALUE_SIZE_NONE
#define APP_CLI_VALUE_SIZE_UINT8          APP_SETTINGS_VALUE_SIZE_UINT8
#define APP_CLI_VALUE_SIZE_UINT16         APP_SETTINGS_VALUE_SIZE_UINT16
#define APP_CLI_VALUE_SIZE_UINT32         APP_SETTINGS_VALUE_SIZE_UINT32

typedef app_settings_entry_t app_cli_entry_t;

// -----------------------------------------------------------------------------
//                          Static Function Declarations
// -----------------------------------------------------------------------------

/**************************************************************************//**
 * @brief Helper to get connection status for getter
 * @param[out] *value_str is the desired value string
 * @param[in] *key_str is the key string of the set command
 * @param[in] *entry is the settings entry
 * @return SL_STATUS_OK if the setting is success.
 * @return SL_STATUS_FAIL if setting failed.
 *****************************************************************************/
static sl_status_t _app_cli_get_connection(char *value_str,
                                           const char *key_str,
                                           const app_cli_entry_t *entry);

/**************************************************************************//**
 * @brief Helper to set network name for setter
 * @param[out] *value_str is the desired value string
 * @param[in] *key_str is the key string of the set command
 * @param[in] *entry is the settings entry
 * @return SL_STATUS_OK if the setting is success.
 * @return SL_STATUS_FAIL if setting failed.
 *****************************************************************************/
static sl_status_t _app_cli_set_network_name(const char *value_str,
                                             const char *key_str,
                                             const app_cli_entry_t *entry);

/**************************************************************************//**
 * @brief Helper to set network size for setter
 * @param[out] *value_str is the desired value string
 * @param[in] *key_str is the key string of the set command
 * @param[in] *entry is the settings entry
 * @return SL_STATUS_OK if the setting is success.
 * @return SL_STATUS_FAIL if setting failed.
 *****************************************************************************/
static sl_status_t _app_cli_set_network_size(const char *value_str,
                                             const char *key_str,
                                             const app_cli_entry_t *entry);

/**************************************************************************//**
 * @brief Helper to set TX power for getter
 * @param[out] *value_str is the desired value string
 * @param[in] *key_str is the key string of the set command
 * @param[in] *entry is the settings entry
 * @return SL_STATUS_OK if the setting is success.
 * @return SL_STATUS_FAIL if setting failed.
 *****************************************************************************/
static sl_status_t _app_cli_set_tx_power(const char *value_str,
                                         const char *key_str,
                                         const app_cli_entry_t *entry);

/**************************************************************************//**
 * @brief Helper to get network name for getter
 * @param[out] *value_str is the desired value string
 * @param[in] *key_str is the key string of the set command
 * @param[in] *entry is the settings entry
 * @return SL_STATUS_OK if the setting is success.
 * @return SL_STATUS_FAIL if setting failed.
 *****************************************************************************/
static sl_status_t _app_cli_get_network_name(char *value_str,
                                             const char *key_str,
                                             const app_cli_entry_t *entry);

/**************************************************************************//**
 * @brief Helper to get network size for getter
 * @param[out] *value_str is the desired value string
 * @param[in] *key_str is the key string of the set command
 * @param[in] *entry is the settings entry
 * @return SL_STATUS_OK if the setting is success.
 * @return SL_STATUS_FAIL if setting failed.
 *****************************************************************************/
static sl_status_t _app_cli_get_network_size(char *value_str,
                                             const char *key_str,
                                             const app_cli_entry_t *entry);

/**************************************************************************//**
 * @brief Helper to get TX power for getter
 * @param[out] *value_str is the desired value string
 * @param[in] *key_str is the key string of the set command
 * @param[in] *entry is the settings entry
 * @return SL_STATUS_OK if the setting is success.
 * @return SL_STATUS_FAIL if setting failed.
 *****************************************************************************/
static sl_status_t _app_cli_get_tx_power(char *value_str,
                                         const char *key_str,
                                         const app_cli_entry_t *entry);

/**************************************************************************//**
 * @brief Helper to get wisun phy for getter
 * @param[out] *value_str is the desired value string
 * @param[in] *key_str is the key string of the set command
 * @param[in] *entry is the settings entry
 * @return SL_STATUS_OK if the setting is success.
 * @return SL_STATUS_FAIL if setting failed.
 *****************************************************************************/
static sl_status_t _app_cli_get_phy(char *value_str,
                                    const char *key_str,
                                    const app_cli_entry_t *entry);

#if defined(SL_CATALOG_WISUN_MODE_SWITCH_PRESENT)
/**************************************************************************//**
 * @brief Helper to get wisun mode switch counters for getter
 * @param[out] *value_str is the desired value string
 * @param[in] *key_str is the key string of the set command
 * @param[in] *entry is the settings entry
 * @return SL_STATUS_OK if the setting is success.
 * @return SL_STATUS_FAIL if setting failed.
 *****************************************************************************/
static sl_status_t _app_ms_get_counters(char *value_str,
                                        const char *key_str,
                                        const app_cli_entry_t *entry);
#endif

#if defined(SL_CATALOG_WISUN_FULL_RADIOCONF_PRESENT)
/**************************************************************************//**
 * @brief Helper to set TX power for getter
 * @param[out] *value_str is the desired value string
 * @param[in] *key_str is the key string of the set command
 * @param[in] *entry is the settings entry
 * @return SL_STATUS_OK if the setting is success.
 * @return SL_STATUS_FAIL if setting failed.
 *****************************************************************************/
static sl_status_t _app_cli_set_phy(const char *value_str,
                                    const char *key_str,
                                    const app_settings_entry_t *entry);
#endif

/**************************************************************************//**
 * @brief Helper to set regulation for setter
 * @param[out] *value_str is the desired value string
 * @param[in] *key_str is the key string of the set command
 * @param[in] *entry is the settings entry
 * @return SL_STATUS_OK if the setting is success.
 * @return SL_STATUS_FAIL if setting failed.
 *****************************************************************************/
static sl_status_t _app_set_regulation(const char *value_str,
                                       const char *key_str,
                                       const app_settings_entry_t *entry);

/**************************************************************************//**
 * @brief Helper to set regulation warning threshold for setter
 * @param[out] *value_str is the desired value string
 * @param[in] *key_str is the key string of the set command
 * @param[in] *entry is the settings entry
 * @return SL_STATUS_OK if the setting is success.
 * @return SL_STATUS_FAIL if setting failed.
 *****************************************************************************/
static sl_status_t _app_set_regulation_warning_threshold(const char *value_str,
                                                         const char *key_str,
                                                         const app_settings_entry_t *entry);

/**************************************************************************//**
 * @brief Helper to set regulation alert threshold for setter
 * @param[out] *value_str is the desired value string
 * @param[in] *key_str is the key string of the set command
 * @param[in] *entry is the settings entry
 * @return SL_STATUS_OK if the setting is success.
 * @return SL_STATUS_FAIL if setting failed.
 *****************************************************************************/
static sl_status_t _app_set_regulation_alert_threshold(const char *value_str,
                                                       const char *key_str,
                                                       const app_settings_entry_t *entry);

/**************************************************************************//**
 * @brief Helper to get regulation for getter
 * @param[out] *value_str is the desired value string
 * @param[in] *key_str is the key string of the set command
 * @param[in] *entry is the settings entry
 * @return SL_STATUS_OK if the setting is success.
 * @return SL_STATUS_FAIL if setting failed.
 *****************************************************************************/
static sl_status_t _app_get_regulation(char *value_str,
                                       const char *key_str,
                                       const app_cli_entry_t *entry);

/**************************************************************************//**
 * @brief Helper to get regulation warning threshold for setter
 * @param[out] *value_str is the desired value string
 * @param[in] *key_str is the key string of the set command
 * @param[in] *entry is the settings entry
 * @return SL_STATUS_OK if the setting is success.
 * @return SL_STATUS_FAIL if setting failed.
 *****************************************************************************/
static sl_status_t _app_get_regulation_warning_threshold(char *value_str,
                                                         const char *key_str,
                                                         const app_cli_entry_t *entry);

/**************************************************************************//**
 * @brief Helper to get regulation alert threshold for setter
 * @param[out] *value_str is the desired value string
 * @param[in] *key_str is the key string of the set command
 * @param[in] *entry is the settings entry
 * @return SL_STATUS_OK if the setting is success.
 * @return SL_STATUS_FAIL if setting failed.
 *****************************************************************************/
static sl_status_t _app_get_regulation_alert_threshold(char *value_str,
                                                       const char *key_str,
                                                       const app_cli_entry_t *entry);

// -----------------------------------------------------------------------------
//                                Static Variables
// -----------------------------------------------------------------------------

/**************************************************************************//**
 * @brief Wi-SUN application regulation
 *****************************************************************************/
static sl_wisun_regulation_t app_regulation = (sl_wisun_regulation_t)WISUN_APP_REGULATION;

// -----------------------------------------------------------------------------
//                                Global Variables
// -----------------------------------------------------------------------------

/**************************************************************************//**
 * @brief Wi-SUN application settings domain string
 *****************************************************************************/
const char *app_settings_domain_str[] = {
  "wisun",
  NULL,
};

/**************************************************************************//**
 * @brief Wi-SUN application settings entry/entries
 *****************************************************************************/
const app_cli_entry_t app_settings_entries[] =
{
  {
    .key = "network_name",
    .domain = WISUN_CLI_DOMAIN_ID,
    .value_size = SL_WISUN_NETWORK_NAME_SIZE + 1,
    .input = APP_CLI_INPUT_FLAG_DEFAULT,
    .output = APP_CLI_OUTPUT_FLAG_DEFAULT,
    .value = NULL,
    .input_enum_list = NULL,
    .output_enum_list = NULL,
    .set_handler = _app_cli_set_network_name,
    .get_handler = _app_cli_get_network_name,
    .description = NULL
  },
  {
    .key = "network_size",
    .domain = WISUN_CLI_DOMAIN_ID,
    .value_size = APP_CLI_VALUE_SIZE_UINT8,
    .input = APP_CLI_INPUT_FLAG_DEFAULT,
    .output = APP_CLI_OUTPUT_FLAG_DEFAULT,
    .value = NULL,
    .input_enum_list = app_wisun_nw_size_enum,
    .output_enum_list = app_wisun_nw_size_enum,
    .set_handler = _app_cli_set_network_size,
    .get_handler = _app_cli_get_network_size,
    .description = NULL
  },
  {
    .key = "tx_power",
    .domain = WISUN_CLI_DOMAIN_ID,
    .value_size = APP_CLI_VALUE_SIZE_UINT8,
    .input = APP_CLI_INPUT_FLAG_DEFAULT | APP_CLI_INPUT_FLAG_SIGNED,
    .output = APP_CLI_OUTPUT_FLAG_DEFAULT | APP_CLI_OUTPUT_FLAG_SIGNED,
    .value = NULL,
    .input_enum_list = NULL,
    .output_enum_list = NULL,
    .set_handler = _app_cli_set_tx_power,
    .get_handler = _app_cli_get_tx_power,
    .description = NULL
  },
  {
    .key = "regulatory_domain",
    .domain = WISUN_CLI_DOMAIN_ID,
    .value_size = APP_CLI_VALUE_SIZE_UINT8,
    .input = APP_CLI_INPUT_FLAG_DEFAULT,
    .output = APP_CLI_OUTPUT_FLAG_DEFAULT,
    .value = NULL,
    .input_enum_list = app_wisun_phy_reg_domain_enum,
    .output_enum_list = app_wisun_phy_reg_domain_enum,
#if defined(SL_CATALOG_WISUN_FULL_RADIOCONF_PRESENT)
    .set_handler = _app_cli_set_phy,
#else
    .set_handler = NULL,
#endif
    .get_handler = _app_cli_get_phy
  },
  {
    .key = "operating_class",
    .domain = WISUN_CLI_DOMAIN_ID,
    .value_size = APP_CLI_VALUE_SIZE_UINT8,
    .input = APP_CLI_INPUT_FLAG_DEFAULT,
    .output = APP_CLI_OUTPUT_FLAG_DEFAULT,
    .value = NULL,
    .input_enum_list = NULL,
    .output_enum_list = NULL,
#if defined(SL_CATALOG_WISUN_FULL_RADIOCONF_PRESENT)
    .set_handler = _app_cli_set_phy,
#else
    .set_handler = NULL,
#endif
    .get_handler = _app_cli_get_phy
  },
  {
    .key = "operating_mode",
    .domain = WISUN_CLI_DOMAIN_ID,
    .value_size = APP_CLI_VALUE_SIZE_UINT8,
    .input = APP_CLI_INPUT_FLAG_DEFAULT,
    .output = APP_CLI_OUTPUT_FLAG_DEFAULT,
    .value = NULL,
    .input_enum_list = NULL,
    .output_enum_list = NULL,
#if defined(SL_CATALOG_WISUN_FULL_RADIOCONF_PRESENT)
    .set_handler = _app_cli_set_phy,
#else
    .set_handler = NULL,
#endif
    .get_handler = _app_cli_get_phy
  },
  {
    .key = "join_state",
    .domain = WISUN_CLI_DOMAIN_ID,
    .value_size = APP_CLI_VALUE_SIZE_UINT8,
    .input = APP_CLI_INPUT_FLAG_DEFAULT,
    .output = APP_CLI_OUTPUT_FLAG_DEFAULT,
    .value = NULL,
    .input_enum_list = app_wisun_conn_status_enum,
    .output_enum_list = app_wisun_conn_status_enum,
    .set_handler = NULL,
    .get_handler = _app_cli_get_connection,
    .description = NULL
  },
  {
    .key = "ip_address_global",
    .domain = WISUN_CLI_DOMAIN_ID,
    .value_size = APP_CLI_VALUE_SIZE_NONE,
    .input = APP_CLI_INPUT_FLAG_DEFAULT,
    .output = APP_CLI_OUTPUT_FLAG_DEFAULT,
    .value = NULL,
    .input_enum_list = NULL,
    .output_enum_list = NULL,
    .set_handler = NULL,
    .get_handler = app_settings_get_ip_address,
    .description = NULL
  },
  {
    .key = "ip_address_link_local",
    .domain = WISUN_CLI_DOMAIN_ID,
    .value_size = APP_CLI_VALUE_SIZE_NONE,
    .input = APP_CLI_INPUT_FLAG_DEFAULT,
    .output = APP_CLI_OUTPUT_FLAG_DEFAULT,
    .value = NULL,
    .input_enum_list = NULL,
    .output_enum_list = NULL,
    .set_handler = NULL,
    .get_handler = app_settings_get_ip_address,
    .description = NULL
  },
  {
    .key = "ip_address_border_router",
    .domain = WISUN_CLI_DOMAIN_ID,
    .value_size = APP_CLI_VALUE_SIZE_NONE,
    .input = APP_CLI_INPUT_FLAG_DEFAULT,
    .output = APP_CLI_OUTPUT_FLAG_DEFAULT,
    .value = NULL,
    .input_enum_list = NULL,
    .output_enum_list = NULL,
    .set_handler = NULL,
    .get_handler = app_settings_get_ip_address,
    .description = NULL
  },
  {
    .key = "ip_address_primary_parent",
    .domain = WISUN_CLI_DOMAIN_ID,
    .value_size = APP_CLI_VALUE_SIZE_NONE,
    .input = APP_CLI_INPUT_FLAG_DEFAULT,
    .output = APP_CLI_OUTPUT_FLAG_DEFAULT,
    .value = NULL,
    .input_enum_list = NULL,
    .output_enum_list = NULL,
    .set_handler = NULL,
    .get_handler = app_settings_get_ip_address,
    .description = NULL
  },
#if defined(SL_CATALOG_WISUN_MODE_SWITCH_PRESENT)
  {
    .key = "mode_switch_tx_counter",
    .domain = WISUN_CLI_DOMAIN_ID,
    .value_size = APP_CLI_VALUE_SIZE_UINT32,
    .input = APP_CLI_INPUT_FLAG_DEFAULT,
    .output = APP_CLI_OUTPUT_FLAG_DEFAULT,
    .value = NULL,
    .input_enum_list = NULL,
    .output_enum_list = NULL,
    .set_handler = NULL,
    .get_handler = _app_ms_get_counters,
    .description = NULL
  },
  {
    .key = "mode_switch_tx_failed_counter",
    .domain = WISUN_CLI_DOMAIN_ID,
    .value_size = APP_CLI_VALUE_SIZE_UINT32,
    .input = APP_CLI_INPUT_FLAG_DEFAULT,
    .output = APP_CLI_OUTPUT_FLAG_DEFAULT,
    .value = NULL,
    .input_enum_list = NULL,
    .output_enum_list = NULL,
    .set_handler = NULL,
    .get_handler = _app_ms_get_counters,
    .description = NULL
  },
  {
    .key = "mode_switch_rx_counter",
    .domain = WISUN_CLI_DOMAIN_ID,
    .value_size = APP_CLI_VALUE_SIZE_UINT32,
    .input = APP_CLI_INPUT_FLAG_DEFAULT,
    .output = APP_CLI_OUTPUT_FLAG_DEFAULT,
    .value = NULL,
    .input_enum_list = NULL,
    .output_enum_list = NULL,
    .set_handler = NULL,
    .get_handler = _app_ms_get_counters,
    .description = NULL
  },
  {
    .key = "mode_switch_rx_failed_counter",
    .domain = WISUN_CLI_DOMAIN_ID,
    .value_size = APP_CLI_VALUE_SIZE_UINT32,
    .input = APP_CLI_INPUT_FLAG_DEFAULT,
    .output = APP_CLI_OUTPUT_FLAG_DEFAULT,
    .value = NULL,
    .input_enum_list = NULL,
    .output_enum_list = NULL,
    .set_handler = NULL,
    .get_handler = _app_ms_get_counters,
    .description = NULL
  },
#endif
  {
    .key = "regulation",
    .domain = WISUN_CLI_DOMAIN_ID,
    .value_size = APP_CLI_VALUE_SIZE_UINT8,
    .input = APP_CLI_INPUT_FLAG_DEFAULT,
    .output = APP_CLI_OUTPUT_FLAG_DEFAULT,
    .value = NULL,
    .input_enum_list = app_regulation_enum,
    .output_enum_list = app_regulation_enum,
    .set_handler = _app_set_regulation,
    .get_handler = _app_get_regulation,
    .description = "Regional regulation [uint8]"
  },
  {
    .key = "regulation_warning_threshold",
    .domain = WISUN_CLI_DOMAIN_ID,
    .value_size = APP_CLI_VALUE_SIZE_UINT8,
    .input = APP_CLI_INPUT_FLAG_SIGNED,
    .output = APP_CLI_OUTPUT_FLAG_SIGNED,
    .value = NULL,
    .input_enum_list = NULL,
    .output_enum_list = NULL,
    .set_handler = _app_set_regulation_warning_threshold,
    .get_handler = _app_get_regulation_warning_threshold,
    .description = "Transmission warning threshold in percent (-1 to disable) [int8]"
  },
  {
    .key = "regulation_alert_threshold",
    .domain = WISUN_CLI_DOMAIN_ID,
    .value_size = APP_CLI_VALUE_SIZE_UINT8,
    .input = APP_CLI_INPUT_FLAG_SIGNED,
    .output = APP_CLI_OUTPUT_FLAG_SIGNED,
    .value = NULL,
    .input_enum_list = NULL,
    .output_enum_list = NULL,
    .set_handler = _app_set_regulation_alert_threshold,
    .get_handler = _app_get_regulation_alert_threshold,
    .description = "Transmission alert threshold in percent (-1 to disable) [int8]"
  },
  {
    .key = NULL,
    .domain = 0,
    .value_size = APP_CLI_VALUE_SIZE_NONE,
    .input = APP_CLI_INPUT_FLAG_DEFAULT,
    .output = APP_CLI_OUTPUT_FLAG_DEFAULT,
    .value = NULL,
    .input_enum_list = NULL,
    .output_enum_list = NULL,
    .set_handler = NULL,
    .get_handler = NULL,
    .description = NULL
  }
};

// -----------------------------------------------------------------------------
//                          Public Function Definitions
// -----------------------------------------------------------------------------

void app_about(void)
{
  app_wisun_project_info_print(false);
}

/* Weak function for app_wisun_network_connect() if the app_core is not added. */
SL_WEAK void app_wisun_network_connect(void)
{
  assert("app_wisun_network_connect() is not implemented" == NULL);
}

/* CLI app connect handler */
void app_connect(sl_cli_command_arg_t *arguments)
{
  (void) arguments;
  app_wisun_cli_mutex_lock();
  app_wisun_network_connect();
  app_wisun_cli_mutex_unlock();
}

/* CLI app disconnect handler */
void app_disconnect(sl_cli_command_arg_t *arguments)
{
  sl_status_t ret;
  sl_wisun_join_state_t join_state = SL_WISUN_JOIN_STATE_DISCONNECTED;
  (void) arguments;

  app_wisun_cli_mutex_lock();

  ret = sl_wisun_get_join_state(&join_state);
  if (ret != SL_STATUS_OK) {
    printf("[Failed: Getting join state failed]\r\n");
    app_wisun_cli_mutex_unlock();
    return;
  }
  if (join_state == SL_WISUN_JOIN_STATE_DISCONNECTED) {
    printf("[Failed: already disconnected]\r\n");
    app_wisun_cli_mutex_unlock();
    return;
  }

  ret = sl_wisun_disconnect();
  if (ret == SL_STATUS_OK) {
    printf("[Disconnecting]\r\n");
  } else {
    printf("[Disconnection failed: %lu]\r\n", ret);
  }

  app_wisun_cli_mutex_unlock();
}

// -----------------------------------------------------------------------------
//                          Static Function Definitions
// -----------------------------------------------------------------------------

/* App CLI getting connection state (join state) */
static sl_status_t _app_cli_get_connection(char *value_str,
                                           const char *key_str,
                                           const app_cli_entry_t *entry)
{
  sl_status_t res = SL_STATUS_FAIL;
  uint32_t value;
  const app_enum_t* value_enum;
  (void)key_str;
  sl_wisun_join_state_t join_state = SL_WISUN_JOIN_STATE_DISCONNECTED;

  if ((value_str == NULL) || (entry == NULL)) {
    return SL_STATUS_FAIL;
  }

  // gets the connection state from the core
  res = sl_wisun_get_join_state(&join_state);
  if (res != SL_STATUS_OK) {
    return res;
  }
  value = (uint32_t)join_state;

  // finds the proper string for the value
  value_enum = entry->output_enum_list;

  if (value_enum->value_str != NULL) {
    while (value_enum) {
      if (value_enum->value == value) {
        // Matching enumeration found
        break;
      }
      value_enum++;
    }
  } else {
    return SL_STATUS_FAIL;
  }

  // creates the value string
  sprintf(value_str, "%s (%d)", value_enum->value_str, (uint8_t)value_enum->value);

  return SL_STATUS_OK;
}

/* App CLI setting network name */
static sl_status_t _app_cli_set_network_name(const char *value_str,
                                             const char *key_str,
                                             const app_cli_entry_t *entry)
{
  (void)key_str;
  (void)entry;

  return app_wisun_setting_set_network_name(value_str);
}

/* App CLI setting network size */
static sl_status_t _app_cli_set_network_size(const char *value_str,
                                             const char *key_str,
                                             const app_cli_entry_t *entry)
{
  (void)key_str;
  (void)entry;
  sl_status_t res = SL_STATUS_FAIL;
  uint32_t value = 0U;

  res = app_util_get_integer(&value,
                             value_str,
                             entry->input_enum_list,
                             entry->input & APP_CLI_INPUT_FLAG_SIGNED);
  if (res == SL_STATUS_OK) {
    // sets the network name
    res = app_wisun_setting_set_network_size((uint8_t*)&value);
  }

  return res;
}

/* App CLI setting tx power */
static sl_status_t _app_cli_set_tx_power(const char *value_str,
                                         const char *key_str,
                                         const app_cli_entry_t *entry)
{
  (void)key_str;
  (void)entry;
  sl_status_t res = SL_STATUS_FAIL;
  int32_t value = 0U;

  res = app_util_get_integer((uint32_t *)&value,
                             value_str,
                             entry->input_enum_list,
                             entry->input & APP_CLI_INPUT_FLAG_SIGNED);
  if (res == SL_STATUS_OK) {
    // sets the tx power
    res = app_wisun_setting_set_tx_power((int8_t*)&value);
  }

  return res;
}

/* App CLI setting network name */
static sl_status_t _app_cli_get_network_name(char *value_str,
                                             const char *key_str,
                                             const app_cli_entry_t *entry)
{
  sl_status_t res = SL_STATUS_FAIL;
  (void)key_str;
  (void)entry;
  char name_buffer[MAX_SIZE_OF_NETWORK_NAME] = { 0 };

  // gets the network name
  res = app_wisun_setting_get_network_name(name_buffer, sizeof(name_buffer));

  if (res == SL_STATUS_OK) {
    sprintf(value_str, "%s", name_buffer);
  }

  return res;
}

/* App CLI setting network size */
static sl_status_t _app_cli_get_network_size(char *value_str,
                                             const char *key_str,
                                             const app_cli_entry_t *entry)
{
  sl_status_t res = SL_STATUS_FAIL;
  (void)key_str;
  (void)entry;
  const app_enum_t* value_enum;
  uint8_t value = 0U;

  res = app_wisun_setting_get_network_size(&value);
  if (res == SL_STATUS_OK) {
    // finds the proper string for the value
    value_enum = entry->output_enum_list;

    if (value_enum->value_str != NULL) {
      while (value_enum) {
        if (value_enum->value == value) {
          // Matching enumeration found
          break;
        }
        value_enum++;
      }
    } else {
      return SL_STATUS_FAIL;
    }

    // creates the value string
    sprintf(value_str, "%s (%d)", value_enum->value_str, (uint8_t)value_enum->value);
  }

  return res;
}

/* App CLI setting tx power */
static sl_status_t _app_cli_get_tx_power(char *value_str,
                                         const char *key_str,
                                         const app_cli_entry_t *entry)
{
  sl_status_t res = SL_STATUS_FAIL;
  (void)key_str;
  (void)entry;
  int8_t value = 0U;

  res = app_wisun_setting_get_tx_power(&value);
  if (res == SL_STATUS_OK) {
    sprintf(value_str, "%d", value);
  } else {
    sprintf(value_str, "N/A");
  }

  return res;
}

/* App CLI getting PHY parameters */
static sl_status_t _app_cli_get_phy(char *value_str,
                                    const char *key_str,
                                    const app_cli_entry_t *entry)
{
  sl_status_t res = SL_STATUS_FAIL;
  const app_enum_t* value_enum;
  (void)key_str;
  app_setting_phy phy;

  if ((value_str == NULL) || (entry == NULL) || (entry->key == NULL)) {
    return res;
  }

  // gets the wisun phy parameters
#if defined(SL_CATALOG_WISUN_FULL_RADIOCONF_PRESENT)
  res = app_wisun_setting_get_phy(&phy);
#else
  res = sl_wisun_util_get_rf_settings(&phy.regulatory_domain, &phy.operating_class,
                                      &phy.operating_mode);
#endif
  if (res != SL_STATUS_OK) {
    return res;
  }

  // regulatory domain
  if (strstr(entry->key, "regulatory_domain")) {
    // finds the proper string for the value for regulatory domain
    value_enum = entry->output_enum_list;

    if (value_enum->value_str != NULL) {
      while (value_enum) {
        if (value_enum->value == phy.regulatory_domain) {
          // Matching enumeration found
          break;
        }
        value_enum++;
      }
    } else {
      return SL_STATUS_FAIL;
    }
    sprintf(value_str, "%s (%d)", value_enum->value_str, (uint8_t)value_enum->value);
    // operating class
  } else if (strstr(entry->key, "operating_class")) {
    sprintf(value_str, "%d", phy.operating_class);
    // operating mode
  } else if (strstr(entry->key, "operating_mode")) {
    sprintf(value_str, "0x%x", phy.operating_mode);
  }

  return SL_STATUS_OK;
}

#if defined(SL_CATALOG_WISUN_FULL_RADIOCONF_PRESENT)
/* App CLI getting PHY parameters */
sl_status_t _app_cli_set_phy(const char *value_str,
                             const char *key_str,
                             const app_settings_entry_t *entry)
{
  (void)key_str;
  sl_status_t res = SL_STATUS_FAIL;
  int32_t value = 0U;
  const app_enum_t* value_enum;
  app_setting_phy phy;

  if ((value_str == NULL) || (entry == NULL) || (entry->key == NULL)) {
    return res;
  }

  app_wisun_setting_get_phy(&phy);

  res = app_util_get_integer((uint32_t *)&value,
                             value_str,
                             entry->input_enum_list,
                             entry->input & APP_SETTINGS_INPUT_FLAG_SIGNED);
  if (res != SL_STATUS_OK) {
    return res;
  }

  if (strstr(entry->key, "regulatory_domain")) {
    value_enum = entry->output_enum_list;
    value_enum += value;
    phy.regulatory_domain = value;
    app_wisun_setting_set_phy(&phy);
  } else if (strstr(entry->key, "operating_class")) {
    phy.operating_class = value;
    app_wisun_setting_set_phy(&phy);
  } else if (strstr(entry->key, "operating_mode")) {
    phy.operating_mode = value;
    app_wisun_setting_set_phy(&phy);
  } else {
  }
  return res;
}
#endif

#if defined(SL_CATALOG_WISUN_MODE_SWITCH_PRESENT)
/* App CLI getting mode switch counters */
static sl_status_t _app_ms_get_counters(char *value_str,
                                        const char *key_str,
                                        const app_cli_entry_t *entry)
{
  sl_status_t res = SL_STATUS_FAIL;
  (void)key_str;
  sl_wisun_statistics_t stat;

  if ((value_str == NULL) || (entry == NULL) || (entry->key == NULL)) {
    return res;
  }

  // gets the statsitic that contains the mode switch information
  res = sl_wisun_get_statistics(SL_WISUN_STATISTICS_TYPE_MAC, &stat);

  if (res != SL_STATUS_OK) {
    return res;
  }

  // mode switch TX-RX counters (inc. failed)
  if (strstr(entry->key, "mode_switch_tx_counter")) {
    sprintf(value_str, "%lu", stat.mac.tx_ms_count);
  } else if (strstr(entry->key, "mode_switch_tx_failed_counter")) {
    sprintf(value_str, "%lu", stat.mac.tx_ms_failed_count);
  } else if (strstr(entry->key, "mode_switch_rx_counter")) {
    sprintf(value_str, "%lu", stat.mac.rx_ms_count);
  } else if (strstr(entry->key, "mode_switch_rx_failed_counter")) {
    sprintf(value_str, "%lu", stat.mac.rx_ms_failed_count);
  } else {
  }

  return SL_STATUS_OK;
}
#endif

static sl_status_t _app_set_regulation(const char *value_str,
                                       const char *key_str,
                                       const app_settings_entry_t *entry)
{
  (void)key_str;
  (void)entry;
  sl_status_t res = SL_STATUS_FAIL;
  uint32_t value = 0U;
  regulation_thresholds_t thresholds;
  sl_wisun_join_state_t join_state = SL_WISUN_JOIN_STATE_DISCONNECTED;

  if ((value_str == NULL) || (entry == NULL) || (entry->key == NULL)) {
    return SL_STATUS_FAIL;
  }

  // checking if the device connected or not
  res = sl_wisun_get_join_state(&join_state);
  if (res != SL_STATUS_OK) {
    return res;
  } else {
    if (join_state != SL_WISUN_JOIN_STATE_DISCONNECTED) {
      printf("[Regulation related settings unavailable, disconnection is needed]\r\n");
      return SL_STATUS_FAIL;
    }
  }

  res = app_util_get_integer(&value,
                             value_str,
                             entry->input_enum_list,
                             entry->input & APP_CLI_INPUT_FLAG_SIGNED);

  if (res != SL_STATUS_OK) {
    printf("[Failed: Get value error: %lu]\r\n", res);
    return res;
  }

  if (strstr(entry->key, "regulation")) {
    // sets the thresholds
    (void)app_wisun_get_regulation_thresholds(&thresholds);
    res = sl_wisun_set_regulation_tx_thresholds(thresholds.warning_threshold,
                                                thresholds.alert_threshold);
    if (res != SL_STATUS_OK) {
      printf("[Failed: unable to set regulation TX thresholds: %lu]\r\n", res);
      return res;
    }

    // sets regulation
    res = sl_wisun_set_regulation((sl_wisun_regulation_t)value);
    if (res != SL_STATUS_OK) {
      printf("[Regulation not valid]\n");
      return res;
    } else {
      app_regulation = (sl_wisun_regulation_t)value;
    }

    // sets status of regulation
    if ((sl_wisun_regulation_t)value == SL_WISUN_REGULATION_NONE) {
      app_wisun_set_regulation_active(false);
    } else {
      app_wisun_set_regulation_active(true);
    }
  }

  return SL_STATUS_OK;
}

static sl_status_t _app_set_regulation_warning_threshold(const char *value_str,
                                                         const char *key_str,
                                                         const app_settings_entry_t *entry)
{
  (void)key_str;
  (void)entry;
  sl_status_t res = SL_STATUS_FAIL;
  uint32_t value = 0U;
  regulation_thresholds_t thresholds;
  sl_wisun_join_state_t join_state = SL_WISUN_JOIN_STATE_DISCONNECTED;

  if ((value_str == NULL) || (entry == NULL) || (entry->key == NULL)) {
    return SL_STATUS_FAIL;
  }

  // checking if the device connected or not
  res = sl_wisun_get_join_state(&join_state);
  if (res != SL_STATUS_OK) {
    return res;
  } else {
    if (join_state != SL_WISUN_JOIN_STATE_DISCONNECTED) {
      printf("[Regulation related settings unavailable, disconnection is needed]\r\n");
      return SL_STATUS_FAIL;
    }
  }

  res = app_util_get_integer(&value,
                             value_str,
                             entry->input_enum_list,
                             entry->input & APP_CLI_INPUT_FLAG_SIGNED);

  if (res != SL_STATUS_OK) {
    printf("[Failed: Get value error: %lu]\r\n", res);
    return res;
  }

  if (strstr(entry->key, "regulation_warning_threshold")) {
    (void)app_wisun_get_regulation_thresholds(&thresholds);
    res = sl_wisun_set_regulation_tx_thresholds((int8_t)value, thresholds.alert_threshold);
    if (res != SL_STATUS_OK) {
      printf("[Failed: unable to set regulation TX warning threshold: %lu]\r\n", res);
      return res;
    } else {
      app_wisun_set_regulation_thresholds((int8_t)value, thresholds.alert_threshold);
    }
  }

  return SL_STATUS_OK;
}

static sl_status_t _app_set_regulation_alert_threshold(const char *value_str,
                                                       const char *key_str,
                                                       const app_settings_entry_t *entry)
{
  (void)key_str;
  (void)entry;
  sl_status_t res = SL_STATUS_FAIL;
  uint32_t value = 0U;
  regulation_thresholds_t thresholds;
  sl_wisun_join_state_t join_state = SL_WISUN_JOIN_STATE_DISCONNECTED;

  if ((value_str == NULL) || (entry == NULL) || (entry->key == NULL)) {
    return SL_STATUS_FAIL;
  }

  // checking if the device connected or not
  res = sl_wisun_get_join_state(&join_state);
  if (res != SL_STATUS_OK) {
    return res;
  } else {
    if (join_state != SL_WISUN_JOIN_STATE_DISCONNECTED) {
      printf("[Regulation related settings unavailable, disconnection is needed]\r\n");
      return SL_STATUS_FAIL;
    }
  }

  res = app_util_get_integer(&value,
                             value_str,
                             entry->input_enum_list,
                             entry->input & APP_CLI_INPUT_FLAG_SIGNED);

  if (res != SL_STATUS_OK) {
    printf("[Failed: Get value error: %lu]\r\n", res);
    return res;
  }

  if (strstr(entry->key, "regulation_alert_threshold")) {
    (void)app_wisun_get_regulation_thresholds(&thresholds);
    res = sl_wisun_set_regulation_tx_thresholds(thresholds.warning_threshold, (int8_t)value);
    if (res != SL_STATUS_OK) {
      printf("[Failed: unable to set regulation TX alert threshold: %lu]\r\n", res);
      return res;
    } else {
      app_wisun_set_regulation_thresholds(thresholds.warning_threshold, (int8_t)value);
    }
  }

  return SL_STATUS_OK;
}

static sl_status_t _app_get_regulation(char *value_str,
                                       const char *key_str,
                                       const app_cli_entry_t *entry)
{
  sl_status_t res = SL_STATUS_FAIL;
  const app_enum_t* value_enum;
  (void)key_str;

  if ((value_str == NULL) || (entry == NULL) || (entry->key == NULL)) {
    return res;
  }

  if (strstr(entry->key, "regulation")) {
    // finds the proper string for the value for regulation
    value_enum = entry->output_enum_list;

    if (value_enum->value_str != NULL) {
      while (value_enum) {
        if (value_enum->value == app_regulation) {
          // Matching enumeration found
          break;
        }
        value_enum++;
      }
    } else {
      return SL_STATUS_FAIL;
    }
    sprintf(value_str, "%s (%d)", value_enum->value_str, (uint8_t)value_enum->value);
  }

  return SL_STATUS_OK;
}

static sl_status_t _app_get_regulation_warning_threshold(char *value_str,
                                                         const char *key_str,
                                                         const app_cli_entry_t *entry)
{
  sl_status_t res = SL_STATUS_FAIL;
  (void)key_str;
  regulation_thresholds_t thresholds;

  if ((value_str == NULL) || (entry == NULL) || (entry->key == NULL)) {
    return res;
  }

  if (strstr(entry->key, "regulation_warning_threshold")) {
    (void)app_wisun_get_regulation_thresholds(&thresholds);
    sprintf(value_str, "%d", thresholds.warning_threshold);
  }

  return SL_STATUS_OK;
}

static sl_status_t _app_get_regulation_alert_threshold(char *value_str,
                                                       const char *key_str,
                                                       const app_cli_entry_t *entry)
{
  sl_status_t res = SL_STATUS_FAIL;
  (void)key_str;
  regulation_thresholds_t thresholds;

  if ((value_str == NULL) || (entry == NULL) || (entry->key == NULL)) {
    return res;
  }

  if (strstr(entry->key, "regulation_alert_threshold")) {
    (void)app_wisun_get_regulation_thresholds(&thresholds);
    sprintf(value_str, "%d", thresholds.alert_threshold);
  }

  return SL_STATUS_OK;
}
