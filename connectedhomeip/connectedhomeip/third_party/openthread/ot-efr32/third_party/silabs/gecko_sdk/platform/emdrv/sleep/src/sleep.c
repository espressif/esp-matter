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

/* Chip specific header file(s). */
#include "em_device.h"
#include "sl_assert.h"
#include "em_core.h"
#include "em_rmu.h"
#include "em_emu.h"

/* Module header file(s). */
#include "sleep.h"

/* stdlib is needed for NULL definition */
#include <stdlib.h>

/***************************************************************************//**
 * @addtogroup sleep
 * @{
 ******************************************************************************/

/*******************************************************************************
 *******************************   MACROS   ************************************
 ******************************************************************************/

/** @cond DO_NOT_INCLUDE_WITH_DOXYGEN */

/* Number of low energy modes (EM2 and EM3). Note: EM4 sleep/wakeup is handled
 * differently therefore it is not part of the list! */
#define SLEEP_NUMOF_LOW_ENERGY_MODES    2U

#if defined(RMU_RSTCAUSE_EM4WURST)
#define EM4_RESET_FLAG  RMU_RSTCAUSE_EM4WURST
#elif defined(RMU_RSTCAUSE_EM4RST)
#define EM4_RESET_FLAG  RMU_RSTCAUSE_EM4RST
#elif defined(EMU_RSTCAUSE_EM4)
#define EM4_RESET_FLAG  EMU_RSTCAUSE_EM4
#endif

/*******************************************************************************
 *******************************   STATICS   ***********************************
 ******************************************************************************/

/* This is the internal context of the sleep driver. */
static SLEEP_Init_t sleepContext = { 0 };

/* Callback functions to call before and after sleep. This is only used to
 * preserve backwards compatibility when using SLEEP_Init. When using
 * SLEEP_InitEx this is no longer needed. */
static SLEEP_CbFuncPtr_t sleepCallback  = NULL;

/* Sleep block counter array representing the nested sleep blocks for the low
 * energy modes (EM2/EM3). Array index 0 corresponds to EM2 and index 1
 * to EM3.
 *
 * Note:
 * - EM4 sleep/wakeup is handled differently therefore it is not part of the
 *   list!
 * - Max. number of sleep block nesting is 255. */
static uint8_t sleepBlockCnt[SLEEP_NUMOF_LOW_ENERGY_MODES];

/**
 * @brief
 *   This function is only used to keep the interface backwards compatible.
 *
 * @details
 *   Previous version of the sleep driver had a sleep callback but it did not
 *   have a return value.
 */
static bool sleepCallbackWrapper(SLEEP_EnergyMode_t emode)
{
  if (NULL != sleepCallback) {
    sleepCallback(emode);
  }
  return true;
}

/*******************************************************************************
 ******************************   PROTOTYPES   *********************************
 ******************************************************************************/

static SLEEP_EnergyMode_t enterEMx(SLEEP_EnergyMode_t eMode);

/** @endcond */

/*******************************************************************************
 ***************************   GLOBAL FUNCTIONS   ******************************
 ******************************************************************************/

/***************************************************************************//**
 * @brief
 *   Initialize the Sleep module.
 *
 * @details
 *   Use this function to initialize the Sleep module, should be called
 *   only once! Pointers to sleep and wake-up callback functions shall be
 *   provided when calling this function.
 *   If SLEEP_EM4_WAKEUP_CALLBACK_ENABLED is set to true, this function checks
 *   for the cause of the reset that implicitly called it and calls the wakeup
 *   callback if the reset was a wakeup from EM4 (does not work on Gecko MCU).
 *
 * @param[in] pSleepCb
 *   Pointer to the callback function that is being called before the device is
 *   going to sleep.
 *
 * @param[in] pWakeUpCb
 *   Pointer to the callback function that is being called after wake up.
 *
 * @deprecated New code should use the @ref SLEEP_InitEx function for
 *   initializing the sleep module.
 ******************************************************************************/
