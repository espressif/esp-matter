/***************************************************************************//**
 * @file
 * @brief VDD Voltage Check using EMLIB Voltage Comparator API
 *******************************************************************************
 * # License
 * <b>Copyright 2018 Silicon Laboratories Inc. www.silabs.com</b>
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

#include <stdint.h>
#include <stdbool.h>
#include "em_cmu.h"
#include "em_vcmp.h"
#include "vddcheck.h"

/***************************************************************************//**
 * @addtogroup kitdrv
 * @{
 ******************************************************************************/

/***************************************************************************//**
 * @addtogroup VddCheck
 * @brief VDD Voltage Check using EMLIB Voltage Comparator (VCOMP) API
 * @details
 * @{
 ******************************************************************************/

/**************************************************************************//**
 * @brief VCMP initialization routine
 *****************************************************************************/
void VDDCHECK_Init(void)
{
  /* Enable LE peripherals */
  CMU_ClockEnable(cmuClock_CORELE, true);

  /* Enable VCMP clock */
  CMU_ClockEnable(cmuClock_VCMP, true);
}

/**************************************************************************//**
 * @brief VCMP deinitialization routine
 *****************************************************************************/
void VDDCHECK_Disable(void)
{
  /* Disable VCMP */
  VCMP_Disable();

  /* Disable clock to VCMP */
  CMU_ClockEnable(cmuClock_VCMP, false);
}

/**************************************************************************//**
 * @brief Check if voltage is higher than indicated
 *
 * @param vdd
 *        The voltage level to compare against.
 * @return
 *        Returns true if voltage is lower, false otherwise
 *****************************************************************************/
bool VDDCHECK_LowVoltage(float vdd)
{
  VCMP_Init_TypeDef vcmp = VCMP_INIT_DEFAULT;

  /* Configure VCMP */
  vcmp.triggerLevel = VCMP_VoltageToLevel(vdd);
  vcmp.warmup       = vcmpWarmTime128Cycles;
  vcmp.lowPowerRef  = false;
  vcmp.enable       = true;

  VCMP_Init(&vcmp);

  /* Delay until warm up ready */
  while (!VCMP_Ready()) ;

  /* If zero result, voltage is lower */
  if (VCMP_VDDHigher()) {
    return false;
  }

  /* Otherwise return false */
  return true;
}

/** @} (end group VddCheck) */
/** @} (end group kitdrv) */
