/***************************************************************************//**
 * @file
 * @brief PA power conversion curves used by Silicon Labs PA power conversion
 *   functions.
 * @details This file contains the curves needed convert PA power levels to
 *   dBm powers.
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

#ifndef SL_RAIL_UTIL_PA_CONVERSIONS_EFR32_CONFIG_H
#define SL_RAIL_UTIL_PA_CONVERSIONS_EFR32_CONFIG_H

#ifdef __cplusplus
extern "C" {
#endif

#ifdef _SILICON_LABS_32B_SERIES_1
#include "efr32xg1x/config/sl_rail_util_pa_config.h"
#elif defined(_SILICON_LABS_32B_SERIES_2_CONFIG_2)
#include "efr32xg22/config/sl_rail_util_pa_config.h"
#elif defined(_SILICON_LABS_32B_SERIES_2_CONFIG_3)
#include "efr32xg23/config/sl_rail_util_pa_config.h"
#elif defined(_SILICON_LABS_32B_SERIES_2_CONFIG_4)
#include "efr32xg24/config/sl_rail_util_pa_config.h"
#elif defined(_SILICON_LABS_32B_SERIES_2_CONFIG_5)
#include "efr32xg25/config/sl_rail_util_pa_config.h"
#elif (_SILICON_LABS_32B_SERIES_2_CONFIG == 7)
#include "efr32xg27/config/sl_rail_util_pa_config.h"
#elif defined (_SILICON_LABS_32B_SERIES_2)
#include "efr32xg21/config/sl_rail_util_pa_config.h"
#else
#error "Unsupported platform!"
#endif

#ifdef __cplusplus
}
#endif

#endif // SL_RAIL_UTIL_PA_CONVERSIONS_EFR32_CONFIG_H
