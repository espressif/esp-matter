/***************************************************************************//**
 * @file
 * @brief Microsecond delay.
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
#include "sl_udelay.h"
#include "em_device.h"
#include "sl_assert.h"
#include <stdbool.h>

/* The Cortex-M33 has a faster execution of the hw loop
 * with the same arm instructions. */
#if defined(__CORTEX_M) && (__CORTEX_M == 33U)
  #define HW_LOOP_CYCLE  3
#else
  #define HW_LOOP_CYCLE  4
#endif

void sli_delay_loop(unsigned n);

void sl_udelay_wait(unsigned us)
{
  uint32_t freq_khz;
  uint32_t ns_period;
  uint32_t cycles;
  uint32_t loops;

  freq_khz = SystemCoreClockGet() / 1000U;
  if (freq_khz == 0) {
    EFM_ASSERT(false);
    return;
  }

  ns_period = 1000000U / freq_khz;
  if (ns_period == 0) {
    EFM_ASSERT(false);
    return;
  }

  cycles = us * 1000U / ns_period;
  loops = cycles / HW_LOOP_CYCLE;
  if (loops > 0U) {
    sli_delay_loop(loops);
  }
}
