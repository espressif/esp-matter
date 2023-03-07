/***************************************************************************//**
 * @file    iot_power_hal.c
 * @brief   Silicon Labs implementation of Common I/O Power API.
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

/* SDK emlib layer */
#include "em_device.h"

/* SDK service layer */
#include "sl_power_manager.h"
#include "sl_atomic.h"

/* FreeRTOS kernel layer */
#include "FreeRTOS.h"
#include "semphr.h"
#include "string.h"

/* Common I/O interface layer */
#include "iot_power.h"

/* Power driver layer */
#include "iot_power_drv.h"

/*******************************************************************************
 *                             Internal types data
 ******************************************************************************/

typedef struct IotPowerDescriptor {
  bool is_open;
  bool cancel_idle;
  bool em1_requirement_added;
  IotPowerCallback_t callback;
  IotPowerMode_t power_mode;
  IotPowerMode_t pre_reset_power_mode;
  IotPowerIdleMode_t last_idle_mode;
  void *callback_user_context;
  uint32_t pc_threshold;
  uint32_t clkoff_threshold;
  TickType_t expected_idle;
} IotPowerDescriptor;

/*******************************************************************************
 *                            STATIC VARIABLES
 ******************************************************************************/

static IotPowerDescriptor initial_power_descriptor =
  INITIAL_POWER_DESCRIPTOR;

static IotPowerDescriptor power_descriptor =
  INITIAL_POWER_DESCRIPTOR;

/*******************************************************************************
 *                         MIDDLE-LAYER PROTOTYPES
 ******************************************************************************/

/* enable power management */
static void sl_power_enable(IotPowerHandle_t const pxPowerHandle);

/* disable power management */
static void sl_power_disable(IotPowerHandle_t const pxPowerHandle);

/* validate power handle */
static bool sl_power_validate(IotPowerHandle_t const pxPowerHandle);

/*******************************************************************************
 *                  MIDDLE LAYER: POWER MANAGER INTERFACE
 ******************************************************************************/

/*********************** sl_power_transition_cb() *****************************/

/**
 * @brief This callback is called when we enter or exit idle.
 */
static void sl_power_transition_cb(sl_power_manager_em_t from,
                                   sl_power_manager_em_t to)
{
  static bool previously_entered_em1_from_lower_em = false;

  if (power_descriptor.is_open == false) {
    return;
  }

  if (to == SL_POWER_MANAGER_EM1 && from != SL_POWER_MANAGER_EM0) {
    previously_entered_em1_from_lower_em = true;
  }

  if (to == SL_POWER_MANAGER_EM0) {
    if (power_descriptor.em1_requirement_added) {
      /* This is a special case that allows us to remove an EM requirement
       * within a notification. This should not be done in normal circumstances.
       */
      sl_power_manager_remove_em_requirement(SL_POWER_MANAGER_EM1);
      power_descriptor.em1_requirement_added = false;
    }

    if (power_descriptor.power_mode == eUnknownPowerMode) {
      power_descriptor.power_mode = power_descriptor.pre_reset_power_mode;
    }

    if (previously_entered_em1_from_lower_em) {
      power_descriptor.last_idle_mode = eIdleClkSrcOffModeMode;
    } else {
      switch (from) {
        case SL_POWER_MANAGER_EM0:
          power_descriptor.last_idle_mode = eIdleNoneMode;
          break;
        case SL_POWER_MANAGER_EM1:
          power_descriptor.last_idle_mode = eIdlePCMode;
          break;
        case SL_POWER_MANAGER_EM2:
          power_descriptor.last_idle_mode = eIdleClkSrcOffModeMode;
          break;
        case SL_POWER_MANAGER_EM3:
          power_descriptor.last_idle_mode = eIdleClkSrcOffModeMode;
          break;
       default:
          power_descriptor.last_idle_mode = eIdleNoneMode;
          break;
      }
    }

    if (power_descriptor.callback != NULL) {
      power_descriptor.callback(false, power_descriptor.callback_user_context);
    }

    previously_entered_em1_from_lower_em = false;
  }
}

/********************* POWER MANAGER EVENT VARIABLES **************************/

static sl_power_manager_em_transition_event_handle_t event_handle;
static sl_power_manager_em_transition_event_info_t event_info = {
  .event_mask = 0xFF,
  .on_event = sl_power_transition_cb,
};

/*************************** sl_power_enable() ********************************/

/**
 * @brief Function to enable power manager event handling when power module
 *        is opened.
 */
static void sl_power_enable(IotPowerHandle_t const pxPowerHandle)
{
  (void) pxPowerHandle;
  sl_power_manager_subscribe_em_transition_event(&event_handle, &event_info);
}

/************************** sl_power_disable() ********************************/

