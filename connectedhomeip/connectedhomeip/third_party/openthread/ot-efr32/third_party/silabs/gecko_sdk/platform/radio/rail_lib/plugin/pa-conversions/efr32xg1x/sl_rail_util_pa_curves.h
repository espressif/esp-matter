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

#ifndef __PA_CURVES_H_
#define __PA_CURVES_H_

#ifdef __cplusplus
extern "C" {
#endif

#define RAIL_PA_CURVES_LP_VALUES 7
#define RAIL_PA_CURVES_2P4_HP_SG_PIECEWISE_SEGMENTS 8

#define RAIL_PA_CURVES_2P4_HP_VBAT_MAX_POWER      200
#define RAIL_PA_CURVES_2P4_HP_VBAT_MIN_POWER      -260
#define RAIL_PA_CURVES_2P4_HP_VBAT_CURVES \
  { { 252, 2776, -300026 },               \
    { 136, 1335, -73192 },                \
    { 83, 772, -7179 },                   \
    { 50, 441, 17309 },                   \
    { 41, 343, 22520 },                   \
    { 22, 209, 22360 },                   \
    { 14, 124, 18896 },                   \
    { 7, 40, 10519 } }

#define RAIL_PA_CURVES_2P4_HP_DCDC_MAX_POWER      140
#define RAIL_PA_CURVES_2P4_HP_DCDC_MIN_POWER      -260
#define RAIL_PA_CURVES_2P4_HP_DCDC_CURVES \
  { { 65535, 0, 0 },                      \
    { 252, 4306, -391604 },               \
    { 117, 1435, -52495 },                \
    { 62, 610, 13579 },                   \
    { 37, 331, 24456 },                   \
    { 23, 224, 23902 },                   \
    { 14, 140, 20330 },                   \
    { 7, 37, 10371 } }

#define RAIL_PA_CURVES_SG_VBAT_MAX_POWER          200
#define RAIL_PA_CURVES_SG_VBAT_MIN_POWER          -260
#define RAIL_PA_CURVES_SG_VBAT_CURVES \
  { { 248, 2757, -319913 },           \
    { 123, 1173, -64900 },            \
    { 74, 694, -8378 },               \
    { 45, 429, 12097 },               \
    { 28, 263, 18309 },               \
    { 18, 167, 18071 },               \
    { 11, 103, 15386 },               \
    { 6, 34, 9064 } }

#define RAIL_PA_CURVES_SG_DCDC_MAX_POWER          150
#define RAIL_PA_CURVES_SG_DCDC_MIN_POWER          -260
#define RAIL_PA_CURVES_SG_DCDC_CURVES \
  { { 248, 9069, -1171644 },          \
    { 199, 3826, -378994 },           \
    { 92, 932, -22748 },              \
    { 50, 470, 13485 },               \
    { 31, 304, 19712 },               \
    { 18, 192, 19146 },               \
    { 11, 110, 15607 },               \
    { 5, 31, 8239 } }

#define RAIL_PA_CURVES_2P4_LP  \
  {                            \
    -125, /*! Power Level 1 */ \
    -65,  /*! Power Level 2 */ \
    -30,  /*! Power Level 3 */ \
    -15,  /*! Power Level 4 */ \
    -7,   /*! Power Level 5 */ \
    -2,   /*! Power Level 6 */ \
    0     /*! Power Level 7 */ \
  }

/**
 * Macros to ease in the delcarations of the various power amplifier units
 * conversion curves.
 */
#define RAIL_DECLARE_PA_2P4_HP_VBAT                                     \
  static const RAIL_TxPowerCurveSegment_t                               \
  twoPointFourHpVbatCurves[RAIL_PA_CURVES_2P4_HP_SG_PIECEWISE_SEGMENTS] \
    = RAIL_PA_CURVES_2P4_HP_VBAT_CURVES;

#define RAIL_DECLARE_PA_2P4_HP_DCDC                                     \
  static const RAIL_TxPowerCurveSegment_t                               \
  twoPointFourHpDcdcCurves[RAIL_PA_CURVES_2P4_HP_SG_PIECEWISE_SEGMENTS] \
    = RAIL_PA_CURVES_2P4_HP_DCDC_CURVES;

#define RAIL_DECLARE_PA_SG_VBAT                                 \
  static const RAIL_TxPowerCurveSegment_t                       \
  subgigVbatCurves[RAIL_PA_CURVES_2P4_HP_SG_PIECEWISE_SEGMENTS] \
    = RAIL_PA_CURVES_SG_VBAT_CURVES;

#define RAIL_DECLARE_PA_SG_DCDC                                 \
  static const RAIL_TxPowerCurveSegment_t                       \
  subgigDcdcCurves[RAIL_PA_CURVES_2P4_HP_SG_PIECEWISE_SEGMENTS] \
    = RAIL_PA_CURVES_SG_DCDC_CURVES;

// Macro to declare the variables needed to initialize
// RAIL_TxPowerCurvesConfigAlt_t for use in RAIL_InitTxPowerCurvesAlt,
// assuming battery powered operation
#define RAIL_DECLARE_TX_POWER_VBAT_CURVES_ALT                      \
  static const RAIL_TxPowerCurveAlt_t RAIL_piecewiseDataHp = {     \
    RAIL_PA_CURVES_2P4_HP_VBAT_MAX_POWER,                          \
    RAIL_PA_CURVES_2P4_HP_VBAT_MIN_POWER,                          \
    RAIL_PA_CURVES_2P4_HP_VBAT_CURVES                              \
  };                                                               \
  static const RAIL_TxPowerCurveAlt_t RAIL_piecewiseDataSg = {     \
    RAIL_PA_CURVES_SG_VBAT_MAX_POWER,                              \
    RAIL_PA_CURVES_SG_VBAT_MIN_POWER,                              \
    RAIL_PA_CURVES_SG_VBAT_CURVES                                  \
  };                                                               \
  static const int16_t RAIL_curves24Lp[RAIL_PA_CURVES_LP_VALUES] = \
    RAIL_PA_CURVES_2P4_LP;
// *INDENT-OFF*

// Macro to declare the variables needed to initialize
// RAIL_TxPowerCurvesConfigAlt_t for use in RAIL_InitTxPowerCurvesAlt,
// assuming DCDC powered operation
#define RAIL_DECLARE_TX_POWER_DCDC_CURVES_ALT                                  \
  static const RAIL_TxPowerCurveAlt_t RAIL_piecewiseDataHp = {                 \
    RAIL_PA_CURVES_2P4_HP_DCDC_MAX_POWER,                                      \
    RAIL_PA_CURVES_2P4_HP_DCDC_MIN_POWER,                                      \
    RAIL_PA_CURVES_2P4_HP_DCDC_CURVES,                                         \
  };                                                                           \
  static const RAIL_TxPowerCurveAlt_t RAIL_piecewiseDataSg = {                 \
    RAIL_PA_CURVES_SG_DCDC_MAX_POWER,                                          \
    RAIL_PA_CURVES_SG_DCDC_MIN_POWER,                                          \
    RAIL_PA_CURVES_SG_DCDC_CURVES                                              \
  };                                                                           \
  static const int16_t RAIL_curves24Lp[RAIL_PA_CURVES_LP_VALUES] =             \
    RAIL_PA_CURVES_2P4_LP;
// *INDENT-OFF*

// Macro to declare the variables needed to initialize
// RAIL_TxPowerCurvesConfig_t for use in RAIL_InitTxPowerCurves,
// assuming battery powered operation
#define RAIL_DECLARE_TX_POWER_VBAT_CURVES(piecewiseSegments, curvesSg,         \
                                          curves24Hp, curves24Lp)              \
  static const uint8_t (piecewiseSegments) =                                   \
    RAIL_PA_CURVES_2P4_HP_SG_PIECEWISE_SEGMENTS;                               \
                                                                               \
  RAIL_DECLARE_PA_2P4_HP_VBAT;                                                 \
  RAIL_DECLARE_PA_SG_VBAT;                                                     \
                                                                               \
  static const RAIL_TxPowerCurves_t curvesSg[1] = {                            \
    { RAIL_PA_CURVES_SG_VBAT_MAX_POWER,                                        \
      RAIL_PA_CURVES_SG_VBAT_MIN_POWER,                                        \
      subgigVbatCurves }                                                       \
  };                                                                           \
  static const RAIL_TxPowerCurves_t curves24Hp[1] = {                          \
    { RAIL_PA_CURVES_2P4_HP_VBAT_MAX_POWER,                                    \
      RAIL_PA_CURVES_2P4_HP_VBAT_MIN_POWER,                                    \
      twoPointFourHpVbatCurves }                                               \
  };                                                                           \
  static const int16_t curves24Lp[RAIL_PA_CURVES_LP_VALUES] =                  \
    RAIL_PA_CURVES_2P4_LP;
