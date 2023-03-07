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
#include <stddef.h>
#include <stdio.h>
#include <inttypes.h>
#include "cmsis_os2.h"
#include "sl_status.h"
#include "sl_cmsis_os2_common.h"
#include "sl_wisun_led_driver.h"
#include "sl_simple_led.h"
#include "sl_simple_led_instances.h"
#include "sl_wisun_trace_util.h"

// -----------------------------------------------------------------------------
//                              Macros and Typedefs
// -----------------------------------------------------------------------------

/// LED message queue size
#define LED_MESSAGE_QUEUE_SIZE      (16U)

/// LED task stack size
#define LED_STACK_SIZE              (80U)

// -----------------------------------------------------------------------------
//                          Static Function Declarations
// -----------------------------------------------------------------------------

/**************************************************************************//**
 * @brief LED task function
 * @details Task function
 * @param[in] arg not used
 *****************************************************************************/
static void _led_task(void *arg);

/**************************************************************************//**
 * @brief Get next LED state
 * @details Use a modifiable LED signal, decrement the values,
 *          and set the H and L values at the end of the period
 * @param[in,out] led_signal Modifyable LED signal
 * @param[in] ref_signal Reference signal
 * @return true LED turn on
 * @return false LED turn off
 *****************************************************************************/
static bool _get_next_led_state(sl_wisun_led_signal_t *led_signal,
                                const sl_wisun_led_signal_t * const ref_signal);

/**************************************************************************//**
 * @brief Turn off LED0 and LED1
 * @details
 *****************************************************************************/
static inline void _turn_off_leds(void);

/**************************************************************************//**
 * @brief Set LED instance by state
 * @details Use simple led instances
 * @param[in] led_sate LED state
 * @param[in,out] led_instance LED instance
 *****************************************************************************/
static inline void _set_led_instance(const bool led_sate,
                                     const sl_led_t *led_instance);

/**************************************************************************//**
 * @brief LED mutex acquire
 * @details Helper function
 *****************************************************************************/
static inline void _led_mutex_acquire(void);

/**************************************************************************//**
 * @brief LED mutex release
 * @details Helper function
 *****************************************************************************/
static inline void _led_mutex_release(void);

/**************************************************************************//**
 * @brief Get LED signal internal pointer
 * @details Helper function
 * @param[in] led_id
 * @return sl_wisun_led_signal_t*
 *****************************************************************************/
static inline sl_wisun_led_signal_t *_get_led_signal_ptr(const sl_wisun_led_id_t led_id);

// -----------------------------------------------------------------------------
//                                Global Variables
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------
//                                Static Variables
// -----------------------------------------------------------------------------

/// Static task control block
__ALIGNED(8) static uint8_t _led_task_cb[osThreadCbSize] = { 0 };

///  Static message queue control block
__ALIGNED(8) static uint8_t _led_msg_queue_cb[osMessageQueueCbSize] = { 0 };

/// Static task stack
static uint64_t _led_stack[LED_STACK_SIZE] = { 0 };

/// LED thread id
static osThreadId_t _led_thr_id = NULL;

///  Message queue
static osMessageQueueId_t _led_msg_queue = NULL;

///  Static led signal buffer
static sl_wisun_led_signal_t _buff[LED_MESSAGE_QUEUE_SIZE] = { 0 };

/// Meter-Collector mutex
static osMutexId_t _led_mtx = { NULL };

/// Meter-Collector mutex control block
__ALIGNED(8) static uint8_t _led_mtx_cb[osMutexCbSize] = { 0 };

/// LED task attribute
static const osThreadAttr_t _led_task_attr = {
  .name        = "WisunLedTask",
  .attr_bits   = osThreadDetached,
  .cb_mem      = _led_task_cb,
  .cb_size     = sizeof(_led_task_cb),
  .stack_mem   = _led_stack,
  .stack_size  = sizeof(_led_stack),
  .priority    = osPriorityBelowNormal,
  .tz_module   = 0
};

