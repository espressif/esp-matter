/***************************************************************************//**
 * @file    iot_watchdog_hal.c
 * @brief   Silicon Labs implementation of Common I/O WatchDog API.
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
#include "em_wdog.h"
#include "em_cmu.h"

/* FreeRTOS kernel layer */
#include "FreeRTOS.h"
#include "semphr.h"
#include "string.h"

/* Watchdog driver layer */
#include "iot_watchdog_desc.h"
#include "iot_watchdog_drv.h"

/* Watchdog interface layer */
#include "iot_watchdog.h"

/*******************************************************************************
 *                                MACROS
 ******************************************************************************/

#define EFM32_ULFRCO_FREQ (1000UL)

/*******************************************************************************
 *                           validate_watchdog_handle()
 ******************************************************************************/

/**
 * @brief Helper function to reduce the size of recurring validation logic.
 */
static bool validate_wdog_handle(IotWatchdogHandle_t const pxWatchdogHandle)
{
  if (pxWatchdogHandle == NULL) {
    return false;
  }
  return pxWatchdogHandle->is_open;
}

/*******************************************************************************
 *                           set_warn_time()
 ******************************************************************************/

/**
 * @brief This helper function is used to compute the warningTime, aka the bark
 *        time, and put its value in the init struct of the handle for future
 *        call to WDOGn_Init.
 *
 * @note The watchdog hardware only supports the bark time as a percentage of
 *       the bite time as discrete values of 25, 50 and 75%.
 *
 * @note This function assumes it is running in an atomic context.
 *
 * @note This is a helper function where its use is controlled. In order to
 *       not duplicate assertion logic, bite_time_ms IS ASSUMED to be set
 *       and pxWatchdogHandle is assumed to be not NULL and opened.
 *
 */
static void set_warn_time(IotWatchdogHandle_t pxWatchdogHandle)
{
#if (_SILICON_LABS_32B_SERIES == 0)
   /* unused parameter */
   (void) pxWatchdogHandle;
#else
  if (pxWatchdogHandle->bark_time_ms == 0) {
    pxWatchdogHandle->em_init.warnSel = wdogWarnDisable;

    /* Return now, otherwise will divide by 0 */
    return;
  }

  /* bark time HAD to be less than bite time. percent will be less than 1.0f */
  float ratio = (float)pxWatchdogHandle->bark_time_ms
                / (float)pxWatchdogHandle->bite_time_ms;

  if (ratio <= 0.25f) {
    pxWatchdogHandle->em_init.warnSel = wdogWarnTime25pct;
  } else if (ratio <= 0.50f) {
    pxWatchdogHandle->em_init.warnSel = wdogWarnTime50pct;
  } else {
    /* In the case of everything between 50 and 100%, there is nothing we can
     * do but set it to 75% */
    pxWatchdogHandle->em_init.warnSel = wdogWarnTime75pct;
  }
#endif
}

/*******************************************************************************
 *                           set_bite_time()
 ******************************************************************************/

/**
 * @brief Helper function to set the bite time. Per the ioctl function
 *        description, it is legal to set any bite time when bark time is not
 *        (yet, maybe never) set. If bark time was already set via a previous
 *        call to IOCTL, ensure bite time is greater. The emlib driver for the
 *        watchdog only accepts a certain number of discrete values for the
 *        compare value to adjust the period of the timeout (bite). The desired
 *        bite time in milliseconds won't be exact because of this limitation.
 *        This function will select the first period value where the desired bite
 *        time will fit completely in it, ensuring at leat bite_time_ms will
 *        have passed.
 *
 * @note This function assumes it is running in an atomic context
 *
 * @return
 *   - IOT_WATCHDOG_SUCCESS on success
 *   - IOT_WATCHDOG_INVALID_VALUE if pxWatchdogHandle is NULL or not opened.
 *                                if bark time was set and bite_time_ms is lower
 *                                or equal to it.
 *
 */
