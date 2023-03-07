/***************************************************************************//**
 * @file
 * @brief Energy Modes management driver
 *******************************************************************************
 * # License
 * <b>Copyright 2018 Silicon Laboratories Inc. www.silabs.com</b>
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
#ifndef SLEEP_H
#define SLEEP_H

#ifndef SL_SUPPRESS_DEPRECATION_WARNINGS_SDK_3_2
#warning "The sleep driver is deprecated and marked for removal in a later release. Please use the power_manager service instead."
#endif

#include <stdint.h>
#include <stdbool.h>

/* Device specific header file(s). */
#include "em_device.h"

#ifdef __cplusplus
extern "C" {
#endif

/// @addtogroup sleep SLEEP - Sleep Driver
/// @brief Sleep Management Driver (DEPRECATED)
///
/// @details
///
///   @n @section sleepdrv_intro Introduction
///
///   @warning This driver is deprecated and will be removed in a later release.
///   Use the Power Manager service instead. For more information about Power Manager,
///   see Services section.
///
///   @note See <a href="https://www.silabs.com/documents/public/application-notes/
///         an1358-migrating-from-sleep-driver-to-power-manager.pdf">AN1358: Migrating
///         from Sleep Driver to Power Manager</a> for information on how to migrate
///         from Sleep Driver to Power Manager.
///
///   This is a sleep management module consisting of sleep.c and sleep.h
///   source files. The main purpose of the module is to make it easy for an
///   application to always enter the lowest possible energy mode using a simple
///   API.
///
///   The module works by providing an API for defining "sleep blocks" in the
///   application code. A "sleep block" will block the MCU from entering a certain
///   energy mode. A "sleep block" can, for instance, block EM2 entry because an
///   EM1 only peripheral is in use. These "sleep blocks" are created by the calls to
///   @ref SLEEP_SleepBlockBegin() and end with @ref SLEEP_SleepBlockEnd().
///
///   To enter a low energy mode, an application can call
///   @ref SLEEP_Sleep() to enter the lowest possible energy mode. This module
///   will use the "sleep blocks" to figure out the lowest possible energy mode.
///
///   The following is an example of the sleep driver initialization and how it can
///   be used to enter EM2.
///
/// @code
///   SLEEP_Init_t sleepConfig = {0};
///   SLEEP_InitEx(&sleepConfig);
///   SLEEP_SleepBlockBegin(sleepEM3); // Block EM3 entry
///   SLEEP_Sleep();
///   SLEEP_SleepBlockEnd(sleepEM3);
/// @endcode
///
///   @n @section sleepdrv_callbacks Sleep and Wakeup Events/Callbacks
///
///   This module also provides a way to add application callbacks to notify
///   the application that the MCU is entering sleep or waking up from sleep.
///   These callbacks can be provided to the driver when calling
///   @ref SLEEP_InitEx().
///
///   The sleepCallback function is called before entering sleep and the
///   wakeupCallback is called after waking up from sleep. The sleepCallback
///   function has a bool return value. This return value can be used to control
///   if the MCU should really go to sleep or not. Returning true will make
///   the MCU enter the selected energy mode, while returning false will force
///   the sleep driver to return without entering a sleep.
///
///   The following is an example of sleep and wakeup callbacks usage.
///
///   @code
///   static bool beforeSleep(SLEEP_EnergyMode_t mode)
///   {
///     printf("sleep\n");
///     return true;
///   }
///
///   static void afterWakeup(SLEEP_EnergyMode_t mode)
///   {
///     printf("wakeup\n");
///     (void) mode;
///   }
///
///   void main(void)
///   {
///     SLEEP_Init_t sleepConfig = {
///       .sleepCallback = beforeSleep,
///       .wakeupCallback = afterWakeup
///     };
///     SLEEP_InitEx(&sleepConfig);
///     SLEEP_SleepBlockBegin(sleepEM3); // Block EM3 entry
///     while (true) {
///       SLEEP_Sleep();
///     }
///     SLEEP_SleepBlockEnd(sleepEM3);
///   }
///   @endcode
///
/// @{
///*****************************************************************************

/*******************************************************************************
 *******************************   MACROS   ************************************
 ******************************************************************************/

/**
 * This flag is returned from the restoreCallback function to
 * signal that the sleep driver should continue as normal. */
#define SLEEP_FLAG_NONE              0x0

/**
 * This flag is returned from the restoreCallback function to
 * signal to the sleep driver that HF clocks should not be restored and that
 * the sleep driver should go right back to sleep again. */
#define SLEEP_FLAG_NO_CLOCK_RESTORE  0x1u

/*******************************************************************************
 ****************************   CONFIGURATION   ********************************
 ******************************************************************************/

/** Enable/disable the HW block for protecting accidental setting of low energy
 *  modes. Note that some devices do not support blocking energy mode entry in
 *  hardware, on these devices SLEEP_HW_LOW_ENERGY_BLOCK_ENABLED will have no
 *  effect. */
#ifndef SLEEP_HW_LOW_ENERGY_BLOCK_ENABLED
#define SLEEP_HW_LOW_ENERGY_BLOCK_ENABLED    false
#endif

/** Enable/disable calling wakeup callback after EM4 reset. */
#ifndef SLEEP_EM4_WAKEUP_CALLBACK_ENABLED
#define SLEEP_EM4_WAKEUP_CALLBACK_ENABLED    true
#endif

/** Configure default lowest energy mode that the system can be set to.
 *  Possible values:
 *  @li sleepEM2 - EM2, CPU core is turned off, all HF clocks are turned off,
 *                 LF clocks are on.
 *  @li sleepEM3 - EM3, like EM2 + LF clocks are off, RAM retention, GPIO and ACMP
 *                   interrupt is on. */
#ifndef SLEEP_LOWEST_ENERGY_MODE_DEFAULT
#define SLEEP_LOWEST_ENERGY_MODE_DEFAULT    sleepEM3
#endif

/*******************************************************************************
 ******************************   TYPEDEFS   ***********************************
 ******************************************************************************/

/** Status value used for showing the Energy Mode the device is currently in. */
typedef enum {
  /** Status value for EM0. */
  sleepEM0 = 0,

  /** Status value for EM1. */
  sleepEM1 = 1,

  /** Status value for EM2. */
  sleepEM2 = 2,

  /** Status value for EM3. */
  sleepEM3 = 3,

  /** Status value for EM4. */
  sleepEM4 = 4
} SLEEP_EnergyMode_t;

/** Callback function pointer type. */
typedef void (*SLEEP_CbFuncPtr_t)(SLEEP_EnergyMode_t);

/**
 * Initialization structure for the sleep driver. This includes optional
 * callback functions that can be used by the application to get notified
 * about sleep related events and that can be used to control the MCU
 * behavior when waking up from sleep.
 */
typedef struct {
  /**
   * Pointer to the callback function that is being called before the device
   * is going to sleep. This function is optional, if no sleep callback is
   * needed by the application then this field must be set to NULL.
   *
   * This callback function has a return value that can be used to force the
   * sleep driver to not enter a sleep mode. This can be used in applications
   * where for instance timing of events will make it inefficient to enter
   * sleep at a certain point in time.
   */
  bool (*sleepCallback)(SLEEP_EnergyMode_t emode);

  /**
   * Pointer to the callback function that is being called after wake up. This
   * function is optional. If no wake up callback is needed by the application,
   * this field must be set to NULL.
   */
  void (*wakeupCallback)(SLEEP_EnergyMode_t emode);

  /**
   * Pointer to the callback function that is being called after wake up and
   * before the HF clock is restored. This function can be used by the
   * application to signal that the sleep driver should wakeup normally or if
   * it should skip restoring the HF clock and go back to sleep. Delaying HF
   * clock restore is an advanced functionality that can be used to save
   * power in a system with frequent interrupts.
   *
   * These are the supported flags that can be returned from the callback
   * function.
   *
   * @ref SLEEP_FLAG_NONE
   * @ref SLEEP_FLAG_NO_CLOCK_RESTORE
   */
  uint32_t (*restoreCallback)(SLEEP_EnergyMode_t emode);
} SLEEP_Init_t;

/*******************************************************************************
 ******************************   PROTOTYPES   *********************************
 ******************************************************************************/

void SLEEP_Init(SLEEP_CbFuncPtr_t pSleepCb, SLEEP_CbFuncPtr_t pWakeUpCb) SL_DEPRECATED_API_SDK_4_1;

void SLEEP_InitEx(const SLEEP_Init_t * init);

SLEEP_EnergyMode_t SLEEP_LowestEnergyModeGet(void);

SLEEP_EnergyMode_t SLEEP_Sleep(void);

void SLEEP_ForceSleepInEM4(void);

void SLEEP_SleepBlockBegin(SLEEP_EnergyMode_t eMode);

void SLEEP_SleepBlockEnd(SLEEP_EnergyMode_t eMode);

/** @} (end addtogroup sleep) */

#ifdef __cplusplus
}
#endif
#endif /* SLEEP_H */
