/***************************************************************************//**
 * @file
 * @brief PA power conversion curve types used by Silicon Labs PA power
 *   conversion functions.
 * @details This file contains the curve types needed convert PA power levels
 *   to dBm powers.
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

#ifndef PA_CURVE_TYPES_EFR32_H
#define PA_CURVE_TYPES_EFR32_H

#include "rail_types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @struct RAIL_TxPowerCurves
 *
 * @brief Structure containing data defining each segment of the
 * power (deci-dBm) to powerLevel mapping curve fits.
 *
 * Note, these used in an equation of the form:
 *
 *    powerLevel * 1000 = slope * power + intercept
 *
 * powerLevel is the 0-252/0-248/1-7 values used in the RAIL_Get/SetTxPower
 * functions, and power is the actual output power of the PA, specified
 * in deci-dBm.
 *
 * @note If the curves are generated with
 * maxPower and increment other than \ref RAIL_TX_POWER_CURVE_DEFAULT_MAX and
 * \ref RAIL_TX_POWER_CURVE_DEFAULT_INCREMENT respectively, then the first
 * \ref RAIL_TxPowerCurveSegment_t has its maxPowerLevel equal to
 * \ref RAIL_TX_POWER_LEVEL_INVALID and its slope and intercept stores the
 * maxPower and increment in deci-dBm respectively.
 */
typedef struct RAIL_TxPowerCurveSegment {
  /** The highest power level that this segment will be used to convert */
  uint16_t maxPowerLevel;
  /** slope of the line */
  int16_t slope;
  /** y-intercept of the line */
  int32_t intercept;
} RAIL_TxPowerCurveSegment_t;

/**
 * @struct RAIL_TxPowerCurves
 *
 * @brief Structure containing the min and max values for a given
 * PA and voltage supply combination (in deci-dBm).
 */
typedef struct RAIL_TxPowerCurves {
  /** max deci-dBm value */
  int16_t maxPower;
  /** min deci-dBm value */
  int16_t minPower;
  /**
   * Pointer to "piecewiseSegments"-length array of
   * RAIL_TxPowerCurveSegment_t of power (deci-dBm) to
   * powerLevel conversion fits.
   */
  const RAIL_TxPowerCurveSegment_t *powerParams;
} RAIL_TxPowerCurves_t;

/**
 * @struct RAIL_TxPowerCurvesConfig
 *
 * @brief Structure containing curve fit information and other metadata
 * required to properly use the WEAK versions of RAIL_ConvertRawToDb
 * and RAIL_ConvertDbmToRaw
 */
typedef struct RAIL_TxPowerCurvesConfig {
  /**
   * Pointer a RAIL_TxPowerCurves_t representing the piecewise linear segments
   * of curves that map power level to power in dBm for the 2.4 GHz high power
   * PA.
   *
   * @note By the default conversion implementation, segments must be specified
   * in decreasing power order. That is, the 0th entry of this array should be
   * used to convert the highest power (levels). Segment at position n is valid
   * from maxPowerLevel+1 from the segment at n+1 (or 0 if n is array length -
   * 1) to maxPowerLevel of segment n, inclusive.
   */
  const RAIL_TxPowerCurves_t *txPower24HpCurves;

  /**
   * Pointer a RAIL_TxPowerCurves_t representing the piecewise linear segments
   * of curves that map power level to power in dBm for the subgig PA.
   *
   * @note By the default conversion implementation, segments must be specified
   * in decreasing power order. That is, the 0th entry of this array should be
   * used to convert the highest power (levels). Segment at position n is valid
   * from maxPowerLevel+1 from the segment at n+1 (or 0 if n is array length -
   * 1) to maxPowerLevel of segment n, inclusive.
   */
  const RAIL_TxPowerCurves_t *txPowerSgCurves;

  /**
   * Look up table for each of the power levels of the 2.4GHz low power
   * amplifier and their equivalent deci-dB value.
   */
  const int16_t *txPower24LpCurves;
  /**
   * The number of piecewise segments provided to the PA in each of the four
   * conversion curve fits. The default is 8, but regardless of the number, it
   * must be the same for all curves.
   */
  uint8_t piecewiseSegments;
} RAIL_TxPowerCurvesConfig_t;

