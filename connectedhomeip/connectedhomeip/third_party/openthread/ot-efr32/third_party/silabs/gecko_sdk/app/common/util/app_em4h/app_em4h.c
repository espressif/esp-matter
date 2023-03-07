/***************************************************************************//**
 * @file
 * @brief Unlatch pin retention when coming back from EM4
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

#include "em_emu.h"
#include "em_device.h"

/***************************************************************************//**
 * Application hook
 *
 * @note User might want to re-apply tunning value before unlatching in order to have
 *       a constantly running oscillator with correct tuning values
 *
 * @note Unlatching will also have side effects if the user are using
 *        EM4 GPIO wakeup, and in those cases it's important that the
 *        customers place the unlatching correctly to see the correct interrupt flags.
 ******************************************************************************/
__WEAK void on_pre_unlatch_pin_retention(void)
{
  // User might want to re-apply tunning value before unlatching in order to have
  // a constantly running oscillator with correct tuning values

  // Unlatching will also have side effects if the user are using
  // EM4 GPIO wakeup, and in those cases it's important that the
  // customers place the unlatching correctly to see the correct interrupt flags.
}

/***************************************************************************//**
 * unlatch pin retention
 ******************************************************************************/
void app_unlatch_pin_retention(void)
{
#if defined(_EMU_EM4CTRL_EM4IORETMODE_MASK)
  on_pre_unlatch_pin_retention();
  EMU_UnlatchPinRetention();
#endif
}
