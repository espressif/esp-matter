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

#include <string.h>
#include "sl_wisun_app_core_util.h"
#include "sl_wisun_collector.h"
#include "sl_wisun_meter_collector_config.h"
#include "cmsis_os2.h"
#include "sl_cmsis_os2_common.h"
#include "sl_sleeptimer.h"
#include "sl_component_catalog.h"
#include "sl_wisun_trace_util.h"
#include "sli_wisun_meter_collector.h"

// -----------------------------------------------------------------------------
//                              Macros and Typedefs
// -----------------------------------------------------------------------------

/// Collector receiver thread stack size
#define COLLECTOR_RECV_THREAD_STACK_SIZE      (128U)

// -----------------------------------------------------------------------------
//                          Static Function Declarations
// -----------------------------------------------------------------------------

/**************************************************************************//**
 * @brief Create the socket shared by the sender and recevier threads
 *****************************************************************************/
static void _create_common_socket(void);

/**************************************************************************//**
 * @brief Collector send request
 * @details Handler function
 * @param[in] sockid The socket used for sending or SOCKET_INVALID_ID for ad-hoc socket creation
 * @param[in] meter Meter entry
 * @param[in] req The request to send
 * @return true On success
 * @return false On failure
 *****************************************************************************/
static bool _collector_send_request(const int32_t sockid,
                                    sl_wisun_meter_entry_t *meter,
                                    sl_wisun_meter_request_t *req);

/**************************************************************************//**
 * @brief Collector receive response
 * @details Handler function
 * @param[in] sockid The socket used for receiving
 * @return sl_wisun_meter_entry_t*
 *****************************************************************************/
static sl_wisun_meter_entry_t * _collector_recv_response(int32_t sockid);

/**************************************************************************//**
 * @brief Collector receiver thread
 * @details Receiver thread handler function
 * @param[in] args Arguments
 *****************************************************************************/
static void _collector_recv_thread_fnc(void * args);

/**************************************************************************//**
 * @brief Collector init meter storage
 * @details Init internal meter storage
 *****************************************************************************/
static void _collector_init_meters(void);

/**************************************************************************//**
 * @brief Collector get meter entry by address
 * @details Handler function
 * @param remote_addr Remote address
 * @return sl_wisun_meter_entry_t* Meter entry or NULL on error
 *****************************************************************************/
static sl_wisun_meter_entry_t *_collector_get_meter_entry_by_address(const wisun_addr_t * const remote_addr);

// -----------------------------------------------------------------------------
//                                Global Variables
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------
//                                Static Variables
// -----------------------------------------------------------------------------

/// Meter internal storage
static sl_wisun_meter_entry_t _meters[SL_WISUN_COLLECTOR_MAX_REG_METER] = { 0 };

static sl_wisun_meter_request_t _measurement_req = { 0 };

/// Collector internal handler
static sl_wisun_collector_hnd_t _collector_hnd = { 0 };

/// Collector task control block
static uint64_t _collector_recv_task_cb[osThreadCbSize] = { 0 };

/// Collector receiver thread stack
static uint64_t _collector_recv_stack[COLLECTOR_RECV_THREAD_STACK_SIZE] = { 0 };

/// Collector receiver task attributes
static const osThreadAttr_t _collector_recv_task_attr = {
  .name        = "CollectorRecvTask",
  .attr_bits   = osThreadDetached,
  .cb_mem      = _collector_recv_task_cb,
  .cb_size     = sizeof(_collector_recv_task_cb),
  .stack_mem   = _collector_recv_stack,
  .stack_size  = sizeof(_collector_recv_stack),
  .priority    = osPriorityBelowNormal,
  .tz_module   = 0
};

/// Collector receiver thread ID
static osThreadId_t _collector_recv_thr_id = NULL;

/// Socket shared among the sender and receiver threads
static int32_t _common_socket = SOCKET_INVALID_ID;

// -----------------------------------------------------------------------------
//                          Public Function Definitions
// -----------------------------------------------------------------------------

int32_t sl_wisun_collector_get_shared_socket(void)
{
  return(_common_socket);
}

void sl_wisun_collector_init()
{
  sl_wisun_meter_request_t req = { 0 };

  // sl_wisun_collector_init_hnd(&_collector_hnd);
  sl_wisun_collector_init_common_resources();

  // init collector-meter token
  sl_wisun_mc_init_token(SL_WISUN_METER_COLLECTOR_TOKEN);

  // Set measurement request
  req.buff = (uint8_t *)SL_WISUN_METER_COLLECTOR_TOKEN;
  req.length = sl_wisun_mc_get_token_size();
  sl_wisun_collector_set_measurement_request(&req);
}

