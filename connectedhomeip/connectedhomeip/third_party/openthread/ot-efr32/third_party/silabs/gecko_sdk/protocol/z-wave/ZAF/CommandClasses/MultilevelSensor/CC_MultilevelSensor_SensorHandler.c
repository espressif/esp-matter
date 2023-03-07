/***************************************************************************//**
 * @file CC_MultilevelSensor_SensorHandler.c
 * @brief CC_MultilevelSensor_SensorHandler.c
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
#include <stdbool.h>
#include "SizeOf.h"
#include "CC_MultilevelSensor_SensorHandler.h"
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
/**< SENSOR_ADMINISTRATION instance which holds the registered sensor interface references. */
static sensor_administration_t sensor_administrator;
// -----------------------------------------------------------------------------
//              Public Function Definitions
// -----------------------------------------------------------------------------
cc_multilevel_sensor_return_value
cc_multilevel_sensor_check_sensor_type_registered(uint8_t sensor_type_value)
{
  cc_multilevel_sensor_return_value retval = CC_MULTILEVEL_SENSOR_RETURN_VALUE_NOT_FOUND;
  sensor_interface_iterator_t* sensor_interface_iterator;
  cc_multilevel_sensor_init_iterator(&sensor_interface_iterator);

  while(sensor_interface_iterator)
  {
    if(sensor_type_value == sensor_interface_iterator->sensor_type->value)
    {
      retval = CC_MULTILEVEL_SENSOR_RETURN_VALUE_OK;
      break;
    }
    cc_multilevel_sensor_next_iterator(&sensor_interface_iterator);
  }

  return retval;
}

cc_multilevel_sensor_return_value
cc_multilevel_sensor_get_default_sensor_type(uint8_t* o_default_sensor_type_value)
{
  cc_multilevel_sensor_return_value retval = CC_MULTILEVEL_SENSOR_RETURN_VALUE_ERROR;

  if((o_default_sensor_type_value != NULL) && (sensor_administrator.number_of_registrated_sensors > 0))
  {
    *o_default_sensor_type_value = sensor_administrator.registrated_sensors[0]->sensor_type->value;
    retval = CC_MULTILEVEL_SENSOR_RETURN_VALUE_OK;
  }

    return retval;
}

uint8_t
cc_multilevel_sensor_check_scale(const sensor_interface_t* i_interface, uint8_t i_scale)
{
  uint8_t default_scale = i_scale;

  if((i_interface != NULL) && ((i_interface->supported_scale&(1<<i_scale)) == 0))
  {
    for(uint8_t mask_shift = 0; mask_shift < 8 ; mask_shift++)
    {
      if(i_interface->supported_scale&(1<<mask_shift))
      {
        default_scale = mask_shift;
        break;
      }
    }
  }

    return default_scale;
}

cc_multilevel_sensor_return_value
cc_multilevel_sensor_get_interface(uint8_t sensor_type_value, sensor_interface_t** o_interface)
{
  cc_multilevel_sensor_return_value retval = CC_MULTILEVEL_SENSOR_RETURN_VALUE_NOT_FOUND;

  if(o_interface != NULL)
  {
    sensor_interface_iterator_t* sensor_interface_iterator;
    cc_multilevel_sensor_init_iterator(&sensor_interface_iterator);

    while(sensor_interface_iterator)
    {
       if(sensor_type_value == sensor_interface_iterator->sensor_type->value)
      {
        *o_interface = sensor_interface_iterator;
        retval = CC_MULTILEVEL_SENSOR_RETURN_VALUE_OK;
        break;
      }
      cc_multilevel_sensor_next_iterator(&sensor_interface_iterator);
    }
  }
  else
  {
    retval = CC_MULTILEVEL_SENSOR_RETURN_VALUE_ERROR;
  }
  

    return retval;
}

cc_multilevel_sensor_return_value
cc_multilevel_sensor_get_supported_scale(uint8_t sensor_type_value , uint8_t* o_supported_scale)
{
    cc_multilevel_sensor_return_value retval = CC_MULTILEVEL_SENSOR_RETURN_VALUE_NOT_FOUND;

  if(o_supported_scale != NULL)
  {
    sensor_interface_iterator_t* sensor_interface_iterator;
    cc_multilevel_sensor_init_iterator(&sensor_interface_iterator);

    while(sensor_interface_iterator)
    {
      if(sensor_type_value == sensor_interface_iterator->sensor_type->value)
      {
        *o_supported_scale = sensor_interface_iterator->supported_scale;
        retval = CC_MULTILEVEL_SENSOR_RETURN_VALUE_OK;
        break;
      }
      cc_multilevel_sensor_next_iterator(&sensor_interface_iterator);
    }
  }
  else
  {
    retval = CC_MULTILEVEL_SENSOR_RETURN_VALUE_ERROR;
  }
  

    return retval;
}

