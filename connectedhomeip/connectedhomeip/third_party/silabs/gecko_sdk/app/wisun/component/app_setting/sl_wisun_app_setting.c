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

#include <stddef.h>
#include <string.h>
#include <assert.h>
#include "sl_status.h"
#include "sl_string.h"
#include "cmsis_os2.h"
#include "sl_cmsis_os2_common.h"
#include "sl_wisun_api.h"
#include "sl_wisun_cli_settings.h"
#include "sl_wisun_app_setting.h"
#if defined(SL_CATALOG_WISUN_DEFAULT_PHY_PRESENT)
  #include "sl_default_phy.h"
#endif

// -----------------------------------------------------------------------------
//                              Macros and Typedefs
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------
//                          Static Function Declarations
// -----------------------------------------------------------------------------

/**************************************************************************//**
 * @brief Acquire application mutex
 * @details Internal mutex lock
 *****************************************************************************/
static inline void _app_wisun_mutex_acquire(void);

/**************************************************************************//**
 * @brief Release application mutex
 * @details Internal mutex release
 *****************************************************************************/
static inline void _app_wisun_mutex_release(void);

/**************************************************************************//**
 * @brief Helper function for checking the name of the Wi-SUN network
 * @param [in] *name is the network name that are wanted to check
 * @param [out] *name_len is the length of the name that calculated by the function.
 * @return char* const char pointer that points to checked name or to the default
 *          one if the checked one is incorrect.
 *****************************************************************************/
static const char* _app_check_nw_name(const char *name, size_t *const name_len);

// -----------------------------------------------------------------------------
//                                Static Variables
// -----------------------------------------------------------------------------

/**************************************************************************//**
 * @brief App framework mutex
 *****************************************************************************/
static osMutexId_t _app_wisun_setting_mtx;

/**************************************************************************//**
 * @brief App framework mutex control block
 *****************************************************************************/
__ALIGNED(8) static uint8_t _app_wisun_setting_mtx_cb[osMutexCbSize] = { 0 };

/**************************************************************************//**
 * @brief App framework mutex attribute
 *****************************************************************************/
static const osMutexAttr_t _app_wisun_setting_mtx_attr = {
  .name      = "AppWisunSettingMutex",
  .attr_bits = osMutexRecursive,
  .cb_mem    = _app_wisun_setting_mtx_cb,
  .cb_size   = sizeof(_app_wisun_setting_mtx_cb)
};

/**************************************************************************//**
 * @brief Wi-SUN application settings
 *****************************************************************************/
static app_setting_wisun_t _wisun_app_settings = { 0 };

/**************************************************************************//**
 * @brief Default settings structure
 *****************************************************************************/
#if defined(SL_CATALOG_WISUN_FULL_RADIOCONF_PRESENT)
static const app_setting_wisun_t wisun_app_settings_default = {
  .network_name = APP_SETTING_NETWORK_NAME,
  .network_size = APP_SETTING_NETWORK_SIZE,
  .tx_power = APP_SETTING_TX_POWER,
  .phy = {
    .regulatory_domain = APP_SETTINGS_WISUN_DEFAULT_REGULATORY_DOMAIN,
    .operating_class = APP_SETTINGS_WISUN_DEFAULT_OPERATING_CLASS,
    .operating_mode = APP_SETTINGS_WISUN_DEFAULT_OPERATING_MODE,
  },
};
#else
static const app_setting_wisun_t wisun_app_settings_default = {
  .network_name = APP_SETTING_NETWORK_NAME,
  .network_size = APP_SETTING_NETWORK_SIZE,
  .tx_power = APP_SETTING_TX_POWER,
  .phy = { 0 },
};
#endif

// -----------------------------------------------------------------------------
//                                Global Variables
// -----------------------------------------------------------------------------

/**************************************************************************//**
 * @brief Wi-SUN application network saving settings
 *****************************************************************************/
const app_saving_item_t network_saving_settings = {
  .data = &_wisun_app_settings,
  .data_size = sizeof(_wisun_app_settings),
  .default_val = &wisun_app_settings_default
};

/**************************************************************************//**
 * @brief Wi-SUN application all saved data
 *****************************************************************************/
const app_saving_item_t *saving_settings[] = {
  &network_saving_settings,
  NULL
};

// -----------------------------------------------------------------------------
//                          Public Function Definitions
// -----------------------------------------------------------------------------

/* Initialization of Wi-SUN setting */
void app_wisun_setting_init(void)
{
  // init wisun network mutex
  _app_wisun_setting_mtx = osMutexNew(&_app_wisun_setting_mtx_attr);
  assert(_app_wisun_setting_mtx != NULL);
}

/* Getting the setting */
sl_status_t app_wisun_setting_get(app_setting_wisun_t *const wisun_setting)
{
  sl_status_t res = SL_STATUS_OK;

  if (wisun_setting == NULL) {
    return SL_STATUS_INVALID_PARAMETER;
  }

  _app_wisun_mutex_acquire();

  // copies the settings
  memcpy(wisun_setting, &_wisun_app_settings, sizeof(app_setting_wisun_t));

  _app_wisun_mutex_release();
  return res;
}