void SLEEP_Init(SLEEP_CbFuncPtr_t pSleepCb, SLEEP_CbFuncPtr_t pWakeUpCb)
{
  /* Initialize callback functions. */
  sleepCallback = pSleepCb;
  sleepContext.sleepCallback = sleepCallbackWrapper;
  sleepContext.wakeupCallback = pWakeUpCb;

  /* Reset sleep block counters. Note: not using for() saves code! */
  sleepBlockCnt[0U] = 0U;
  sleepBlockCnt[1U] = 0U;

#if (SLEEP_EM4_WAKEUP_CALLBACK_ENABLED == true) && !defined(_EFM32_GECKO_FAMILY)
  /* Check if the Init() happened after an EM4 reset. */
  if (RMU_ResetCauseGet() & EM4_RESET_FLAG) {
    /* Clear the cause of the reset. */
    RMU_ResetCauseClear();
    /* Call wakeup callback with EM4 parameter. */
    if (NULL != sleepContext.wakeupCallback) {
      sleepContext.wakeupCallback(sleepEM4);
    }
  }
#endif
}

/***************************************************************************//**
 * @brief
 *   Initialize the Sleep module.
 *
 * @details
 *   Use this function to initialize the Sleep module.
 *
 * @param[in] init
 *   Pointer to the sleep module init structure containing callback function
 *   and configuration parameters.
 ******************************************************************************/
void SLEEP_InitEx(const SLEEP_Init_t * init)
{
  SLEEP_Init(NULL, init->wakeupCallback);
  sleepContext.sleepCallback   = init->sleepCallback;
  sleepContext.restoreCallback = init->restoreCallback;
  sleepContext.wakeupCallback  = init->wakeupCallback;
}

/***************************************************************************//**
 * @brief
 *   Sets the system to sleep into the lowest possible energy mode.
 *
 * @details
 *   This function takes care of the system states protected by the sleep block
 *   provided by SLEEP_SleepBlockBegin() / SLEEP_SleepBlockEnd(). It allows
 *   the system to go into the lowest possible energy mode that the device can
 *   be set into at the time of the call of this function.
 *   This function will not go lower than EM3 because leaving EM4 requires
 *   resetting MCU. To enter into EM4 call SLEEP_ForceSleepInEM4().
 *
 * @return
 *   Energy Mode that was entered. Possible values:
 *   @li sleepEM0
 *   @li sleepEM1
 *   @li sleepEM2
 *   @li sleepEM3
 ******************************************************************************/
SLEEP_EnergyMode_t SLEEP_Sleep(void)
{
  CORE_DECLARE_IRQ_STATE;
  SLEEP_EnergyMode_t allowedEM;
  SLEEP_EnergyMode_t modeEntered = sleepEM0;
  uint32_t flags = 0;

  /* Critical section to allow sleep blocks in ISRs. */
  CORE_ENTER_CRITICAL();
  allowedEM = SLEEP_LowestEnergyModeGet();
  if (allowedEM == sleepEM2 || allowedEM == sleepEM3) {
    EMU_Save();
  }

  do {
    allowedEM = SLEEP_LowestEnergyModeGet();

    if ((allowedEM >= sleepEM1) && (allowedEM <= sleepEM3)) {
      modeEntered = enterEMx(allowedEM);
    }

    if (NULL != sleepContext.restoreCallback) {
      flags = sleepContext.restoreCallback(modeEntered);
    }
  } while ((flags & SLEEP_FLAG_NO_CLOCK_RESTORE) > 0u);

  if (modeEntered == sleepEM2 || modeEntered == sleepEM3) {
    EMU_Restore();
  }
  CORE_EXIT_CRITICAL();

  return modeEntered;
}

