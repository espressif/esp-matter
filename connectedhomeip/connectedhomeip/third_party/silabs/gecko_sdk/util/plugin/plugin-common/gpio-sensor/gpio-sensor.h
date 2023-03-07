// *****************************************************************************
// * gpio-sensor.h
// *
// * API for interfacing with a generic gpio sensor
// *
// * Copyright 2015 Silicon Laboratories, Inc.                              *80*
// *****************************************************************************

// This code uses a number of ::EmberEventControl events to properly function.
// The application framework will generally manage the event automatically.
// Customers who do not use the framework must ensure the event is run, by
// calling either ::emberRunEvents or ::emberRunTask.

#ifndef __GPIO_SENSOR_H__
#define __GPIO_SENSOR_H__

typedef enum {
  HAL_GPIO_SENSOR_ACTIVE = 0x01,
  HAL_GPIO_SENSOR_NOT_ACTIVE = 0x00,
} HalGpioSensorState;

/** @brief Initialize the GPIO Sensor.  The application framework will
 * generally initialize this plugin automatically.  Customers who do not use the
 * framework must ensure the plugin is initialized by calling this function.
 *
 * This function can be used to initialize all hardware necessary to use the
 * GPIO Sensor plugin.
 */
void halGpioSensorInitialize(void);

/** @brief Get the state (active or not active) of the gpio sensor
 *
 * This function can be used to determine if the gpio sensor is currently
 * active.
 *
 * @return  HAL_GPIO_SENSOR_ACTIVE or HAL_GPIO_SENSOR_NOT_ACTIVE
 */
HalGpioSensorState halGpioSensorGetSensorValue(void);

#endif // __GPIO_SENSOR_H__
