/***************************************************************************//**
 * @file
 * @brief Provides the host utility functions for counters.
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

#include "app/framework/include/af.h"
#include "app/util/serial/sl_zigbee_command_interpreter.h"
#include "app/util/common/common.h"
#include "counters.h"
#include "counters-ota.h"

uint16_t emberCounters[EMBER_COUNTER_TYPE_COUNT];
uint16_t emberCountersThresholds[EMBER_COUNTER_TYPE_COUNT];

void emberAfPluginCountersInitCallback(SLXU_INIT_ARG)
{
  SLXU_INIT_UNUSED_ARG;

  emberAfPluginCountersClear();
  emberAfPluginCountersResetThresholds();
}

void ezspCounterRolloverHandler(EmberCounterType type)
{
  emberAfPluginCountersRolloverCallback(type);
}

void emberAfPluginCountersClear(void)
{
  EzspStatus status;
  status = ezspSetValue(EZSP_VALUE_CLEAR_COUNTERS, 0, NULL);
  if (status != EZSP_SUCCESS) {
    emberAfCorePrintln("ezsp counters failed %u", status);
  }
}

void emberAfPluginCountersResetThresholds(void)
{
  //Clear local copy of thresholds.

  EzspStatus status;
  status = ezspSetValue(EZSP_VALUE_RESET_COUNTER_THRESHOLDS, 0, NULL);
  if (status != EZSP_SUCCESS) {
    emberAfCorePrintln("ezsp Reset Threshold error %u", status);
  } else {
    MEMSET(emberCountersThresholds, 0xFF, sizeof(emberCountersThresholds));
  }
}

void emberAfPluginCountersSetThreshold(EmberCounterType type, uint16_t threshold)
{
  EzspStatus status;
  uint8_t ezspThreshold[3];
  ezspThreshold[0] = type;
  ezspThreshold[1] = LOW_BYTE(threshold);
  ezspThreshold[2] = HIGH_BYTE(threshold);
  status = ezspSetValue(EZSP_VALUE_SET_COUNTER_THRESHOLD, sizeof(ezspThreshold), ezspThreshold);
  if (status != EZSP_SUCCESS) {
    emberAfCorePrintln("ezsp Set Threshold error %u", status);
  } else {
    emberCountersThresholds[type] = threshold;
  }
}