/// PA conversion algorithms types for converting between dBm and power levels
RAIL_ENUM(RAIL_PaConversionAlgorithm_t) {
  RAIL_PA_ALGORITHM_PIECEWISE_LINEAR, /** Piecewise linear fit */
  RAIL_PA_ALGORITHM_MAPPING_TABLE, /** Mapping table between quantities */
};

#ifndef DOXYGEN_SHOULD_SKIP_THIS
// Self-referencing defines minimize compiler complaints when using RAIL_ENUM
#define RAIL_PA_ALGORITHM_PIECEWISE_LINEAR ((RAIL_PaConversionAlgorithm_t) RAIL_PA_ALGORITHM_PIECEWISE_LINEAR)
#define RAIL_PA_ALGORITHM_MAPPING_TABLE    ((RAIL_PaConversionAlgorithm_t) RAIL_PA_ALGORITHM_MAPPING_TABLE)
#endif//DOXYGEN_SHOULD_SKIP_THIS

/**
 * @struct RAIL_TxPowerCurvesAlt
 *
 * @brief Structure containing the min and max values for a given
 * PA and voltage supply combination (in deci-dBm).
 */
typedef struct RAIL_TxPowerCurveAlt {
  /** max deci-dBm value */
  int16_t maxPower;
  /** min deci-dBm value */
  int16_t minPower;
  /**
   * "piecewiseSegments"-length array of RAIL_TxPowerCurveSegment_t
   * of power (deci-dBm) to powerLevel conversion fits.
   */
  RAIL_TxPowerCurveSegment_t powerParams[];
} RAIL_TxPowerCurveAlt_t;

typedef union RAIL_PowerConversion {
  /**
   * Pointer to a powerCurve containing line segment data for the curves
   * corresponding to a specific PA.
   *
   * @note By the default conversion implementation, segments must be specified
   * in decreasing power order. That is, the 0th entry of this array should be
   * used to convert the highest power (levels). Segment at position n is valid
   * from maxPowerLevel+1 from the segment at n+1 (or 0 if n is array length -
   * 1) to maxPowerLevel of segment n, inclusive.
   */
  const RAIL_TxPowerCurveAlt_t *powerCurve;
  /**
   * Lookup table for PA's which use the mapping table algorithm for converting
   * between deci-dBm and power levels.
   */
  const int16_t *mappingTable;
} RAIL_PowerConversion_t;

/// PA descriptor as used in the PA conversion functions
typedef struct RAIL_PaDescriptor {
  /** Algorithm used to map dBm to power levels for this PA */
  RAIL_PaConversionAlgorithm_t algorithm;
  /**
   * The number of piecewise segments provided to the PA in a piecewise linear
   * curve fit. The default is 8. Should be set to 0 when not using the
   * piecewise linear algorithm.
   */
  uint8_t segments;
  /** Min power level for this PA */
  RAIL_TxPowerLevel_t min;
  /** Max power level for this PA */
  RAIL_TxPowerLevel_t max;
  /** Union containing a pointer to algorithm-specific conversion data. */
  RAIL_PowerConversion_t conversion;
} RAIL_PaDescriptor_t;

/**
 * @typedef RAIL_TxPowerCurvesConfigAlt_t
 *
 * @brief More generic structure containing information about
 * piecewise linear curves and mapping tables, instead of specific PA's.
 */
typedef struct RAIL_TxPowerCurvesConfigAlt {
  RAIL_PaDescriptor_t curves[RAIL_NUM_PA];
  uint32_t signature;
  uint16_t paVoltage;
} RAIL_TxPowerCurvesConfigAlt_t;

#ifdef __cplusplus
}
#endif

#endif // PA_CURVE_TYPES_EFR32_H
