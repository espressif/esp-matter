/***************************************************************************//**
 * @file
 * @brief PA power conversion functions provided to the customer as source for
 *   highest level of customization.
 * @details This file contains the curves and logic that convert PA power
 *   levels to dBm powers.
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

#ifndef PA_CONVERSIONS_EFR32_H
#define PA_CONVERSIONS_EFR32_H

#include "rail_types.h"

// This macro is defined when Silicon Labs builds curves into the library as WEAK
// to ensure it can be overriden by customer versions of these functions. It
// should *not* be defined in a customer build.
#if !defined(RAIL_PA_CONVERSIONS_WEAK)
#ifdef SL_RAIL_UTIL_PA_CONFIG_HEADER
#include SL_RAIL_UTIL_PA_CONFIG_HEADER
#else
#include "sl_rail_util_pa_conversions_efr32_config.h"
#endif
#endif

#ifdef HAL_CONFIG
#include "hal-config.h"
#ifdef HAL_PA_CURVE_HEADER
#ifdef SL_RAIL_UTIL_PA_CURVE_HEADER
#undef SL_RAIL_UTIL_PA_CURVE_HEADER
#endif
#define SL_RAIL_UTIL_PA_CURVE_HEADER HAL_PA_CURVE_HEADER
#endif
#endif

#ifdef SL_RAIL_UTIL_PA_CURVE_HEADER
#include SL_RAIL_UTIL_PA_CURVE_HEADER
#else
#include "pa_curves_efr32.h"
#endif

#ifdef SL_RAIL_UTIL_PA_CURVE_TYPES
#include SL_RAIL_UTIL_PA_CURVE_TYPES
#else
#include "pa_curve_types_efr32.h"
#endif

#ifdef __cplusplus
extern "C" {
#endif

/// The curves to be used when battery voltage powers transmission
extern const RAIL_TxPowerCurvesConfigAlt_t RAIL_TxPowerCurvesVbat;

/// The curves to be used when the DC-DC converter powers transmission
extern const RAIL_TxPowerCurvesConfigAlt_t RAIL_TxPowerCurvesDcdc;

/**
 * Initialize TxPower curves.
 *
 * @param[in] txPowerCurvesConfig Struct containing pointers to custom
 * tx power curves.
 * @return RAIL_Status_t indicating success or an error.
 *
 * @note: This function is deprecated, and will no longer be supported
 * for any chips released after EFRXG1X parts. Please use
 * RAIL_InitTxPowerCurvesAlt instead.
 */
RAIL_Status_t RAIL_InitTxPowerCurves(const RAIL_TxPowerCurvesConfig_t *config);

/**
 * Initialize TxPower curves.
 *
 * @param[in] txPowerCurvesConfig Struct containing pointers to custom
 * tx power curves.
 * @return RAIL_Status_t indicating success or an error.
 *
 */
RAIL_Status_t RAIL_InitTxPowerCurvesAlt(const RAIL_TxPowerCurvesConfigAlt_t *config);

/**
 * Gets the curve that should be used for conversion functions based on the
 * current PA configuration.
 *
 * @param[in] mode PA mode whose curves are needed.
 * @return RAIL_TxPowerCurves_t that should be used for conversion functions.
 *
 * @note: If the mode is not supported by the the chip,
 * then NULL will be returned.
 */
RAIL_TxPowerCurves_t const * RAIL_GetTxPowerCurve(RAIL_TxPowerMode_t mode);

/**
 * Gets the maximum power in deci-dBm that should be used for calculating
 * the segments and to find right curve segment to convert Dbm to raw power
 * level for a specific PA.
 * For the PAs with \ref RAIL_PaConversionAlgorithm_t
 * \ref RAIL_PA_ALGORITHM_PIECEWISE_LINEAR , if the curves are generated with
 * maxPower and increment other than \ref RAIL_TX_POWER_CURVE_DEFAULT_MAX and
 * \ref RAIL_TX_POWER_CURVE_DEFAULT_INCREMENT respectively, then the first
 * \ref RAIL_TxPowerCurveSegment_t has its maxPowerLevel equal to
 * \ref RAIL_TX_POWER_LEVEL_INVALID and its slope and intercept stores the
 * maxPower and increment in deci-dBm respectively.
 *
 * @param[in] railHandle A RAIL instance handle.
 * @param[in] mode PA mode whose curves are needed.
 * @param[in] maxpower A pointer to memory allocated to hold the maxpower in
 * deci-dBm used in calculation of curve segments .
 * A NULL configuration will produce undefined behavior.
 * @param[in] increment A pointer to memory allocated to hold the increment in
 * deci-dBm used in calculation of curve segments.
 * A NULL configuration will produce undefined behavior.
 * @return RAIL_Status_t indicating success or an error.
 *
 */
RAIL_Status_t RAIL_GetTxPowerCurveLimits(RAIL_Handle_t railHandle,
                                         RAIL_TxPowerMode_t mode,
                                         RAIL_TxPower_t *maxpower,
                                         RAIL_TxPower_t *increment);

/**
 * Initialize PA TX Curves
 *
 */
void sl_rail_util_pa_init(void);

/**
 * Get a pointer to the TX Power Config 2.4 GHz structure.
 *
 * @return a pointer to the TX Power Config stucture.
 *
 */
RAIL_TxPowerConfig_t *sl_rail_util_pa_get_tx_power_config_2p4ghz(void);

/**
 * Get a pointer to the TX Power Config Sub-GHz structure.
 *
 * @return a pointer to the TX Power Config stucture.
 *
 */
RAIL_TxPowerConfig_t *sl_rail_util_pa_get_tx_power_config_subghz(void);

/**
 * Provide a channel config change callback capable of configuring the PA
 * correctly.
 *
 * @param[in] rail_handle The RAIL handle being passed into this callback.
 * @param[in] entry The channel config entry being switched to by hardware.
 *
 */
void sl_rail_util_pa_on_channel_config_change(RAIL_Handle_t rail_handle,
                                              const RAIL_ChannelConfigEntry_t *entry);

#ifdef __cplusplus
}
#endif

#endif // PA_CONVERSIONS_EFR32_H
