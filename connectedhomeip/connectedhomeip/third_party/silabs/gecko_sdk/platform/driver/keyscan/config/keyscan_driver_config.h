/***************************************************************************//**
 * @file
 * @brief KEYSCAN driver configuration.
 *******************************************************************************
 * # License
 * <b>Copyright 2018 Silicon Laboratories Inc. www.silabs.com</b>
 *******************************************************************************
 *
 * The licensor of this software is Silicon Laboratories Inc. Your use of this
 * software is governed by the terms of Silicon Labs Master Software License
 * Agreement (MSLA) available at
 * www.silabs.com/about-us/legal/master-software-license-agreement. This
 * software is distributed to you in Source Code format and is governed by the
 * sections of the MSLA applicable to Source Code.
 *
 ******************************************************************************/

// <<< Use Configuration Wizard in Context Menu >>>

#ifndef KEYSCAN_DRIVER_CONFIG_H
#define KEYSCAN_DRIVER_CONFIG_H

// <o SL_KEYSCAN_DRIVER_COLUMN_NUMBER> Number of columns in keyscan <1-8>
// <i> Default: 1
#define SL_KEYSCAN_DRIVER_COLUMN_NUMBER 1

// <o SL_KEYSCAN_DRIVER_ROW_NUMBER> Number of rows in keyscan <3-6>
// <i> Default: 3
#define SL_KEYSCAN_DRIVER_ROW_NUMBER    3

// <o SL_KEYSCAN_DRIVER_SCAN_DELAY_MS> Scan Delay
//    <SL_KEYSCAN_DELAY_2MS=>     2ms
//    <SL_KEYSCAN_DELAY_4MS=>     4ms
//    <SL_KEYSCAN_DELAY_6MS=>     6ms
//    <SL_KEYSCAN_DELAY_8MS=>     8ms
//    <SL_KEYSCAN_DELAY_10MS=>    10ms
//    <SL_KEYSCAN_DELAY_12MS=>    12ms
//    <SL_KEYSCAN_DELAY_14MS=>    14ms
//    <SL_KEYSCAN_DELAY_16MS=>    16ms
//    <SL_KEYSCAN_DELAY_18MS=>    18ms
//    <SL_KEYSCAN_DELAY_20MS=>    20ms
//    <SL_KEYSCAN_DELAY_22MS=>    22ms
//    <SL_KEYSCAN_DELAY_24MS=>    24ms
//    <SL_KEYSCAN_DELAY_26MS=>    26ms
//    <SL_KEYSCAN_DELAY_28MS=>    28ms
//    <SL_KEYSCAN_DELAY_30MS=>    30ms
//    <SL_KEYSCAN_DELAY_32MS=>    32ms
// <i> Duration of the scan period per column
// <i> Default: SL_KEYSCAN_DELAY_2MS
#define SL_KEYSCAN_DRIVER_SCAN_DELAY_MS    SL_KEYSCAN_DELAY_2MS

// <o SL_KEYSCAN_DRIVER_DEBOUNCE_DELAY_MS> Debounce Delay
//    <SL_KEYSCAN_DELAY_2MS=>     2ms
//    <SL_KEYSCAN_DELAY_4MS=>     4ms
//    <SL_KEYSCAN_DELAY_6MS=>     6ms
//    <SL_KEYSCAN_DELAY_8MS=>     8ms
//    <SL_KEYSCAN_DELAY_10MS=>    10ms
//    <SL_KEYSCAN_DELAY_12MS=>    12ms
//    <SL_KEYSCAN_DELAY_14MS=>    14ms
//    <SL_KEYSCAN_DELAY_16MS=>    16ms
//    <SL_KEYSCAN_DELAY_18MS=>    18ms
//    <SL_KEYSCAN_DELAY_20MS=>    20ms
//    <SL_KEYSCAN_DELAY_22MS=>    22ms
//    <SL_KEYSCAN_DELAY_24MS=>    24ms
//    <SL_KEYSCAN_DELAY_26MS=>    26ms
//    <SL_KEYSCAN_DELAY_28MS=>    28ms
//    <SL_KEYSCAN_DELAY_30MS=>    30ms
//    <SL_KEYSCAN_DELAY_32MS=>    32ms
// <i> Duration of debounce period once a key press is detected.
// <i> Default: SL_KEYSCAN_DELAY_2MS
#define SL_KEYSCAN_DRIVER_DEBOUNCE_DELAY_MS    SL_KEYSCAN_DELAY_2MS

