/***************************************************************************//**
 * @file
 *******************************************************************************
 * # License
 * <b>Copyright 2018 Silicon Laboratories Inc. www.silabs.com</b>
 *******************************************************************************
 *
 * The licensor of this software is Silicon Laboratories Inc.  Your use of this
 * software is governed by the terms of Silicon Labs Master Software License
 * Agreement (MSLA) available at
 * www.silabs.com/about-us/legal/master-software-license-agreement.  This
 * software is distributed to you in Source Code format and is governed by the
 * sections of the MSLA applicable to Source Code.
 *
 ******************************************************************************/
#ifndef PROFILER_INTERFACE_H
#define PROFILER_INTERFACE_H

// NOTE these configurations are chosen in a compiler directive that assigns a
// value to OUTPUT_MODE.  The directive can be found and configurations
// changed using the pull-down menu in Project->Target Toolchain configuration.
#define RAW_ONLY 1
#define FULL_OUTPUT_RX_FROM_FILE 2
#define FULL_OUTPUT_RX_FROM_SENSOR 3
#define SLIDER_OUTPUT_RX_FROM_SENSOR 4
#define CUSTOM_OUTPUT      5

#define OUTPUT_MODE FULL_OUTPUT_RX_FROM_SENSOR
void CSLIB_commUpdate(void);

// FULL_OUTPUT_RX_FROM_SENSOR.  This setting uses real sensor data
// and outputs most algorithmic data for analysis.

#if OUTPUT_MODE == FULL_OUTPUT_RX_FROM_SENSOR
#define RECEIVE_DATA_SENSOR      1
#define RECEIVE_DATA_FILE        0

#define PRINT_OUTPUTBUFFER       1
#define PRINT_BUTTON_STATE       0
#define PRINT_BASELINES          1
#define PRINT_SLIDER             0
#define PRINT_RAW                1
#define PRINT_PROCESS            1
#define PRINT_SINGLE_ACTIVE      1
#define PRINT_DEBOUNCE_ACTIVE    1
#define PRINT_TOUCH_DELTA        1
#define PRINT_NOISE              1
#define PRINT_EXP_VALUE          1
#define PRINT_GLOBAL_NOISE_EST   1

#endif

#endif // PROFILER_INTERFACE_H
