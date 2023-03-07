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
#include <string.h>
#include <stdio.h>
#include "sl_wisun_ping.h"
#include "cmsis_os2.h"
#include "sl_cmsis_os2_common.h"
#include "socket_hnd.h"
#include "sl_sleeptimer.h"
#include "sl_wisun_trace_util.h"

// -----------------------------------------------------------------------------
//                              Macros and Typedefs
// -----------------------------------------------------------------------------

/// Ping service stack size
#define PING_STACK_SIZE                  (288U)

/// Ping stack size in bytes
#define PING_STACK_SIZE_BYTES            ((PING_STACK_SIZE * sizeof(void *)) & 0xFFFFFFF8U)

/// Ping TCP delay between connect and send
#define PING_TCP_CONNECT_SEND_TIMEOUT_MS (10000U)

// -----------------------------------------------------------------------------
//                          Static Function Declarations
// -----------------------------------------------------------------------------

/**************************************************************************//**
 * @brief Ping mutex acquire
 * @details Mutex handler function
 *****************************************************************************/
static inline void _ping_mutex_acquire(void);

/**************************************************************************//**
 * @brief Ping mutex release
 * @details Mutex handler function
 *****************************************************************************/
static inline void _ping_mutex_release(void);

/**************************************************************************//**
 * @brief Fill ping packet payload
 * @details Helper function
 * @param[in,out] icmp_req
 *****************************************************************************/
static inline void _fill_payload(sl_wisun_ping_echo_request_t * const icmp_req);

/**************************************************************************//**
 * @brief Compare request fields with response fields
 * @details Helper function
 * @param[in] req Request
 * @param[in] resp
 * @return true Request and response are mached
 * @return false Comparision failed
 *****************************************************************************/
static inline bool _compare_req_resp(const sl_wisun_ping_echo_request_t * const req,
                                     const sl_wisun_ping_echo_response_t * const resp);

/**************************************************************************//**
 * @brief Get milisec value of time spent from start time stamp.
 * @details Helper function
 * @param[in] ping Ping packet
 * @return uint32_t Milisec value
 *****************************************************************************/
static inline uint32_t _get_ms_val_from_start_time_stamp(const sl_wisun_ping_info_t * const ping);

/**************************************************************************//**
 * @brief Preapre failed response
 * @details Helper function
 * @param[out] resp Destinatino response
 * @param[in] status_flags Status flags
 *****************************************************************************/
static inline void _prepare_and_push_failed_response(sl_wisun_ping_info_t * const resp, const uint32_t status_flags);

/**************************************************************************//**
 * @brief Ping task function
 * @details Service task function implemented for handling ICMP packets
 * @param[in] args Arguments (empty)
 *****************************************************************************/
static void _ping_task_fnc(void *args);

// -----------------------------------------------------------------------------
//                                Static Variables
// -----------------------------------------------------------------------------

/// Internal ping socket id
static int32_t _sockid = SOCKET_INVALID_ID;

/// Internal ICMPv6 request structure
static sl_wisun_ping_echo_request_t _icmp_req = { 0 };

/// Internal ICMPv6 response structure
static sl_wisun_ping_echo_response_t _icmp_resp = { 0 };

/// Ping task control block static allocation
__ALIGNED(8) static uint8_t _ping_task_cb[sizeof(osThread_t)] = { 0 };

/// Ping stack static allocation
__ALIGNED(8) static uint8_t _ping_stack[PING_STACK_SIZE_BYTES] = { 0 };

/// Ping task attributions
static  const osThreadAttr_t _ping_task_attr = {
  .name        = "Ping",
  .attr_bits   = osThreadDetached,
  .cb_mem      = _ping_task_cb,
  .cb_size     = sizeof(_ping_task_cb),
  .stack_mem   = _ping_stack,
  .stack_size  = PING_STACK_SIZE_BYTES,
  .priority    = osPriorityNormal2,
  .tz_module   = 0
};

/// Ping request message queue
static osMessageQueueId_t _ping_req_msg_queue = NULL;

/// Ping request message queue buffer
static sl_wisun_ping_info_t _ping_req_buff[SL_WISUN_PING_MAX_REQUEST_RESPONSE] = { 0 };

/// Ping request message queue control block
__ALIGNED(8) static uint8_t _ping_req_msg_queue_cb[osMessageQueueCbSize] = { 0 };

/// Ping request messagequeue attributions
static const osMessageQueueAttr_t _ping_req_msg_queue_attr = {
  .name = "PingReqMsgQueue",
  .attr_bits = 0,
  .cb_mem = _ping_req_msg_queue_cb,
  .cb_size = sizeof(_ping_req_msg_queue_cb),
  .mq_mem = _ping_req_buff,
  .mq_size = sizeof(_ping_req_buff)
};

