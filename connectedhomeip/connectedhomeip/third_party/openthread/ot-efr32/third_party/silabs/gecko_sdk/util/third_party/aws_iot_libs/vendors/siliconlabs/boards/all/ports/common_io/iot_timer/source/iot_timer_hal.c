/***************************************************************************//**
 * @file    iot_timer_hal.c
 * @brief   Silicon Labs implementation of Common I/O Timer API.
 *******************************************************************************
 * # License
 * <b>Copyright 2020 Silicon Laboratories Inc. www.silabs.com</b>
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

/*******************************************************************************
 *                               INCLUDES
 ******************************************************************************/

/* FreeRTOS kernel layer */
#include "FreeRTOS.h"
#include "task.h"

/* SDK emlib layer */
#include "em_core.h"
#include "em_device.h"

/* SDK service layer */
#include "sl_sleeptimer.h"

/* Timer driver layer */
#include "iot_timer_desc.h"
#include "iot_timer_drv.h"

/* Timer interface layer */
#include "iot_timer.h"

/* Compiler agnostic definitions */
#include "cmsis_compiler.h"

/*---------------------------------------------------------------------------*/
#ifndef __ARM_ARCH_6M__
  #define __ARM_ARCH_6M__         0
#endif
#ifndef __ARM_ARCH_7M__
  #define __ARM_ARCH_7M__         0
#endif
#ifndef __ARM_ARCH_7EM__
  #define __ARM_ARCH_7EM__        0
#endif
#ifndef __ARM_ARCH_8M_MAIN__
  #define __ARM_ARCH_8M_MAIN__    0
#endif
#ifndef __ARM_ARCH_7A__
  #define __ARM_ARCH_7A__         0
#endif

#if   ((__ARM_ARCH_7M__      == 1U) \
  || (__ARM_ARCH_7EM__     == 1U)   \
  || (__ARM_ARCH_8M_MAIN__ == 1U))
#define IS_IRQ_MASKED()           ((__get_PRIMASK() != 0U) || (__get_BASEPRI() != 0U))
#elif  (__ARM_ARCH_6M__      == 1U)
#define IS_IRQ_MASKED()           (__get_PRIMASK() != 0U)
#elif (__ARM_ARCH_7A__       == 1U)
/* CPSR mask bits */
#define CPSR_MASKBIT_I            0x80U

#define IS_IRQ_MASKED()           ((__get_CPSR() & CPSR_MASKBIT_I) != 0U)
#else
#define IS_IRQ_MASKED()           (__get_PRIMASK() != 0U)
#endif

#if    (__ARM_ARCH_7A__      == 1U)
/* CPSR mode bitmasks */
#define CPSR_MODE_USER            0x10U
#define CPSR_MODE_SYSTEM          0x1FU

#define IS_IRQ_MODE()             ((__get_mode() != CPSR_MODE_USER) && (__get_mode() != CPSR_MODE_SYSTEM))
#else
#define IS_IRQ_MODE()             (__get_IPSR() != 0U)
#endif

#define IS_IRQ()                  (IS_IRQ_MODE() || (IS_IRQ_MASKED() && (xTaskGetSchedulerState() == taskSCHEDULER_RUNNING)))

/*******************************************************************************
 *                         MIDDLE-LAYER PROTOTYPES
 ******************************************************************************/

/* get timer elapsed time */
static uint64_t sl_timer_elapsed_ticks(IotTimerHandle_t const pxTimerHandle);
static uint64_t sl_timer_elapsed_us(IotTimerHandle_t const pxTimerHandle);

/* timer operation */
static void sl_timer_op_start(IotTimerHandle_t const pxTimerHandle);
static void sl_timer_op_stop(IotTimerHandle_t const pxTimerHandle);

/* timer counter/match */
static void sl_timer_counter_set(IotTimerHandle_t const pxTimerHandle,
                                 uint32_t ulDelayMicroSeconds);
static void sl_timer_counter_cancel(IotTimerHandle_t const pxTimerHandle);

/* timer handle validation */
static bool sl_timer_validate(IotTimerHandle_t pxTimer);

/*******************************************************************************
 *                  MIDDLE LAYER: SLEEP TIMER INTERFACE
 ******************************************************************************/

/************************** sl_timer_callback() *******************************/