static int32_t set_bite_time(IotWatchdogHandle_t pxWatchdogHandle, uint32_t bite_time_ms)
{
  WDOG_PeriodSel_TypeDef periodSelect;

  if (!validate_wdog_handle(pxWatchdogHandle)) {
    return IOT_WATCHDOG_INVALID_VALUE;
  }

  if (pxWatchdogHandle->bark_time_ms == 0) {
    /* Bark time is not set. Bite time can be anything */
    pxWatchdogHandle->bite_time_ms = bite_time_ms;

    /* There is the possibility the bark time will never be set if the user
     * doesn't want to use it. But because the reset value of the warnTime is
     * wdogWarnDisable, it's not mandatory to call sli_set_warn_time since it
     * will set it to that value again */
  } else {
    if (bite_time_ms <= pxWatchdogHandle->bark_time_ms) {
      /* Bite time invalid in respect to bark time */
      return IOT_WATCHDOG_INVALID_VALUE;
    } else {
      /* Bite time valid. */
      pxWatchdogHandle->bite_time_ms = bite_time_ms;

      /* Bark time was previously set, but since it HAD to wait
       * for bite time in order to compute warnTime, we know for sure we have
       * to do it now by calling sli_set_warn_time */
      set_warn_time(pxWatchdogHandle);
    }
  }

#if (_SILICON_LABS_32B_SERIES < 2)
  uint32_t wdog_freq = EFM32_ULFRCO_FREQ;
#else
  uint32_t wdog_freq = CMU_ClockFreqGet(pxWatchdogHandle->cmu_clock);
#endif

  wdog_freq *= bite_time_ms;

  uint32_t clock_cycles = wdog_freq / 1000;

  uint32_t clock_cycles_remainder = clock_cycles % 1000;

  clock_cycles += (clock_cycles_remainder > 500) ? 1 : 0;

  if (clock_cycles <= 9) {
    periodSelect = wdogPeriod_9;
  } else if (clock_cycles <= 17) {
    periodSelect = wdogPeriod_17;
  } else if (clock_cycles <= 33) {
    periodSelect = wdogPeriod_33;
  } else if (clock_cycles <= 65) {
    periodSelect = wdogPeriod_65;
  } else if (clock_cycles <= 129) {
    periodSelect = wdogPeriod_129;
  } else if (clock_cycles <= 257) {
    periodSelect = wdogPeriod_257;
  } else if (clock_cycles <= 513) {
    periodSelect = wdogPeriod_513;
  } else if (clock_cycles <= 1025) {
    periodSelect = wdogPeriod_1k;
  } else if (clock_cycles <= 2049) {
    periodSelect = wdogPeriod_2k;
  } else if (clock_cycles <= 4097) {
    periodSelect = wdogPeriod_4k;
  } else if (clock_cycles <= 8193) {
    periodSelect = wdogPeriod_8k;
  } else if (clock_cycles <= 16385) {
    periodSelect = wdogPeriod_16k;
  } else if (clock_cycles <= 32769) {
    periodSelect = wdogPeriod_32k;
  } else if (clock_cycles <= 65537) {
    periodSelect = wdogPeriod_64k;
  } else if (clock_cycles <= 131073) {
    periodSelect = wdogPeriod_128k;
  } else if (clock_cycles <= 262145) {
    periodSelect = wdogPeriod_256k;
  } else {
    /* With the current watchdog clock source, the desired period cannot be met.
     * Pick the largest period possible. */
    periodSelect = wdogPeriod_256k;
  }

  pxWatchdogHandle->em_init.perSel = periodSelect;

  return IOT_WATCHDOG_SUCCESS;
}

/*******************************************************************************
 *                           set_bark_time()
 ******************************************************************************/

/**
 * @brief Helper function to set the bark time. Per the ioctl function
 *        description, it is legal to set any bark time when bite time is not
 *        yet set. If bite time was already set via a previous call to IOCTL
 *        ensure bark time is lower. The emlib driver (the hardware) for the
 *        watchdog only accepts values of 25%, 50% and 75% of the bite time as
 *        compare valued. The desired bark time in milliseconds won't be exact
 *        because of this limitation. This function will select the first choice
 *        for which the bark delay
 *
 * @note This function assumes it is running in an atomic context
 *
 * @return
 *   - IOT_WATCHDOG_SUCCESS on success
 *   - IOT_WATCHDOG_INVALID_VALUE if pxWatchdogHandle is NULL or not opened.
 *                                if bite time was set and bark_time_ms is higher
 *                                or equal to it.
 *
 */
