/***************************************************************************//**
 * @file sl_legacy_hal_wdog.c
 * @brief Legacy HAL Watchdog
 *******************************************************************************
 * # License
 * <b>Copyright 2022 Silicon Laboratories, Inc, www.silabs.com</b>
 *******************************************************************************
 *
 * The licensor of this software is Silicon Laboratories Inc. Your use of this
 * software is governed by the terms of Silicon Labs Master Software License
 * Agreement (MSLA) available at
 * www.silabs.com/about-us/legal/master-software-license-agreement. This
 * software is distributed to you in Source Code format and is governed by the
 * sections of the MSLA applicable to Source Code.
 *
 ******************************************************************************/

#include PLATFORM_HEADER
#include "sl_legacy_hal_wdog.h"

// sl_legacy_hal_wdog.h needs to be able to define all SL_LEGACY_HAL_WDOG_*
// before these function will compile.
#if defined(SL_LEGACY_HAL_WDOG)             \
  && defined(SL_LEGACY_HAL_WDOG_IRQn)       \
  && defined(SL_LEGACY_HAL_WDOG_IRQHandler) \
  && defined(SL_LEGACY_HAL_WDOG_CMUCLOCK)   \
  && defined(SL_LEGACY_HAL_WDOG_CMU_CLKENx_WDOGx_MASK)

void halInternalEnableWatchDog(void)
{
  // Enable LE interface
#if !defined(_SILICON_LABS_32B_SERIES_2)
  CMU_ClockEnable(cmuClock_HFLE, true);
  CMU_OscillatorEnable(cmuOsc_LFRCO, true, true);
#endif

#if defined(_SILICON_LABS_32B_SERIES_2) && !defined(_SILICON_LABS_32B_SERIES_2_CONFIG_1)
  CMU_ClockEnable(SL_LEGACY_HAL_WDOG_CMUCLOCK, true);
#endif

  // Make sure FULL reset is used on WDOG timeout
#if defined(_RMU_CTRL_WDOGRMODE_MASK)
  RMU_ResetControl(rmuResetWdog, rmuResetModeFull);
#endif

  /* Note: WDOG_INIT_DEFAULT comes from platform/emlib/inc/em_wdog.h */
  WDOG_Init_TypeDef init = WDOG_INIT_DEFAULT;

  /* Trigger watchdog reset after 2 seconds (64k / 32k) and
   * warning interrupt is triggered after 1.5 seconds (75% of timeout). */
  init.perSel = wdogPeriod_64k;
  init.warnSel = wdogWarnTime75pct;

#if defined(_WDOG_CTRL_CLKSEL_MASK)
  init.clkSel = wdogClkSelLFRCO;
#else
  // Series 2 devices select watchdog oscillator with the CMU.
  CMU_ClockSelectSet(SL_LEGACY_HAL_WDOG_CMUCLOCK, cmuSelect_LFRCO);
#endif

  WDOGn_Init(SL_LEGACY_HAL_WDOG, &init);

  /* Enable WARN interrupt. */
#if defined(WDOG_IF_WARN) && !defined(BOOTLOADER)
  NVIC_ClearPendingIRQ(SL_LEGACY_HAL_WDOG_IRQn);
  WDOGn_IntClear(SL_LEGACY_HAL_WDOG, WDOG_IF_WARN);
  NVIC_EnableIRQ(SL_LEGACY_HAL_WDOG_IRQn);
  WDOGn_IntEnable(SL_LEGACY_HAL_WDOG, WDOG_IEN_WARN);
#endif
}

void halResetWatchdog(void)
{
#if defined(_CMU_HFBUSCLKEN0_LE_MASK)
  if ((CMU->HFBUSCLKEN0 & _CMU_HFBUSCLKEN0_LE_MASK) != 0) {
    WDOGn_Feed(SL_LEGACY_HAL_WDOG);
  }
#elif defined(SL_LEGACY_HAL_WDOG_CMU_CLKENx_WDOGx_MASK)
  if ((CMU->CLKEN0 & SL_LEGACY_HAL_WDOG_CMU_CLKENx_WDOGx_MASK) != 0) {
    WDOGn_Feed(SL_LEGACY_HAL_WDOG);
  }
#else
  WDOGn_Feed(SL_LEGACY_HAL_WDOG);
#endif
}

void halInternalDisableWatchDog(uint8_t magicKey)
{
#if defined(_CMU_HFBUSCLKEN0_LE_MASK)
  if ((CMU->HFBUSCLKEN0 & _CMU_HFBUSCLKEN0_LE_MASK) != 0) {
    if ( magicKey == MICRO_DISABLE_WATCH_DOG_KEY ) {
      WDOGn_Enable(SL_LEGACY_HAL_WDOG, false);
    }
  }
#elif defined(SL_LEGACY_HAL_WDOG_CMU_CLKENx_WDOGx_MASK)
  if ((CMU->CLKEN0 & SL_LEGACY_HAL_WDOG_CMU_CLKENx_WDOGx_MASK) != 0) {
    if ( magicKey == MICRO_DISABLE_WATCH_DOG_KEY ) {
      WDOGn_Enable(SL_LEGACY_HAL_WDOG, false);
    }
  }
#else
  if ( magicKey == MICRO_DISABLE_WATCH_DOG_KEY ) {
    WDOGn_Enable(SL_LEGACY_HAL_WDOG, false);
  }
#endif
}

bool halInternalWatchDogEnabled(void)
{
#if defined(_CMU_HFBUSCLKEN0_LE_MASK)
  if ((CMU->HFBUSCLKEN0 & _CMU_HFBUSCLKEN0_LE_MASK) != 0) {
    return WDOGn_IsEnabled(SL_LEGACY_HAL_WDOG);
  } else {
    return 0;
  }
#elif defined(SL_LEGACY_HAL_WDOG_CMU_CLKENx_WDOGx_MASK)
  if ((CMU->CLKEN0 & SL_LEGACY_HAL_WDOG_CMU_CLKENx_WDOGx_MASK) != 0) {
    return WDOGn_IsEnabled(SL_LEGACY_HAL_WDOG);
  } else {
    return 0;
  }
#else
  return WDOGn_IsEnabled(SL_LEGACY_HAL_WDOG);
#endif
}

#else

void halInternalEnableWatchDog(void)
{
}

void halResetWatchdog(void)
{
}

void halInternalDisableWatchDog(uint8_t magicKey)
{
  (void) magicKey;
}

bool halInternalWatchDogEnabled(void)
{
  return false;
}

#endif