// *INDENT-OFF*

// Macro to declare the variables needed to initialize
// RAIL_TxPowerCurvesConfig_t for use in RAIL_InitTxPowerCurves,
// assuming DCDC powered operation
#define RAIL_DECLARE_TX_POWER_DCDC_CURVES(piecewiseSegments, curvesSg,         \
                                          curves24Hp, curves24Lp)              \
  static const uint8_t piecewiseSegments =                                     \
    RAIL_PA_CURVES_2P4_HP_SG_PIECEWISE_SEGMENTS;                               \
                                                                               \
  RAIL_DECLARE_PA_2P4_HP_DCDC;                                                 \
  RAIL_DECLARE_PA_SG_DCDC;                                                     \
                                                                               \
  static const RAIL_TxPowerCurves_t curvesSg[1] = {                            \
    { RAIL_PA_CURVES_SG_DCDC_MAX_POWER,                                        \
      RAIL_PA_CURVES_SG_DCDC_MIN_POWER,                                        \
      subgigDcdcCurves }                                                       \
  };                                                                           \
  static const RAIL_TxPowerCurves_t curves24Hp[1] = {                          \
    { RAIL_PA_CURVES_2P4_HP_DCDC_MAX_POWER,                                    \
      RAIL_PA_CURVES_2P4_HP_DCDC_MIN_POWER,                                    \
      twoPointFourHpDcdcCurves }                                               \
  };                                                                           \
  static const int16_t curves24Lp[RAIL_PA_CURVES_LP_VALUES] =                  \
    RAIL_PA_CURVES_2P4_LP;