static int32_t set_bark_time(IotWatchdogHandle_t pxWatchdogHandle, uint32_t bark_time_ms)
{
  if (!validate_wdog_handle(pxWatchdogHandle)) {
    return IOT_WATCHDOG_INVALID_VALUE;
  }

  if (pxWatchdogHandle->bite_time_ms == 0) {
    /* Bite time has not been configured yet. For this reason, the bark time
     * provided is used as is with no sanity check (bark time must
     * be lower than bite time, but we don't know it yet)*/
    pxWatchdogHandle->bark_time_ms = bark_time_ms;

    /* We need bite time in order to compute the warning time for emlib driver
     * since bark time (warning time) is a percentage of bite time. Don't
     * compute the warning time just yet, it will be when set_bite_time
     * is called from sli_set_bite_time. Since it's mandatory to set bite time
     * before starting the watchdog, warning time is guaranteed to be set in the
     * future via the call to sli_set_warn_time in sli_set_bite_time
     */
  } else {
    if (bark_time_ms >= pxWatchdogHandle->bite_time_ms) {
      /* Bark time invalid in respect to bite time */
      return IOT_WATCHDOG_INVALID_VALUE;
    } else {
      /* Bite time valid */
      pxWatchdogHandle->bark_time_ms = bark_time_ms;

      /* Bite time is set and bark time is valid, compute the warning time */
      set_warn_time(pxWatchdogHandle);
    }
  }

  return IOT_WATCHDOG_SUCCESS;
}

/*******************************************************************************
 *                           set_bite_config()
 ******************************************************************************/

/**
 * @brief Helper function to set the correct flag for 'resetDiasable' in the
 *        watchdog init struct.
 *
 * @note This function assumes it executes in an atomic context
 *
 * @return
 *   - IOT_WATCHDOG_SUCCESS on success
 *   - IOT_WATCHDOG_INVALID_VALUE if pxWatchdogHandle is NULL or not opened.
 *   - IOT_WATCHDOG_TIME_NOT_SET if bite_config is out of the switch
 */
static int32_t set_bite_config(IotWatchdogHandle_t pxWatchdogHandle,
                               IotWatchdogBiteConfig_t bite_config)
{
  if (!validate_wdog_handle(pxWatchdogHandle)) {
    return IOT_WATCHDOG_INVALID_VALUE;
  }

#if (_SILICON_LABS_32B_SERIES >= 1)
  switch (bite_config) {
    case eWatchdogBiteTimerReset:
      pxWatchdogHandle->em_init.resetDisable = false;
      break;

    case eWatchdogBiteTimerInterrupt:
      pxWatchdogHandle->em_init.resetDisable = true;
      break;

    default:
      /* Should never get here */
      return IOT_WATCHDOG_TIME_NOT_SET;
  }
#endif

  pxWatchdogHandle->bite_config = bite_config;

  return IOT_WATCHDOG_SUCCESS;
}

/*******************************************************************************
 *                           wdog_irq_handler()
 ******************************************************************************/

#if (_SILICON_LABS_32B_SERIES >= 1)
/**
 * @brief watchdog interrupt handler. Only deals with bark and bite interrupt
 * source.
 */
static void wdog_irq_handler(IotWatchdogHandle_t const pxWatchdogHandle)
{
  uint32_t sources;

  /* Only deal with pending interrupts which are enabled */
  sources = WDOGn_IntGetEnabled(pxWatchdogHandle->em_instance);

  WDOGn_IntClear(pxWatchdogHandle->em_instance, sources);

  /* Watchdog bite */
  if (sources & _WDOG_IEN_TOUT_MASK) {
    pxWatchdogHandle->status = eWatchdogTimerBiteExpired;

    if (pxWatchdogHandle->bite_config == eWatchdogBiteTimerInterrupt) {
      if (pxWatchdogHandle->callback != NULL) {
        pxWatchdogHandle->callback(pxWatchdogHandle->context);
      }
    }
  }

  /* Watchdog bark */
  if (sources & _WDOG_IEN_WARN_MASK) {
    pxWatchdogHandle->status = eWatchdogTimerBarkExpired;

    if (pxWatchdogHandle->callback != NULL) {
      pxWatchdogHandle->callback(pxWatchdogHandle->context);
    }
  }

  /* The rest of the interrupt sources are not relevant for this API */
  if (sources & _WDOG_IEN_WIN_MASK) {
  }

  if (sources & _WDOG_IEN_PEM0_MASK) {
  }

  if (sources & _WDOG_IEN_PEM1_MASK) {
  }
}

