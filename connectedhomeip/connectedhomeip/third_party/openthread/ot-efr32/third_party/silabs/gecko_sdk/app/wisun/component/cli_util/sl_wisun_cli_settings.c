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

// -----------------------------------------------------------------------------
//                                   Includes
// -----------------------------------------------------------------------------

#include "sl_wisun_cli_settings.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "sl_wisun_ip6string.h"
#include "nvm3.h"

// -----------------------------------------------------------------------------
//                              Macros and Typedefs
// -----------------------------------------------------------------------------

/**************************************************************************//**
 * @brief  NVM key base address define
 *****************************************************************************/
#define APP_SETTINGS_NVM_KEY_BASE    (0x00000000U)

/**************************************************************************//**
 * @brief App settings None value string
 *****************************************************************************/
#define APP_SETTINGS_NONE_VALUE_STR  "None"

// -----------------------------------------------------------------------------
//                          Static Function Declarations
// -----------------------------------------------------------------------------

/**************************************************************************//**
 * @brief Load settings from NVM
 * @details Copy settings from the nvm3 memory to the destination
 * @param[in] settings_domain settings domain
 * @param[in,out] settings destination ptr of settings
 * @param[in] settings_size size of the settings
 * @return sl_status_t SL_STATUS_OK on succes, SL_STATUS_FAIL on error
 *****************************************************************************/
static sl_status_t app_settings_nvm_load(uint8_t settings_domain,
                                         void *const settings,
                                         size_t settings_size);

/**************************************************************************//**
* @brief Save settings to NVM
* @details Save the settings into nvm3 memory
* @param[in] settings_domain settings domain
* @param[in] settings source settings
* @param[in] settings_size size of the settings
* @return sl_status_t SL_STATUS_OK on succes, SL_STATUS_FAIL on error
******************************************************************************/
static sl_status_t app_settings_nvm_save(uint8_t settings_domain,
                                         const void *const settings,
                                         size_t settings_size);

/**************************************************************************//**
 * @brief Delete settings from NVM
 * @details Remove settings from nvm3 memory
 * @param[in] settings_domain settings domain
 *****************************************************************************/
static void app_settings_nvm_delete(uint8_t settings_domain);

/**************************************************************************//**
 * @brief App help print and pad
 * @details Print help informations on 2 columns
 * @param permission Command permission
 * @param entry App settings entry
 *****************************************************************************/
static void app_help_print_and_pad(const char * permission, const app_settings_entry_t *entry);

// -----------------------------------------------------------------------------
//                                Global Variables
// -----------------------------------------------------------------------------

/**************************************************************************//**
 * @brief App settings domain string array (weak implementation)
 *****************************************************************************/
SL_WEAK const char *app_settings_domain_str[] = { "" };

/**************************************************************************//**
 * @brief App settings entries array (weak implementation)
 *****************************************************************************/
SL_WEAK const app_settings_entry_t app_settings_entries[] = { 0 };

/**************************************************************************//**
 * @brief Saveing settings array (weak implementation)
 *****************************************************************************/
SL_WEAK const app_saving_item_t *saving_settings[] = { NULL };

// -----------------------------------------------------------------------------
//                                Static Variables
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------
//                          Public Function Definitions
// -----------------------------------------------------------------------------

/* App settings init */
void app_settings_init(void)
{
  uint8_t index = 0;
  sl_status_t ret = SL_STATUS_OK;

  while (saving_settings[index]) {
    ret = app_settings_nvm_load(index,
                                saving_settings[index]->data,
                                saving_settings[index]->data_size);
    if (ret != SL_STATUS_OK) {
      if (saving_settings[index]->default_val) {
        memcpy(saving_settings[index]->data, saving_settings[index]->default_val, saving_settings[index]->data_size);
      }
    }
    index++;
  }
}

/* App setinngs save */
sl_status_t app_settings_save(void)
{
  uint8_t index = 0;
  sl_status_t ret = SL_STATUS_OK;

  while (saving_settings[index]) {
    ret = app_settings_nvm_save(index,
                                saving_settings[index]->data,
                                saving_settings[index]->data_size);
    if (ret != SL_STATUS_OK) {
      break;
    }
    index++;
  }

  return ret;
}

