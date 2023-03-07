/***************************************************************************//**
 * @file MultilevelSensor_interface_temperature.c
 * @brief MultilevelSensor_interface_temperature.c
 * @copyright 2020 Silicon Laboratories Inc.
 *******************************************************************************
 * # License
 * <b>Copyright 2020 Silicon Laboratories Inc. www.silabs.com</b>
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

// -----------------------------------------------------------------------------
//                   Includes
// -----------------------------------------------------------------------------
#include <string.h>
#include <stddef.h>
#include <stdbool.h>
#include "ZW_typedefs.h"
#include "MultilevelSensor_interface.h"
// -----------------------------------------------------------------------------
//                Macros and Typedefs
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------
//              Static Function Declarations
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------
//                Global Variables
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------
//                Static Variables
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------
//              Public Function Definitions
// -----------------------------------------------------------------------------

ZW_WEAK bool MultilevelSensor_interface_temperature_init(void)
{
  return true;
}

ZW_WEAK bool MultilevelSensor_interface_temperature_deinit(void)
{
  return true;
}

ZW_WEAK bool MultilevelSensor_interface_temperature_read(sensor_read_result_t* o_result, uint8_t i_scale)
{
  static int32_t  temperature_celsius = 3220;

  if(o_result != NULL)
  {
    memset(o_result, 0, sizeof(sensor_read_result_t));
    o_result->precision  = SENSOR_READ_RESULT_PRECISION_3;
    o_result->size_bytes = SENSOR_READ_RESULT_SIZE_4;
    
    if(i_scale == SENSOR_SCALE_FAHRENHEIT)
    {
      float temperature_celsius_divided = (float)temperature_celsius/(float)1000;
      int32_t temperature_fahrenheit = (int32_t)((temperature_celsius_divided * ((float)9/(float)5) + (float)32)*1000);

      o_result->raw_result[3] = (uint8_t)(temperature_fahrenheit&0xFF);
      o_result->raw_result[2] = (uint8_t)((temperature_fahrenheit>>8 )&0xFF);
      o_result->raw_result[1] = (uint8_t)((temperature_fahrenheit>>16)&0xFF);
      o_result->raw_result[0] = (uint8_t)((temperature_fahrenheit>>24)&0xFF);
    }
    else
    {
      o_result->raw_result[3] = (uint8_t)(temperature_celsius&0xFF);
      o_result->raw_result[2] = (uint8_t)((temperature_celsius>>8 )&0xFF);
      o_result->raw_result[1] = (uint8_t)((temperature_celsius>>16)&0xFF);
      o_result->raw_result[0] = (uint8_t)((temperature_celsius>>24)&0xFF);
    }
  }

  return true;
}

// -----------------------------------------------------------------------------
//              Static Function Definitions
// -----------------------------------------------------------------------------