void WDOG0_IRQHandler(void)
{
  int32_t lInstNum = -1;

  /* search for matched descriptor */
  while (iot_watchdog_desc_get(++lInstNum)->em_instance != WDOG0);

  /* call consolidated IRQ handler */
  wdog_irq_handler(iot_watchdog_desc_get(lInstNum));
}

#if WDOG_COUNT > 1
void WDOG1_IRQHandler(void)
{
  int32_t lInstNum = -1;

  /* search for matched descriptor */
  while (iot_watchdog_desc_get(++lInstNum)->em_instance != WDOG1);

  /* call consolidated IRQ handler */
  wdog_irq_handler(iot_watchdog_desc_get(lInstNum));
}
#endif
#endif

/*******************************************************************************
 *                             iot_watchdog_open()
 ******************************************************************************/

/**
 * @brief   iot_watchdog_open is used to initialize the WatchDog,
 *          This function will stop the timer if it was started and resets the timer
 *          if any was configured earlier.
 *
 * @param[in]   lWatchdogInstance   The instance of the WatchDog to initialize.
 *
 * @return
 *   - Handle to IotWatchdogHandle_t on success
 *   - NULL if
 *      - invalid lWatchdogInstance
 *      - instance is already open
 */
IotWatchdogHandle_t iot_watchdog_open(int32_t lWatchdogInstance)
{
  IotWatchdogHandle_t handle;
  WDOG_Init_TypeDef   init_default = WDOG_INIT_DEFAULT;

  /* retrieve watchdog handle by instance number */
  handle = iot_watchdog_desc_get(lWatchdogInstance);

  /* validate handle */
  if (handle == NULL) {
    return NULL;
  }

  portENTER_CRITICAL();

  if (handle->is_open) {
    portEXIT_CRITICAL();
    return NULL;
  }

  CMU_ClockEnable(handle->cmu_clock, true);

  handle->em_init              = init_default;

  handle->em_init.enable       = false;                      /* Don't start watchdog when initialization completed. */
  handle->em_init.debugRun     = false;                      /* WDOG is not counting during debug halt. */
  handle->em_init.em2Run       = true;                       /* WDOG will be counting when in EM2. */
  handle->em_init.em3Run       = true;                       /* WDOG will be counting when in EM3. */
  handle->em_init.em4Block     = false;                      /* EM4 can be entered. */
  handle->em_init.lock         = false;                      /* Do not lock WDOG configuration. */
  handle->em_init.perSel       = wdogPeriod_256k;            /* Set longest possible timeout period. */
#if (_SILICON_LABS_32B_SERIES >= 1)
  handle->em_init.warnSel      = wdogWarnDisable;            /* Disable warning (bark) */
  handle->em_init.winSel       = wdogIllegalWindowDisable;   /* Disable illegal window interrupt. */
  /* resetDisable is in accordance with default biteConfig which is eWatchdogBiteTimerReset */
  handle->em_init.resetDisable = false;                      /* Reset the device when timeout occurs. */
#endif
#if (_SILICON_LABS_32B_SERIES < 2)
  handle->em_init.clkSel       = wdogClkSelULFRCO;           /* Select ULFRCO (1KHz) clock source. */
#endif
#if (_SILICON_LABS_32B_SERIES == 2)
  CMU_ClockSelectSet(handle->cmu_clock, cmuSelect_ULFRCO);
#endif

  /* Make sure the timer is stopped at the hardware level and the counter at reset value */
  WDOGn_Enable(handle->em_instance, false);

  handle->status        = eWatchdogTimerStopped;
  handle->callback      = NULL;
  handle->context       = NULL;
  handle->bite_config   = eWatchdogBiteTimerReset;
  handle->bark_time_ms  = 0;
  handle->bite_time_ms  = 0;

  handle->is_open = true;

  portEXIT_CRITICAL();

  return handle;
}

/*******************************************************************************
 *                           iot_watchdog_start()
 ******************************************************************************/
/**
 * @brief   iot_watchdog_start is used to start the WatchDog timer counter.
 *          WatchDog expiry (bite) time must be set before starting the WatchDog counter.
 *          Set the bite time using eSetBiteTime IOCTL.
 *
 * @param[in]   pxWatchdogHandle handle to WatchDog interface returned in
 *                              iot_watchdog_open.
 *
 * @return
 *   - IOT_WATCHDOG_SUCCESS on success
 *   - IOT_WATCHDOG_INVALID_VALUE if pxWatchdogHandle is NULL
 *   - IOT_WATCHDOG_TIME_NOT_SET if bite time has not been set.
 */
