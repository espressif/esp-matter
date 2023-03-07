/***************************************************************************//**
 * @file
 * @brief Device initialization for USB PLL.
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
#include "sl_device_init_usbpll.h"

#include "em_cmu.h"

#include <stdbool.h>

sl_status_t sl_device_init_usbpll(void)
{
  CMU_PLL_Init_TypeDef pll_config;
  uint32_t hfxo_freq = SystemHFXOClockGet();

  // Validate that HFXO frequency is adequate for USB PLL and set the right frequency
  switch (hfxo_freq) {
    case 38000000:
      pll_config.hfxoRefFreq = cmuHFXORefFreq_38M0Hz;
      break;

    case 38400000:
      pll_config.hfxoRefFreq = cmuHFXORefFreq_38M4Hz;
      break;

    case 39000000:
      pll_config.hfxoRefFreq = cmuHFXORefFreq_39M0Hz;
      break;

    case 40000000:
      pll_config.hfxoRefFreq = cmuHFXORefFreq_40M0Hz;
      break;

    default:
      return SL_STATUS_FAIL;
  }

  // Set additional configurations
  pll_config.shuntRegEn = false;
  pll_config.disOnDemand = false;
  pll_config.regLock = true;

  // Set Force Enable feature at first to force the PLL to start and validate it works.
  pll_config.forceEn = true;

  // Initialize USB PLL and wait for it to be ready.
  CMU_USBPLLInit(&pll_config);

  // Remove the Force Enable feature to let PLL module on-demand
  pll_config.forceEn = false;

  // Re-initialized without the Force Enable feature
  CMU_USBPLLInit(&pll_config);

  return SL_STATUS_OK;
}