/* Setting Wi-SUN network name */
sl_status_t app_wisun_setting_set_network_name(const char *const name)
{
  const char *network_name = NULL;
  size_t name_len = 0U;

  if (name == NULL) {
    return SL_STATUS_INVALID_PARAMETER;
  }

  _app_wisun_mutex_acquire();
  // check the network name, and return checked name.
  network_name = _app_check_nw_name(name, &name_len);

  if (name_len < MAX_SIZE_OF_NETWORK_NAME) {
    memcpy(_wisun_app_settings.network_name, network_name, name_len);
    _wisun_app_settings.network_name[name_len] = 0U;
  } else {
    _app_wisun_mutex_release();
    return SL_STATUS_FAIL;
  }

  _app_wisun_mutex_release();
  return SL_STATUS_OK;
}

/* Setting Wi-SUN network size */
sl_status_t app_wisun_setting_set_network_size(const uint8_t *const size)
{
  if (size == NULL) {
    return SL_STATUS_INVALID_PARAMETER;
  }

  _app_wisun_mutex_acquire();

  _wisun_app_settings.network_size = *size;

  _app_wisun_mutex_release();
  return SL_STATUS_OK;
}

/* Setting Wi-SUN TX power */
sl_status_t app_wisun_setting_set_tx_power(const int8_t *const tx_power)
{
  if (tx_power == NULL) {
    return SL_STATUS_INVALID_PARAMETER;
  }

  _app_wisun_mutex_acquire();

  _wisun_app_settings.tx_power = *tx_power;

  _app_wisun_mutex_release();
  return SL_STATUS_OK;
}

/* Setting Wi-SUN PHY */
sl_status_t app_wisun_setting_set_phy(const app_setting_phy *const phy)
{
  if (phy == NULL) {
    return SL_STATUS_INVALID_PARAMETER;
  }

  _app_wisun_mutex_acquire();

  memcpy(&_wisun_app_settings.phy, phy, sizeof(app_setting_phy));

  _app_wisun_mutex_release();
  return SL_STATUS_OK;
}

/* Getting network name */
sl_status_t app_wisun_setting_get_network_name(char *const name, uint8_t size)
{
  uint8_t name_len = 0U;

  if ((name == NULL) || (size < MAX_SIZE_OF_NETWORK_NAME)) {
    return SL_STATUS_INVALID_PARAMETER;
  }

  _app_wisun_mutex_acquire();

  name_len = (uint8_t)sl_strlen(_wisun_app_settings.network_name);

  if (name_len < MAX_SIZE_OF_NETWORK_NAME) {
    memset(name, 0U, MAX_SIZE_OF_NETWORK_NAME);
    memcpy(name, _wisun_app_settings.network_name, name_len);
  } else {
    return SL_STATUS_FAIL;
  }

  _app_wisun_mutex_release();
  return SL_STATUS_OK;
}

/* Getting network size */
sl_status_t app_wisun_setting_get_network_size(uint8_t *const size)
{
  if (size == NULL) {
    return SL_STATUS_INVALID_PARAMETER;
  }

  _app_wisun_mutex_acquire();

  *size = _wisun_app_settings.network_size;

  _app_wisun_mutex_release();
  return SL_STATUS_OK;
}

/* Getting TX power */
sl_status_t app_wisun_setting_get_tx_power(int8_t *const tx_power)
{
  if (tx_power == NULL) {
    return SL_STATUS_INVALID_PARAMETER;
  }

  _app_wisun_mutex_acquire();

  *tx_power = _wisun_app_settings.tx_power;

  _app_wisun_mutex_release();
  return SL_STATUS_OK;
}

/* Getting PHY */
sl_status_t app_wisun_setting_get_phy(app_setting_phy *const phy)
{
  if (phy == NULL) {
    return SL_STATUS_INVALID_PARAMETER;
  }

  _app_wisun_mutex_acquire();

  memcpy(phy, &_wisun_app_settings.phy, sizeof(app_setting_phy));

  _app_wisun_mutex_release();
  return SL_STATUS_OK;
}

// -----------------------------------------------------------------------------
//                          Static Function Definitions
// -----------------------------------------------------------------------------

/* Mutex acquire */
static inline void _app_wisun_mutex_acquire(void)
{
  assert(osMutexAcquire(_app_wisun_setting_mtx, osWaitForever) == osOK);
}

/* Mutex release */
static inline void _app_wisun_mutex_release(void)
{
  assert(osMutexRelease(_app_wisun_setting_mtx) == osOK);
}

static const char* _app_check_nw_name(const char *name, size_t *const name_len)
{
  const char* ret_name = WISUN_CONFIG_NETWORK_NAME;

  *name_len = sl_strnlen((char*)name, SL_WISUN_NETWORK_NAME_SIZE);
  if (!(*name_len < SL_WISUN_NETWORK_NAME_SIZE) || (*name_len == 0) ) {
    // sets the default name and its size
    ret_name = WISUN_CONFIG_NETWORK_NAME;
    *name_len = sl_strnlen(WISUN_CONFIG_NETWORK_NAME, SL_WISUN_NETWORK_NAME_SIZE);
    printf("\r\n[Warning: The name of Wi-SUN network is incorrect, default name used, \"%s\" ]\r\n", ret_name);
  } else {
    ret_name = name;
  }

  return ret_name;
}