/// Ping response message queue
static osMessageQueueId_t _ping_resp_msg_queue = NULL;

/// Ping response message queue buffer
static sl_wisun_ping_info_t _ping_resp_buff[SL_WISUN_PING_MAX_REQUEST_RESPONSE] = { 0 };

/// Ping response message queue control block
__ALIGNED(8) static uint8_t _ping_resp_msg_queue_cb[osMessageQueueCbSize] = { 0 };

/// Ping response message queue attributes
static const osMessageQueueAttr_t _ping_resp_msg_queue_attr = {
  .name = "PingRespMsgQueue",
  .attr_bits = 0,
  .cb_mem = _ping_resp_msg_queue_cb,
  .cb_size = sizeof(_ping_resp_msg_queue_cb),
  .mq_mem = _ping_resp_buff,
  .mq_size = sizeof(_ping_resp_buff)
};

/// Ping thread id
static osThreadId_t _ping_thr_id;

/// Ping mutex
static osMutexId_t _ping_mtx;

/// Ping mutex control block
__ALIGNED(8) static uint8_t _ping_mtx_cb[osMutexCbSize] = { 0 };

/// Ping mutex attributes
static const osMutexAttr_t _ping_mtx_attr = {
  .name      = "PingMutex",
  .attr_bits = osMutexRecursive,
  .cb_mem    = _ping_mtx_cb,
  .cb_size   = sizeof(_ping_mtx_cb)
};

// -----------------------------------------------------------------------------
//                          Public Function Definitions
// -----------------------------------------------------------------------------

/* Ping init */
void sl_wisun_ping_init(void)
{
  // init mutex
  _ping_mtx = osMutexNew(&_ping_mtx_attr);
  assert(_ping_mtx != NULL);
  _ping_req_msg_queue = osMessageQueueNew(SL_WISUN_PING_MAX_REQUEST_RESPONSE,
                                          sizeof(sl_wisun_ping_info_t),
                                          &_ping_req_msg_queue_attr);
  assert(_ping_req_msg_queue != NULL);

  _ping_resp_msg_queue = osMessageQueueNew(SL_WISUN_PING_MAX_REQUEST_RESPONSE,
                                           sizeof(sl_wisun_ping_info_t),
                                           &_ping_resp_msg_queue_attr);
  assert(_ping_resp_msg_queue != NULL);
  _ping_thr_id = osThreadNew(_ping_task_fnc, NULL, &_ping_task_attr);
  assert(_ping_thr_id != NULL);
}

/* Ping request */
void sl_wisun_ping_request(const sl_wisun_ping_info_t * const ping_request)
{
  osMessageQueuePut(_ping_req_msg_queue, ping_request, 0U, 0U);
}

/* Ping response */
void sl_wisun_ping_response(sl_wisun_ping_info_t * const ping_response)
{
  uint8_t msg_prio = 0;
  osMessageQueueGet(_ping_resp_msg_queue, ping_response, &msg_prio, osWaitForever);
  (void) msg_prio;
}

