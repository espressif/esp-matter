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
#include <assert.h>
#include <string.h>
#include "sli_wisun_meter_collector.h"
#include "cmsis_os2.h"
#include "sl_cmsis_os2_common.h"
#include "sl_wisun_meter_collector_config.h"
#include "sl_wisun_app_core_util.h"
#include "sl_sleeptimer.h"

// -----------------------------------------------------------------------------
//                              Macros and Typedefs
// -----------------------------------------------------------------------------

/// Unpack measurement packet internal macro function
#define __pack_unpack_measurement_packets(dest, src) \
  do {                                               \
    if (src == NULL) {                               \
      return;                                        \
    }                                                \
    dest->id          = src->id;                     \
    dest->temperature = src->temperature;            \
    dest->humidity    = src->humidity;               \
    dest->light       = src->light;                  \
  } while (0)

/// Print measurement packet internal macro function
#define __print_packet(ip_address, packet)                              \
  do {                                                                  \
    printf("[%s: {id: %u}{temp: %lu.%.2lu}{hum: %lu.%.2lu}{lx: %u}]\n", \
           ip_address,                                                  \
           packet->id,                                                  \
           packet->temperature / 1000,                                  \
           (packet->temperature % 1000) / 10,                           \
           packet->humidity / 1000,                                     \
           (packet->humidity % 1000) / 10,                              \
           packet->light);                                              \
  } while (0)

/// Common handler init helper macro function
#define __common_init(hnd)                        \
  do {                                            \
    hnd->resource_hnd.lock = _mc_mutex_acquire;   \
    hnd->resource_hnd.unlock = _mc_mutex_release; \
    _meter_collector_common_mtx_init();           \
    if (hnd->resource_hnd.init != NULL) {         \
      hnd->resource_hnd.init();                   \
    }                                             \
  } while (0)

// -----------------------------------------------------------------------------
//                          Static Function Declarations
// -----------------------------------------------------------------------------
/**************************************************************************//**
 * @brief Meter - Collector common mutex init
 * @details Helper function
 *****************************************************************************/
static void _meter_collector_common_mtx_init(void);

/**************************************************************************//**
 * @brief Mutex acquire
 * @details Helper function
 *****************************************************************************/
static void _mc_mutex_acquire(void);

/**************************************************************************//**
 * @brief Mutex release
 * @details Helper function
 *****************************************************************************/
static void _mc_mutex_release(void);

/**************************************************************************//**
 * @brief Default collector timeout handler
 * @details Default handler
 * @param[in,out] meter Meter entry
 *****************************************************************************/
static void _def_collector_timeout_hnd(sl_wisun_meter_entry_t *meter);

/**************************************************************************//**
 * @brief Default collector send
 * @details Default handler
 * @param[in] sockid The socket to use for transmission
 * @param[in,out] meter Meter entry
 * @param[in,out] req Request
 * @return true Never
 * @return false Always
 *****************************************************************************/
static bool _def_collector_send(const int32_t sockid,
                                sl_wisun_meter_entry_t *meter,
                                sl_wisun_meter_request_t *req);

/**************************************************************************//**
 * @brief Default collector receive
 * @details Default handler
 * @param[in] sockid Socket ID
 * @return sl_wisun_meter_entry_t* NULL
 *****************************************************************************/
static sl_wisun_meter_entry_t * _def_collector_recv(int32_t sockid);

/**************************************************************************//**
 * @brief Default get meter
 * @details Default handler
 * @param[in] remote_addr Remote address
 * @return sl_wisun_meter_entry_t* NULL
 *****************************************************************************/
static sl_wisun_meter_entry_t *_def_collector_get_meter(const wisun_addr_t * const remote_addr);

// -----------------------------------------------------------------------------
//                                Global Variables
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------
//                                Static Variables
// -----------------------------------------------------------------------------

/// Internal reference token
static char _token[SL_WISUN_METER_COLLECTOR_TOKEN_MAX_SIZE] = { 0 };

/// Internal reference token length without '\0'
static uint16_t _token_length = 0;