/***************************************************************************//**
 * @brief
 *   Force the device to go to EM4 without doing any checks.
 *
 * @details
 *   This function unblocks the low energy sleep block then goes to EM4.
 *
 * @note
 *   Regular RAM is not retained in EM4 and the wake up causes a reset.
 *   If the configuration option SLEEP_EM4_WAKEUP_CALLBACK_ENABLED is set to
 *   true, the SLEEP_Init() function checks for the reset cause and calls the
 *   EM4 wakeup callback.
 ******************************************************************************/
void SLEEP_ForceSleepInEM4(void)
{
#if (SLEEP_HW_LOW_ENERGY_BLOCK_ENABLED == true)
  /* Unblock the EM2/EM3/EM4 block in the EMU. */
  EMU_EM2UnBlock();
#endif

  /* Request entering to EM4. */
  enterEMx(sleepEM4);
}

/// ****************************************************************************
/// @brief
///   Begin sleep block in the requested energy mode.
///
/// @details
///   Blocking a critical system state from a certain energy mode makes sure that
///   the system is not set to that energy mode while the block is not being
///   released.
///   Every SLEEP_SleepBlockBegin() increases the corresponding counter and
///   every SLEEP_SleepBlockEnd() decreases it.
///
///   Example:\code
///      SLEEP_SleepBlockBegin(sleepEM2);  // do not allow EM2 or higher
///      // do some stuff that requires EM1 at least, like ADC sampling
///      SLEEP_SleepBlockEnd(sleepEM2);    // remove restriction for EM2\endcode
///
/// @note
///   Be aware that there is limit of maximum blocks nesting to 255.
///
/// @param[in] eMode
///   Energy mode to begin to block. Possible values:
///   @li sleepEM2 - Begin to block the system from being set to EM2/EM3/EM4.
///   @li sleepEM3 - Begin to block the system from being set to EM3/EM4.
/// ****************************************************************************
void SLEEP_SleepBlockBegin(SLEEP_EnergyMode_t eMode)
{
  EFM_ASSERT((eMode >= sleepEM2) && (eMode < sleepEM4));
  EFM_ASSERT((sleepBlockCnt[eMode - 2U]) < 255U);

  if ((eMode == sleepEM2) || (eMode == sleepEM3)) {
    /* Increase the sleep block counter of the selected energy mode. */
    sleepBlockCnt[eMode - 2U]++;

#if (SLEEP_HW_LOW_ENERGY_BLOCK_ENABLED == true) && defined(_EMU_CTRL_EM2BLOCK_MASK)
    /* Block EM2/EM3 sleep if the EM2 block begins. */
    if (eMode == sleepEM2) {
      EMU_EM2Block();
    }
#endif
  }
}

/// ****************************************************************************
/// @brief
///   End sleep block in the requested energy mode.
///
/// @details
///   Release restriction for entering certain energy mode. Every call of this
///   function reduce blocking counter by 1. Once the counter for specific energy
///   mode is 0 and all counters for lower energy modes are 0 as well, using
///   particular energy mode is allowed.
///   Every SLEEP_SleepBlockBegin() increases the corresponding counter and
///   every SLEEP_SleepBlockEnd() decreases it.
///
///   Example:\code
///      // at start all energy modes are allowed
///      SLEEP_SleepBlockBegin(sleepEM3); // EM3 and EM4 are blocked
///      SLEEP_SleepBlockBegin(sleepEM2); // EM2, EM3 and EM4 are blocked
///      SLEEP_SleepBlockBegin(sleepEM2); // EM2, EM3 and EM4 are blocked
///      SLEEP_SleepBlockEnd(sleepEM3);   // EM2, EM3 and EM4 are still blocked
///      SLEEP_SleepBlockEnd(sleepEM2);   // EM2, EM3 and EM4 are still blocked
///      SLEEP_SleepBlockEnd(sleepEM2);   // all energy modes are allowed now\endcode
///
/// @param[in] eMode
///   Energy mode to end to block. Possible values:
///   @li sleepEM2 - End to block the system from being set to EM2/EM3/EM4.
///   @li sleepEM3 - End to block the system from being set to EM3/EM4.
/// ****************************************************************************
void SLEEP_SleepBlockEnd(SLEEP_EnergyMode_t eMode)
{
  EFM_ASSERT((eMode >= sleepEM2) && (eMode < sleepEM4));

  if ((eMode == sleepEM2) || (eMode == sleepEM3)) {
    /* Decrease the sleep block counter of the selected energy mode. */
    if (sleepBlockCnt[eMode - 2U] > 0U) {
      sleepBlockCnt[eMode - 2U]--;
    }

#if (SLEEP_HW_LOW_ENERGY_BLOCK_ENABLED == true) && defined(_EMU_CTRL_EM2BLOCK_MASK)
    /* Check if the EM2/EM3 block should be unblocked in the EMU. */
    if (0U == sleepBlockCnt[sleepEM2 - 2U]) {
      EMU_EM2UnBlock();
    }
#endif
  }
}