int32_t iot_watchdog_start(IotWatchdogHandle_t const pxWatchdogHandle)
{
#if (_SILICON_LABS_32B_SERIES >= 1)
  uint32_t int_en_flags = 0;
#endif

  portENTER_CRITICAL();

  if (!validate_wdog_handle(pxWatchdogHandle)) {
    portEXIT_CRITICAL();
    return IOT_WATCHDOG_INVALID_VALUE;
  }

  if (pxWatchdogHandle->bite_time_ms == 0) {
    portEXIT_CRITICAL();
    return IOT_WATCHDOG_TIME_NOT_SET;
  }

#if (_SILICON_LABS_32B_SERIES >= 1)
  /* If requested bite behavior is interrupt (not reset),
   * enable timeout interrupt */
  if (pxWatchdogHandle->bite_config == eWatchdogBiteTimerInterrupt) {
    int_en_flags |= WDOG_IEN_TOUT;
  }

  /* If a bark time is specified, enable the warning interrupt */
  if (pxWatchdogHandle->bark_time_ms != 0) {
    int_en_flags |= WDOG_IEN_WARN;
  }
#endif

  /* At this point, the watchdog has a valid configuration that can be
   * pushed to the hardware. At minimum, the bite time had to be configured
   * (it has been checked above), every other configuration is either default
   * (which is valid) or set and validated at the same time. */
  WDOGn_Init(pxWatchdogHandle->em_instance, &pxWatchdogHandle->em_init);

  /* Make sure the watchdog couter is reset to 0 */
  WDOGn_Feed(pxWatchdogHandle->em_instance);

#if (_SILICON_LABS_32B_SERIES >= 1)
  /* Clear any previous interrupts */
  WDOGn_IntClear(pxWatchdogHandle->em_instance, _WDOG_IEN_MASK);
  NVIC_ClearPendingIRQ(pxWatchdogHandle->irq_num);

  /* Enable watchdog interrupt */
  WDOGn_IntEnable(pxWatchdogHandle->em_instance, int_en_flags);
  NVIC_EnableIRQ(pxWatchdogHandle->irq_num);
#endif

  /* Start the counter */
  WDOGn_Enable(pxWatchdogHandle->em_instance, true);

  pxWatchdogHandle->status = eWatchdogTimerRunning;

  portEXIT_CRITICAL();

  return IOT_WATCHDOG_SUCCESS;
}

/*******************************************************************************
 *                            iot_watchdog_stop()
 ******************************************************************************/

/**
 * @brief   iot_watchdog_stop is used to stop and resets the WatchDog timer counter.
 *          After stopping the timer and before starting the timer again,
 *          expireTime must be set.
 *
 * @param[in]   pxWatchdogHandle handle to WatchDog interface returned in
 *                              iot_watchdog_open.
 *
 * @return
 *   - IOT_WATCHDOG_SUCCESS on success
 *   - IOT_WATCHDOG_INVALID_VALUE if pxWatchdogHandle is NULL
 *   - IOT_WATCHDOG_NOT_SUPPORTED if stop operation not supported.
 */
int32_t iot_watchdog_stop(IotWatchdogHandle_t const pxWatchdogHandle)
{
  portENTER_CRITICAL();

  if (!validate_wdog_handle(pxWatchdogHandle)) {
    portEXIT_CRITICAL();
    return IOT_WATCHDOG_INVALID_VALUE;
  }

  /* stop the timer */
  WDOGn_Enable(pxWatchdogHandle->em_instance, false);

  /* reset the timer */
  WDOGn_Feed(pxWatchdogHandle->em_instance);

  /* In order to enforce the necessity of re-setting the expireTime after a
   * stop and before a start, clear the value here. It will have to be set again
   * with IOCTL otherwise subsequent start will fail.
   */
  pxWatchdogHandle->bite_time_ms = 0;

  /* Reset the bark time as well because it is with its value (0==?)
   * that the function iot_watchdog_start knows if it has to activate the
   * bark functionality (ie enabling its interrupt)*/
  pxWatchdogHandle->bark_time_ms = 0;

  pxWatchdogHandle->status = eWatchdogTimerStopped;

  portEXIT_CRITICAL();

  return IOT_WATCHDOG_SUCCESS;
}