/// Meter-Collector mutex
static osMutexId_t _mc_mtx = { NULL };

/// Meter-Collector mutex control block
__ALIGNED(8) static uint8_t _mc_mtx_cb[osMutexCbSize] = { 0 };

/// Meter-Collector mutex attribute
static const osMutexAttr_t _mc_mtx_attr = {
  .name      = "MeterCollMutex",
  .attr_bits = osMutexRecursive,
  .cb_mem    = _mc_mtx_cb,
  .cb_size   = sizeof(_mc_mtx_cb)
};

// -----------------------------------------------------------------------------
//                          Public Function Definitions
// -----------------------------------------------------------------------------

void sl_wisun_collector_init_hnd(sl_wisun_collector_hnd_t *hnd)
{
  __common_init(hnd);
}

void sl_wisun_meter_init_hnd(sl_wisun_meter_hnd_t *hnd)
{
  __common_init(hnd);
}

void sl_wisun_collector_set_initializer(sl_wisun_collector_hnd_t *hnd,
                                        sl_wisun_mc_init_t initializer)
{
  hnd->resource_hnd.init = initializer;
}

void sl_wisun_meter_set_initializer(sl_wisun_meter_hnd_t *hnd,
                                    sl_wisun_mc_init_t initializer)
{
  hnd->resource_hnd.init = initializer;
}

void sl_wisun_collector_set_handler(sl_wisun_collector_hnd_t *hnd,
                                    sl_wisun_collector_recv_hnd_t receiver,
                                    sl_wisun_collector_send_hnd_t sender,
                                    sl_wisun_collector_timeout_hnd_t timeout_hnd,
                                    sl_wisun_collector_get_meter_entry_t get_meter)
{
  if (hnd->resource_hnd.lock != NULL) {
    hnd->resource_hnd.lock();
  }
  hnd->recv = receiver == NULL ? _def_collector_recv : receiver;
  hnd->send = sender == NULL ? _def_collector_send : sender;
  hnd->timeout = timeout_hnd == NULL ? _def_collector_timeout_hnd : timeout_hnd;
  hnd->get_meter = get_meter == NULL ? _def_collector_get_meter : get_meter;
  if (hnd->resource_hnd.unlock != NULL) {
    hnd->resource_hnd.unlock();
  }
}

/* Init token */
void sl_wisun_mc_init_token(const char * const token_str)
{
  if (token_str == NULL) {
    printf("[ERROR: token string is NULL]\n");
    return;
  }

  _mc_mutex_acquire();
  _token_length = 0;
  for (uint16_t i = 0; i < SL_WISUN_METER_COLLECTOR_TOKEN_MAX_SIZE - 1; ++i, ++_token_length) {
    if (!token_str[i]) {
      break;
    }
    _token[i] = token_str[i];
  }
  if (_token_length == SL_WISUN_METER_COLLECTOR_TOKEN_MAX_SIZE - 1) {
    printf("[WARNING: Token length is reached the maximum size: %d]\n", _token_length);
  }
  _token[_token_length] = 0;
  _mc_mutex_release();
}

/* Get token length */
uint16_t sl_wisun_mc_get_token_size(void)
{
  uint16_t rval = 0;
  _mc_mutex_acquire();
  rval = _token_length;
  _mc_mutex_release();
  return rval;
}

/* Pack packet */
void sl_wisun_mc_unpack_measurement_packet(sl_wisun_meter_packet_t * const dest,
                                           const sl_wisun_meter_packet_packed_t * const src)
{
  __pack_unpack_measurement_packets(dest, src);
}

/* Unpack packet */
void sl_wisun_mc_pack_measurement_packet(sl_wisun_meter_packet_packed_t * const dest,
                                         const sl_wisun_meter_packet_t * const src)
{
  __pack_unpack_measurement_packets(dest, src);
}

