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

#define RAIL_PA_CURVES_PIECEWISE_SEGMENTS (8U)
#define RAIL_PA_CURVES_LP_VALUES (16U)

#define RAIL_PA_CURVES_2P4_HP_VBAT_MAX_POWER      100
#define RAIL_PA_CURVES_2P4_HP_VBAT_MIN_POWER      -260
#define RAIL_PA_CURVES_2P4_HP_VBAT_CURVES \
  { { 65535, 0, 0 },                      \
    { 65535, 0, 0 },                      \
    { 90, 1644, -90531 },                 \
    { 38, 468, 1744 },                    \
    { 21, 223, 11774 },                   \
    { 11, 100, 11531 },                   \
    { 7, 61, 9783 },                      \
    { 3, 6, 3749 } }

#define RAIL_PA_CURVES_2P4_LP_VBAT_MAX_POWER      0
#define RAIL_PA_CURVES_2P4_LP_VBAT_MIN_POWER      -260
#define RAIL_PA_CURVES_2P4_LP_VBAT_CURVES \
  {                                       \
    -250, /*! Power Level 0 */            \
    -148, /*! Power Level 1 */            \
    -95,  /*! Power Level 2 */            \
    -68,  /*! Power Level 3 */            \
    -51,  /*! Power Level 4 */            \
    -40,  /*! Power Level 5 */            \
    -32,  /*! Power Level 6 */            \
    -26,  /*! Power Level 7 */            \
    -22,  /*! Power Level 8 */            \
    -18,  /*! Power Level 9 */            \
    -16,  /*! Power Level 10 */           \
    -13,  /*! Power Level 11 */           \
    -12,  /*! Power Level 12 */           \
    -10,  /*! Power Level 13 */           \
    -9,   /*! Power Level 14 */           \
    -9,   /*! Power Level 15 */           \
  }
// *INDENT-OFF*
// Macro to declare the variables needed to initialize RAIL_TxPowerCurvesConfig_t for use in
// RAIL_InitTxPowerCurves, assuming battery powered operation
#define RAIL_DECLARE_TX_POWER_VBAT_CURVES_ALT                                  \
  static const RAIL_TxPowerCurveAlt_t RAIL_piecewiseDataHp = {                 \
    RAIL_PA_CURVES_2P4_HP_VBAT_MAX_POWER,                                      \
    RAIL_PA_CURVES_2P4_HP_VBAT_MIN_POWER,                                      \
    RAIL_PA_CURVES_2P4_HP_VBAT_CURVES,                                         \
  };                                                                           \
  static const int16_t RAIL_curves24Lp[RAIL_PA_CURVES_LP_VALUES] =             \
    RAIL_PA_CURVES_2P4_LP_VBAT_CURVES;
// *INDENT-OFF*

#define RAIL_DECLARE_TX_POWER_CURVES_CONFIG_ALT                                \
  {                                                                            \
    .curves = {                                                                \
      {                                                                        \
        .algorithm = RAIL_PA_ALGORITHM_PIECEWISE_LINEAR,                       \
        .segments = RAIL_PA_CURVES_PIECEWISE_SEGMENTS,                         \
        .min = RAIL_TX_POWER_LEVEL_2P4_HP_MIN,                                 \
        .max = RAIL_TX_POWER_LEVEL_2P4_HP_MAX,                                 \
        .conversion = { .powerCurve = &RAIL_piecewiseDataHp },                 \
      },                                                                       \
      {                                                                        \
        .algorithm = RAIL_PA_ALGORITHM_MAPPING_TABLE,                          \
        .segments = 0U,                                                        \
        .min = RAIL_TX_POWER_LEVEL_2P4_LP_MIN,                                 \
        .max = RAIL_TX_POWER_LEVEL_2P4_LP_MAX,                                 \
        .conversion = { .mappingTable = &RAIL_curves24Lp[0] },                 \
      },                                                                        \
    }                                                                          \
  }
// *INDENT-OFF*

#ifdef __cplusplus
}
#endif

#endif