static void sl_timer_callback(sl_sleeptimer_timer_handle_t *handle,
                              void *data)
{
  IotTimerHandle_t timer_handle;
  IotTimerCallback_t callback;
  void *context;

  timer_handle = (IotTimerDescriptor_t *)data;

  callback = timer_handle->callback;
  context = timer_handle->context;

  timer_handle->is_expired = true;

  if (!timer_handle->is_canceled) {
    if (callback != NULL) {
      callback(context);
    }
  }

  /* Increment the total tick count */
  timer_handle->total_elapsed_ticks += timer_handle->timeout_in_ticks;

  sl_sleeptimer_start_timer(handle,
                            UINT32_MAX,
                            sl_timer_callback,
                            timer_handle,
                            0u,
                            0u);
}

/************************ sl_timer_elapsed_ticks() ****************************/

static uint64_t sl_timer_elapsed_ticks(IotTimerHandle_t const pxTimerHandle)
{
  uint64_t elapsed_ticks;
  uint32_t remaining_ticks;

  sl_sleeptimer_get_timer_time_remaining(&pxTimerHandle->sleeptimer_handle,
                                         &remaining_ticks);

  if (!pxTimerHandle->is_expired) {
    elapsed_ticks = pxTimerHandle->timeout_in_ticks - remaining_ticks;
  } else {
    elapsed_ticks = UINT32_MAX - remaining_ticks;
  }

  return elapsed_ticks + pxTimerHandle->total_elapsed_ticks;
}

/************************* sl_timer_elapsed_us() ******************************/

static uint64_t sl_timer_elapsed_us(IotTimerHandle_t const pxTimerHandle)
{
  uint64_t elapsed_ticks;
  uint64_t elapsed_us;

  elapsed_ticks = sl_timer_elapsed_ticks(pxTimerHandle);
  elapsed_us = (elapsed_ticks*1000000) / sl_sleeptimer_get_timer_frequency();

  return elapsed_us;
}

/************************** sl_timer_op_start() *******************************/

static void sl_timer_op_start(IotTimerHandle_t const pxTimerHandle)
{
  if (pxTimerHandle->timeout_in_us == 0 ||
      pxTimerHandle->is_expired ||
      pxTimerHandle->is_canceled) {
    sl_sleeptimer_start_timer(&pxTimerHandle->sleeptimer_handle,
                              UINT32_MAX,
                              sl_timer_callback,
                              pxTimerHandle,
                              pxTimerHandle->default_priority,
                              0u);
  } else {
    sl_sleeptimer_start_timer(&pxTimerHandle->sleeptimer_handle,
                              pxTimerHandle->timeout_in_ticks,
                              sl_timer_callback,
                              pxTimerHandle,
                              0u,
                              0u);
  }
}

/************************** sl_timer_op_stop() ********************************/

static void sl_timer_op_stop(IotTimerHandle_t const pxTimerHandle)
{
  /* Remember the elapsed tick count */
  pxTimerHandle->total_elapsed_ticks = sl_timer_elapsed_ticks(pxTimerHandle);

  sl_sleeptimer_stop_timer(&pxTimerHandle->sleeptimer_handle);
}

/************************ sl_timer_counter_set() ******************************/

static void sl_timer_counter_set(IotTimerHandle_t const pxTimerHandle,
                                 uint32_t ulDelayMicroSeconds)
{
  pxTimerHandle->timeout_in_us = ulDelayMicroSeconds;
  pxTimerHandle->timeout_in_ticks =
   ((uint64_t)sl_sleeptimer_get_timer_frequency()*ulDelayMicroSeconds)/1000000;

  pxTimerHandle->is_expired = false;
  pxTimerHandle->is_canceled = false;

 if (pxTimerHandle->is_running) {
   sl_timer_op_stop(pxTimerHandle);
   sl_timer_op_start(pxTimerHandle);
 }
}

/*********************** sl_timer_counter_cancel() ****************************/

static void sl_timer_counter_cancel(IotTimerHandle_t const pxTimerHandle)
{
  pxTimerHandle->is_canceled = true;
}

/************************** sl_timer_validate() *******************************/

static bool sl_timer_validate(IotTimerHandle_t pxTimer)
{
  if (pxTimer == NULL) {
    return false;
  }
  return pxTimer->is_open;
}

/*******************************************************************************
 *                             iot_timer_open()
 ******************************************************************************/

