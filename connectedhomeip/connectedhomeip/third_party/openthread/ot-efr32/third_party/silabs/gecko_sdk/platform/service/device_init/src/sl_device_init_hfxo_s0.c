/***************************************************************************//**
 * @file
 * @brief Device initialization for HFXO.
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
#include "sl_device_init_hfxo.h"
#include "sl_device_init_hfxo_config.h"

#include "em_cmu.h"

sl_status_t sl_device_init_hfxo(void)
{
  CMU_HFXOInit_TypeDef hfxoInit = CMU_HFXOINIT_DEFAULT;
  hfxoInit.mode = SL_DEVICE_INIT_HFXO_MODE;
  hfxoInit.boost = SL_DEVICE_INIT_HFXO_BOOST;

  SystemHFXOClockSet(SL_DEVICE_INIT_HFXO_FREQ);
  CMU_HFXOInit(&hfxoInit);

  return SL_STATUS_OK;
}