/* App settings reset */
void app_settings_reset(void)
{
  uint8_t index = 0;

  while (saving_settings[index]) {
    app_settings_nvm_delete(index);
    if (saving_settings[index]->default_val) {
      memcpy(saving_settings[index]->data, saving_settings[index]->default_val, saving_settings[index]->data_size);
    }
    index++;
  }
}

/* App settings set */
sl_status_t app_settings_set(char *const domain_and_key, const char *const value_str)
{
  const app_settings_entry_t *iter;
  const char *domain = NULL;
  char *key = NULL;
  const char *nested_key = NULL;

  if (!domain_and_key || !value_str) {
    return SL_STATUS_INVALID_KEY;
  }

  key = domain_and_key;
  domain = strtok(key, ".");
  key = strtok(NULL, ".");
  nested_key = strtok(NULL, ".");

  if (!domain || !key) {
    return SL_STATUS_INVALID_KEY;
  }

  iter = app_settings_entries;
  while (iter->key) {
    if (!strcmp(domain, app_settings_domain_str[iter->domain])) {
      if (!strcmp(iter->key, key)) {
        if (iter->set_handler) {
          printf("%s.%s = %s\r\n", app_settings_domain_str[iter->domain], iter->key, value_str);
          return iter->set_handler(value_str, nested_key, iter);
        } else {
          return SL_STATUS_PERMISSION;
        }
      }
    }
    iter++;
  }

  return SL_STATUS_INVALID_KEY;
}

/* App settings get */
sl_status_t app_settings_get(char *const domain_and_key)
{
  sl_status_t ret;
  const app_settings_entry_t *iter;
  const char *domain = NULL;
  char *key = NULL;
  const char *nested_key = NULL;
  char value_str[128];

  key = domain_and_key;
  domain = strtok(key, ".");
  key = strtok(NULL, ".");
  nested_key = strtok(NULL, ".");

  iter = app_settings_entries;
  while (iter->key) {
    if (!domain || !strcmp(domain, app_settings_domain_str[iter->domain])) {
      if (!key || !strcmp(iter->key, key)) {
        if (iter->get_handler) {
          ret = iter->get_handler(value_str, nested_key, iter);
          if (ret == SL_STATUS_OK) {
            printf("%s.%s = %s\r\n", app_settings_domain_str[iter->domain], iter->key, value_str);
          }
        }
      }
    }
    iter++;
  }

  return SL_STATUS_OK;
}

/* App settings help */
sl_status_t app_settings_help(char *const domain_and_key, bool get)
{
  const app_settings_entry_t *iter;
  const char *domain = NULL;
  char *key = NULL;

  uint8_t permission_write = 0x01;
  uint8_t permission_read = 0x02;

  const char *permission_str[] =
  {
    "[??]",
    "[wo]",
    "[ro]",
    "[rw]"
  };

  key = domain_and_key;
  domain = strtok(key, ".");
  key = strtok(NULL, ".");

  iter = app_settings_entries;

  if (!domain) {
    printf("Help of get and set methods\r\n\r\nAvailable domains :\r\n");
    uint8_t domains_nb = 0;
    while (app_settings_domain_str[domains_nb] != NULL) {
      printf(" %s\r\n", app_settings_domain_str[domains_nb]);
      domains_nb++;
    }
    printf("Type '[get or set] domain help'\r\n");
    printf("\r\neg. 'get %s help' to get the help of %s\r\n    'get %s' for all 'get' commands\r\n",
           app_settings_domain_str[0], app_settings_domain_str[0], app_settings_domain_str[0]);
    printf("\r\nCommands permissions :\r\n %s : Write Only\r\n %s : Read Only\r\n %s : Read and Write\r\n",
           permission_str[1], permission_str[2], permission_str[3]);
  } else if (domain) {
    while (iter->key) {
      if (!domain || !strcmp(domain, app_settings_domain_str[iter->domain])) {
        if (!key || !strcmp(iter->key, key)) {
          uint8_t permission = 0;
          permission |= (iter->get_handler ? permission_read : 0);
          permission |= (iter->set_handler ? permission_write : 0);
          if ((iter->get_handler && get) || (iter->set_handler && !get)) {
            app_help_print_and_pad(permission_str[permission], iter);
          }
        }
      }
      iter++;
    }
  }
  return SL_STATUS_OK;
}