/*******************************************************************************
 *                           iot_watchdog_restart()
 ******************************************************************************/

/**
 * @brief   iot_watchdog_restart is used to restart the WatchDog timer to the
 *          originally programmed values. This function is usually used
 *          once the WatchDog timer (either bark or bite) expired and generated
 *          a callback, so caller can restart the timer to original values to restart
 *          the WatchDog timer. The main difference b/w iot_watchdog_start and iot_watchdog_restart
 *          APIs are, the former requires the time values are set using the IOCTLs and the latter
 *          re-uses the already programmed values and re-programs them. If restart_timer is used
 *          without first setting the timers, it will return an error.
 *
 * @param[in]   pxWatchdogHandle handle to WatchDog interface returned in
 *                              iot_watchdog_open.
 *
 * @return
 *   - IOT_WATCHDOG_SUCCESS on success
 *   - IOT_WATCHDOG_INVALID_VALUE if pxWatchdogHandle is NULL
 *   - IOT_WATCHDOG_TIME_NOT_SET if watchdog bark or bite time have not been set.
 */
int32_t iot_watchdog_restart(IotWatchdogHandle_t const pxWatchdogHandle)
{
  portENTER_CRITICAL();

  if (!validate_wdog_handle(pxWatchdogHandle)) {
    portEXIT_CRITICAL();
    return IOT_WATCHDOG_INVALID_VALUE;
  }

  /* If bark and bite time are both 0, either the watchdog has not been configured yet
   * or it has been stoped. Either way, it cannot be restarted in this state */
  if (pxWatchdogHandle->bite_time_ms == 0 && pxWatchdogHandle->bark_time_ms == 0) {
    portEXIT_CRITICAL();
    return IOT_WATCHDOG_TIME_NOT_SET;
  }

  pxWatchdogHandle->status = eWatchdogTimerRunning;

  /* Only feeding the watchdog (reseting the counter to 0) is necessary. This
   * function fails if the counter has been stopped, ie the bark and bite time
   * are 0 */
  WDOGn_Feed(pxWatchdogHandle->em_instance);

  portEXIT_CRITICAL();

  return IOT_WATCHDOG_SUCCESS;
}

/*******************************************************************************
 *                         iot_wathcdog_set_callback()
 ******************************************************************************/

/*!
 * @brief   iot_watchdog_set_callback is used to set the callback to be called when
 *          bark time reaches the WatchDog counter or if the bite time is configured to
 *          generate interrupt (if supported by HW). The caller must set the timers using
 *          IOCTL and start the timer for the callback to be called back.
 *          Caller must restart the timer when bark timer expires and bite time is configured
 *          to reset the device to avoid the target reset.
 *
 * @note Single callback is used, per instance, if eWatchdogBiteTimerInterrupt has been configured or
 *       if the bark time reaches the WatchDog counter.
 * @note Newly set callback overrides the one previously set
 *
 * @warning If input handle or if callback function is NULL, this function silently takes no action.
 *
 * @param[in]   pxWatchdogHandle handle to WatchDog interface returned in
 *                              iot_watchdog_open.
 * @param[in]   xCallback       The callback function to be called.
 * @param[in]   pvUserContext   The user context to be passed when callback is called.
 */
void iot_watchdog_set_callback(IotWatchdogHandle_t const pxWatchdogHandle,
                               IotWatchdogCallback_t xCallback,
                               void * pvUserContext)
{
  portENTER_CRITICAL();

  if (!validate_wdog_handle(pxWatchdogHandle) || xCallback == NULL) {
    portEXIT_CRITICAL();
    return;
  }

  pxWatchdogHandle->callback = xCallback;

  pxWatchdogHandle->context = pvUserContext;

  portEXIT_CRITICAL();
}

/*******************************************************************************
 *                            iot_watchdog_ioctl()
 ******************************************************************************/

/**
 * @brief   iot_watchdog_ioctl is used to configure the WatchDog timer properties
 *          like the WatchDog timeout value, WatchDog clock, handler for
 *          WatchDog interrupt etc.
 *
 * @param[in]   pxWatchdogHandle handle to WatchDog interface returned in
 *                              iot_watchdog_open.
 * @param[in]   xRequest    configuration request of type IotFlashIoctlRequest_t
 * @param[in,out] pvBuffer   the configuration buffer to hold the request or response of IOCTL.
 *
 * @return
 *   - returns IOT_WATCHDOG_SUCCESS on success
 *   - IOT_WATCHDOG_INVALID_VALUE if
 *      - pxWatchdogHandle is NULL
 *      - xRequest is invalid
 *      - pvBuffer is NULL
 *   - IOT_WATCHDOG_TIME_NOT_SET on error
 *   - IOT_WATCHDOG_FUNCTION_NOT_SUPPORTED
 */
