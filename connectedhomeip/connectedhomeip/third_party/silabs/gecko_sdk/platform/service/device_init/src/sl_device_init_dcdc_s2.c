/***************************************************************************//**
 * @file
 * @brief Device initialization for DC/DC converter.
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
#include "sl_device_init_dcdc.h"
#include "sl_device_init_dcdc_config.h"

#include "em_emu.h"

sl_status_t sl_device_init_dcdc(void)
{
#if SL_DEVICE_INIT_DCDC_ENABLE
  EMU_DCDCInit_TypeDef dcdcInit = EMU_DCDCINIT_DEFAULT;
#if SL_DEVICE_INIT_DCDC_BYPASS
  dcdcInit.mode = emuDcdcMode_Bypass;
#endif
  EMU_DCDCInit(&dcdcInit);
#if SL_DEVICE_INIT_DCDC_PFMX_IPKVAL_OVERRIDE
  EMU_DCDCSetPFMXModePeakCurrent(SL_DEVICE_INIT_DCDC_PFMX_IPKVAL);
#endif
#else // SL_DEVICE_INIT_DCDC_ENABLE
  EMU_DCDCPowerOff();
#endif // SL_DEVICE_INIT_DCDC_ENABLE

  return SL_STATUS_OK;
}
