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

#include "cslib_config.h"
#include "cslib.h"

// Struct defined in sensor_node_routines.h.  Contains all state data for a
// sensor.
SensorStruct_t CSLIB_node[DEF_NUM_SENSORS];
uint8_t CSLIB_numSensors = DEF_NUM_SENSORS;
uint8_t CSLIB_sensorBufferSize = DEF_SENSOR_BUFFER_SIZE;
uint8_t CSLIB_buttonDebounce = DEF_BUTTON_DEBOUNCE;
uint16_t CSLIB_activeModePeriod = DEF_ACTIVE_MODE_PERIOD;
uint16_t CSLIB_sleepModePeriod = DEF_SLEEP_MODE_PERIOD;
uint16_t CSLIB_countsBeforeSleep = DEF_COUNTS_BEFORE_SLEEP;
uint8_t CSLIB_freeRunSetting = DEF_FREE_RUN_SETTING;
uint8_t CSLIB_sleepModeEnable = DEF_SLEEP_MODE_ENABLE;
