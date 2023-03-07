/***************************************************************************//**
 * @file CC_MultilevelSensor_SensorHandlerTypes.c
 * @brief CC_MultilevelSensor_SensorHandlerTypes.c
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
#include <stddef.h>
#include <string.h>
#include "CC_MultilevelSensor_SensorHandlerTypes.h"
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
/**< Configuration table of the possible sensor types. This table is filled based on SDS13812 table */
static const sensor_type_t sensor_types[] = {
  [SENSOR_NAME_AIR_TEMPERATURE] = {.value = 0x01, .byte_offset = 1, .bit_mask = 0, .max_scale_value = 0x01},
  [SENSOR_NAME_GENERAL_PURPOSE] = {.value = 0x02, .byte_offset = 1, .bit_mask = 1, .max_scale_value = 0x01},
  [SENSOR_NAME_ILLUMINANCE]     = {.value = 0x03, .byte_offset = 1, .bit_mask = 2, .max_scale_value = 0x01},
  [SENSOR_NAME_POWER]           = {.value = 0x04, .byte_offset = 1, .bit_mask = 3, .max_scale_value = 0x01},
  [SENSOR_NAME_HUMIDITY]        = {.value = 0x05, .byte_offset = 1, .bit_mask = 4, .max_scale_value = 0x01},
};
// -----------------------------------------------------------------------------
//              Public Function Definitions
// -----------------------------------------------------------------------------
const sensor_type_t* cc_multilevel_sensor_get_sensor_type(sensor_name_t i_sensor_name)
{
#ifdef UNIT_TEST
  if((i_sensor_name < SENSOR_NAME_MAX_COUNT) && (i_sensor_name >= SENSOR_NAME_AIR_TEMPERATURE))
#else
/*warnings is ignored since (i_sensor_name >= SENSOR_NAME_AIR_TEMPERATURE) always returns true due limited range of data type*/
/*Only disable warning when building targtes. Unit test will fai if warning disabled*/
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wtype-limits"
  if((i_sensor_name < SENSOR_NAME_MAX_COUNT) && (i_sensor_name >= SENSOR_NAME_AIR_TEMPERATURE))
#pragma GCC diagnostic pop
#endif
  {
    return &sensor_types[i_sensor_name];
  }
  return NULL;
}

sensor_interface_return_value_t 
cc_multilevel_sensor_init_interface(sensor_interface_t* i_instance, sensor_name_t i_name)
{
  sensor_interface_return_value_t retval = SENSOR_INTERFACE_RETURN_VALUE_ERROR;
  if((i_instance != NULL) && (i_name < SENSOR_NAME_MAX_COUNT))
  {
    memset(i_instance, 0, sizeof(sensor_interface_t));
    i_instance->sensor_type = cc_multilevel_sensor_get_sensor_type(i_name);
    retval = SENSOR_INTERFACE_RETURN_VALUE_OK;
  }

  return retval;
}

sensor_interface_return_value_t
cc_multilevel_sensor_add_supported_scale_interface(sensor_interface_t* i_instance, uint8_t i_scale)
{
  sensor_interface_return_value_t retval = SENSOR_INTERFACE_RETURN_VALUE_OK;
  if(i_instance != NULL)
  {
    if(i_scale <= i_instance->sensor_type->max_scale_value)
    {
      uint8_t pattern = (uint8_t)(1<<i_scale);
      
      if((i_instance->supported_scale & pattern) > 0 )
      {
        retval = SENSOR_INTERFACE_RETURN_VALUE_ALREADY_SET;
      }
      else
      {
        i_instance->supported_scale |= pattern;
      }
    }
    else
    {
      retval = SENSOR_INTERFACE_RETURN_VALUE_INVALID_SCALE_VALUE;
    }
    
  }
  else
  {
    retval = SENSOR_INTERFACE_RETURN_VALUE_ERROR;
  }
  
  return retval;
}

// -----------------------------------------------------------------------------
//              Static Function Definitions
// -----------------------------------------------------------------------------