/**
 * @brief   iot_timer_open is used to initialize the timer.
 *          This function will start the timer.
 *
 * @param[in]   lTimerInstance  instance of the timer to initialize.
 *
 * @return
 *   - Handle to IotTimerHandle_t on SUCCESS
 *   - NULL if
 *      - lTimerInstance is invalid
 *      - lTimerInstance is already open.
 */
IotTimerHandle_t iot_timer_open(int32_t lTimerInstance)
{
  IotTimerHandle_t handle = NULL;

  handle = iot_timer_desc_get(lTimerInstance);

  if (handle == NULL) {
    return NULL;
  }

  portENTER_CRITICAL();

  if (handle->is_open) {
    portEXIT_CRITICAL();
    return NULL;
  }

  handle->sleeptimer_handle.callback_data = NULL;
  handle->sleeptimer_handle.priority = 0;
  handle->sleeptimer_handle.option_flags = 0;
  handle->sleeptimer_handle.next = NULL;
  handle->sleeptimer_handle.callback = NULL;
  handle->sleeptimer_handle.timeout_periodic = 0;
  handle->sleeptimer_handle.delta = 0;

  handle->total_elapsed_ticks = 0;
  handle->timeout_in_us = 0;
  handle->timeout_in_ticks = 0;

  handle->callback = NULL;
  handle->context = NULL;
  handle->is_running = false;
  handle->is_expired = true;
  handle->is_canceled = true;

  handle->is_open = true;

  portEXIT_CRITICAL();

  return handle;
}

/*******************************************************************************
 *                          iot_timer_set_callback()
 ******************************************************************************/

/*!
 * @brief   iot_timer_set_callback is used to set the callback to be called when
 *          the timer reaches the count (delay) set by the caller.
 *          Callers can set the delay using the iot_timer_delay API.
 *
 * @note Newly set callback overrides the one previously set
 * @note This callback is per handle. Each instance has its own callback.
 *
 * @warning This function silently does nothing if either pxTimerHandle or
 *          xCallback handle are NULL.
 *
 * @param[in]   pxTimerHandle   handle to Timer interface returned in
 *                              iot_timer_open()
 * @param[in]   xCallback       The callback function to be called.
 * @param[in]   pvUserContext   The user context to be passed when callback is called.
 *
 */
void iot_timer_set_callback(IotTimerHandle_t const pxTimerHandle,
                            IotTimerCallback_t xCallback,
                            void * pvUserContext)
{
  portENTER_CRITICAL();

  if (!sl_timer_validate(pxTimerHandle) || xCallback == NULL) {
      portEXIT_CRITICAL();
      return;
  }

  pxTimerHandle->callback = xCallback;
  pxTimerHandle->context = pvUserContext;

  portEXIT_CRITICAL();
}

/*******************************************************************************
 *                            iot_timer_start()
 ******************************************************************************/

/*!
 * @brief   iot_timer_start is used to start the timer counter. This call only makes the
 *          timer counter running, and does not setup any match values etc..
 *
 * @param[in]   pxTimerHandle   handle to Timer interface returned in
 *                              iot_timer_open()
 *
 * @return
 *   - IOT_TIMER_SUCCESS on success
 *   - IOT_TIMER_INVALID_VALUE if pxTimerHandle is NULL
 */
int32_t iot_timer_start(IotTimerHandle_t const pxTimerHandle)
{
  portENTER_CRITICAL();

  if (!sl_timer_validate(pxTimerHandle)) {
      portEXIT_CRITICAL();
      return IOT_TIMER_INVALID_VALUE;
  }

  if (!pxTimerHandle->is_running) {
    sl_timer_op_start(pxTimerHandle);
    pxTimerHandle->is_running = true;
  }

  portEXIT_CRITICAL();

  return IOT_TIMER_SUCCESS;
}

/*******************************************************************************
 *                            iot_timer_stop()
 ******************************************************************************/

/*!
 * @brief   iot_timer_stop is used to stop the timer counter if the timer is running.
 *
 * @param[in]   pxTimerHandle    handle to Timer interface returned in
 *                              iot_timer_open()
 *
 * @return  returns IOT_TIMER_SUCCESS on success or returns
 *          one of IOT_TIMER_INVALID_VALUE, IOT_TIMER_FUNCTION_NOT_SUPPORTED on error.
 * @return
 *   - IOT_TIMER_SUCCESS on success
 *   - IOT_TIMER_INVALID_VALUE if pxTimerHandle is NULL.
 *   - IOT_TIMER_FUNCTION_NOT_SUPPORTED, if the free running timer on SoC cant be stopped.
 *   - IOT_TIMER_NOT_RUNNING if iot_timer_start has not been called.
 */
