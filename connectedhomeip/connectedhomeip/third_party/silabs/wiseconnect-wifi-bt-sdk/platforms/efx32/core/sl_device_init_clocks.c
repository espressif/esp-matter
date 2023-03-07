/***************************************************************************//**
 * @file
 * @brief Device initialization for clocks.
 *******************************************************************************
 * # License
 * <b>Copyright 2019 Silicon Laboratories Inc. www.silabs.com</b>
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
#include "sl_status.h"
#include "sl_device_init_hfxo_config.h"
#include "sl_device_init_dpll_config.h"
#include "em_cmu.h"

sl_status_t sl_device_init_clocks(void)
{
  CMU_ClockSelectSet(cmuClock_SYSCLK, cmuSelect_HFRCODPLL);
#if defined(CMU_EM01GRPACLKCTRL_MASK)
  CMU_ClockSelectSet(cmuClock_EM01GRPACLK, cmuSelect_HFRCODPLL);
#endif
#if defined(CMU_EM01GRPBCLKCTRL_MASK)
  CMU_ClockSelectSet(cmuClock_EM01GRPBCLK, cmuSelect_HFRCODPLL);
#endif
  CMU_ClockSelectSet(cmuClock_EM23GRPACLK, cmuSelect_LFXO);
  CMU_ClockSelectSet(cmuClock_EM4GRPACLK, cmuSelect_LFXO);
#if defined(RTCC_PRESENT)
  CMU_ClockSelectSet(cmuClock_RTCC, cmuSelect_LFXO);
#endif
  CMU_ClockSelectSet(cmuClock_WDOG0, cmuSelect_LFXO);
#if WDOG_COUNT > 1
  CMU_ClockSelectSet(cmuClock_WDOG1, cmuSelect_LFXO);
#endif

  return SL_STATUS_OK;
}


// Fetch CTUNE value from USERDATA page as a manufacturing token
#define MFG_CTUNE_ADDR 0x0FE00100UL
#define MFG_CTUNE_VAL  (*((uint16_t *) (MFG_CTUNE_ADDR)))
sl_status_t sl_device_init_hfxo(void)
{
  CMU_HFXOInit_TypeDef hfxoInit = CMU_HFXOINIT_DEFAULT;
  hfxoInit.mode = SL_DEVICE_INIT_HFXO_MODE;

  int ctune = -1;

#if defined(_DEVINFO_MODXOCAL_HFXOCTUNE_MASK)
  // Use HFXO tuning value from DEVINFO if available (PCB modules)
  if ((DEVINFO->MODULEINFO & _DEVINFO_MODULEINFO_HFXOCALVAL_MASK) == 0) {
    ctune = DEVINFO->MODXOCAL & _DEVINFO_MODXOCAL_HFXOCTUNEXIANA_MASK;
  }
#endif

  // Use HFXO tuning value from MFG token in UD page if not already set
  if ((ctune == -1) && (MFG_CTUNE_VAL != 0xFFFF)) {
    ctune = MFG_CTUNE_VAL;
  }

  // Use HFXO tuning value from configuration header as fallback
  if (ctune == -1) {
    ctune = SL_DEVICE_INIT_HFXO_CTUNE;
  }

  if (ctune != -1) {
    hfxoInit.ctuneXoAna = ctune;
    hfxoInit.ctuneXiAna = ctune;
  }

  SystemHFXOClockSet(SL_DEVICE_INIT_HFXO_FREQ);
  CMU_HFXOInit(&hfxoInit);

  return SL_STATUS_OK;
}


sl_status_t sl_device_init_dpll(void)
{
  CMU_DPLLInit_TypeDef dpll_init = {
    .frequency = SL_DEVICE_INIT_DPLL_FREQ,
    .n = SL_DEVICE_INIT_DPLL_N,
    .m = SL_DEVICE_INIT_DPLL_M,
    .refClk = SL_DEVICE_INIT_DPLL_REFCLK,
    .edgeSel = SL_DEVICE_INIT_DPLL_EDGE,
    .lockMode = SL_DEVICE_INIT_DPLL_LOCKMODE,
    .autoRecover = SL_DEVICE_INIT_DPLL_AUTORECOVER,
    .ditherEn = SL_DEVICE_INIT_DPLL_DITHER
  };
//  CMU_DPLLInit_TypeDef dpllInit = CMU_DPLL_HFXO_TO_80MHZ;
  CMU_Select_TypeDef selected_sysclk = CMU_ClockSelectGet(cmuClock_SYSCLK);

  if (selected_sysclk == cmuSelect_HFRCODPLL) {
    // From Reference Manual:
    // The CMU should not be running from the HFRCO. If necessary, the CMU
    // should switch to the FSRCO until after the DPLL has locked to avoid
    // over-clocking due to overshoot.
    CMU_ClockSelectSet(cmuClock_SYSCLK, cmuSelect_FSRCO);
  }

#if defined (_SILICON_LABS_32B_SERIES_2_CONFIG_2) || defined(_SILICON_LABS_32B_SERIES_2_CONFIG_3)
  CMU_ClockEnable(cmuClock_DPLL0, true);
#endif

  bool success = CMU_DPLLLock(&dpll_init);

  if (selected_sysclk == cmuSelect_HFRCODPLL) {
    CMU_ClockSelectSet(cmuClock_SYSCLK, selected_sysclk);
  }

  if (success) {
    return SL_STATUS_OK;
  } else {
    return SL_STATUS_FAIL;
  }
}