void sl_wisun_collector_inherit_common_hnd(sl_wisun_collector_hnd_t *hnd)
{
  // do not overwrite resource_hnd and get_meter
  _collector_hnd.send = hnd->send;
  _collector_hnd.recv = hnd->recv;
  _collector_hnd.timeout = hnd->timeout;
  hnd->get_meter = _collector_hnd.get_meter;
}

void sl_wisun_collector_init_common_resources(void)
{
  // init collector handler
  sl_wisun_collector_set_handler(&_collector_hnd,
                                 _collector_recv_response,
                                 _collector_send_request,
                                 NULL,
                                 _collector_get_meter_entry_by_address);
  sl_wisun_collector_init_hnd(&_collector_hnd);
  _collector_init_meters();

  _collector_recv_thr_id = osThreadNew(_collector_recv_thread_fnc, NULL, &_collector_recv_task_attr);
  assert(_collector_recv_thr_id != NULL);
}

/* Register meter */
bool sl_wisun_collector_register_meter(const wisun_addr_t *meter_addr)
{
  int32_t first_free_idx = -1;

  sl_wisun_mc_mutex_acquire(_collector_hnd);

  for (int32_t idx = SL_WISUN_COLLECTOR_MAX_REG_METER; idx >= 0; --idx) {
    if (_meters[idx].used == true    // is exist
        && sl_wisun_collector_compare_address(&_meters[idx].addr, meter_addr)) {
      sl_wisun_mc_release_mtx_and_return_val(_collector_hnd, false);
    } else if (_meters[idx].used == false) {
      first_free_idx = idx;
    }
  }

  if (first_free_idx != -1) {
    _meters[first_free_idx].used = true;
    memcpy(&_meters[first_free_idx].addr, meter_addr, sizeof(wisun_addr_t));
    sl_wisun_mc_release_mtx_and_return_val(_collector_hnd, true);
  }

  sl_wisun_mc_mutex_release(_collector_hnd);

  return false;
}

/* Remove meter */
bool sl_wisun_collector_remove_meter(const wisun_addr_t *meter_addr)
{
  sl_wisun_mc_mutex_acquire(_collector_hnd);

  for (uint16_t i = 0; i < SL_WISUN_COLLECTOR_MAX_REG_METER; ++i) {
    if (sl_wisun_collector_compare_address(&_meters[i].addr, meter_addr)) {
      _meters[i].used = false;
      sl_wisun_mc_release_mtx_and_return_val(_collector_hnd, true);
    }
  }

  sl_wisun_mc_mutex_release(_collector_hnd);

  return false;
}

/* init meters */
static void _collector_init_meters(void)
{
  sl_wisun_mc_mutex_acquire(_collector_hnd);
  for (uint16_t i = 0; i < SL_WISUN_COLLECTOR_MAX_REG_METER; ++i) {
    _meters[i].used = false; // set all of meter slots to unused
  }
  sl_wisun_mc_mutex_release(_collector_hnd);
}

/* comapre addresses */
bool sl_wisun_collector_compare_address(const wisun_addr_t *addr1, const wisun_addr_t *addr2)
{
  uint8_t *p1 = (uint8_t *)&addr1->sin6_addr;
  uint8_t *p2 = (uint8_t *)&addr2->sin6_addr;
  for (uint8_t i = 0; i < sizeof(in6_addr_t); ++i) {
    if (p1[i] != p2[i]) {
      return false;
    }
  }
  return true;
}

/* Measurement loop */
void sl_wisun_collector_measurement_request_loop(void)
{
  const char *ip_addr = NULL;
  bool res            = false;

  if (!app_wisun_network_is_connected()) {
    return;
  }

  sl_wisun_mc_mutex_acquire(_collector_hnd);
  if (_collector_hnd.send == NULL) {
    sl_wisun_mc_release_mtx_and_return(_collector_hnd);
  }
  for (uint16_t i = 0; i < SL_WISUN_COLLECTOR_MAX_REG_METER; ++i) {
    if (!_meters[i].used) {
      continue;
    }
    res = _collector_hnd.send(_common_socket, &_meters[i], &_measurement_req);
    if (!res) {
      ip_addr = app_wisun_trace_util_get_ip_address_str(&_meters[i].addr.sin6_addr);
      printf("[Collector cannot send measurement request to the meter: %s]\n", ip_addr);
    }
    _meters[i].req_sent_timestamp = sl_sleeptimer_get_tick_count();
  }
  sl_wisun_mc_mutex_release(_collector_hnd);
}

void sl_wisun_collector_set_measurement_request(const sl_wisun_meter_request_t * const req)
{
  sl_wisun_mc_mutex_acquire(_collector_hnd);
  memcpy(&_measurement_req, req, sizeof(_measurement_req));
  sl_wisun_mc_mutex_release(_collector_hnd);
}
// -----------------------------------------------------------------------------
//                          Static Function Definitions
// -----------------------------------------------------------------------------

