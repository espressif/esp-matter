
/***************************************************************************//**
 * @file MultilevelSensor_interface.h
 * @brief MultilevelSensor_interface.h
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
#ifndef MULTILEVELSENSOR_INTERFACE_H
#define MULTILEVELSENSOR_INTERFACE_H
// -----------------------------------------------------------------------------
//                                   Includes
// -----------------------------------------------------------------------------
#include <stdbool.h>
#include <stdint.h>
#include "CC_MultilevelSensor_SensorHandlerTypes.h"
// -----------------------------------------------------------------------------
//                              Macros and Typedefs
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------
//                                Global Variables
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------
//                          Public Function Declarations
// -----------------------------------------------------------------------------

/**
 * Init function for the temperature sensor.
 * @return True in case of success, else false.
 */
bool MultilevelSensor_interface_temperature_init(void);

/**
 * Deinit function for the temperature sensor.
 * In this case this function is a placeholder, does not have any actual functionality
 * @return True in case of success, else false.
 */
bool MultilevelSensor_interface_temperature_deinit(void);

/**
 * Reading function for the temperature sensor, this function is called by the Multilevel Sensor Command Class
 * @param[out] o_result This struct pointer will hold the raw reading result, the precision and result size values.
 * @param[in] i_scale This is a switch, which indicates which scale should be used for a sensor reading
 * @return True if success, else false
 */
bool MultilevelSensor_interface_temperature_read(sensor_read_result_t* o_result, uint8_t i_scale);

/**
 * Init function for the humidity sensor.
 * @return True in case of success, else false.
 */
bool MultilevelSensor_interface_humidity_init(void);

/**
 * Deinit function for the humidity sensor.
 * In this case this function is a placeholder, does not have any actual functionality
 * @return True in case of success, else false.
 */
bool MultilevelSensor_interface_humidity_deinit(void);

/**
 * Reading function for the humidity sensor, this function is called by the Multilevel Sensor Command Class
 * @param[out] o_result This struct pointer will hold the raw reading result, the precision and result size values.
 * @param[in] i_scale This is a switch, which indicates which scale should be used for a sensor reading
 * @return True if success, else false
 */
bool MultilevelSensor_interface_humidity_read(sensor_read_result_t* o_result, uint8_t i_scale);

#endif  // SENSOR_INTERFACE_H
