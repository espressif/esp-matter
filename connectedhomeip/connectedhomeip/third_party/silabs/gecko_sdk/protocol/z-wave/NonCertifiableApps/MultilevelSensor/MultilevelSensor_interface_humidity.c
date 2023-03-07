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

ZW_WEAK bool MultilevelSensor_interface_humidity_init(void)
{
  return true;
}

ZW_WEAK bool MultilevelSensor_interface_humidity_deinit(void)
{
  return true;
}

ZW_WEAK bool MultilevelSensor_interface_humidity_read(sensor_read_result_t* o_result, uint8_t i_scale)
{
  (void)i_scale;
  static uint32_t humidity = 75;

  if(o_result != NULL)
  {
    memset(o_result, 0, sizeof(sensor_read_result_t));
    o_result->precision  = SENSOR_READ_RESULT_PRECISION_3;
    o_result->size_bytes = SENSOR_READ_RESULT_SIZE_4;
    
    //Percentage is also the default value in this case
    o_result->raw_result[3] = (uint8_t)(humidity   &0xFF);
    o_result->raw_result[2] = (uint8_t)((humidity>>8 )&0xFF);
    o_result->raw_result[1] = (uint8_t)((humidity>>16)&0xFF);
    o_result->raw_result[0] = (uint8_t)((humidity>>24)&0xFF);
  }

  return true;
}
// -----------------------------------------------------------------------------
//              Static Function Definitions
// -----------------------------------------------------------------------------