/* App help print and pad */
static void app_help_print_and_pad(const char * permission, const app_settings_entry_t *entry)
{
  printf("%s %s.%s", permission, app_settings_domain_str[entry->domain], entry->key);
  size_t string_length = strlen(permission) + strlen(app_settings_domain_str[entry->domain]) + strlen(entry->key);
  for (; string_length < 60; string_length++) {
    printf(" ");
  }
  printf("%s\r\n", entry->description ? entry->description : " ");
}

/* App settings set string */
sl_status_t app_settings_set_string(const char *value_str,
                                    const char *key_str,
                                    const app_settings_entry_t *entry)
{
  char * entry_value_str;
  (void)key_str;

  entry_value_str = entry->value;
  strncpy(entry_value_str, value_str, entry->value_size - 1);
  entry_value_str[entry->value_size - 1] = '\0';

  return SL_STATUS_OK;
}

/* App settings get string */
sl_status_t app_settings_get_string(char *value_str,
                                    const char *key_str,
                                    const app_settings_entry_t *entry)
{
  (void)key_str;

  sprintf(value_str, "\"%s\"", (const char *)entry->value);

  return SL_STATUS_OK;
}

/* App settings set integer */
sl_status_t app_settings_set_integer(const char *value_str,
                                     const char *key_str,
                                     const app_settings_entry_t *entry)
{
  uint32_t value;
  sl_status_t ret;
  (void)key_str;

  ret = app_util_get_integer(&value,
                             value_str,
                             entry->input_enum_list,
                             entry->input & APP_SETTINGS_INPUT_FLAG_SIGNED);
  if (ret != SL_STATUS_OK) {
    return SL_STATUS_INVALID_TYPE;
  }

  switch (entry->value_size) {
    case APP_SETTINGS_VALUE_SIZE_UINT8:
      *((uint8_t *)entry->value) = (uint8_t)value;
      break;
    case APP_SETTINGS_VALUE_SIZE_UINT16:
      *((uint16_t *)entry->value) = (uint16_t)value;
      break;
    case APP_SETTINGS_VALUE_SIZE_UINT32:
      *((uint32_t *)entry->value) = value;
      break;
    default:
      // Unsupported integer size
      return SL_STATUS_INVALID_TYPE;
  }

  return SL_STATUS_OK;
}

/* App settings get integer */
sl_status_t app_settings_get_integer(char *value_str,
                                     const char *key_str,
                                     const app_settings_entry_t *entry)
{
  uint32_t value;
  uint8_t value_length = 0;
  (void)key_str;

  switch (entry->value_size) {
    case APP_SETTINGS_VALUE_SIZE_UINT8:
      value = *((uint8_t *)entry->value);
      if ((entry->output & APP_SETTINGS_OUTPUT_FLAG_SIGNED)
          && (value & 0x80)) {
        value += 0xFFFFFF00;
      }
      break;
    case APP_SETTINGS_VALUE_SIZE_UINT16:
      value = *((uint16_t *)entry->value);
      if ((entry->output & APP_SETTINGS_OUTPUT_FLAG_SIGNED)
          && (value & 0x8000)) {
        value += 0xFFFF0000;
      }
      break;
    case APP_SETTINGS_VALUE_SIZE_UINT32:
      value = *((uint32_t *)entry->value);
      break;
    default:
      // Unsupported integer size
      return SL_STATUS_INVALID_TYPE;
  }

  if (entry->output & APP_SETTINGS_OUTPUT_FLAG_FIXEDSIZE) {
    if (entry->output & APP_SETTINGS_OUTPUT_FLAG_HEX) {
      value_length = entry->value_size * 2;
    }
  }

  return app_util_get_string(value_str,
                             value,
                             entry->output_enum_list,
                             entry->output & APP_SETTINGS_OUTPUT_FLAG_SIGNED,
                             entry->output & APP_SETTINGS_OUTPUT_FLAG_HEX,
                             value_length);
}

