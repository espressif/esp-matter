/***************************************************************************//**
 * @file
 * @brief
 *******************************************************************************
 * # License
 * <b>Copyright 2022 Silicon Laboratories Inc. www.silabs.com</b>
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
#include <stddef.h>
#include <stdint.h>

#include "sl_iperf.h"
#include "sl_iperf_udp_srv.h"
#include "sl_iperf_udp_clnt.h"
#include "sl_iperf_util.h"

#if !defined(SL_IPERF_CMSIS_RTOS_DISABLED)
#include "em_common.h"
#include "cmsis_os2.h"
#include "sl_status.h"
#include "sl_cmsis_os2_common.h"
#endif
// -----------------------------------------------------------------------------
//                              Macros and Typedefs
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------
//                          Static Function Declarations
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------
//                                Static Variables
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------
//                          Public Function Definitions
// -----------------------------------------------------------------------------

#if !defined(SL_IPERF_CMSIS_RTOS_DISABLED)
/**************************************************************************//**
 * @brief iPerf UDP server thread function
 * @details Thread function
 *****************************************************************************/
static void _iperf_thr_fnc (void * args);

/**************************************************************************//**
 * @brief iperf UDP Server mutex acquire
 * @details Helper function
 *****************************************************************************/
static inline void _iperf_mutex_acquire(void);

/**************************************************************************//**
 * @brief iperf UDP Server mutex release
 * @details Helper function
 *****************************************************************************/
static inline void _iperf_mutex_release(void);

/**************************************************************************//**
 * @brief Convert OS status to bool value
 * @details Helper function
 * @param[in] status OS Status
 * @return true if status is osOK
 * @return false if status is NOT osOK
 *****************************************************************************/
static inline bool _os_status_to_bool(const osStatus_t status);

/// Thread ID
static osThreadId_t _iperf_thr = NULL;

/// Static task control block
__ALIGNED(8) static uint8_t _iperf_task_cb[osThreadCbSize] = { 0 };

/// Static stack allocation
__ALIGNED(8) static uint8_t _iperf_stack[SL_IPERF_STACK_SIZE] = { 0 };

/// LED task attribute
static const osThreadAttr_t _iperf_thr_attr = {
  .name        = "IperfThread",
  .attr_bits   = osThreadDetached,
  .cb_mem      = _iperf_task_cb,
  .cb_size     = sizeof(_iperf_task_cb),
  .stack_mem   = _iperf_stack,
  .stack_size  = sizeof(_iperf_stack),
  .priority    = osPriorityNormal7,
  .tz_module   = 0
};

/// Test request message queue ID
static osMessageQueueId_t _iperf_test_req_msg_queue = NULL;

/// Test request static message queue control block
__ALIGNED(8) static uint8_t _iperf_test_req_msg_queue_cb[osMessageQueueCbSize] = { 0 };

/// Test request static message queue storage allocation
static sl_iperf_test_t _iperf_test_req_msg_queue_buff[SL_IPERF_MESSAGE_QUEUE_SIZE] = { 0 };

/// Test request message queue attributes
static const osMessageQueueAttr_t _iperf_test_req_msg_queue_attr = {
  .name = "IperfReqMsgQueue",
  .attr_bits = 0,
  .cb_mem = _iperf_test_req_msg_queue_cb,
  .cb_size = sizeof(_iperf_test_req_msg_queue_cb),
  .mq_mem = _iperf_test_req_msg_queue_buff,
  .mq_size = sizeof(_iperf_test_req_msg_queue_buff)
};

/// Test result message queue ID
static osMessageQueueId_t _iperf_test_res_msg_queue = NULL;

/// Test result static message queue control block
__ALIGNED(8) static uint8_t _iperf_test_res_msg_queue_cb[osMessageQueueCbSize] = { 0 };

/// Test result static message queue storage allocation
static sl_iperf_test_t _iperf_test_res_msg_queue_buff[SL_IPERF_MESSAGE_QUEUE_SIZE] = { 0 };

/// Test result message queue attributes
static const osMessageQueueAttr_t _iperf_test_res_msg_queue_attr = {
  .name = "IperfResMsgQueue",
  .attr_bits = 0,
  .cb_mem = _iperf_test_res_msg_queue_cb,
  .cb_size = sizeof(_iperf_test_res_msg_queue_cb),
  .mq_mem = _iperf_test_res_msg_queue_buff,
  .mq_size = sizeof(_iperf_test_res_msg_queue_buff)
};

/// Mutex ID
static osMutexId_t _iperf_mtx = NULL;

/// Static mutex control block allocation
__ALIGNED(8) static uint8_t _iperf_mtx_cb[osMutexCbSize] = { 0 };

/// Mutex attribute
static const osMutexAttr_t _iperf_mtx_attr = {
  .name      = "IperfUdpSrvMtx",
  .attr_bits = osMutexRecursive,
  .cb_mem    = _iperf_mtx_cb,
  .cb_size   = sizeof(_iperf_mtx_cb)
};

#endif

/// Receive buffer for Server test
static uint8_t _iperf_buff[SL_IPERF_BUFFER_SIZE] = { 0 };

/// Default log instance
static sl_iperf_log_t _def_log = { 0 };

