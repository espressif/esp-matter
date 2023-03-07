/***************************************************************************//**
 * @file
 * @brief Device initialization for NVIC.
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
#include "em_device.h"
#include "em_core.h"

/***************************************************************************//**
 * Sets default priority for all configurable interrupts in NVIC.
 *
 * @note Fault exceptions are let at priority 0.
 ******************************************************************************/
sl_status_t sl_device_init_nvic(void)
{
  for (IRQn_Type i = SVCall_IRQn; i < EXT_IRQ_COUNT; i++) {
    NVIC_SetPriority(i, CORE_INTERRUPT_DEFAULT_PRIORITY);
  }

  return SL_STATUS_OK;
}