/**
 * @brief Function to disable power manager event handling when power module
 *        is closed. EM1 requirement is also removed if was added before.
 */
static void sl_power_disable(IotPowerHandle_t const pxPowerHandle)
{
  (void) pxPowerHandle;

  sl_power_manager_unsubscribe_em_transition_event(&event_handle);

  if (power_descriptor.em1_requirement_added) {
    sl_power_manager_remove_em_requirement(SL_POWER_MANAGER_EM1);
    power_descriptor.em1_requirement_added = false;
  }
}

/************************** sl_power_validate() *******************************/

/**
 * @brief Validate the power handle by making sure the pointer is not null
 *        or that it was not previously opened.
 */
static bool sl_power_validate(IotPowerHandle_t const pxPowerHandle)
{
  if (pxPowerHandle == NULL || !pxPowerHandle->is_open) {
    return false;
  }
  return true;
}

/*******************************************************************************
 *                    FREERTOS PORT POWER FUNCTIONS
 ******************************************************************************/

/****************** iot_power_set_expected_idle() *************************/

/**
 * @brief Set the expected_idle time (called before going into tickless sleep)
 */
void sli_iot_power_set_expected_idle(TickType_t expected_idle)
{
  power_descriptor.expected_idle = expected_idle;
}


/********************* iot_power_ok_to_sleep() ****************************/

/**
 * @brief Check if we can go in idle and also updare the EM requirement according
 *        to the configured thresholds.
 */
bool sli_iot_power_ok_to_sleep(void)
{
  bool em1_required = false;

  if (!power_descriptor.is_open) {
    return true;
  }

  if (power_descriptor.expected_idle < power_descriptor.pc_threshold) {
    return false;
  }

  /* Give one last chance to cancel the sleep by notifying the user */
  if ((eTaskConfirmSleepModeStatus() != eAbortSleep) &&
      (power_descriptor.callback != NULL)) {
    power_descriptor.cancel_idle = false;
    power_descriptor.callback(true, power_descriptor.callback_user_context);

    if (power_descriptor.cancel_idle) {
      return false;
    }
  }

  /* find out if EM1 is required */
  if ((power_descriptor.expected_idle >= power_descriptor.pc_threshold &&
       power_descriptor.clkoff_threshold == 0)
      ||
      (power_descriptor.expected_idle >= power_descriptor.pc_threshold &&
       power_descriptor.expected_idle < power_descriptor.clkoff_threshold)
      ||
      power_descriptor.power_mode != eUnknownPowerMode) {
    em1_required = true;
  }

  /* Check if we can go to sleep and if we need the EM1 requirement */
  if (eTaskConfirmSleepModeStatus() != eAbortSleep) {
    if (em1_required && !power_descriptor.em1_requirement_added) {
      sl_power_manager_add_em_requirement(SL_POWER_MANAGER_EM1);
      power_descriptor.em1_requirement_added = true;
    }
    return true;
  }

  return false;
}

/*******************************************************************************
 *                             iot_power_open()
 ******************************************************************************/

/**
 * @brief   iot_power_open is used to Initialize the CPU power management driver.
 *          power_open sets up the default mode to eNomralMode, so when the power_open is called,
 *          the caller must set the mode using set_mode to a specific mode of interest.
 *
 * @return
 *   - handle to power interface on success
 *   - NULL if already open
 */
IotPowerHandle_t iot_power_open(void)
{
  portENTER_CRITICAL();

  if (power_descriptor.is_open) {
    portEXIT_CRITICAL();
    return NULL;
  }

  power_descriptor.is_open = true;
  iot_power_set_mode(&power_descriptor, eNormalPowerPerfMode);

  sl_power_enable(&power_descriptor);

  portEXIT_CRITICAL();

  return &power_descriptor;
}

/*******************************************************************************
 *                           iot_power_set_mode()
 ******************************************************************************/

/**
 * @brief   iot_power_set_mode is used to set or change the power mode to a specific mode. The actual changes made as part
 *          of switching to a specific power mode are platform specific. When set_mode is called to switch
 *          from high mode to low mode, the target may go through several steps to enter low performance mode.
 *          The aggregation of switching to a particular mode must be made prior to calling this, as a call
 *          to this API may change the CPU frequency and voltage which may affect the software execution.
 *          This API is blocking and the mode switched in the same context as the caller and is a blocking call.
 *
 * @param[in]   pxPowerHandle   handle to power driver returned in
 *                              iot_power_open
 * @param[in]   xMode           power mode to be switched to.
 *
 * @return
 *   - IOT_POWER_SUCCESS on success
 *   - IOT_POWER_INVALID_VALUE if any parameter is invalid
 *   - IOT_POWER_SET_FAILED if unable to set to the desired mode
 *   - IOT_POWER_FUNCTION_NOT_SUPPORTED if specified mode is not supported.
 */