/// LED message queue attribute
static const osMessageQueueAttr_t _led_msg_queue_attr = {
  .name = "LedMsgQueue",
  .attr_bits = 0,
  .cb_mem = _led_msg_queue_cb,
  .cb_size = sizeof(_led_msg_queue_cb),
  .mq_mem = _buff,
  .mq_size = sizeof(_buff)
};

/// Led mutex attribute
static const osMutexAttr_t _led_mtx_attr = {
  .name      = "LedMutex",
  .attr_bits = osMutexRecursive,
  .cb_mem    = _led_mtx_cb,
  .cb_size   = sizeof(_led_mtx_cb)
};

/// LED0 signal internal instance
static sl_wisun_led_signal_t _led0 = { 0 };

/// LED1 signal internal instance
static sl_wisun_led_signal_t _led1 = { 0 };

// -----------------------------------------------------------------------------
//                          Public Function Definitions
// -----------------------------------------------------------------------------

/* Set LED */
void sl_wisun_led_set(const sl_wisun_led_signal_t *led_signal)
{
  osMessageQueuePut(_led_msg_queue, led_signal, 0U, 0U);
}

/* Clear LED */
void sl_wisun_led_clear(const sl_wisun_led_id_t led_id)
{
  sl_wisun_led_signal_t tmp_signal = {
    .id = led_id,
    .high_ms = 0,
    .low_ms = 0,
    .period = 0
  };
  osMessageQueuePut(_led_msg_queue, &tmp_signal, 0U, 0U);
}

/* LED driver init */
void sl_wisun_led_driver_init(void)
{
  assert(sl_led_init(&sl_led_led0) == SL_STATUS_OK);
  assert(sl_led_init(&sl_led_led1) == SL_STATUS_OK);

  _turn_off_leds();

  _led_mtx = osMutexNew(&_led_mtx_attr);
  assert(_led_mtx != NULL);

  _led_msg_queue = osMessageQueueNew(LED_MESSAGE_QUEUE_SIZE,
                                     sizeof(sl_wisun_led_signal_t),
                                     &_led_msg_queue_attr);
  assert(_led_msg_queue != NULL);

  _led_thr_id = osThreadNew(_led_task, NULL, &_led_task_attr);
  assert(_led_thr_id != NULL);
}

sl_status_t sl_wisun_led_get_signal(const sl_wisun_led_id_t led_id,
                                    sl_wisun_led_signal_t * const dest)
{
  sl_wisun_led_signal_t *led = NULL;

  led = _get_led_signal_ptr(led_id);

  if (led == NULL || dest == NULL) {
    return SL_STATUS_FAIL;
  }

  _led_mutex_acquire();
  memcpy(dest, led, sizeof(sl_wisun_led_signal_t));
  _led_mutex_release();

  return SL_STATUS_OK;
}

sl_status_t sl_wisun_led_toggle(const sl_wisun_led_id_t led_id)
{
  sl_wisun_led_signal_t *led = NULL;
  sl_wisun_led_signal_t tmp_signal = {
    .id = led_id,
    .high_ms = 0,
    .low_ms = 0,
    .period = SL_WISUN_LED_PERIOD_CONTINOUS_SIGNAL
  };

  led = _get_led_signal_ptr(led_id);

  if (led == NULL) {
    return SL_STATUS_FAIL;
  }

  if (led->high_ms) {
    tmp_signal.high_ms = 0;
    tmp_signal.low_ms = 100;
  } else {
    tmp_signal.high_ms = 100;
    tmp_signal.low_ms = 0;
  }

  osMessageQueuePut(_led_msg_queue, &tmp_signal, 0U, 0U);

  return SL_STATUS_OK;
}

/* Terminate LED thread */
sl_status_t sl_wisun_led_terminate(void)
{
  osStatus_t status;

  _led_mutex_acquire();
  sl_led_turn_off(&sl_led_led0);
  sl_led_turn_off(&sl_led_led1);
  _led_mutex_release();

  status = osThreadTerminate(_led_thr_id);
  if (status == osOK) {
    return SL_STATUS_OK;
  }
  return SL_STATUS_FAIL;
}