/* Ping */
bool sl_wisun_ping(const wisun_addr_t *const remote_addr,
                   const uint16_t packet_count,
                   const uint16_t packet_length,
                   sl_wisun_ping_stat_hnd_t stat_hnd,
                   sl_wisun_ping_req_resp_done_hnd_t req_resp_sent_hnd)
{
  sl_wisun_ping_info_t req   = { 0 };
  sl_wisun_ping_info_t resp  = { 0 };
  static uint16_t id         =   0;
  uint16_t packet_ok_count   =   0;
  bool res                   = true;
  char rem_ip_str[40]        = { 0 };
  sl_wisun_ping_stat_t stat  = { 0 };
  bool min_max_ref_set           = false;

  // Check args
  if (!packet_count || !packet_length || remote_addr == NULL) {
    return false;
  }

  // fill statistic
  stat.packet_count  = packet_count;
  stat.lost          = 0;
  stat.min_time_ms   = 0;
  stat.max_time_ms   = 0;
  stat.avg_time_ms   = 0;

  memcpy(&req.remote_addr, remote_addr, sizeof(wisun_addr_t));
  memcpy(&stat.remote_addr, remote_addr, sizeof(wisun_addr_t));

  req.identifier = id++;
  req.sequence_number = 1;
  req.packet_length = packet_length;

  // check packet length, modify if it's necessary to avoid buffer underflow/overflow
  req.packet_length = req.packet_length < SL_WISUN_PING_MIN_PACKET_LENGTH ? SL_WISUN_PING_MIN_PACKET_LENGTH : req.packet_length;
  req.packet_length = req.packet_length > SL_WISUN_PING_MAX_PACKET_LENGTH ? SL_WISUN_PING_MAX_PACKET_LENGTH : req.packet_length;
  stat.packet_length = req.packet_length;

  if (inet_ntop(AF_WISUN, &req.remote_addr.sin6_addr,
                rem_ip_str, sizeof(sockaddr_storage_t)) == NULL) {
    return false;
  }

  _ping_mutex_acquire();
  printf("PING %s: %u data bytes\r\n",
         rem_ip_str, req.packet_length);
  _ping_mutex_release();

  for (uint16_t i = 0; i < packet_count; ++i) {
    sl_wisun_ping_request(&req);
    sl_wisun_ping_response(&resp);

    if (resp.status & SL_WISUN_PING_STATUS_INTERRUPT_REQUESTED) {
      return false;
    }

    _ping_mutex_acquire();
    if (resp.lost) {
      printf("[Destination is unreachable]\n");
    } else {
      printf("[%u bytes from %s: seq=%u time=%lu ms]\r\n",
             resp.packet_length,
             rem_ip_str,
             htons(resp.sequence_number),
             resp.response_time_ms);
    }
    _ping_mutex_release();

    req.sequence_number++;

    if (resp.lost) {
      ++stat.lost;
    } else {
      stat.avg_time_ms += resp.response_time_ms;
      if (!min_max_ref_set) {
        stat.min_time_ms = resp.response_time_ms;
        stat.max_time_ms = resp.response_time_ms;
        min_max_ref_set = true;
      } else {
        stat.min_time_ms = stat.min_time_ms > resp.response_time_ms ? resp.response_time_ms : stat.min_time_ms;
        stat.max_time_ms = stat.max_time_ms < resp.response_time_ms ? resp.response_time_ms : stat.max_time_ms;
      }
    }
    _ping_mutex_acquire();
    if (req_resp_sent_hnd != NULL) {
      req_resp_sent_hnd(&req, &resp);
    }
    _ping_mutex_release();
    osDelay(SL_WISUN_PING_PACKET_INTERVAL);
  }

  if (stat.lost) {
    res = false;
  }

  packet_ok_count = packet_count - stat.lost;
  if (packet_ok_count) {
    stat.avg_time_ms /= packet_ok_count;
  } else {
    stat.avg_time_ms = 0;
  }

  _ping_mutex_acquire();
  if (stat_hnd == NULL) {
    printf("\nPing statistics for %s:\n", rem_ip_str);
    printf("  Packets: Sent = %d, Received = %d, Lost = %d, (%d%% loss)\n",
           packet_count, packet_count - stat.lost, stat.lost, (stat.lost * 100) / packet_count);
    printf("Approximate round trip times in milli-seconds:\n");
    printf("  Minimum = %lums, Maximum = %lums, Average = %lums\n\n",
           stat.min_time_ms, stat.max_time_ms, stat.avg_time_ms);
  } else {
    stat_hnd(&stat);
  }
  _ping_mutex_release();

  return res;
}

// -----------------------------------------------------------------------------
//                          Static Function Definitions
// -----------------------------------------------------------------------------