int32_t iot_power_set_mode(IotPowerHandle_t const pxPowerHandle,
                           IotPowerMode_t xMode)
{
  if (!sl_power_validate(pxPowerHandle)) {
    return IOT_POWER_SET_FAILED;
  }

  if (xMode == eLowPowerPerfMode || xMode == eHighPowerPerfMode) {
    return IOT_POWER_FUNCTION_NOT_SUPPORTED;
  }

  if (xMode != eNormalPowerPerfMode) {
    return IOT_POWER_INVALID_VALUE;
  }

  portENTER_CRITICAL();

  pxPowerHandle->power_mode = xMode;
  pxPowerHandle->cancel_idle = true;

  portEXIT_CRITICAL();
  return IOT_POWER_SUCCESS;
}

/*******************************************************************************
 *                           iot_power_reset_mode()
 ******************************************************************************/

/**
 * @brief   iot_power_reset_mode is used to reset the mode (i,e remove the vote to set mode). When reset mode is called,
 *          its implicitly notifying the OS to enter deepest low power mode possible in idle state based on
 *          existing timers. If a set_mode is called with any of the performance modes with out calling reset_mode,
 *          then when target enters idle state, it will not be able to enter deep power states since there is an
 *          existing vote for a performance mode from the caller. The target will still do clock gating of the core
 *          by doing a WFI and WFE.
 *          Additionally, if a reset_mode is called, followed by a get_mode, eUnKnownMode will be returned to the caller.
 *          But if reset_mode is called, and then target enters idle state, when coming out of the idle_state it will resume
 *          to a known previous state.
 *          For example, if we take the following flow:
 *          1. set_mode(eHighPerformanceMode)
 *          2. reset_modqe() -> At this point the current mode is set to eUnKnownMode
 *          3. Idle checks to see if it can enter a deep power state based on existing timers.
 *          4. If a callback is registered, the callback is called with bIdle set to "true"
 *          4a.  The callback can request a mode change causing idle to abort.
 *          5. Target enters Idle state and goes to 'some' deep power state
 *          6. Target wakes up due to external interrupt or scheduled timer interrupt.
 *          7. If a callback is registered, the callback is called with bIdle set to "false"
 *          8. At this point the caller can set the desired performance state.
 *          9. Target is set to eHighPerformanceMode as this was the known mode before entering idle. (optional if mode was not set at #6)
 *
 * @param[in]   pxPowerHandle   handle to power driver returned in
 *                              iot_power_open
 *
 * @return
 *   - IOT_POWER_SUCCESS on success
 *   - IOT_POWER_INVALID_VALUE on pxPowerHandle == NULL
 *   - IOT_POWER_FUNCTION_NOT_SUPPORTED if power reset not supported.
 */
int32_t iot_power_reset_mode(IotPowerHandle_t const pxPowerHandle)
{
  if (!sl_power_validate(pxPowerHandle)) {
    return IOT_POWER_INVALID_VALUE;
  }

  portENTER_CRITICAL();

  pxPowerHandle->pre_reset_power_mode = pxPowerHandle->power_mode;
  pxPowerHandle->power_mode = eUnknownPowerMode;

  portEXIT_CRITICAL();
  return IOT_POWER_SUCCESS;
}

/*******************************************************************************
 *                          iot_power_set_callback()
 ******************************************************************************/

/**
 * @brief   iot_power_set_callback is used to set the callback to be called when the target is entering low power mode,
 *          and when target is coming out of low power mode. Callback must not use any HW resources
 *          when this callback is called with out first requesting for a performance mode.
 *
 * @note Single callback is used for both entering and exiting low power mode.
 * @note Newly set callback overrides the one previously set
 *
 * @warning If input handle or if callback function is NULL, this function silently takes no action.
 *
 * @param[in]   pxPowerHandle   handle to power driver returned in
 *                              iot_power_open
 * @param[in]   xCallback       The callback function to be called.
 * @param[in]   pvUserContext   The user context to be passed when callback is called.
 */
void iot_power_set_callback(IotPowerHandle_t const pxPowerHandle,
                            IotPowerCallback_t xCallback,
                            void * pvUserContext)
{
  portENTER_CRITICAL();

  if (!sl_power_validate(pxPowerHandle) || xCallback == NULL) {
    portEXIT_CRITICAL();
    return;
  }

  pxPowerHandle->callback = xCallback;
  pxPowerHandle->callback_user_context = pvUserContext;
  portEXIT_CRITICAL();
}

/*******************************************************************************
 *                             iot_power_ioctl()
 ******************************************************************************/