// -----------------------------------------------------------------------------
//                          Static Function Definitions
// -----------------------------------------------------------------------------

/* Get next state */
static bool _get_next_led_state(sl_wisun_led_signal_t *led_signal,
                                const sl_wisun_led_signal_t * const ref_signal)
{
  bool res = false;
  if (led_signal->period) {
    if (led_signal->high_ms) {
      res = true;
      --led_signal->high_ms;
    } else if (led_signal->low_ms) {
      res = false;
      --led_signal->low_ms;
    } else {
      // -1: infinit repeat
      if (led_signal->period != SL_WISUN_LED_PERIOD_CONTINOUS_SIGNAL) {
        --led_signal->period;
      }
      // reset low and high values
      led_signal->low_ms = ref_signal->low_ms;
      led_signal->high_ms = ref_signal->high_ms;
    }
  } else {
    res = false;
  }
  return res;
}

/* LED task */
static void _led_task(void *arg)
{
  sl_wisun_led_signal_t led_msg  = { 0 }; // incomming led message
  sl_wisun_led_signal_t led0_ref = { 0 }; // LED0 reference storage
  sl_wisun_led_signal_t led1_ref = { 0 }; // LED1 reference storage
  uint8_t msg_prio = 0;
  bool led0_state  = false;                // led0 state
  bool led1_state  = false;                // led1 state
  osStatus_t stat;

  (void) arg;

  SL_WISUN_THREAD_LOOP {
    // check msg queue
    stat = osMessageQueueGet(_led_msg_queue, &led_msg, &msg_prio, 0U);

    _led_mutex_acquire();

    if (stat == osOK) {
      // invalid period count ( < -1 )
      if (led_msg.period < SL_WISUN_LED_PERIOD_CONTINOUS_SIGNAL) {
        continue;
      }
      // set leds
      switch (led_msg.id) {
        case SL_WISUN_LED0_ID:
          memcpy(&led0_ref, &led_msg, sizeof(led_msg));
          memcpy(&_led0, &led_msg, sizeof(led_msg));
          break;
        case SL_WISUN_LED1_ID:
          memcpy(&led1_ref, &led_msg, sizeof(led_msg));
          memcpy(&_led1, &led_msg, sizeof(led_msg));
          break;
        default: break;
      }
    }

    // get led states
    led0_state = _get_next_led_state(&_led0, &led0_ref);
    led1_state = _get_next_led_state(&_led1, &led1_ref);

    //handle leds
    _set_led_instance(led0_state, &sl_led_led0);
    _set_led_instance(led1_state, &sl_led_led1);

    _led_mutex_release();

    // 1 ms delay
    osDelay(1);
  }
}

/* Turn off LEDs */
static inline void _turn_off_leds(void)
{
  sl_led_turn_off(&sl_led_led0);
  sl_led_turn_off(&sl_led_led1);
}

/* Set LED instance */
static inline void _set_led_instance(const bool led_sate,
                                     const sl_led_t *led_instance)
{
  if (led_sate) {
    sl_led_turn_on(led_instance);
  } else {
    sl_led_turn_off(led_instance);
  }
}

/* Mutex acquire */
static inline void _led_mutex_acquire(void)
{
  assert(osMutexAcquire(_led_mtx, osWaitForever) == osOK);
}

/* Mutex release */
static inline void _led_mutex_release(void)
{
  assert(osMutexRelease(_led_mtx) == osOK);
}

static inline sl_wisun_led_signal_t *_get_led_signal_ptr(const sl_wisun_led_id_t led_id)
{
  switch (led_id) {
    case SL_WISUN_LED0_ID: return &_led0;
    case SL_WISUN_LED1_ID: return &_led1;
    default:               return NULL;
  }
}