int32_t iot_timer_stop(IotTimerHandle_t const pxTimerHandle)
{
  portENTER_CRITICAL();

  if (!sl_timer_validate(pxTimerHandle)) {
    portEXIT_CRITICAL();
    return IOT_TIMER_INVALID_VALUE;
  }

  if (!pxTimerHandle->is_running) {
    portEXIT_CRITICAL();
    return IOT_TIMER_NOT_RUNNING;
  }

  sl_timer_op_stop(pxTimerHandle);
  pxTimerHandle->is_running = false;

  portEXIT_CRITICAL();

  return IOT_TIMER_SUCCESS;
}

/*******************************************************************************
 *                           iot_timer_get_value()
 ******************************************************************************/

/*!
 * @brief   iot_timer_get_value is used to get the current timer value in micro seconds.
 *
 * @param[in]   pxTimerHandle    handle to Timer interface returned in
 *                               iot_timer_open()
 * @param[out]  ullMicroSeconds  current timer count in microseconds.
 *
 * @return
 *   - IOT_TIMER_SUCCESS on success
 *   - IOT_TIMER_INVALID_VALUE if pxTimerHandle or ulMicroSeconds pointers are NULL
 *   - IOT_TIMER_NOT_RUNNING if timer hasn't been started.
 */
int32_t iot_timer_get_value(IotTimerHandle_t const pxTimerHandle,
                            uint64_t * ullMicroSeconds)
{
  uint32_t isrm;
  bool irq = false;
  int32_t rtn = IOT_TIMER_SUCCESS;

  if (IS_IRQ()) {
    isrm = portSET_INTERRUPT_MASK_FROM_ISR();
    irq = true;
  } else {
    portENTER_CRITICAL();
  }

  if (!sl_timer_validate(pxTimerHandle) || ullMicroSeconds == NULL) {
    rtn = IOT_TIMER_INVALID_VALUE;
    goto exit;
  }

  if (!pxTimerHandle->is_running) {
    rtn = IOT_TIMER_NOT_RUNNING;
    goto exit;
  }

  *ullMicroSeconds = sl_timer_elapsed_us(pxTimerHandle);

exit:
  if (irq) {
    portCLEAR_INTERRUPT_MASK_FROM_ISR(isrm);
  } else {
    portEXIT_CRITICAL();
  }

  return rtn;
}

/*******************************************************************************
 *                             iot_timer_delay()
 ******************************************************************************/

/*!
 * @brief   iot_timer_delay is used to set up a delay/wake-up time in microseconds.
 *          The caller can use this API to delay current execution until the specified microSeconds.
 *          A callback is called once the delay is expired (i,e the amount of microseconds is passed
 *          from the time the API is called).
 *          If no callback is registered by the caller, then once the delay is expired, caller cannot be
 *          notified, but this mechanism can be useful to wake up the target from sleep.
 *
 * @param[in]   pxTimerHandle       handle to Timer interface returned in
 *                                  iot_timer_open()
 * @param[in]   ulDelayMicroSeconds delay time in micro seconds
 *
 * <b>Example timer delay execution</b>
 * The callback function is called to signal when the timer reaches the count (delay)
 * set by the caller.
 * @code{c}
 *
 * IotTimerHandle_t xTimerHandle;
 *
 * xTimerHandle = iot_timer_open( ltestIotTimerInstance);
 * // assert( xTimerHandle == NULL );
 *
 * // Set the callback to call prvTimerCallbackFunction() when delay reached.
 * iot_timer_set_callback(xTimerHandle, prvTimerCallbackFunction, NULL);
 *
 * // Set the timer delay to be TIMER_DEFAULT_DELAY_US
 * lRetVal = iot_timer_delay(xTimerHandle, TIMER_DEFAULT_DELAY_US );
 * // assert( lRetVal != IOT_TIMER_SUCCESS );
 *
 * //Start the timer
 * lRetVal = iot_timer_start(xTimerHandle);
 * // assert ( lRetVal != IOT_TIMER_SUCCESS);
 *
 * // Wait for the Delay callback to be called.  Inside of prvTimerCallbackFunction()
 * // the function will use xSemaphoreGive() to signal completion.
 * lRetVal = xSemaphoreTake(IotTimerSemaphore, portMAX_DELAY);
 * // assert( lRetVal != TRUE );
 *
 * lRetVal = iot_timer_close(xTimerHandle);
 * //assert ( lRetVal != IOT_TIMER_SUCCESS);
 * @endcode
 *
 * @return
 *   - IOT_TIMER_SUCCESS on success
 *   - IOT_TIMER_INVALID_VALUE if pxTimerHandle or ulMicroSeconds pointers are NULL
 *   - IOT_TIMER_NOT_RUNNING if timer hasn't been started.
 *   - IOT_TIMER_SET_FAILED on error.
 */