/**
 * @brief   iot_power_ioctl is used to set power configuration and
 *          power properties like Wakeup time thresholds.
 *          Supported IOCTL requests are defined in iot_PowerIoctlRequest_t
 *
 * @param[in]   pxPowerHandle  handle to power driver returned in
 *                          iot_power_open()
 * @param[in]   xRequest    configuration request of type IotPowerIoctlRequest_t
 * @param[in,out] pvBuffer  buffer holding Power set and get values.
 *
 * @return
 *   - IOT_POWER_SUCCESS on success
 *   - IOT_POWER_INVALID_VALUE on any invalid parameter
 *   - IOT_POWER_FUNCTION_NOT_SUPPORTED for any IotPowerIoctlRequest_t not supported.
 *
 * @note:  For any Set ioctl that is supported, the corresponding Get ioctl must also be supported.
 */
int32_t iot_power_ioctl(IotPowerHandle_t const pxPowerHandle,
                        IotPowerIoctlRequest_t xRequest,
                        void * const pvBuffer)
{
  int32_t ret = IOT_POWER_SUCCESS;

  if (!sl_power_validate(pxPowerHandle) || pvBuffer == NULL) {
    return IOT_POWER_INVALID_VALUE;
  }

  switch (xRequest) {
    case eSetPCWakeThreshold:
      sl_atomic_store(pxPowerHandle->pc_threshold, *(uint32_t *)pvBuffer);
      break;

    case eGetPCWakeThreshold:
      sl_atomic_store(*(uint32_t *)pvBuffer, pxPowerHandle->pc_threshold);
      break;

    case eSetClkSrcOffWakeThreshold:
      sl_atomic_store(pxPowerHandle->clkoff_threshold, *(uint32_t *)pvBuffer);
      break;

    case eGetClkSrcOffWakeThreshold:
      sl_atomic_store(*(uint32_t *)pvBuffer, pxPowerHandle->clkoff_threshold);
      break;

    case eSetVddOfffWakeThreshold:
      ret = IOT_POWER_FUNCTION_NOT_SUPPORTED;
      break;

    case eGetVddOfffWakeThreshold:
      ret = IOT_POWER_FUNCTION_NOT_SUPPORTED;
      break;

    case eSetWakeupSources:
      ret = IOT_POWER_FUNCTION_NOT_SUPPORTED;
      break;

    case eGetWakeupSources:
      ret = IOT_POWER_FUNCTION_NOT_SUPPORTED;
      break;

    case eGetLastIdleMode:
      sl_atomic_store(*(IotPowerIdleMode_t *)pvBuffer,
                      pxPowerHandle->last_idle_mode);
      break;

    default:
      ret = IOT_POWER_INVALID_VALUE;
      break;
  }

  return ret;
}

/*******************************************************************************
 *                            iot_power_get_mode()
 ******************************************************************************/

/**
 * @brief   iot_power_get_mode is used to get the current power mode the target is in.
 *
 * @param[in]   pxPowerHandle   handle to power driver returned in
 *                              iot_power_open
 * @param[out]  xMode      current power mode.
 *
 * @return
 *   - IOT_POWER_SUCCESS on success
 *   - IOT_POWER_INVALID_VALUE on pxPowerHandle or xMode being NULL
 *   - IOT_POWER_GET_FAILED on error.
 */
int32_t iot_power_get_mode(IotPowerHandle_t const pxPowerHandle,
                           IotPowerMode_t * xMode)
{
  if (!sl_power_validate(pxPowerHandle) || xMode == NULL) {
    return IOT_POWER_GET_FAILED;
  }

  sl_atomic_store(*xMode, pxPowerHandle->power_mode);

  return IOT_POWER_SUCCESS;
}

/*******************************************************************************
 *                             iot_power_close()
 ******************************************************************************/

/**
 * @brief   iot_power_close is used to de-initialize power driver.
 *
 * @param[in]   pxPowerHandle   handle to power driver returned in
 *                              iot_power_open
 *
 * @return
 *   - IOT_POWER_SUCCESS on success
 *   - IOT_POWER_INVALID_VALUE if
 *      - pxPowerHandle == NULL
 *      - not in open state (already closed).
 */
int32_t iot_power_close(IotPowerHandle_t const pxPowerHandle)
{

  if (pxPowerHandle == NULL) {
    return IOT_POWER_INVALID_VALUE;
  } else if (pxPowerHandle->is_open == false) {
    return IOT_POWER_NOT_INITIALIZED;
  }

  portENTER_CRITICAL();
  sl_power_disable(pxPowerHandle);
  pxPowerHandle->callback = NULL;
  pxPowerHandle->callback_user_context = NULL;
  pxPowerHandle->is_open = false;
  power_descriptor = initial_power_descriptor;
  portEXIT_CRITICAL();

  return IOT_POWER_SUCCESS;
}