// <o SL_KEYSCAN_DRIVER_STABLE_DELAY_MS> Stable Delay
//    <SL_KEYSCAN_DELAY_2MS=>     2ms
//    <SL_KEYSCAN_DELAY_4MS=>     4ms
//    <SL_KEYSCAN_DELAY_6MS=>     6ms
//    <SL_KEYSCAN_DELAY_8MS=>     8ms
//    <SL_KEYSCAN_DELAY_10MS=>    10ms
//    <SL_KEYSCAN_DELAY_12MS=>    12ms
//    <SL_KEYSCAN_DELAY_14MS=>    14ms
//    <SL_KEYSCAN_DELAY_16MS=>    16ms
//    <SL_KEYSCAN_DELAY_18MS=>    18ms
//    <SL_KEYSCAN_DELAY_20MS=>    20ms
//    <SL_KEYSCAN_DELAY_22MS=>    22ms
//    <SL_KEYSCAN_DELAY_24MS=>    24ms
//    <SL_KEYSCAN_DELAY_26MS=>    26ms
//    <SL_KEYSCAN_DELAY_28MS=>    28ms
//    <SL_KEYSCAN_DELAY_30MS=>    30ms
//    <SL_KEYSCAN_DELAY_32MS=>    32ms
// <i> Duration of the stable period after the debounce stage.
// <i> Default: SL_KEYSCAN_DELAY_2MS
#define SL_KEYSCAN_DRIVER_STABLE_DELAY_MS    SL_KEYSCAN_DELAY_2MS

// <q SL_KEYSCAN_DRIVER_SINGLEPRESS> keyscan single-press functionality
// <i> Enable or disable single-press functionality.
// <i> Default: 0
#define SL_KEYSCAN_DRIVER_SINGLEPRESS  0

// <<< end of configuration section >>>

// <<< sl:start pin_tool >>>
// <keyscan signal=COL_OUT_0,(COL_OUT_1),(COL_OUT_2),(COL_OUT_3),(COL_OUT_4),(COL_OUT_5),(COL_OUT_6),(COL_OUT_7),ROW_SENSE_0,ROW_SENSE_1,ROW_SENSE_2,(ROW_SENSE_3),(ROW_SENSE_4),(ROW_SENSE_5)> SL_KEYSCAN_DRIVER_KEYSCAN
// $[KEYSCAN_SL_KEYSCAN_DRIVER_KEYSCAN]
#define SL_KEYSCAN_DRIVER_KEYSCAN_COL_OUT_0_PORT  gpioPortC
#define SL_KEYSCAN_DRIVER_KEYSCAN_COL_OUT_0_PIN   0

#define SL_KEYSCAN_DRIVER_KEYSCAN_COL_OUT_1_PORT  gpioPortC
#define SL_KEYSCAN_DRIVER_KEYSCAN_COL_OUT_1_PIN   1

#define SL_KEYSCAN_DRIVER_KEYSCAN_COL_OUT_2_PORT  gpioPortC
#define SL_KEYSCAN_DRIVER_KEYSCAN_COL_OUT_2_PIN   2

#define SL_KEYSCAN_DRIVER_KEYSCAN_COL_OUT_3_PORT  gpioPortC
#define SL_KEYSCAN_DRIVER_KEYSCAN_COL_OUT_3_PIN   3

#define SL_KEYSCAN_DRIVER_KEYSCAN_COL_OUT_4_PORT  gpioPortC
#define SL_KEYSCAN_DRIVER_KEYSCAN_COL_OUT_4_PIN   4

#define SL_KEYSCAN_DRIVER_KEYSCAN_COL_OUT_5_PORT  gpioPortC
#define SL_KEYSCAN_DRIVER_KEYSCAN_COL_OUT_5_PIN   5

#define SL_KEYSCAN_DRIVER_KEYSCAN_COL_OUT_6_PORT  gpioPortC
#define SL_KEYSCAN_DRIVER_KEYSCAN_COL_OUT_6_PIN   6

#define SL_KEYSCAN_DRIVER_KEYSCAN_COL_OUT_7_PORT  gpioPortC
#define SL_KEYSCAN_DRIVER_KEYSCAN_COL_OUT_7_PIN   7

#define SL_KEYSCAN_DRIVER_KEYSCAN_ROW_SENSE_0_PORT  gpioPortA
#define SL_KEYSCAN_DRIVER_KEYSCAN_ROW_SENSE_0_PIN   0

#define SL_KEYSCAN_DRIVER_KEYSCAN_ROW_SENSE_1_PORT  gpioPortA
#define SL_KEYSCAN_DRIVER_KEYSCAN_ROW_SENSE_1_PIN   1

#define SL_KEYSCAN_DRIVER_KEYSCAN_ROW_SENSE_2_PORT  gpioPortA
#define SL_KEYSCAN_DRIVER_KEYSCAN_ROW_SENSE_2_PIN   2

#define SL_KEYSCAN_DRIVER_KEYSCAN_ROW_SENSE_3_PORT  gpioPortA
#define SL_KEYSCAN_DRIVER_KEYSCAN_ROW_SENSE_3_PIN   3

#define SL_KEYSCAN_DRIVER_KEYSCAN_ROW_SENSE_4_PORT  gpioPortA
#define SL_KEYSCAN_DRIVER_KEYSCAN_ROW_SENSE_4_PIN   4

#define SL_KEYSCAN_DRIVER_KEYSCAN_ROW_SENSE_5_PORT  gpioPortA
#define SL_KEYSCAN_DRIVER_KEYSCAN_ROW_SENSE_5_PIN   5
// [KEYSCAN_SL_KEYSCAN_DRIVER_KEYSCAN]$
// <<< sl:end pin_tool >>>

#endif /* KEYSCAN_DRIVER_CONFIG_H */