int32_t iot_timer_delay(IotTimerHandle_t const pxTimerHandle,
                        uint32_t ulDelayMicroSeconds)
{
  int32_t ret = IOT_TIMER_SUCCESS;

  portENTER_CRITICAL();

  if (!sl_timer_validate(pxTimerHandle)) {
    portEXIT_CRITICAL();
    return IOT_TIMER_INVALID_VALUE;
  }

  sl_timer_counter_set(pxTimerHandle, ulDelayMicroSeconds);

  if (!pxTimerHandle->is_running) {
    ret = IOT_TIMER_NOT_RUNNING;
  }

  portEXIT_CRITICAL();

  return ret;
}

/*******************************************************************************
 *                            iot_timer_cancel()
 ******************************************************************************/

/**
 * @brief   iot_timer_cancel is used to cancel any existing delay call.
 *          If a call to iot_timer_delay() was made earlier, calling iot_timer_cancel
 *          will cancel that delay call, so no call-back will be called.
 *
 * @param[in]   pxTimerHandle   handle to Timer interface returned in
 *                              iot_timer_open()
 *
 * @return
 *   - IOT_TIMER_SUCCESS on success
 *   - IOT_TIMER_INVALID_VALUE if pxTimerHandle is NULL
 *   - IOT_TIMER_FUNCTION_NOT_SUPPORTED if timer can't be cancelled.
 *   - IOT_TIMER_NOTHING_TO_CANCEL if there is no timer running to cancel.
 */
int32_t iot_timer_cancel(IotTimerHandle_t const pxTimerHandle)
{
  portENTER_CRITICAL();

  if (!sl_timer_validate(pxTimerHandle)) {
    portEXIT_CRITICAL();
    return IOT_TIMER_INVALID_VALUE;
  }

  sl_timer_counter_cancel(pxTimerHandle);

  portEXIT_CRITICAL();

  return IOT_TIMER_SUCCESS;
}

/*******************************************************************************
 *                             iot_timer_close()
 ******************************************************************************/

/**
 * @brief   iot_timer_close is used to de-initializes the timer, stops the timer
 *          if it was started and cancels the delay calls, and resets the timer value.
 *
 * @param[in]   pxTimerHandle   handle to Timer interface returned in
 *                              iot_timer_open()
 *
 * @return
 *   - IOT_TIMER_SUCCESS on success
 *   - IOT_TIMER_INVALID_VALUE if
 *      - pxTimerHandle is NULL.
 *      - pxTimerHandle not open (previously closed).
 */
int32_t iot_timer_close(IotTimerHandle_t const pxTimerHandle)
{
  portENTER_CRITICAL();

  if (!sl_timer_validate(pxTimerHandle)) {
    portEXIT_CRITICAL();
    return IOT_TIMER_INVALID_VALUE;
  }

  sl_timer_op_stop(pxTimerHandle);

  pxTimerHandle->sleeptimer_handle.callback_data = NULL;
  pxTimerHandle->sleeptimer_handle.priority = 0;
  pxTimerHandle->sleeptimer_handle.option_flags = 0;
  pxTimerHandle->sleeptimer_handle.next = NULL;
  pxTimerHandle->sleeptimer_handle.callback = NULL;
  pxTimerHandle->sleeptimer_handle.timeout_periodic = 0;
  pxTimerHandle->sleeptimer_handle.delta = 0;

  pxTimerHandle->total_elapsed_ticks = 0;
  pxTimerHandle->timeout_in_us = 0;
  pxTimerHandle->timeout_in_ticks = 0;

  pxTimerHandle->callback = NULL;
  pxTimerHandle->context = NULL;
  pxTimerHandle->is_running = false;
  pxTimerHandle->is_expired = true;
  pxTimerHandle->is_canceled = true;

  pxTimerHandle->is_open = false;

  portEXIT_CRITICAL();

  return IOT_TIMER_SUCCESS;
}