/* Ping task function */
static void _ping_task_fnc(void *args)
{
  osStatus_t stat              = osError;
  sl_wisun_ping_info_t req     = { 0 };
  sl_wisun_ping_info_t resp    = { .lost = true };
  wisun_addr_t tmp_remote_addr = { 0 };
  uint32_t time_cnt            =   0;
  socklen_t len                = sizeof(wisun_addr_t);
  int32_t r                    = RETVAL_ERROR;
  uint8_t msg_prio             = 0;
  (void) args;

  _ping_mutex_acquire(); // lock()

  _icmp_req.type = SL_WISUN_PING_TYPE_ECHO_REQUEST;
  _icmp_req.code = SL_WISUN_PING_CODE_ECHO_REQUEST;
  _icmp_req.checksum = 0;

  // fill payload
  _fill_payload(&_icmp_req);

  _ping_mutex_release(); // unlock()

  SL_WISUN_THREAD_LOOP {
    stat = osMessageQueueGet(_ping_req_msg_queue, &req, &msg_prio, 0U);
    (void) msg_prio;
    if (stat != osOK) {
      osDelay(1);
      continue;
    }
    memset(&resp, 0, sizeof(resp));
    resp.lost = true;

    if (req.status & SL_WISUN_PING_STATUS_INTERRUPT_REQUESTED) {
      osMessageQueueReset(_ping_req_msg_queue);
      _prepare_and_push_failed_response(&resp, SL_WISUN_PING_STATUS_INTERRUPT_REQUESTED);
      continue;
    }
    _ping_mutex_acquire(); // lock()

    // create socket and slect port
    req.remote_addr.sin6_family = AF_WISUN;
    req.remote_addr.sin6_port = htons(SL_WISUN_PING_ICMP_PORT);

    _sockid = socket(AF_WISUN, SOCK_RAW, IPPROTO_ICMP);

    if (_sockid == SOCKET_INVALID_ID) {
      _prepare_and_push_failed_response(&resp, SL_WISUN_PING_STATUS_SOCKET_ERROR);
      continue;
    }

    // reset previous response
    memset(&_icmp_resp, 0, sizeof(_icmp_resp));
    _icmp_req.identifier = htons(req.identifier);
    _icmp_req.sequence_number = app_wisun_trace_swap_u16(req.sequence_number);

    // send request
    r = sendto(_sockid, (const void *) &_icmp_req, req.packet_length, 0,
               (const struct sockaddr *)&req.remote_addr, sizeof(wisun_addr_t));
    if (r == RETVAL_ERROR) {
      _prepare_and_push_failed_response(&resp, SL_WISUN_PING_STATUS_SEND_ERROR);
      close(_sockid);
      continue;
    }

    // create timestamp after send/sendto
    req.start_time_stamp = sl_sleeptimer_get_tick_count();
    // calculate time milisec counter
    time_cnt = _get_ms_val_from_start_time_stamp(&req);

    while (time_cnt < SL_WISUN_PING_TIMEOUT_MS) {
      // receive response
      r = recvfrom(_sockid, (void *)&_icmp_resp, req.packet_length, 0,
                   (struct sockaddr *)&tmp_remote_addr, &len);
      if (r > 0) {
        break;
      }
      osDelay(1);
      time_cnt = _get_ms_val_from_start_time_stamp(&req);
    }

    // init response with the request content
    memcpy(&resp, &req, sizeof(sl_wisun_ping_info_t));
    // address comparison and packet validation
    if (!memcmp(&tmp_remote_addr.sin6_addr.s6_addr,
                &req.remote_addr.sin6_addr.s6_addr,
                sizeof(req.remote_addr.sin6_addr.s6_addr))
        && _compare_req_resp(&_icmp_req, &_icmp_resp)) {
      resp.lost = false;
    } else {
      resp.lost = true;
    }

    resp.stop_time_stamp = sl_sleeptimer_get_tick_count();
    resp.response_time_ms = sl_sleeptimer_tick_to_ms(resp.stop_time_stamp - resp.start_time_stamp);

    close(_sockid);
    _ping_mutex_release(); // unlock()

    // put response into the message queue
    osMessageQueuePut(_ping_resp_msg_queue, &resp, 0U, 0U);
    osDelay(1); // thread dispatch
  }
}

/* Compare request response */
static inline bool _compare_req_resp(const sl_wisun_ping_echo_request_t * const req, const sl_wisun_ping_echo_response_t * const resp)
{
  return req->identifier == resp->identifier
         && req->sequence_number == resp->sequence_number
         && req->type == SL_WISUN_PING_TYPE_ECHO_REQUEST
         && resp->type == SL_WISUN_PING_TYPE_ECHO_RESPONSE ? true : false;
}

/* Prepare and push failed response */
static inline void _prepare_and_push_failed_response(sl_wisun_ping_info_t * const resp, const uint32_t status_flags)
{
  resp->start_time_stamp = 0;
  resp->stop_time_stamp = 0;
  resp->lost = true;
  resp->status |= status_flags;
  osMessageQueuePut(_ping_resp_msg_queue, resp, 0U, 0U);
}

/* Mutex acquire */
static inline void _ping_mutex_acquire(void)
{
  assert(osMutexAcquire(_ping_mtx, osWaitForever) == osOK);
}

/* Mutex release */
static inline void _ping_mutex_release(void)
{
  assert(osMutexRelease(_ping_mtx) == osOK);
}

/* Get milisec value from start time stamp*/
static inline uint32_t _get_ms_val_from_start_time_stamp(const sl_wisun_ping_info_t * const ping)
{
  return sl_sleeptimer_tick_to_ms(sl_sleeptimer_get_tick_count() - ping->start_time_stamp);
}

/* Fill payload */
static inline void _fill_payload(sl_wisun_ping_echo_request_t * const icmp_req)
{
  // fill payload
  char c = '0';
  for (uint16_t i = 0; i < SL_WISUN_PING_MAX_PAYLOAD_LENGTH; ++i) {
    if (c > '9') {
      c = '0';
    }
    icmp_req->payload[i] = c;
    ++c;
  }
}

void sl_wisun_ping_stop(void)
{
  sl_wisun_ping_info_t ping_req = { 0 };
  ping_req.status |= SL_WISUN_PING_STATUS_INTERRUPT_REQUESTED;
  osThreadSuspend(_ping_thr_id);
  osMessageQueueReset(_ping_req_msg_queue);
  osMessageQueueReset(_ping_resp_msg_queue);
  sl_wisun_ping_request(&ping_req);
  osThreadResume(_ping_thr_id);
}
