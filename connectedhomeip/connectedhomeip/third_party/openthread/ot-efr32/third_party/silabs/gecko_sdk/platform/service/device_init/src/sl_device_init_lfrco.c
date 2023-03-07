/***************************************************************************//**
 * @file
 * @brief Device initialization for LFRCO.
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
#include "sl_device_init_lfrco.h"
#include "em_cmu.h"

#if defined(_SILICON_LABS_32B_SERIES_1)
#include "sl_device_init_lfrco_config.h"
#elif defined(_SILICON_LABS_32B_SERIES_2) && defined(PLFRCO_PRESENT)
#include "sl_device_init_lfrco_config.h"
#endif

sl_status_t sl_device_init_lfrco(void)
{
#if defined(_SILICON_LABS_32B_SERIES_2) && defined(PLFRCO_PRESENT)
  CMU_LFRCOSetPrecision(SL_DEVICE_INIT_LFRCO_PRECISION);
#endif

#if defined(_SILICON_LABS_32B_SERIES_1)
#if SL_DEVICE_INIT_LFRCO_ENVREF == 0
  CMU->LFRCOCTRL &= ~(CMU_LFRCOCTRL_ENVREF);
#else
  CMU->LFRCOCTRL |= CMU_LFRCOCTRL_ENVREF;
#endif

#if SL_DEVICE_INIT_LFRCO_ENCHOP == 0
  CMU->LFRCOCTRL &= ~(CMU_LFRCOCTRL_ENCHOP);
#else
  CMU->LFRCOCTRL |= CMU_LFRCOCTRL_ENCHOP;
#endif

#if SL_DEVICE_INIT_LFRCO_ENDEM == 0
  CMU->LFRCOCTRL &= ~(CMU_LFRCOCTRL_ENDEM);
#else
  CMU->LFRCOCTRL |= CMU_LFRCOCTRL_ENDEM;
#endif
#endif // defined(_SILICON_LABS_32B_SERIES_1)

  return SL_STATUS_OK;
}