cc_multilevel_sensor_return_value
cc_multilevel_sensor_get_supported_sensors(uint8_t* o_supported_sensor_buffer)
{
  cc_multilevel_sensor_return_value retval = CC_MULTILEVEL_SENSOR_RETURN_VALUE_OK;

  if(o_supported_sensor_buffer != NULL)
  {
    memset(o_supported_sensor_buffer, 0, 11);

    sensor_interface_iterator_t* sensor_interface_iterator;
    cc_multilevel_sensor_init_iterator(&sensor_interface_iterator);

    while(sensor_interface_iterator)
    {
      uint8_t byte_offset =  sensor_interface_iterator->sensor_type->byte_offset;
      uint8_t bit_mask    = (uint8_t)(1 << sensor_interface_iterator->sensor_type->bit_mask);

      o_supported_sensor_buffer[byte_offset-1] |= bit_mask;
      cc_multilevel_sensor_next_iterator(&sensor_interface_iterator);
    }
  }
  else
  {
    retval = CC_MULTILEVEL_SENSOR_RETURN_VALUE_ERROR;
  }
  

  return retval;
}

cc_multilevel_sensor_return_value
cc_multilevel_sensor_registration(sensor_interface_t* i_new_sensor)
{
  cc_multilevel_sensor_return_value retval = CC_MULTILEVEL_SENSOR_RETURN_VALUE_OK;

  if(i_new_sensor != NULL)
  {
    if(sensor_administrator.number_of_registrated_sensors < MULTILEVEL_SENSOR_REGISTERED_SENSOR_NUMBER_LIMIT)
    {
      if(cc_multilevel_sensor_check_sensor_type_registered(i_new_sensor->sensor_type->value) ==
         CC_MULTILEVEL_SENSOR_RETURN_VALUE_NOT_FOUND)
      {
        sensor_administrator.registrated_sensors[sensor_administrator.number_of_registrated_sensors] = i_new_sensor;
        sensor_administrator.number_of_registrated_sensors++;
      }
      else
      {
        retval = CC_MULTILEVEL_SENSOR_RETURN_VALUE_ALREADY_REGISTRATED;
      }
    }
    else
    {
    retval = CC_MULTILEVEL_SENSOR_RETURN_VALUE_REGISTRATION_LIMIT_REACHED;
    }
  }
  else
  {
    retval = CC_MULTILEVEL_SENSOR_RETURN_VALUE_ERROR;
  }
  
  
  return retval;
}

cc_multilevel_sensor_return_value
cc_multilevel_sensor_init_iterator(sensor_interface_iterator_t** i_iterator)
{
  cc_multilevel_sensor_return_value retval = CC_MULTILEVEL_SENSOR_RETURN_VALUE_OK;

  if(sensor_administrator.number_of_registrated_sensors == 0)
  {
    *i_iterator = NULL;
  }
  else
  {
    *i_iterator = sensor_administrator.registrated_sensors[0];
  }
  

  return retval;
}

void
cc_multilevel_sensor_next_iterator(sensor_interface_iterator_t** i_iterator)
{
  if((sensor_administrator.number_of_registrated_sensors > 0) && (*i_iterator != NULL))
  {
    int8_t current_interface_ix = -1;
    uint8_t last_interface;
    uint8_t sensor_number_limit = sensor_administrator.number_of_registrated_sensors;

    if( sensor_number_limit > MULTILEVEL_SENSOR_REGISTERED_SENSOR_NUMBER_LIMIT)
    {
      sensor_number_limit = MULTILEVEL_SENSOR_REGISTERED_SENSOR_NUMBER_LIMIT;
    }

    last_interface = sensor_number_limit - 1;

    for(uint8_t ix = 0; ix < sensor_number_limit; ix++)
    {
      if(sensor_administrator.registrated_sensors[ix] == *i_iterator)
      {
        current_interface_ix = (int8_t)ix;
        break;
      }
    }

    if(current_interface_ix >= 0)
    {
      if(current_interface_ix == last_interface)
      {
        *i_iterator = NULL;
      }
      else
      {
        *i_iterator = sensor_administrator.registrated_sensors[current_interface_ix + 1];
      }
    }     
  }
  else
  {
    *i_iterator = NULL;
  }
}

void
cc_multilevel_sensor_init_all_sensor(void)
{
  sensor_interface_iterator_t* sensor_interface_iterator;
  cc_multilevel_sensor_init_iterator(&sensor_interface_iterator);

  while(sensor_interface_iterator)
  {
    if(sensor_interface_iterator->init != NULL)
    {
      sensor_interface_iterator->init();
    }
    cc_multilevel_sensor_next_iterator(&sensor_interface_iterator);
  }
}

uint8_t
cc_multilevel_sensor_get_number_of_registered_sensors(void)
{
  return sensor_administrator.number_of_registrated_sensors;
}

void
cc_multilevel_sensor_reset_administration(void)
{
  memset(&sensor_administrator, 0, sizeof(sensor_administration_t));
}
// -----------------------------------------------------------------------------
//              Static Function Definitions
// -----------------------------------------------------------------------------
