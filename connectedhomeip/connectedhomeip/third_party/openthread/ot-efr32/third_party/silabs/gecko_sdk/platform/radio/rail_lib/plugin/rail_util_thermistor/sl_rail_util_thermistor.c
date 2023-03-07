/***************************************************************************//**
 * @file
 * @brief Thermistor conversion functions provided to the customer as source for
 *   highest level of customization.
 * @details This file contains the logic that converts thermistor impedance
 * to temperature in Celsius.
 *******************************************************************************
 * # License
 * <b>Copyright 2022 Silicon Laboratories Inc. www.silabs.com</b>
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

#include "sl_rail_util_thermistor.h"

#if RAIL_SUPPORTS_EXTERNAL_THERMISTOR

#include <math.h>

RAIL_Status_t sl_rail_util_thermistor_init(void)
{
  const RAIL_HFXOThermistorConfig_t hfxoThermistorConfig = {
    .port = GPIO_THMSW_EN_PORT,
    .pin = GPIO_THMSW_EN_PIN
  };

  return RAIL_ConfigHFXOThermistor(RAIL_EFR32_HANDLE, &hfxoThermistorConfig);
}

/* This macro is defined when Silicon Labs builds this into the library as WEAK
   to ensure it can be overriden by customer versions of these functions. The macro
   should *not* be defined in a customer build. */
#ifdef RAIL_UTIL_THERMISTOR_WEAK
__WEAK
#endif
RAIL_Status_t RAIL_ConvertThermistorImpedance(RAIL_Handle_t railHandle,
                                              uint32_t thermistorImpedance,
                                              int16_t *thermistorTemperatureC)
{
  (void) railHandle;
  // T = 4200 / (log(Rtherm/100000) + 4200/(273.15+25)) - 273.15
  double impedanceLn = log((double)(thermistorImpedance) / 100000U);
  // Multiply by 8 to convert in eighth of Celsius degrees
  *thermistorTemperatureC = (int16_t) ((4200.0 / (impedanceLn + 4200.0 / (273.15 + 25U)) - 273.15) * 8U);
  return RAIL_STATUS_NO_ERROR;
}

#ifdef RAIL_UTIL_THERMISTOR_WEAK
__WEAK
#endif
RAIL_Status_t RAIL_ComputeHFXOPPMError(RAIL_Handle_t railHandle,
                                       int16_t crystalTemperatureC,
                                       int8_t *crystalPPMError)
{
  (void) railHandle;
  // Equation is:
  // f(T) = 1.05*10^-4(T-T0)^3 + 1.0*10^-4(T-T0)^2 - 0.74(T-T0) - 0.35, Reference to 30℃
  // Cache T-T0
  double deltaRefTempC = crystalTemperatureC - 30;
  *crystalPPMError = (int8_t) (1.05 * 0.0001 * pow(deltaRefTempC, 3)
                               + 0.0001 * pow(deltaRefTempC, 2)
                               - 0.74 * deltaRefTempC
                               - 0.35);
  return RAIL_STATUS_NO_ERROR;
}

#endif // RAIL_SUPPORTS_EXTERNAL_THERMISTOR
