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

#include "cslib_hwconfig.h"
#include "hardware_routines.h"
#include "cslib_config.h"

/// @brief Derive thresholds at init based on config profile settings
uint16_t active_thresh[DEF_NUM_SENSORS];
uint16_t inactive_thresh[DEF_NUM_SENSORS];

const GPIO_Port_TypeDef CSLIB_ports[DEF_NUM_SENSORS] =
{
  CSLIB_CHANNEL_PORT
};

const unsigned long CSLIB_pins[DEF_NUM_SENSORS] =
{
  CSLIB_CHANNEL_PIN
};

const uint8_t CSLIB_muxValues[DEF_NUM_SENSORS] =
{
  MUX_VALUE_ARRAY
};

const uint8_t CSLIB_activeThreshold[DEF_NUM_SENSORS] =
{
  ACTIVE_THRESHOLD_ARRAY
};
const uint8_t CSLIB_inactiveThreshold[DEF_NUM_SENSORS] =
{
  INACTIVE_THRESHOLD_ARRAY
};

const uint8_t CSLIB_averageTouchDelta[DEF_NUM_SENSORS] =
{
  AVERAGE_TOUCH_DELTA_ARRAY
};