int32_t iot_watchdog_ioctl(IotWatchdogHandle_t const pxWatchdogHandle,
                           IotWatchdogIoctlRequest_t xRequest,
                           void * const pvBuffer)
{
  int32_t ret_val = IOT_WATCHDOG_SUCCESS;

  portENTER_CRITICAL();

  if (!validate_wdog_handle(pxWatchdogHandle) || pvBuffer == NULL) {
    portEXIT_CRITICAL();
    return IOT_WATCHDOG_INVALID_VALUE;
  }

  switch (xRequest) {
    case eSetWatchdogBarkTime:
      /* Return values of sli_set_bark_time match the ones of this function */
      ret_val = set_bark_time(pxWatchdogHandle, *(uint32_t*)pvBuffer);
      break;

    case eGetWatchdogBarkTime:
      *(uint32_t*)pvBuffer = pxWatchdogHandle->bark_time_ms;
      break;

    case eSetWatchdogBiteTime:
      /* Return values of set_bite_time match the ones of this function */
      ret_val = set_bite_time(pxWatchdogHandle, *(uint32_t*)pvBuffer);
      break;

    case eGetWatchdogBiteTime:
      *(uint32_t*)pvBuffer = pxWatchdogHandle->bite_time_ms;
      break;

    case eGetWatchdogStatus:
      *((IotWatchdogStatus_t*)pvBuffer) = pxWatchdogHandle->status;
      break;

    case eSetWatchdogBiteBehaviour:
      /* Return values of sli_set_bite_config match the ones of this function */
      ret_val = set_bite_config(pxWatchdogHandle, *(IotWatchdogBiteConfig_t*)pvBuffer);
      break;

    default:
      ret_val = IOT_WATCHDOG_INVALID_VALUE;
      break;
  }

  portEXIT_CRITICAL();

  return ret_val;
}

/*******************************************************************************
 *                            iot_watchdog_close()
 ******************************************************************************/

/**
 * @brief   iot_watchdog_close is used to de-initializes the WatchDog, stops the timer
 *          if it was started and resets the timer value.
 *
 * @param[in]   pxWatchdogHandle handle to WatchDog interface returned in
 *                              iot_watchdog_open.
 * @return
 *   - IOT_WATCHDOG_SUCCESS on success
 *   - IOT_WATCHDOG_INVALID_VALUE if
 *      - pxWatchdogHandle == NULL
 *      - pxWatchdogHandle is not open (previously closed).
 */
int32_t iot_watchdog_close(IotWatchdogHandle_t const pxWatchdogHandle)
{
  WDOG_Init_TypeDef   init_default = WDOG_INIT_DEFAULT;

  portENTER_CRITICAL();

  if (!validate_wdog_handle(pxWatchdogHandle)) {
    portEXIT_CRITICAL();
    return IOT_WATCHDOG_INVALID_VALUE;
  }

  /* Stop the timer if it isn't already */
  if (pxWatchdogHandle->status != eWatchdogTimerStopped) {
    iot_watchdog_stop(pxWatchdogHandle);
  }

  /* Now that the watchdog is closed, its clock can be disabled,
   * but since no clock management is implemented, don't disable the clock */
  //CMU_ClockEnable(pxWatchdogHandle->cmu_clock, false);

  /* Reset the watchdog descriptor to default values */
  pxWatchdogHandle->is_open       = false;
  pxWatchdogHandle->em_init       = init_default;
  pxWatchdogHandle->status        = eWatchdogTimerStopped;
  pxWatchdogHandle->callback      = NULL;
  pxWatchdogHandle->context       = NULL;
  pxWatchdogHandle->bite_config   = eWatchdogBiteTimerReset;
  pxWatchdogHandle->bark_time_ms  = 0;
  pxWatchdogHandle->bite_time_ms  = 0;

  portEXIT_CRITICAL();

  return IOT_WATCHDOG_SUCCESS;
}
