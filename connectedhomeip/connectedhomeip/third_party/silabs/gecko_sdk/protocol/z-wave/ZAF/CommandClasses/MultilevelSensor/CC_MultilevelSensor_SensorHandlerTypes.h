/***************************************************************************//**
 * @file CC_MultilevelSensor_SensorHandlerTypes.h
 * @brief CC_MultilevelSensor_SensorHandlerTypes.h
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
#ifndef CC_MULTILEVELSENSOR_SENSORHANDLER_TYPES_H
#define CC_MULTILEVELSENSOR_SENSORHANDLER_TYPES_H
// -----------------------------------------------------------------------------
//                   Includes
// -----------------------------------------------------------------------------
#include <stdint.h>
#include <stdbool.h>
#include "CC_MultilevelSensor_Configuration.h"
// -----------------------------------------------------------------------------
//                Macros and Typedefs
// -----------------------------------------------------------------------------
#define SENSOR_SCALE_DEFAULT        0x00
#define SENSOR_SCALE_CELSIUS        0x00
#define SENSOR_SCALE_FAHRENHEIT     0x01
#define SENSOR_SCALE_PERCENTAGE     0x00
#define SENSOR_SCALE_DIMENSIONLESS  0x01
#define SENSOR_SCALE_LUX            0x01
#define SENSOR_SCALE_WATT           0x00
#define SENSOR_SCALE_BTU_H          0x01 /*Btu/h*/
#define SENSOR_SCALE_ABSOLUTE_HUMIDITY  0x01 /*g/m3*/

/**
 * Defined possible sensor types
 */
typedef enum sensor_name{
  SENSOR_NAME_AIR_TEMPERATURE,
  SENSOR_NAME_GENERAL_PURPOSE,
  SENSOR_NAME_ILLUMINANCE,
  SENSOR_NAME_POWER,
  SENSOR_NAME_HUMIDITY,
  SENSOR_NAME_MAX_COUNT
}sensor_name_t;

/**
 * Structure that holds the attributes of a sensor type.
 */
typedef struct _sensor_type{
  uint8_t value;        ///< Sensor type id from SDS13812 
  uint8_t byte_offset;    ///< Supported bitmask byte number from SDS13812 
  uint8_t bit_mask;       ///< Supported bitmask bit number from SDS13812 
  uint8_t max_scale_value;  ///< Maximum supported scale number from SDS13812
}sensor_type_t;

/**
 * Defined possible sensor read size
 */
typedef enum {
  SENSOR_READ_RESULT_SIZE_1 = 1,
  SENSOR_READ_RESULT_SIZE_2 = 2,
  SENSOR_READ_RESULT_SIZE_4 = 4,
  SENSOR_READ_RESULT_SIZE_END = 5
}sensor_read_result_size_t;

/**
 * Defined possible sensor read precision
 */
typedef enum {
  SENSOR_READ_RESULT_PRECISION_1 = 1,
  SENSOR_READ_RESULT_PRECISION_2,
  SENSOR_READ_RESULT_PRECISION_3,
  SENSOR_READ_RESULT_PRECISION_END
}sensor_read_result_precision;

/**
 * Structure that holds a read result from the read interface.
 */
#define SLI_MAX_RAW_RESULT_BYTES 4
typedef struct _sensor_read_result {
  uint8_t raw_result[SLI_MAX_RAW_RESULT_BYTES];   ///< The raw buffer which holds the result.
  sensor_read_result_precision precision;     ///< The precision which the raw result should be interpret with
  sensor_read_result_size_t size_bytes;     ///< The size which the raw result should be interpret with
}sensor_read_result_t;

/**
 * Structure that holds a read result from the read interface.
 */
typedef struct _sensor_interface {
  const sensor_type_t* sensor_type;                  ///< Reference of a sensor type structure
  uint8_t supported_scale;                        ///< Each bit represents a supported scale
  bool (*init)(void);                           ///< Function pointer to initialize a sensor
  bool (*deinit)(void);                           ///< Function pointer to deinitialize a sensor
  bool (*read_value)(sensor_read_result_t* o_result, uint8_t i_scale); ///< Function pointer to read a sensor value
}sensor_interface_t, sensor_interface_iterator_t;

/**
 * Defined sensor interface return values
 */
typedef enum {
  SENSOR_INTERFACE_RETURN_VALUE_OK,
  SENSOR_INTERFACE_RETURN_VALUE_ALREADY_SET,
  SENSOR_INTERFACE_RETURN_VALUE_INVALID_SCALE_VALUE,
  SENSOR_INTERFACE_RETURN_VALUE_ERROR,
  SENSOR_INTERFACE_RETURN_VALUE_END
}sensor_interface_return_value_t;

/**
 * Defined Multilevel Sensor return values
 */
typedef enum {
  CC_MULTILEVEL_SENSOR_RETURN_VALUE_OK,
  CC_MULTILEVEL_SENSOR_RETURN_VALUE_ERROR,
  CC_MULTILEVEL_SENSOR_RETURN_VALUE_NOT_FOUND,
  CC_MULTILEVEL_SENSOR_RETURN_VALUE_ALREADY_REGISTRATED,
  CC_MULTILEVEL_SENSOR_RETURN_VALUE_REGISTRATION_LIMIT_REACHED,
  CC_MULTILEVEL_SENSOR_RETURN_VALUE_END
}cc_multilevel_sensor_return_value;
 
/**
 * Structure that administrate all of the registered sensors.
 */

typedef struct _sensor_administration {
  sensor_interface_t* registrated_sensors[MULTILEVEL_SENSOR_REGISTERED_SENSOR_NUMBER_LIMIT];  ///< Reference of the sensor interfaces
  uint8_t number_of_registrated_sensors;                  ///< Stores how many sensors are registered
}sensor_administration_t;

// -----------------------------------------------------------------------------
//                Global Variables
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------
//              Public Function Declarations
// -----------------------------------------------------------------------------
/**
 * Returns a sensor_type_t struct from the sensor_types config table
 * @param[in] i_sensor_name The name of the sensor type, the sensor type attributes'
 * reference will be look for based on this value
 *
 * @return sensor_type_t struct which includes the type attributes.
 */
const sensor_type_t*
cc_multilevel_sensor_get_sensor_type(sensor_name_t i_sensor_name);

/**
 * Initialize a sensor interface instance. Sets the whole struct to zero and sets
 * the reference to the appropiate sensor_type_t struct.
 * @param[in] i_instance Pointer to an existing sensor interface to init
 * @param[in] i_name The name of the sensor type, the sensor type attributes'
 * reference will be look for based on this value
 *
 * @return Status of the initialization.
 */
sensor_interface_return_value_t
cc_multilevel_sensor_init_interface(sensor_interface_t* i_instance, sensor_name_t i_name);

/**
 * Adds a new scale to an existing interface. Supported scales must be set by this function during initialization.
 * @param[in] i_instance Pointer to an existing sensor interface which the new scale will be registered to
 * @param[in] i_scale The new scale which will be registered
 *
 * @return Status of the new scale value registration.
 */
sensor_interface_return_value_t
cc_multilevel_sensor_add_supported_scale_interface(sensor_interface_t* i_instance, uint8_t i_scale);

#endif  // CC_MULTILEVELSENSOR_SENSORHANDLER_TYPES_H