/***************************************************************************//**
 * @brief
 *   Gets the lowest energy mode that the system is allowed to be set to.
 *
 * @details
 *   This function uses the low energy mode block counters to determine the
 *   lowest possible that the system is allowed to be set to.
 *
 * @return
 *   Lowest energy mode that the system can be set to. Possible values:
 *   @li sleepEM1
 *   @li sleepEM2
 *   @li sleepEM3
 ******************************************************************************/
SLEEP_EnergyMode_t SLEEP_LowestEnergyModeGet(void)
{
  SLEEP_EnergyMode_t tmpLowestEM = sleepEM1;

  /* Check which is the lowest energy mode that the system can be set to. */
  if (0U == sleepBlockCnt[sleepEM2 - 2U]) {
    tmpLowestEM = sleepEM2;
    if (0U == sleepBlockCnt[sleepEM3 - 2U]) {
      tmpLowestEM = sleepEM3;
    }
  }

  /* Compare with the default lowest energy mode setting. */
  if (SLEEP_LOWEST_ENERGY_MODE_DEFAULT < tmpLowestEM) {
    tmpLowestEM = SLEEP_LOWEST_ENERGY_MODE_DEFAULT;
  }

  return tmpLowestEM;
}

/** @cond DO_NOT_INCLUDE_WITH_DOXYGEN */

/***************************************************************************//**
 * @brief
 *   Call the callbacks and enter the requested energy mode.
 *
 * @details
 *   This function is not part of the API, therefore it shall not be called by
 *   the user directly as it doesn not have any checks if the system is ready
 *   for sleep!
 *
 * @note
 *   The EM4 wakeup callback is not being called from this function because
 *   waking up from EM4 causes a reset.
 *   If SLEEP_EM4_WAKEUP_CALLBACK_ENABLED is set to true, SLEEP_Init() function
 *   checks for the cause of the reset and calls the wakeup callback if the
 *   reset was a wakeup from EM4.
 ******************************************************************************/
static SLEEP_EnergyMode_t enterEMx(SLEEP_EnergyMode_t eMode)
{
  bool enterSleep = true;
  EFM_ASSERT((eMode > sleepEM0) && (eMode <= sleepEM4));

  /* Call sleepCallback() before going to sleep. */
  if (NULL != sleepContext.sleepCallback) {
    /* Call the callback before going to sleep. */
    enterSleep = sleepContext.sleepCallback(eMode);
  }

  if (!enterSleep) {
    return sleepEM0;
  }

  /* Enter the requested energy mode. */
  switch (eMode) {
    case sleepEM1:
      EMU_EnterEM1();
      break;

    case sleepEM2:
      EMU_EnterEM2(false);
      break;

    case sleepEM3:
      EMU_EnterEM3(false);
      break;

    case sleepEM4:
      EMU_EnterEM4();
      break;

    default:
      /* Don't do anything, stay in EM0. */
      break;
  }

  /* Call the callback after waking up from sleep. */
  if (NULL != sleepContext.wakeupCallback) {
    sleepContext.wakeupCallback(eMode);
  }

  return eMode;
}
/** @endcond */

/** @} (end addtogroup sleep */