/* App settings get ip address */
sl_status_t app_settings_get_ip_address(char *value_str,
                                        const char *key_str,
                                        const app_settings_entry_t *entry)
{
  sl_status_t ret;
  sl_wisun_ip_address_t address;
  sl_wisun_ip_address_type_t address_type = SL_WISUN_IP_ADDRESS_TYPE_GLOBAL;
  (void)key_str;

  if (strcmp("ip_address_global", entry->key) == 0) {
    address_type = SL_WISUN_IP_ADDRESS_TYPE_GLOBAL;
  } else if (strcmp("ip_address_link_local", entry->key) == 0) {
    address_type = SL_WISUN_IP_ADDRESS_TYPE_LINK_LOCAL;
  } else if (strcmp("ip_address_border_router", entry->key) == 0) {
    address_type = SL_WISUN_IP_ADDRESS_TYPE_BORDER_ROUTER;
  } else if (strcmp("ip_address_primary_parent", entry->key) == 0) {
    address_type = SL_WISUN_IP_ADDRESS_TYPE_PRIMARY_PARENT;
  } else if (strcmp("ip_address_secondary_parent", entry->key) == 0) {
    address_type = SL_WISUN_IP_ADDRESS_TYPE_SECONDARY_PARENT;
  } else {
    //do nothing: global ip address is the default
  }

  ret = sl_wisun_get_ip_address(address_type, &address);
  if (ret == SL_STATUS_OK) {
    sl_wisun_ip6tos(address.address, value_str);
  } else {
    memcpy(value_str, APP_SETTINGS_NONE_VALUE_STR,
           strlen(APP_SETTINGS_NONE_VALUE_STR) + 1);
  }

  return SL_STATUS_OK;
}

// -----------------------------------------------------------------------------
//                          Static Function Definitions
// -----------------------------------------------------------------------------

/* App settings nvm load */
static sl_status_t app_settings_nvm_load(uint8_t settings_domain,
                                         void *const settings,
                                         size_t settings_size)
{
  nvm3_ObjectKey_t nvm_key;
  uint32_t nvm_type;
  size_t nvm_size;
  Ecode_t ret;

  nvm_key = APP_SETTINGS_NVM_KEY_BASE + settings_domain;
  ret = nvm3_getObjectInfo(nvm3_defaultHandle,
                           nvm_key,
                           &nvm_type,
                           &nvm_size);
  if (ret != ECODE_NVM3_OK) {
    return SL_STATUS_FAIL;
  }

  if (nvm_size != settings_size) {
    return SL_STATUS_FAIL;
  }

  ret = nvm3_readData(nvm3_defaultHandle,
                      nvm_key,
                      settings,
                      nvm_size);
  if (ret != ECODE_NVM3_OK) {
    return SL_STATUS_FAIL;
  }

  return SL_STATUS_OK;
}

/* App settings NVM save */
static sl_status_t app_settings_nvm_save(uint8_t settings_domain,
                                         const void *const settings,
                                         size_t settings_size)
{
  nvm3_ObjectKey_t nvm_key;
  Ecode_t ret;

  nvm_key = APP_SETTINGS_NVM_KEY_BASE + settings_domain;
  ret = nvm3_writeData(nvm3_defaultHandle,
                       nvm_key,
                       settings,
                       settings_size);
  if (ret != ECODE_NVM3_OK) {
    return SL_STATUS_FAIL;
  }

  return SL_STATUS_OK;
}

/* App settings NVM delete */
static void app_settings_nvm_delete(uint8_t settings_domain)
{
  nvm3_ObjectKey_t nvm_key;

  nvm_key = APP_SETTINGS_NVM_KEY_BASE + settings_domain;
  (void)nvm3_deleteObject(nvm3_defaultHandle, nvm_key);
}
