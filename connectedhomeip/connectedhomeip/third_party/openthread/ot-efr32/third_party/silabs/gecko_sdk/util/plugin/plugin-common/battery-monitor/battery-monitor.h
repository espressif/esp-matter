// *****************************************************************************
// * battery-monitor.h
// *
// * API for monitoring the voltage level on a battery.
// *
// * Copyright 2015 Silicon Laboratories, Inc.                              *80*
// *****************************************************************************

// Some implementations of this function may make use of ::EmberEventControl
// events.  If so, the application framework will generally manage the events
// automatically.  Customers who do not use the framework must ensure the events
// are run, by calling either ::emberRunEvents or ::emberRunTask.  See the
// specific implementation for more information.

#ifndef __BATTERY_MONITOR_H__
#define __BATTERY_MONITOR_H__

/** @brief Get the temperature in millidegrees Celsius
 *
 * This function will perform whatever hardware interaction is necessary to
 * read a temperature value from the temperature sensor and return it in units
 * of millidegrees Celsius.
 *
 * @return The temperature in millidegrees Celsius
 */
uint16_t halGetBatteryVoltageMilliV(void);

/** @brief Initializes the battery monitor hardware.  The application
 * framework will generally initialize this plugin automatically.  Customers who
 * do not use the framework must ensure the plugin is initialized by calling
 * this function.
 */
void halBatteryMonitorInitialize(void);

/** @brief Callback generated when new measured battery voltage data is
 * available.
 *
 * @note This callback will be called when the battery monitor plugin has
 * collected a new measurement of the current battery voltage level.
 *
 * @param voltageMilliV  The battery level measured, in milli volts.
 */
void emberAfPluginBatteryMonitorDataReadyCallback(uint16_t batteryVoltageMilliV);

#endif // __BATTERY_MONITOR_H__
