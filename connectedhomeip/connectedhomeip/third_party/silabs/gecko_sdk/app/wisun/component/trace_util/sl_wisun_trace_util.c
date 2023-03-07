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
#include <assert.h>
#include "cmsis_os2.h"
#include "sl_cmsis_os2_common.h"
#include "sl_wisun_trace_util.h"
#include "sl_wisun_app_util_config.h"

// -----------------------------------------------------------------------------
//                              Macros and Typedefs
// -----------------------------------------------------------------------------

///  Size of the IPv6 string
#define IPV6_STRING_SIZE                (40U)

///  IPv6 string buffer storage typedef
typedef struct ipv6_string_buffer {
  /// Next free slot index
  uint8_t next_index;
  /// Buffer allocation
  char buff[WISUN_APP_IP_STRING_BUFFER_COUNT * IPV6_STRING_SIZE];
} ipv6_string_buffer_t;

// -----------------------------------------------------------------------------
//                          Static Function Declarations
// -----------------------------------------------------------------------------

/**************************************************************************//**
 * @brief Get the next IPv6 string buffer ptr
 * @details Use internal storage
 * @return char* destination ptr
 *****************************************************************************/
static char *_get_next_ip_str_ptr(void);

/**************************************************************************//**
 * @brief Event manager mutex lock
 *****************************************************************************/
static void app_wisun_util_mutex_lock(void);

/**************************************************************************//**
 * @brief Event manager mutex unlock
 *****************************************************************************/
static void app_wisun_util_mutex_unlock(void);

/**************************************************************************//**
 * @brief Get string key by value from app_enum_t
 * @details Helper function
 * @param[in] app_enum Application enum storage
 * @param[in] val Reference value
 * @return const char* String value
 *****************************************************************************/
static const char *_get_str_key_by_val_from_enum(const app_enum_t * const app_enum,
                                                 const uint32_t val);

// -----------------------------------------------------------------------------
//                                Global Variables
// -----------------------------------------------------------------------------

/// Application enum for connection status
const app_enum_t app_wisun_conn_status_enum[] = {
  { "Disconnected", SL_WISUN_JOIN_STATE_DISCONNECTED },
  { "Select PAN", SL_WISUN_JOIN_STATE_SELECT_PAN },
  { "Authenticate", SL_WISUN_JOIN_STATE_AUTHENTICATE },
  { "Acquire PAN configuration", SL_WISUN_JOIN_STATE_ACQUIRE_PAN_CONFIG },
  { "Configuring routing", SL_WISUN_JOIN_STATE_CONFIGURE_ROUTING },
  { "Operational", SL_WISUN_JOIN_STATE_OPERATIONAL },
  { NULL, 0 }
};

/// Application enum for regulatory domains of first phy parameter
const app_enum_t app_wisun_phy_reg_domain_enum[] = {
  { "WW", SL_WISUN_REGULATORY_DOMAIN_WW },
  { "NA", SL_WISUN_REGULATORY_DOMAIN_NA },
  { "JP", SL_WISUN_REGULATORY_DOMAIN_JP },
  { "EU", SL_WISUN_REGULATORY_DOMAIN_EU },
  { "CN", SL_WISUN_REGULATORY_DOMAIN_CN },
  { "IN", SL_WISUN_REGULATORY_DOMAIN_IN },
  { "MX", SL_WISUN_REGULATORY_DOMAIN_MX },
  { "BZ", SL_WISUN_REGULATORY_DOMAIN_BZ },
  { "AZ/NZ", SL_WISUN_REGULATORY_DOMAIN_AZ },        // NZ also
  { "KR", SL_WISUN_REGULATORY_DOMAIN_KR },
  { "PH", SL_WISUN_REGULATORY_DOMAIN_PH },
  { "MY", SL_WISUN_REGULATORY_DOMAIN_MY },
  { "HK", SL_WISUN_REGULATORY_DOMAIN_HK },
  { "SG", SL_WISUN_REGULATORY_DOMAIN_SG },
  { "TH", SL_WISUN_REGULATORY_DOMAIN_TH },
  { "VN", SL_WISUN_REGULATORY_DOMAIN_VN },
  { "APPLICATION", SL_WISUN_REGULATORY_DOMAIN_APP },  // application specific domain
  { NULL, 0 }
};