// *INDENT-OFF*

#define RAIL_DECLARE_TX_POWER_CURVES_CONFIG_ALT                                \
  {                                                                            \
    .curves = {                                                                \
      {                                                                        \
        .algorithm = RAIL_PA_ALGORITHM_PIECEWISE_LINEAR,                       \
        .segments = RAIL_PA_CURVES_2P4_HP_SG_PIECEWISE_SEGMENTS,               \
        .min = RAIL_TX_POWER_LEVEL_2P4_HP_MIN,                                     \
        .max = RAIL_TX_POWER_LEVEL_2P4_HP_MAX,                                     \
        .conversion = { .powerCurve = &RAIL_piecewiseDataHp },                 \
      },                                                                       \
      {                                                                        \
        .algorithm = RAIL_PA_ALGORITHM_MAPPING_TABLE,                          \
        .segments = 0U,                                                        \
        .min = RAIL_TX_POWER_LEVEL_2P4_LP_MIN,                                     \
        .max = RAIL_TX_POWER_LEVEL_2P4_LP_MAX,                                     \
        .conversion = { .mappingTable = &RAIL_curves24Lp[0] },                 \
      },                                                                       \
      {                                                                        \
        .algorithm = RAIL_PA_ALGORITHM_PIECEWISE_LINEAR,                       \
        .segments = RAIL_PA_CURVES_2P4_HP_SG_PIECEWISE_SEGMENTS,               \
        .min = RAIL_TX_POWER_LEVEL_SUBGIG_MIN,                                 \
        .max = RAIL_TX_POWER_LEVEL_SUBGIG_HP_MAX,                                 \
        .conversion = { .powerCurve = &RAIL_piecewiseDataSg },                 \
      },                                                                       \
    }                                                                          \
  }
// *INDENT-OFF*

#ifdef __cplusplus
}
#endif

#endif
