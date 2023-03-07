/***************************************************************************//**
 * @file MultilevelSensor_interface_sensor.c
 * @brief MultilevelSensor_interface_sensor.c
 * @copyright 2022 Silicon Laboratories Inc.
 *******************************************************************************
 * # License
 * <b>Copyright 2022 Silicon Laboratories Inc. www.silabs.com</b>
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
//#define DEBUGPRINT
#include "DebugPrint.h"
#include "sl_i2cspm.h"
#include "sl_i2cspm_instances.h"
#include "sl_si70xx.h"
#include "MultilevelSensor_interface.h"

// -----------------------------------------------------------------------------
//                Macros and Typedefs
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------
//                Global Variables
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------
//                Static Variables
// -----------------------------------------------------------------------------
static bool initialized = false;

// -----------------------------------------------------------------------------
//              Static Function
// -----------------------------------------------------------------------------
static bool 
MultilevelSensor_sensor_init(void)
{
  sl_status_t sc;

  if(initialized) {
    return true;
  }

  sc = sl_si70xx_init(sl_i2cspm_sensor, SI7021_ADDR);
  
  initialized = (sc == SL_STATUS_OK);

  return initialized;
}

static bool 
MultilevelSensor_sensor_deinit(void)
{
  if(!initialized) {
    return true;
  }

  initialized = false;

  return true;
}

static bool 
MultilevelSensor_sensor_read(uint32_t *rh_data, int32_t *temp_data)
{
  if(!initialized) {
    return false;
  }

  return (sl_si70xx_measure_rh_and_temp(sl_i2cspm_sensor, SI7021_ADDR, rh_data, temp_data) == SL_STATUS_OK);
}

// -----------------------------------------------------------------------------
//              Public Function Definitions
// -----------------------------------------------------------------------------

bool 
MultilevelSensor_interface_humidity_init(void)
{
  return MultilevelSensor_sensor_init();
}

bool 
MultilevelSensor_interface_temperature_init(void)
{
  return MultilevelSensor_sensor_init();
}

bool 
MultilevelSensor_interface_humidity_deinit(void)
{
  return MultilevelSensor_sensor_deinit();
}

bool 
MultilevelSensor_interface_temperature_deinit(void)
{
  return MultilevelSensor_sensor_deinit();
}

bool 
MultilevelSensor_interface_humidity_read(sensor_read_result_t* o_result, uint8_t i_scale)
{
  (void)i_scale;

  uint32_t rh_data;
  int32_t temp_data;

  if(o_result != NULL)
  {
    memset(o_result, 0, sizeof(sensor_read_result_t));
    o_result->precision  = SENSOR_READ_RESULT_PRECISION_3;
    o_result->size_bytes = SENSOR_READ_RESULT_SIZE_4;
    
    MultilevelSensor_sensor_read(&rh_data, &temp_data);

    DPRINTF("Humidity: %d\n", rh_data);

    o_result->raw_result[3] = (uint8_t)(rh_data   &0xFF);
    o_result->raw_result[2] = (uint8_t)((rh_data>>8 )&0xFF);
    o_result->raw_result[1] = (uint8_t)((rh_data>>16)&0xFF);
    o_result->raw_result[0] = (uint8_t)((rh_data>>24)&0xFF);
  }

  return true;
}

bool 
MultilevelSensor_interface_temperature_read(sensor_read_result_t* o_result, uint8_t i_scale)
{
  uint32_t rh_data;
  int32_t temp_data, temperature_fahrenheit;
  float temperature_celsius_divided;

  if(o_result != NULL)
  {
    memset(o_result, 0, sizeof(sensor_read_result_t));
    o_result->precision  = SENSOR_READ_RESULT_PRECISION_3;
    o_result->size_bytes = SENSOR_READ_RESULT_SIZE_4;
    
    MultilevelSensor_sensor_read(&rh_data, &temp_data);

    DPRINTF("Temperature: %d\n", rh_data);

    if(i_scale == SENSOR_SCALE_FAHRENHEIT)
    {
      temperature_celsius_divided = (float)temp_data/(float)1000;
      temperature_fahrenheit = (int32_t)((temperature_celsius_divided * ((float)9/(float)5) + (float)32)*1000);

      o_result->raw_result[3] = (uint8_t)(temperature_fahrenheit & 0xFF);
      o_result->raw_result[2] = (uint8_t)((temperature_fahrenheit >> 8 ) & 0xFF);
      o_result->raw_result[1] = (uint8_t)((temperature_fahrenheit >> 16) & 0xFF);
      o_result->raw_result[0] = (uint8_t)((temperature_fahrenheit >> 24) & 0xFF);
    }
    else
    {
      o_result->raw_result[3] = (uint8_t)(temp_data & 0xFF);
      o_result->raw_result[2] = (uint8_t)((temp_data >> 8 ) & 0xFF);
      o_result->raw_result[1] = (uint8_t)((temp_data >> 16) & 0xFF);
      o_result->raw_result[0] = (uint8_t)((temp_data >> 24) & 0xFF);
    }
  }

  return true;
}
