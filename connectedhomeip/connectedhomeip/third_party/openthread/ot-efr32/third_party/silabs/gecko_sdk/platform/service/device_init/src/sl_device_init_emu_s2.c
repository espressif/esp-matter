/***************************************************************************//**
 * @file
 * @brief Device initialization for EMU.
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

#include "sl_device_init_emu.h"
#include "sl_device_init_emu_config.h"

#include "em_emu.h"

sl_status_t sl_device_init_emu(void)
{
  // EM2 set debug enable
  EMU->CTRL = (EMU->CTRL & ~_EMU_CTRL_EM2DBGEN_MASK)
              | (SL_DEVICE_INIT_EMU_EM2_DEBUG_ENABLE << _EMU_CTRL_EM2DBGEN_SHIFT);

  // EM4 Init
  EMU_EM4Init_TypeDef em4_init = EMU_EM4INIT_DEFAULT;

  em4_init.pinRetentionMode = SL_DEVICE_INIT_EMU_EM4_PIN_RETENTION_MODE;
  EMU_EM4Init(&em4_init);

  return SL_STATUS_OK;
}