/// Application enum for network size
const app_enum_t app_wisun_nw_size_enum[] = {
  { "automatic", SL_WISUN_NETWORK_SIZE_AUTOMATIC },
  { "small", SL_WISUN_NETWORK_SIZE_SMALL },
  { "medium", SL_WISUN_NETWORK_SIZE_MEDIUM },
  { "large", SL_WISUN_NETWORK_SIZE_LARGE },
  { "test", SL_WISUN_NETWORK_SIZE_TEST },
  { "certification", SL_WISUN_NETWORK_SIZE_CERTIFICATION },
  { NULL, 0 }
};

/// Application enum for regulation
const app_enum_t app_regulation_enum[] = {
  { "none", SL_WISUN_REGULATION_NONE },
  { "arib", SL_WISUN_REGULATION_ARIB },
  { NULL, 0 }
};

// -----------------------------------------------------------------------------
//                                Static Variables
// -----------------------------------------------------------------------------

/// App trace-util mutex
static osMutexId_t _app_wisun_util_mtx;

/// App trace-util mutex control block
__ALIGNED(8) static uint8_t _app_wisun_util_mtx_cb[osMutexCbSize] = { 0 };

/// App trace-util mutex attribute
static const osMutexAttr_t _app_wisun_util_mtx_attr = {
  .name      = "AppWisunEventMgrMutex",
  .attr_bits = osMutexRecursive,
  .cb_mem    = _app_wisun_util_mtx_cb,
  .cb_size   = sizeof(_app_wisun_util_mtx_cb)
};

/// Internal circular IPv6 string buffer
static ipv6_string_buffer_t _ip_str_buff = {
  .next_index = 0,
  .buff = { 0 },
};

// -----------------------------------------------------------------------------
//                          Public Function Definitions
// -----------------------------------------------------------------------------

void app_wisun_trace_util_init(void)
{
  _app_wisun_util_mtx = osMutexNew(&_app_wisun_util_mtx_attr);
  assert(_app_wisun_util_mtx != NULL);
}

const char* app_wisun_trace_util_get_ip_address_str(const void *const addr)
{
  char *dst = NULL;
  bool ipv6_res = false;

  if (addr == NULL) {
    return NULL;
  }

  app_wisun_util_mutex_lock();

  dst = _get_next_ip_str_ptr();
  ipv6_res = ip6tos(addr, dst);  // convert address binary to text for wisun and ipv6

  app_wisun_util_mutex_unlock();
  return ipv6_res ? dst : NULL; // dst -> success, NULL -> error;
}

const char * app_wisun_trace_util_conn_state_to_str(const uint32_t val)
{
  return _get_str_key_by_val_from_enum(app_wisun_conn_status_enum, val);
}

const char * app_wisun_trace_util_reg_domain_to_str(const uint32_t val)
{
  return _get_str_key_by_val_from_enum(app_wisun_phy_reg_domain_enum, val);
}

const char * app_wisun_trace_util_nw_size_to_str(const uint32_t val)
{
  return _get_str_key_by_val_from_enum(app_wisun_nw_size_enum, val);
}

// -----------------------------------------------------------------------------
//                          Static Function Definitions
// -----------------------------------------------------------------------------

/* Get next string storage pointer*/
static char *_get_next_ip_str_ptr(void)
{
  char *p = NULL;
  p = _ip_str_buff.buff + (_ip_str_buff.next_index * IPV6_STRING_SIZE);
  if (_ip_str_buff.next_index < (WISUN_APP_IP_STRING_BUFFER_COUNT - 1)) {
    ++_ip_str_buff.next_index;
  } else {
    _ip_str_buff.next_index = 0;
  }
  return p;
}

/* Mutex acquire */
static void app_wisun_util_mutex_lock(void)
{
  assert(osMutexAcquire(_app_wisun_util_mtx, osWaitForever) == osOK);
}

/* Mutex release */
static void app_wisun_util_mutex_unlock(void)
{
  assert(osMutexRelease(_app_wisun_util_mtx) == osOK);
}

static const char *_get_str_key_by_val_from_enum(const app_enum_t * const app_enum,
                                                 const uint32_t val)
{
  const app_enum_t *p = NULL;

  p = app_enum;
  while (p->value_str != NULL) {
    if (p->value == val) {
      return p->value_str;
    }
    ++p;
  }
  return NULL;
}
