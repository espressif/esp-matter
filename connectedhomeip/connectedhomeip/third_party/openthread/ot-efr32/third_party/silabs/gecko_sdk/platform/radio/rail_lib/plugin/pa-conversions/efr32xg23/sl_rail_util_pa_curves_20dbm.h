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

#define RAIL_PA_CURVES_PIECEWISE_SEGMENTS (9U)

#define RAIL_PA_CURVES_SUBGIG_HP_VBAT_MAX_POWER      210
#define RAIL_PA_CURVES_SUBGIG_HP_VBAT_MIN_POWER      -210
#define RAIL_PA_CURVES_SUBGIG_HP_VBAT_CURVES \
  { { 255, 210, 40 },                        \
    { 240, 3646, -515219 },                  \
    { 102, 1181, -97819 },                   \
    { 54, 527, -13663 },                     \
    { 33, 322, 4470 },                       \
    { 20, 193, 10656 },                      \
    { 12, 110, 11194 },                      \
    { 7, 71, 9771 },                         \
    { 4, 28, 6330 } }

#define RAIL_PA_CURVES_SUBGIG_MP_VBAT_MAX_POWER      130
#define RAIL_PA_CURVES_SUBGIG_MP_VBAT_MIN_POWER      -360
#define RAIL_PA_CURVES_SUBGIG_MP_VBAT_CURVES \
  { { 255, 130, 30 },                        \
    { 240, 2274, -50612 },                   \
    { 159, 1766, -3389 },                    \
    { 113, 1140, 38094 },                    \
    { 79, 813, 50006 },                      \
    { 54, 536, 51646 },                      \
    { 39, 409, 48623 },                      \
    { 27, 259, 40694 },                      \
    { 15, 68, 21730 } }

#define RAIL_PA_CURVES_SUBGIG_LP_VBAT_MAX_POWER      30
#define RAIL_PA_CURVES_SUBGIG_LP_VBAT_MIN_POWER      -440
#define RAIL_PA_CURVES_SUBGIG_LP_VBAT_CURVES \
  { { 255, 30, 30 },                         \
    { 240, 2162, 197334 },                   \
    { 174, 1829, 193947 },                   \
    { 132, 1296, 175889 },                   \
    { 94, 897, 150815 },                     \
    { 65, 635, 125867 },                     \
    { 48, 474, 106204 },                     \
    { 32, 331, 83878 },                      \
    { 18, 87, 35666 } }

#define RAIL_PA_CURVES_SUBGIG_LLP_VBAT_MAX_POWER      -70
#define RAIL_PA_CURVES_SUBGIG_LLP_VBAT_MIN_POWER      -460
#define RAIL_PA_CURVES_SUBGIG_LLP_VBAT_CURVES \
  { { 255, -70, 30 },                         \
    { 240, 2063, 387282 },                    \
    { 165, 1619, 339696 },                    \
    { 123, 1184, 281418 },                    \
    { 86, 858, 227870 },                      \
    { 62, 619, 181853 },                      \
    { 43, 433, 140250 },                      \
    { 31, 304, 107624 },                      \
    { 18, 115, 52124 } }

// *INDENT-OFF*
// Macro to declare the variables needed to initialize RAIL_TxPowerCurvesConfig_t for use in
// RAIL_InitTxPowerCurves, assuming battery powered operation
#define RAIL_DECLARE_TX_POWER_VBAT_CURVES_ALT                      \
  static const RAIL_TxPowerCurveAlt_t RAIL_piecewiseDataHp = {     \
    RAIL_PA_CURVES_SUBGIG_HP_VBAT_MAX_POWER,                       \
    RAIL_PA_CURVES_SUBGIG_HP_VBAT_MIN_POWER,                       \
    RAIL_PA_CURVES_SUBGIG_HP_VBAT_CURVES,                          \
  };                                                               \
  static const RAIL_TxPowerCurveAlt_t RAIL_piecewiseDataMp = {     \
    RAIL_PA_CURVES_SUBGIG_MP_VBAT_MAX_POWER,                       \
    RAIL_PA_CURVES_SUBGIG_MP_VBAT_MIN_POWER,                       \
    RAIL_PA_CURVES_SUBGIG_MP_VBAT_CURVES,                          \
  };                                                               \
  static const RAIL_TxPowerCurveAlt_t RAIL_piecewiseDataLp = {     \
    RAIL_PA_CURVES_SUBGIG_LP_VBAT_MAX_POWER,                       \
    RAIL_PA_CURVES_SUBGIG_LP_VBAT_MIN_POWER,                       \
    RAIL_PA_CURVES_SUBGIG_LP_VBAT_CURVES,                          \
  };                                                               \
  static const RAIL_TxPowerCurveAlt_t RAIL_piecewiseDataLlp = {    \
    RAIL_PA_CURVES_SUBGIG_LLP_VBAT_MAX_POWER,                      \
    RAIL_PA_CURVES_SUBGIG_LLP_VBAT_MIN_POWER,                      \
    RAIL_PA_CURVES_SUBGIG_LLP_VBAT_CURVES,                         \
  };

// *INDENT-OFF*

// *INDENT-OFF*
// Macro to declare the variables needed to initialize RAIL_TxPowerCurvesConfig_t for use in
// RAIL_InitTxPowerCurves, assuming battery powered operation
#define RAIL_DECLARE_TX_POWER_CURVES_CONFIG_ALT                                \
  {                                                                            \
    .curves = {                                                                \
      {                                                                        \
        .algorithm = RAIL_PA_ALGORITHM_PIECEWISE_LINEAR,                       \
        .segments = RAIL_PA_CURVES_PIECEWISE_SEGMENTS,                         \
        .min = RAIL_TX_POWER_LEVEL_SUBGIG_HP_MIN,                              \
        .max = RAIL_TX_POWER_LEVEL_SUBGIG_HP_MAX,                              \
        .conversion = { .powerCurve = &RAIL_piecewiseDataHp },                 \
      },                                                                       \
      {                                                                        \
        .algorithm = RAIL_PA_ALGORITHM_PIECEWISE_LINEAR,                       \
        .segments = RAIL_PA_CURVES_PIECEWISE_SEGMENTS,                         \
        .min = RAIL_TX_POWER_LEVEL_SUBGIG_MP_MIN,                              \
        .max = RAIL_TX_POWER_LEVEL_SUBGIG_MP_MAX,                              \
        .conversion = { .powerCurve = &RAIL_piecewiseDataMp },                 \
      },                                                                       \
      {                                                                        \
        .algorithm = RAIL_PA_ALGORITHM_PIECEWISE_LINEAR,                       \
        .segments = RAIL_PA_CURVES_PIECEWISE_SEGMENTS,                         \
        .min = RAIL_TX_POWER_LEVEL_SUBGIG_LP_MIN,                              \
        .max = RAIL_TX_POWER_LEVEL_SUBGIG_LP_MAX,                              \
        .conversion = { .powerCurve = &RAIL_piecewiseDataLp },                 \
      },                                                                       \
      {                                                                        \
        .algorithm = RAIL_PA_ALGORITHM_PIECEWISE_LINEAR,                       \
        .segments = RAIL_PA_CURVES_PIECEWISE_SEGMENTS,                         \
        .min = RAIL_TX_POWER_LEVEL_SUBGIG_LLP_MIN,                             \
        .max = RAIL_TX_POWER_LEVEL_SUBGIG_LLP_MAX,                             \
        .conversion = { .powerCurve = &RAIL_piecewiseDataLlp },                \
      },                                                                       \
    }                                                                          \
  }
// *INDENT-OFF*

#ifdef __cplusplus
}
#endif

#endif
