/***************************************************************************//**
 * @file sl_legacy_hal_wdog.h
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

#include <stdio.h>
#include "hal.h"
#include "em_cmu.h"
#include "em_wdog.h"
#include "em_rmu.h"
#include "sl_component_catalog.h"

#include "sl_legacy_hal_wdog_config.h"

#if (SL_LEGACY_HAL_WDOGn == 0)

#if defined(WDOG0)
#define SL_LEGACY_HAL_WDOG WDOG0
#endif // WDOG0

#if defined(WDOG0_IRQn)
#define SL_LEGACY_HAL_WDOG_IRQn WDOG0_IRQn
#endif // WDOG0_IRQn

#if defined(WDOG0_IRQHandler)
#define SL_LEGACY_HAL_WDOG_IRQHandler (WDOG0_IRQHandler)
#endif // WDOG0_IRQHandler

#if cmuClock_WDOG0
#define SL_LEGACY_HAL_WDOG_CMUCLOCK (cmuClock_WDOG0)
#endif // cmuClock_WDOG0

#if _CMU_CLKEN0_WDOG0_MASK
#define SL_LEGACY_HAL_WDOG_CMU_CLKENx_WDOGx_MASK (_CMU_CLKEN0_WDOG0_MASK)
#endif //_CMU_CLKEN0_WDOG0_MASK

#endif

#if (SL_LEGACY_HAL_WDOGn == 1)

#if defined(WDOG1)
#define SL_LEGACY_HAL_WDOG WDOG1
#endif // WDOG1

#if defined(WDOG1_IRQn)
#define SL_LEGACY_HAL_WDOG_IRQn WDOG1_IRQn
#endif // WDOG1_IRQn

#if defined(WDOG1_IRQHandler)
#define SL_LEGACY_HAL_WDOG_IRQHandler (WDOG1_IRQHandler)
#endif // WDOG1_IRQHandler

#if cmuClock_WDOG1
#define SL_LEGACY_HAL_WDOG_CMUCLOCK (cmuClock_WDOG1)
#endif // cmuClock_WDOG1

#if _CMU_CLKEN1_WDOG1_MASK
#define SL_LEGACY_HAL_WDOG_CMU_CLKENx_WDOGx_MASK (_CMU_CLKEN1_WDOG1_MASK)
#endif //_CMU_CLKEN1_WDOG1_MASK

#endif
