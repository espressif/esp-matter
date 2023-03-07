/***************************************************************************//**
 * @file CC_MultilevelSensor_SensorHandler.h
 * @brief CC_MultilevelSensor_SensorHandler.h
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
#ifndef CC_MULTILEVELSENSOR_SENSORHANDLER_H
#define CC_MULTILEVELSENSOR_SENSORHANDLER_H
// -----------------------------------------------------------------------------
//                                   Includes
// -----------------------------------------------------------------------------
#include <stdbool.h>
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
 * Calls the init function for all of the registered sensor interfaces.
 */
void
cc_multilevel_sensor_init_all_sensor(void);

/**
 * Checks if the wanted scale is a legal one for an interface.
 * If the scale is not set the function will return with the first available scale.
 * The function will iterate from bit 0 to 7 to look for the first scale which is supported.
 * That will be the default scale.
 * @param[in] i_interface Pointer to an existing sensor interface
 * @param[in] i_scale Scale value to check
 * @return a usable scale value, if the input i_scale is valid that will be return, else the default one.
 */
uint8_t
cc_multilevel_sensor_check_scale(const sensor_interface_t* i_interface, uint8_t i_scale);

/**
 * Checks if the wanted sensor type has a registered interface or not.
 * @param[in] sensor_type_value Sensor type value to check, this is the value in sensor_type_t struct's value
 * @return CC_MULTILEVEL_SENSOR_RETURN_VALUE_OK if interface is registered for a sensor, else CC_MULTILEVEL_SENSOR_RETURN_VALUE_NOT_FOUND.
 */
cc_multilevel_sensor_return_value
cc_multilevel_sensor_check_sensor_type_registered(uint8_t sensor_type_value);

/**
 * Registers an interface to a specific sensor type. The number of registered sensor
 * types is maximized with REGISTERED_SENSOR_NUMBER_LIMIT 
 * @param[in] i_new_sensor Pointer to a filled sensor interface struct
 * @return CC_MULTILEVEL_SENSOR_RETURN_VALUE_OK if interface is registered for a sensor.
 */
cc_multilevel_sensor_return_value
cc_multilevel_sensor_registration(sensor_interface_t* i_new_sensor);

/**
 * Fills an input buffer with flags where each flag represents a supported sensor.
 * @param[out] o_supported_sensor_buffer Pointer to a buffer where the supported sensors' are represented as flags in a byte array.
 * The position of each flag is calculated based on SDS13812. The buffer must be 11 byte long at least.
 * @return CC_MULTILEVEL_SENSOR_RETURN_VALUE_OK if success, else CC_MULTILEVEL_SENSOR_RETURN_VALUE_ERROR.
 */
cc_multilevel_sensor_return_value
cc_multilevel_sensor_get_supported_sensors(uint8_t* o_supported_sensor_buffer);

/**
 * Fills an input buffer with flags where each flag represents a supported scale
 * @param[in] sensor_type_value Sensor type value to check, this is the value in sensor_type_t struct's value
 * @param[out] o_supported_scale Pointer to a buffer where the supported scales will be represented for a sensor type
 * @return CC_MULTILEVEL_SENSOR_RETURN_VALUE_OK if interface is registered for a sensor.
 */
cc_multilevel_sensor_return_value
cc_multilevel_sensor_get_supported_scale(uint8_t sensor_type_value , uint8_t* o_supported_scale);

/**
 * Getter function for a sensor type's interface
 * @param[in] sensor_type_value Sensor type value to check, this is the value in sensor_type_t struct's value
 * @param[out] o_interface This is a double pointer which will hold an interface reference
 * @return CC_MULTILEVEL_SENSOR_RETURN_VALUE_OK if interface is registered for a sensor, else CC_MULTILEVEL_SENSOR_RETURN_VALUE_NOT_FOUND.
 */
cc_multilevel_sensor_return_value
cc_multilevel_sensor_get_interface(uint8_t sensor_type_value, sensor_interface_t** o_interface);

/**
 * Gets the default sensor type value. Basically the first registered sensor interface' type is the default one.
 * @param[in] o_default_sensor_type Pointer to a byte buffer which will hold the default sensor type value
 * @return CC_MULTILEVEL_SENSOR_RETURN_VALUE_OK if there is any registered interface, else CC_MULTILEVEL_SENSOR_RETURN_VALUE_ERROR.
 */
cc_multilevel_sensor_return_value
cc_multilevel_sensor_get_default_sensor_type(uint8_t* o_default_sensor_type);

/**
 * Initialize an iterator struct for the registered sensor interfaces.
 * If there is any registered interface the iterator will be NULL.
 * @param[out] i_iterator This is a double pointer which will hold an interface reference to the first registered interface
 * @return CC_MULTILEVEL_SENSOR_RETURN_VALUE_OK if success, else CC_MULTILEVEL_SENSOR_RETURN_VALUE_ERROR..
 */
cc_multilevel_sensor_return_value
cc_multilevel_sensor_init_iterator(sensor_interface_iterator_t** i_iterator);

/**
 * Moves the iterator reference forward to the next registered interface, if the current is the last one then NULL
 * If there is any registered interface the iterator will be NULL.
 * @param[out] i_iterator This is a double pointer which will hold an interface reference
 * @return CC_MULTILEVEL_SENSOR_RETURN_VALUE_OK.
 */
void
cc_multilevel_sensor_next_iterator(sensor_interface_iterator_t** i_iterator);

/**
 * Returns the number of registered sensor interfaces
 * @return Number of registered sensor interfaces.
 */
uint8_t
cc_multilevel_sensor_get_number_of_registered_sensors(void);

/**
 * Removes all of the registered sensor interfaces from the administration
 */
void
cc_multilevel_sensor_reset_administration(void);

#endif  // CC_MULTILEVELSENSOR_SENSORHANDLER_H
