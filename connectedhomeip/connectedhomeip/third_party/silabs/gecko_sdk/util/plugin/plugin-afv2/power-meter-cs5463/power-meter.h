// *****************************************************************************
// * power-meter.h
// *
// * API for reading power-meter data from a power-metering device
// *
// * Copyright 2015 Silicon Laboratories, Inc.                              *80*
// *****************************************************************************

// Some implementations of this function may make use of ::EmberEventControl
// events.  If so, the application framework will generally manage the events
// automatically.  Customers who do not use the framework must ensure the events
// are run, by calling either ::emberRunEvents or ::emberRunTask.  See the
// specific implementation for more information.

#ifndef __POWER_METER_H__
#define __POWER_METER_H__

// define for default value for current gain setting
#define HAL_POWER_METER_CURRENT_GAIN_DEFAULT 0xFFFF

//define status code for a power quality fault
#define HAL_POWER_METER_POWER_QUALITY_ELECTRICITY_FAULT 0x10

/** @brief Over Current Status Change
 *
 * This function is called upon the status change of over current condition.
 *
 * @param status 0: changed from over current to normal; 1: over current
 * occured.  Ver.: always
 */
void emberAfPluginPowerMeterCs5463OverCurrentStatusChangeCallback(uint8_t status);

/** @brief Over Heat Status Change
 *
 * This function is called upon the status change of over heat condition.
 *
 * @param status 0: changed from over heat to normal; 1: over heat occured.
 * Ver.: always
 */
void emberAfPluginPowerMeterCs5463OverHeatStatusChangeCallback(uint8_t status);

/** @brief Get Vrms in milliV
 *
 * This function will perform whatever hardware interaction is necessary to
 * read a Vrms value from the power meter and return it in units
 * of milli volts.
 *
 * @return The Vrms in milli Volt
 */
uint32_t halGetVrmsMilliV(void);

/** @brief Get Apparent Power in mW
 *
 * This function will perform whatever hardware interaction is necessary to
 * read a Apparent Power value from the power meter and return it in units
 * of milli Walt.
 *
 * @return The apparent power in milli Walt
 */
uint32_t halGetApparentPowerMilliW(void);

/** @brief Get Crms in milliA
 *
 * This function will perform whatever hardware interaction is necessary to
 * read a current value from the power meter and return it in units
 * of milli ampere.
 *
 * @return The Crms in milli Amp
 */
uint32_t halGetCrmsMilliA(void);

/** @brief Get Power Factor in the scale of 0-99
 *
 * This function will perform whatever hardware interaction is necessary to
 * read a power factor from the power meter and return it in 1/100s (0-99)
 *
 *
 * @return Power facotr in 0-99
 */
int8_t halGetPowerFactor(void);

/** @brief Get Active Power in mW
 *
 * This function will perform whatever hardware interaction is necessary to
 * read a Active Power value from the power meter and return it in units
 * of milli Walt.
 *
 * @return The signed active power in milli Walt
 */
int32_t halGetActivePowerMilliW(void);

/** @brief Get Reactive Power in mW
 *
 * This function will perform whatever hardware interaction is necessary to
 * read a Reactive Power value from the power meter and return it in units
 * of milli Walt.
 *
 * @return The signed reactive power in milli Walt
 */
int32_t halGetReactivePowerMilliW(void);

/** @brief Get Temperature in Centi C
 *
 * This function will perform whatever hardware interaction is necessary to
 * read a temperature value from the power meter IC and return it in units
 * of CentiC.
 *
 * @return The Temperature in Centi C
 */
int16_t halGetPowerMeterTempCentiC(void);

/** @brief Get Status of power meter
 *
 * This function will get the status of Power meter so that it can fit into
 * the Simple Metering Server/status attribute (0x200)
 * Bit7 Reserved
 * Bit6 Service Disconnect/Open
 * Bit5 Leak Detect
 * Bit4 Power Quality
 * Bit3 Power Failure
 * Bit2 Tamper Detect
 * Bit1 Low Battery
 * Bit0 CheckMeter
 *
 * @return The status Byte
 */
uint8_t halGetPowerMeterStatus(void);

/** @brief Initializes the power meter hardware, along with any hardware
 * peripherals necessary to interface with the hardware. The application
 * framework will generally initialize this plugin automatically.  Customers who
 * do not use the framework must ensure the plugin is initialized by calling
 * this function.
 */
void halPowerMeterInit(void);

/** @brief perform a calibration procedure for power meter sensor. It takes the
 * reference current gain setting and starts a calibration procedure. The
 * halPowerMeterCalibrationFinishedCallback will be called when the
 * calibration is done.
 *
 * @param referenceCurrentMa reference current in mA
 *
 * @return true if calibration was started, false if calibration failed
 *
 */
bool halPowerMeterCalibrateCurrentGain(uint16_t referenceCurrentMa);

/** @brief Set Current Gain of the power meter
 *
 * This function will Set the current gain factor of the power meter. It takes
 * in a 16 bit unsigned value as gain factor, the value of 0x4000 represents a
 * magnitude factor of 1. the factor value should be calculated by:
 *
 * currentGain = 0x4000 * (desired magnitude factor)
 *
 * so the desired magnitude factor should be limited between 0 and 3.9999
 *
 * @param currentGain: current gain setting, a value of
 * HAL_POWER_METER_CURRENT_GAIN_DEFAULT will reset the gain setting
 * to a default unity gain
 *
 */
void halSetCurrentGain(uint16_t currentGain);

/** @brief Over Heat Callback
 *
 * This function is called upon the status change of over heat condition.
 *
 * @param status  OVER_HEAT_TO_NORMAL (0):changed from over heat to normal;
 *                NORMAL_TO_OVER_CURRENT (1):over heat occured.
 *
 */
void halPowerMeterOverHeatStatusChangeCallback(uint8_t status);

/** @brief Over Current Callback
 *
 * This function is called upon the status change of over current condition.
 *
 * @param status OVER_CURRENT_TO_NORMAL (0):changed from over current to normal;
 *               NORMAL_TO_OVER_CURRENT (1):over current occured.
 *
 */
void halPowerMeterOverCurrentStatusChangeCallback(uint8_t status);

/** @brief Calibration Finished Callback
 *
 * This function is called upon a calibration procedure is done
 *
 * @param gainSetting gain register setting.
 *
 */
void halPowerMeterCalibrationFinishedCallback(uint16_t gainSetting);

#endif // __POWER_METER_H__