static void _create_common_socket(void)
{
  int32_t res                   = SOCKET_INVALID_ID;
  wisun_addr_t collector_addr   = { 0 };

  _common_socket = socket(AF_WISUN, SOCK_DGRAM, IPPROTO_UDP);
  assert(_common_socket != SOCKET_INVALID_ID);

  collector_addr.sin6_family = AF_WISUN;
  collector_addr.sin6_addr = in6addr_any;
  collector_addr.sin6_port = htons(SL_WISUN_COLLECTOR_PORT);

  res = bind(_common_socket, (const struct sockaddr *) &collector_addr,
             sizeof(struct sockaddr_in6));
  assert(res != RETVAL_ERROR);
}

static bool _collector_send_request(const int32_t sockid,
                                    sl_wisun_meter_entry_t *meter,
                                    sl_wisun_meter_request_t *req)
{
#if !defined(SL_CATALOG_WISUN_COAP_PRESENT)
  int32_t res        = SOCKET_INVALID_ID;
  socklen_t len      = 0;
  bool retval        = true;

  if (sockid == SOCKET_INVALID_ID) {
    return false;
  }

  len = sizeof(meter->addr);

  res = sendto(sockid,
               req->buff,
               req->length, 0,
               (const struct sockaddr *)&meter->addr,
               len);

  if (res == RETVAL_ERROR) {
    retval = false;
  }

  return retval;
#else
  (void) sockid;
  (void) meter;
  (void) req;
  return false;
#endif
}

static sl_wisun_meter_entry_t * _collector_recv_response(int32_t sockid)
{
#if !defined(SL_CATALOG_WISUN_COAP_PRESENT)
  sl_wisun_meter_packet_packed_t packet = { 0 };
  wisun_addr_t remote_addr              = { 0 };
  const char *ip_addr                   = NULL;
  sl_wisun_meter_entry_t *meter         = NULL;
  socklen_t len                         = 0;
  int32_t res                           = SOCKET_INVALID_ID;

  len = sizeof(remote_addr);
  memset(&packet, 0, sizeof(packet));
  res = recvfrom(sockid, &packet, sizeof(packet), 0,
                 (struct sockaddr *)&remote_addr, &len);
  if (res == sizeof(packet)) {
    sl_wisun_mc_mutex_acquire(_collector_hnd);
    meter = _collector_hnd.get_meter(&remote_addr);
    sl_wisun_mc_mutex_release(_collector_hnd);
    if (meter != NULL) {
      ip_addr = app_wisun_trace_util_get_ip_address_str(&remote_addr.sin6_addr);
      sl_wisun_mc_print_mesurement(ip_addr, &packet, true);
      return meter;
    } else {
      printf("[Unknown remote message received!]\n");
    }
  }
  return NULL;
#else
  (void) sockid;
  return NULL;
#endif
}

static void _collector_recv_thread_fnc(void * args)
{
  sl_wisun_meter_entry_t *meter = NULL;
  uint32_t response_time_ms     = 0;

  (void) args;

  _create_common_socket();

  SL_WISUN_THREAD_LOOP {
    if (!app_wisun_network_is_connected()) {
      msleep(100);
      continue;
    }

    meter = NULL;
    if (_collector_hnd.recv != NULL) {
      meter = _collector_hnd.recv(_common_socket);
    }
    if (meter != NULL) {
      meter->resp_recv_timestamp = sl_sleeptimer_get_tick_count();
      response_time_ms = sl_sleeptimer_tick_to_ms(meter->resp_recv_timestamp
                                                  - meter->req_sent_timestamp);
      printf("[Response time: %ldms]\n", response_time_ms);
      if (response_time_ms > SL_WISUN_COLLECTOR_TIMEOUT_MS) {
        _collector_hnd.timeout(meter);
      }
    }
  }
}

static sl_wisun_meter_entry_t *_collector_get_meter_entry_by_address(const wisun_addr_t * const remote_addr)
{
  for (uint16_t i = 0; i < SL_WISUN_COLLECTOR_MAX_REG_METER; ++i) {
    if (!_meters[i].used) {
      continue;
    }
    if (sl_wisun_collector_compare_address(remote_addr, &_meters[i].addr)) {
      return &_meters[i];
    }
  }
  return NULL;
}

bool sl_wisun_collector_get_meter(const wisun_addr_t *meter_addr,
                                  sl_wisun_meter_entry_t * const dest_meter)
{
  sl_wisun_meter_entry_t *meter = NULL;
  bool retval = false;

  sl_wisun_mc_mutex_acquire(_collector_hnd);

  meter = _collector_get_meter_entry_by_address(meter_addr);

  if (meter != NULL) {
    memcpy(dest_meter, meter, sizeof(sl_wisun_meter_entry_t));
    retval = true;
  }

  sl_wisun_mc_mutex_release(_collector_hnd);

  return retval;
}
