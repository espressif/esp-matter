/***************************************************************************//**
 * @file
 * @brief Device initialization for DPLL.
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
#include "sl_device_init_dpll.h"
#include "sl_device_init_dpll_config.h"

#include "em_cmu.h"

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

  CMU_Select_TypeDef selected_sysclk = CMU_ClockSelectGet(cmuClock_SYSCLK);

  if (selected_sysclk == cmuSelect_HFRCODPLL) {
    // From Reference Manual:
    // The CMU should not be running from the HFRCO. If necessary, the CMU
    // should switch to the FSRCO until after the DPLL has locked to avoid
    // over-clocking due to overshoot.
    CMU_ClockSelectSet(cmuClock_SYSCLK, cmuSelect_FSRCO);
  }

#if (_SILICON_LABS_32B_SERIES_2_CONFIG > 1)
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