/* Print measurement packet */
void sl_wisun_mc_print_mesurement(const char *ip_address, const void *packet, const bool is_packed)
{
  const sl_wisun_meter_packet_t *unpacked_packet = NULL;
  const sl_wisun_meter_packet_packed_t *packed_packet = NULL;

  if (ip_address == NULL || packet == NULL) {
    return;
  }
  if ( is_packed ) {
    packed_packet = (const sl_wisun_meter_packet_packed_t *) packet;
    __print_packet(ip_address, packed_packet);
  } else {
    unpacked_packet = (const sl_wisun_meter_packet_t *) packet;
    __print_packet(ip_address, unpacked_packet);
  }
}

/* compare token */
bool sl_wisun_mc_compare_token(const uint8_t *token, const uint16_t token_size)
{
  bool res = true;

  if (token_size != _token_length) {
    return false;
  }
  _mc_mutex_acquire();
  for (uint16_t i = 0; i < _token_length; ++i) {
    if (_token[i] != token[i]) {
      res = false;
      break;
    }
  }
  _mc_mutex_release();
  return res;
}

// -----------------------------------------------------------------------------
//                          Static Function Definitions
// -----------------------------------------------------------------------------

/* Meter-Collector init */
static void _meter_collector_common_mtx_init(void)
{
  if (_mc_mtx == NULL) {
    _mc_mtx = osMutexNew(&_mc_mtx_attr);
    assert(_mc_mtx != NULL);
  }
}

/* Mutex acquire */
static void _mc_mutex_acquire(void)
{
  if (osKernelGetState() == osKernelRunning) {
    assert(osMutexAcquire(_mc_mtx, osWaitForever) == osOK);
  }
}

/* Mutex release */
static void _mc_mutex_release(void)
{
  if (osKernelGetState() == osKernelRunning) {
    assert(osMutexRelease(_mc_mtx) == osOK);
  }
}

static void _def_collector_timeout_hnd(sl_wisun_meter_entry_t *meter)
{
  const char *ip_str = NULL;

  ip_str = app_wisun_trace_util_get_ip_address_str(&meter->addr.sin6_addr);

  printf("[Measurement response timed out: %ldms %s]",
         sl_sleeptimer_tick_to_ms(meter->resp_recv_timestamp - meter->req_sent_timestamp),
         ip_str);
}

static bool _def_collector_send(const int32_t sockid,
                                sl_wisun_meter_entry_t *meter,
                                sl_wisun_meter_request_t *req)
{
  (void) sockid;
  (void) meter;
  (void) req;
  assert("[Collector Send not implemented!]" == NULL);
  return false;
}

static sl_wisun_meter_entry_t * _def_collector_recv(int32_t sockid)
{
  (void) sockid;
  assert("[Collector Receive not implemented!]" == NULL);
  return NULL;
}

static sl_wisun_meter_entry_t *_def_collector_get_meter(const wisun_addr_t * const remote_addr)
{
  (void) remote_addr;
  assert("[Collector Get meter not implemented!]" == NULL);
  return NULL;
}

uint8_t sl_wisun_mc_get_led_id_from_payload(const char *payload_str)
{
  if (!strncmp(payload_str, SL_WISUN_METER_LED0_TOGGLE_PAYLOAD_STR,
               SL_WISUN_METER_LED_TOGGLE_PAYLOAD_STR_MAX_LEN)) {
    return SL_WISUN_METER_LED0;
  } else if (!strncmp(payload_str, SL_WISUN_METER_LED1_TOGGLE_PAYLOAD_STR,
                      SL_WISUN_METER_LED_TOGGLE_PAYLOAD_STR_MAX_LEN)) {
    return SL_WISUN_METER_LED1;
  } else {
    return SL_WISUN_METER_LED_UNKNOWN;
  }
}

const char *sl_wisun_mc_get_led_payload_by_id(const uint8_t led_id)
{
  switch (led_id) {
    case SL_WISUN_METER_LED0:        return SL_WISUN_METER_LED0_TOGGLE_PAYLOAD_STR;
    case SL_WISUN_METER_LED1:        return SL_WISUN_METER_LED1_TOGGLE_PAYLOAD_STR;
    default:                         return "Unknown";
  }
}

#undef __print_packet
#undef __pack_unpack_measurement_packets
#undef __common_init
