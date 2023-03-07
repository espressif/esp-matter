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
#include <stdio.h>
#include <stdint.h>
#include "cslib_sensor_descriptors.h"

/// @brief Contains string names for each defined sensor
const char* sensorDescriptors[DEF_NUM_SENSORS] =
{
  SENSOR_DESCRIPTOR_LIST
};

/***************************************************************************//**
 * @brief
 *   Outputs a line to Capacitive Sensing Profiler naming each sensor
 *
 ******************************************************************************/
void outputsensorDescriptors(void)
{
  uint8_t index;
  // Output control word showing Profiler that line contains sensor names
  printf("*SENSOR_DESCRIPTORS ");

  for (index = 0; index < DEF_NUM_SENSORS; index++) {
    printf("%s | ", sensorDescriptors[index]);
  }
  printf("\n");
}
