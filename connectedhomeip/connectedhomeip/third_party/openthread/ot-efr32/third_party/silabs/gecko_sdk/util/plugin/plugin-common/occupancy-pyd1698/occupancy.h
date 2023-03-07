// *****************************************************************************
// * occupancy.h
// *
// * API to interface with an occupancy sensor
// *
// * Copyright 2015 Silicon Laboratories, Inc.                              *80*
// *****************************************************************************

// Some implementations of this function may make use of ::EmberEventControl
// events.  If so, the application framework will generally manage the events
// automatically.  Customers who do not use the framework must ensure the events
// are run, by calling either ::emberRunEvents or ::emberRunTask.  See the
// specific implementation for more information.

#ifndef __OCCUPANCY_H__
#define __OCCUPANCY_H__

// enum used to track the type of occupancy sensor being implemented
typedef enum {
  HAL_OCCUPANCY_SENSOR_TYPE_PIR = 0x00,
  HAL_OCCUPANCY_SENSOR_TYPE_ULTRASONIC = 0x01,
  HAL_OCCUPANCY_SENSOR_TYPE_PIR_AND_ULTRASONIC = 0x02,
} HalOccupancySensorType;

typedef enum {
  HAL_OCCUPANCY_STATE_UNOCCUPIED = 0x00,
  HAL_OCCUPANCY_STATE_OCCUPIED = 0x01,
} HalOccupancyState;

/** @brief Initializes the occupancy sensor, along with any hardware
 * peripherals necessary to interface with the hardware.  The application
 * framework will generally initialize this plugin automatically.  Customers who
 * do not use the framework must ensure the plugin is initialized by calling
 * this function.
 */
void halOccupancyInit(void);

/** @brief Get the hardware mechanism used to detect occupancy
 *
 * This function should be used to determine what kind of hardware mechanism
 * is driving the occupancy functionality.
 *
 * @return HAL_OCCUPANCY_SENSOR_TYPE_PIR, HAL_OCCUPANCY_SENSOR_TYPE_ULTRASONIC,
 * or HAL_OCCUPANCY_SENSOR_TYPE_PIR_AND_ULTRASONIC, which are defined to match
 * the values used by the ZCL defined SensorType attribute
 */
HalOccupancySensorType halOccupancyGetSensorType(void);

/** @brief Notify the system of a change in occupancy state
 *
 * This function will be called whenever the occupancy state of the system
 * changes.
 *
 * @param occupancyState The new occupancy state
 */
void halOccupancyStateChangedCallback(HalOccupancyState occupancyState);

#endif // __OCCUPANCY_H__
