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

#define RAIL_PA_CURVES_SUBGIG_HP_VBAT_MAX_POWER      150
#define RAIL_PA_CURVES_SUBGIG_HP_VBAT_MIN_POWER      -230
#define RAIL_PA_CURVES_SUBGIG_HP_VBAT_CURVES \
  { { 255, 150, 30 },                        \
    { 240, 5223, -545497 },                  \
    { 83, 1159, -55791 },                    \
    { 46, 537, -1086 },                      \
    { 30, 336, 10409 },                      \
    { 20, 208, 13964 },                      \
    { 13, 133, 13628 },                      \
    { 9, 90, 12109 },                        \
    { 5, 34, 7791 } }

#define RAIL_PA_CURVES_SUBGIG_MP_VBAT_MAX_POWER      110
#define RAIL_PA_CURVES_SUBGIG_MP_VBAT_MIN_POWER      -350
#define RAIL_PA_CURVES_SUBGIG_MP_VBAT_CURVES \
  { { 255, 110, 30 },                        \
    { 240, 3012, -93837 },                   \
    { 136, 1606, 13953 },                    \
    { 91, 963, 44887 },                      \
    { 60, 624, 50476 },                      \
    { 42, 438, 48173 },                      \
    { 29, 287, 41553 },                      \
    { 20, 184, 33923 },                      \
    { 12, 58, 18582 } }

#define RAIL_PA_CURVES_SUBGIG_LP_VBAT_MAX_POWER      30
#define RAIL_PA_CURVES_SUBGIG_LP_VBAT_MIN_POWER      -380
#define RAIL_PA_CURVES_SUBGIG_LP_VBAT_CURVES \
  { { 255, 30, 10 },                         \
    { 240, 2079, 182853 },                   \
    { 212, 2594, 175536 },                   \
    { 186, 2360, 176677 },                   \
    { 165, 1942, 174595 },                   \
    { 144, 1695, 170848 },                   \
    { 129, 1505, 166174 },                   \
    { 115, 1347, 160920 },                   \
    { 75, 346, 97054 } }

#define RAIL_PA_CURVES_SUBGIG_LLP_VBAT_MAX_POWER      -50
#define RAIL_PA_CURVES_SUBGIG_LLP_VBAT_MIN_POWER      -380
#define RAIL_PA_CURVES_SUBGIG_LLP_VBAT_CURVES \
  { { 255, -50, 10 },                         \
    { 240, 1940, 337021 },                    \
    { 211, 2248, 356938 },                    \
    { 186, 2011, 339016 },                    \
    { 166, 1871, 327092 },                    \
    { 148, 1653, 306345 },                    \
    { 133, 1524, 292865 },                    \
    { 118, 1216, 257702 },                    \
    { 79, 421, 141147 } }

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