void sl_iperf_service_init(void)
{
  // init network interface
  sl_iperf_nw_interface_init();

#if !defined(SL_IPERF_CMSIS_RTOS_DISABLED)
  // init mutex
  _iperf_mtx = osMutexNew(&_iperf_mtx_attr);
  assert(_iperf_mtx != NULL);

  // init teset request msg queue
  _iperf_test_req_msg_queue = osMessageQueueNew(SL_IPERF_MESSAGE_QUEUE_SIZE,
                                                sizeof(sl_iperf_test_t),
                                                &_iperf_test_req_msg_queue_attr);
  assert(_iperf_test_req_msg_queue != NULL);

  // init teset result msg queue
  _iperf_test_res_msg_queue = osMessageQueueNew(SL_IPERF_MESSAGE_QUEUE_SIZE,
                                                sizeof(sl_iperf_test_t),
                                                &_iperf_test_res_msg_queue_attr);
  assert(_iperf_test_res_msg_queue != NULL);

  // init thread
  _iperf_thr = osThreadNew(_iperf_thr_fnc, NULL, &_iperf_thr_attr);
  assert(_iperf_thr != NULL);
#endif

  // init default log
  sl_iperf_log_init(&_def_log);
}

void sl_iperf_test_init(sl_iperf_test_t * const test, sl_iperf_mode_t mode, sl_iperf_protocol_t protocol)
{
  static uint32_t test_id = 0;

  if (test == NULL) {
    return;
  }
  // clear structure
  memset(test, 0, sizeof(sl_iperf_test_t));

  // set test id
  test->id = test_id++;

  // set mode
  test->opt.mode = mode;

  // reset callback
  test->cb = NULL;

  // set protocol
  test->opt.protocol = protocol;

  // set bandwidth format
  test->opt.bw_format = SL_IPERF_OPT_BW_FORMAT_BITS_PER_SEC;

  // set update interval
  test->opt.interval_ms = SL_IPERF_DEFAULT_INTERVAL_MS;

  // reset run indicator
  test->conn.run = false;

  // set connection buffer
  sl_iperf_test_set_default_buff(test);

  // init default logger instance
  sl_iperf_test_set_default_logger(test);
}

void sl_iperf_test_set_default_logger(sl_iperf_test_t * const test)
{
  test->log = &_def_log;
}

void sl_iperf_test_set_default_buff(sl_iperf_test_t * const test)
{
  test->conn.buff = _iperf_buff;
  test->conn.buff_size = SL_IPERF_BUFFER_SIZE;
}

#if !defined(SL_IPERF_CMSIS_RTOS_DISABLED)
bool sl_iperf_test_add(sl_iperf_test_t * const test)
{
  osStatus_t status = osError;
  status = osMessageQueuePut(_iperf_test_req_msg_queue, test, 0U, osWaitForever);
  return _os_status_to_bool(status);
}

bool sl_iperf_test_get(sl_iperf_test_t * const test)
{
  osStatus_t status = osError;
  uint8_t msg_prio = 0U;
  status = osMessageQueueGet(_iperf_test_res_msg_queue, (void *)test, &msg_prio, osWaitForever);
  return _os_status_to_bool(status);
}

static inline void _iperf_mutex_acquire(void)
{
  assert(osMutexAcquire(_iperf_mtx, osWaitForever) == osOK);
}

static inline void _iperf_mutex_release(void)
{
  assert(osMutexRelease(_iperf_mtx) == osOK);
}

/// Thread function declaration
static void _iperf_thr_fnc(void * args)
{
  sl_iperf_test_t test = { 0 };
  sl_iperf_test_t *pt  = &test;
  uint8_t msg_prio     =   0;
  osStatus_t status    = osError;

  (void) args;

  // wait for network connected state
  while (!sl_iperf_network_is_connected()) {
    sl_iperf_delay_ms(1);
  }

  SL_IPERF_SERVICE_LOOP() {
    // Pop Test from the queue
    status = osMessageQueueGet(_iperf_test_req_msg_queue, &test, &msg_prio, osWaitForever);
    if (status != osOK) {
      sl_iperf_delay_ms(1);
      continue;
    }

    // Force-Reset statistics
    memset(&test.statistic, 0, sizeof(sl_iperf_stats_t));

    // check network connection
    if (!sl_iperf_network_is_connected()) {
      sl_iperf_test_set_err_and_stat(&test, SL_IPERF_ERR_NETWORK_CONNECTION,
                                     SL_IPERF_TEST_STATUS_ERR);
      sl_iperf_test_log(pt, "Network not connected.\n");
      break;
    }

    // Lock reasources, execute particular test
    _iperf_mutex_acquire();
    switch (test.opt.mode) {
      case SL_IPERF_MODE_CLIENT:
        if (sl_iperf_test_is_udp_clnt(&test)) {
          // TODO
          sl_iperf_test_udp_client(&test);
        } else if (sl_iperf_test_is_tcp_clnt(&test)) {
          // TODO
          // Call TCP Client
          (void) 0L;
        } else {
          sl_iperf_test_log(pt, "Wrong Client mode/protocol setting.\n");
        }
        break;
      case SL_IPERF_MODE_SERVER:
        if (sl_iperf_test_is_udp_srv(&test)) {
          /// Call UDP Server
          sl_iperf_test_udp_server(&test);
        } else if (sl_iperf_test_is_tcp_srv(&test)) {
          // TODO
          // Call TCP Server
          (void) 0L;
        } else {
          sl_iperf_test_log(pt, "Wrong Server mode/protocol setting.\n");
        }
        break;
      default: break;
    }
    // Post test handler
    if (test.cb != NULL) {
      test.cb(&test);
    }

    // Release reasources
    _iperf_mutex_release();

    // Push test content to the queue
    osMessageQueuePut(_iperf_test_res_msg_queue, &test, 0U, osWaitForever);
  }
}

static inline bool _os_status_to_bool(const osStatus_t status)
{
  return status == osOK ? true : false;
}

#endif

// -----------------------------------------------------------------------------
//                          Static Function Definitions
// -----------------------------------------------------------------------------
