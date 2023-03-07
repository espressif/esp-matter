/***************************************************************************//**
 * @file
 * @brief Software Watchdog API implementation.
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

#include "sl_watchdog_config.h"
#include "sl_watchdog.h"
#include "em_core.h"
#include "em_cmu.h"

static uint32_t active_watchdogs = 0;
static uint32_t fed_watchdogs = 0;

#if !defined(SL_WATCHDOG_PERIOD)
#define SL_WATCHDOG_PERIOD (wdogPeriod_256k)
#endif

/***************************************************************************//**
 * Initialize Software Watchdog.
 ******************************************************************************/
sl_status_t sl_watchdog_init(void)
{
  WDOG_Init_TypeDef init = WDOG_INIT_DEFAULT;

  // auto enable will take effect when first wdog is added
  init.enable = false;
  init.perSel = SL_WATCHDOG_PERIOD;
  init.debugRun = SL_WATCHDOG_RUN_DURING_DEBUG_HALT;
#if defined(_WDOG_CTRL_WARNSEL_MASK) \
  || defined(_WDOG_CFG_WARNSEL_MASK)
  init.warnSel = SL_WATCHDOG_WARNING_SELECTION;
#endif
#if defined(_WDOG_CTRL_WINSEL_MASK) \
  || defined(_WDOG_CFG_WINSEL_MASK)
  init.winSel = SL_WATCHDOG_WINDOW_SELECTION;
#endif
#if defined(_WDOG_CTRL_CLKSEL_MASK)
  init.clkSel = wdogClkSelULFRCO;
#else
  CMU_ClockSelectSet(cmuClock_WDOG0, cmuSelect_ULFRCO);
#endif
#if defined(_WDOG_CTRL_WDOGRSTDIS_MASK) \
  || defined(_WDOG_CFG_WDOGRSTDIS_MASK)
  init.resetDisable = SL_WATCHDOG_DISABLE_RESET;
#endif

#if defined(_SILICON_LABS_32B_SERIES_2)
  CMU_ClockEnable(cmuClock_WDOG0, true);
#else
  CMU_ClockEnable(cmuClock_HFLE, true);
#endif

  WDOGn_Init(DEFAULT_WDOG, &init);

  // reset counter to 0
  WDOGn_Feed(DEFAULT_WDOG);

#if (_SILICON_LABS_32B_SERIES >= 1)
  WDOGn_IntClear(DEFAULT_WDOG, _WDOG_IEN_MASK);
  NVIC_ClearPendingIRQ(WDOG0_IRQn);

  // Enable warning and timeout interrupts
  WDOGn_IntEnable(DEFAULT_WDOG, WDOG_IEN_TOUT | WDOG_IEN_WARN);
  NVIC_EnableIRQ(WDOG0_IRQn);
#endif

  return SL_STATUS_OK;
}

/***************************************************************************//**
 * Register Software Watchdog.
 ******************************************************************************/
sl_status_t sl_watchdog_register_watchdog(sl_watchdog_handle_t *watchdog)
{
  uint32_t mask;
  CORE_DECLARE_IRQ_STATE;

  CORE_ENTER_ATOMIC();

  if (active_watchdogs == UINT32_MAX) {
    CORE_EXIT_ATOMIC();
    return SL_STATUS_ALLOCATION_FAILED;
  }

  // find rightmost 0
  mask = ~active_watchdogs & (active_watchdogs + 1);
  mask = __CLZ(mask);

  mask = 0x80000000 >> mask;
  active_watchdogs |= mask;

  *watchdog = mask;

  // start timer on first watchdog
  if ((active_watchdogs == 1)) {
    CORE_EXIT_ATOMIC();
    WDOGn_Enable(DEFAULT_WDOG, true);
    return SL_STATUS_OK;
  }

  CORE_EXIT_ATOMIC();

  return SL_STATUS_OK;
}

/***************************************************************************//**
 * Unregister Software Watchdog.
 ******************************************************************************/
sl_status_t sl_watchdog_unregister_watchdog(sl_watchdog_handle_t watchdog)
{
  CORE_DECLARE_IRQ_STATE;

  CORE_ENTER_ATOMIC();
  active_watchdogs &= ~watchdog;
  fed_watchdogs &= ~watchdog;
  // if there are no active watchdogs, the watchdog will
  // never be fed, so will time out unless stopped
  if (active_watchdogs == 0) {
    CORE_EXIT_ATOMIC();
    WDOGn_Enable(DEFAULT_WDOG, false);
    return SL_STATUS_OK;
  }

  CORE_EXIT_ATOMIC();

  return SL_STATUS_OK;
}

/***************************************************************************//**
 * Feed Software Watchdog instance.
 ******************************************************************************/
sl_status_t sl_watchdog_feed_watchdog(sl_watchdog_handle_t watchdog)
{
  CORE_DECLARE_IRQ_STATE;

  CORE_ENTER_ATOMIC();
  fed_watchdogs |= watchdog;

  if ((fed_watchdogs & active_watchdogs) == active_watchdogs) {
    fed_watchdogs = 0;
    CORE_EXIT_ATOMIC();
    WDOGn_Feed(DEFAULT_WDOG);
    return SL_STATUS_OK;
  }

  CORE_EXIT_ATOMIC();

  return SL_STATUS_OK;
}

/***************************************************************************//**
 * Watchdog Interrupt Handler.
 ******************************************************************************/
#if (_SILICON_LABS_32B_SERIES >= 1)
void WDOG0_IRQHandler(void)
{
  uint32_t interrupts;

  interrupts = WDOGn_IntGet(DEFAULT_WDOG);
  WDOGn_IntClear(DEFAULT_WDOG, interrupts);

  if (interrupts & _WDOG_IEN_WARN_MASK) {
    sl_watchdog_warning_callback(active_watchdogs & ~fed_watchdogs);
  }
}
#endif
