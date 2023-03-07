/***************************************************************************//**
 * @file
 * @brief Board Control API
 *******************************************************************************
 * # License
 * <b>Copyright 2020 Silicon Laboratories Inc. www.silabs.com</b>
 *******************************************************************************
 *
 * SPDX-License-Identifier: Zlib
 *
 * The licensor of this software is Silicon Laboratories Inc.
 *
 * This software is provided 'as-is', without any express or implied
 * warranty. In no event will the authors be held liable for any damages
 * arising from the use of this software.
 *
 * Permission is granted to anyone to use this software for any purpose,
 * including commercial applications, and to alter it and redistribute it
 * freely, subject to the following restrictions:
 *
 * 1. The origin of this software must not be misrepresented; you must not
 *    claim that you wrote the original software. If you use this software
 *    in a product, an acknowledgment in the product documentation would be
 *    appreciated but is not required.
 * 2. Altered source versions must be plainly marked as such, and must not be
 *    misrepresented as being the original software.
 * 3. This notice may not be removed or altered from any source distribution.
 *
 ******************************************************************************/
#ifndef SL_BOARD_CONTROL_H
#define SL_BOARD_CONTROL_H

#include "sl_status.h"
#include "sl_enum.h"

/***************************************************************************//**
 * @addtogroup board_control Board Control
 * @brief Functions to control Silicon Labs board features
 * @{
 ******************************************************************************/

/// Board Sensor Type
SL_ENUM_GENERIC(sl_board_sensor_t, int) {
  SL_BOARD_SENSOR_RHT           = (1UL << 0UL),     ///< Relative Humidity and Temperature Sensor
  SL_BOARD_SENSOR_LIGHT         = (1UL << 1UL),     ///< UV Index and Ambient Light Sensor
  SL_BOARD_SENSOR_PRESSURE      = (1UL << 2UL),     ///< Barometric Pressure Sensor
  SL_BOARD_SENSOR_HALL          = (1UL << 3UL),     ///< Hall Effect Sensor
  SL_BOARD_SENSOR_GAS           = (1UL << 4UL),     ///< Gas Sensor
  SL_BOARD_SENSOR_IMU           = (1UL << 5UL),     ///< Inertial Measurement Unit (Accelerometer/Gyroscope)
  SL_BOARD_SENSOR_MICROPHONE    = (1UL << 6UL),     ///< Microphone
};

/// Board Memory Type
SL_ENUM_GENERIC(sl_board_memory_t, int) {
  SL_BOARD_MEMORY_SDCARD        = (1UL << 0UL),     ///< SD Card
  SL_BOARD_MEMORY_QSPI          = (1UL << 1UL),     ///< Quad SPI Flash
};

/// Board Oscillator Type
SL_ENUM_GENERIC(sl_board_oscillator_t, int) {
  SL_BOARD_OSCILLATOR_TCXO      = (1UL << 0UL),     ///< TCXO
};

/***************************************************************************//**
 * @brief Enable Virtual COM UART.
 *
 * @return Status code
 * @retval SL_STATUS_OK VCOM was successfully enabled
 * @retval SL_STATUS_FAIL Enabling VCOM failed
 * @retval SL_STATUS_NOT_AVAILABLE VCOM control is not available on this board
 ******************************************************************************/
sl_status_t sl_board_enable_vcom(void);

/***************************************************************************//**
 * @brief Disable Virtual COM UART.
 *
 * @return Status code
 * @retval SL_STATUS_OK VCOM was successfully disabled
 * @retval SL_STATUS_FAIL Disabling VCOM failed
 * @retval SL_STATUS_NOT_AVAILABLE VCOM control is not available on this board
 ******************************************************************************/
sl_status_t sl_board_disable_vcom(void);

/***************************************************************************//**
 * @brief Enable a sensor.
 *
 * @warning
 *   On boards 4166A, 4184A, and 4184B sensors
 *     - Pressure Sensor, RH/Temp Sensor, and UV/Ambient Light Sensor;
 *     - UV/Ambient Light Sensor, Hall-effect Sensor, and RH/Temp Sensor;
 *     - Ambient Light Sensor, Hall-effect Sensor, and RH/Temp Sensor
 *   respectively, are tied to the same enable pin. Calling the enable function
 *   for only one of these sensors has the side-effect of enabling all three;
 *   and calling the disable function for only one of them has the
 *   side-effect of disabling all three.
 *   The latter scenario seems less than desirable.
 *
 * @param[in] sensor Sensor to enable
 *
 * @return Status code
 * @retval SL_STATUS_OK Sensor was successfully enabled
 * @retval SL_STATUS_FAIL Enabling sensor failed
 * @retval SL_STATUS_NOT_AVAILABLE Sensor control is not available on this board
 ******************************************************************************/
sl_status_t sl_board_enable_sensor(sl_board_sensor_t sensor);

/***************************************************************************//**
 * @brief Disable a sensor.
 *
 * @warning
 *   On boards 4166A, 4184A, and 4184B sensors
 *     - Pressure Sensor, RH/Temp Sensor, and UV/Ambient Light Sensor;
 *     - UV/Ambient Light Sensor, Hall-effect Sensor, and RH/Temp Sensor;
 *     - Ambient Light Sensor, Hall-effect Sensor, and RH/Temp Sensor
 *   respectively, are tied to the same enable pin. Calling the enable function
 *   for only one of these sensors has the side-effect of enabling all three;
 *   and calling the disable function for only one of them has the
 *   side-effect of disabling all three.
 *   The latter scenario seems less than desirable.
 *
 * @param[in] sensor Sensors to disable
 *
 * @return Status code
 * @retval SL_STATUS_OK Sensor was successfully disabled
 * @retval SL_STATUS_FAIL Disabling sensor failed
 * @retval SL_STATUS_NOT_AVAILABLE Sensor control is not available on this board
 ******************************************************************************/
sl_status_t sl_board_disable_sensor(sl_board_sensor_t sensor);

/***************************************************************************//**
 * @brief Enable display.
 *
 * @return Status code
 * @retval SL_STATUS_OK Display was successfully enabled
 * @retval SL_STATUS_FAIL Enabling display failed
 * @retval SL_STATUS_NOT_AVAILABLE Display control is not available on this board
 ******************************************************************************/
sl_status_t sl_board_enable_display(void);

/***************************************************************************//**
 * @brief Disable display.
 *
 * @return Status code
 * @retval SL_STATUS_OK Display was successfully disabled
 * @retval SL_STATUS_FAIL Disabling display failed
 * @retval SL_STATUS_NOT_AVAILABLE Display control is not available on this board
 ******************************************************************************/
sl_status_t sl_board_disable_display(void);

/***************************************************************************//**
 * @brief Enable memory.
 *
 * @param[in] memory Memory to enable
 *
 * @return Status code
 * @retval SL_STATUS_OK Memory was successfully enabled
 * @retval SL_STATUS_FAIL Enabling memory failed
 * @retval SL_STATUS_NOT_AVAILABLE Memory control is not available on this board
 ******************************************************************************/
sl_status_t sl_board_enable_memory(sl_board_memory_t memory);

/***************************************************************************//**
 * @brief Disable memory.
 *
 * @param[in] memory Memory to disable
 *
 * @return Status code
 * @retval SL_STATUS_OK Memory was successfully disabled
 * @retval SL_STATUS_FAIL Disabling memory failed
 * @retval SL_STATUS_NOT_AVAILABLE Memory control is not available on this board
 ******************************************************************************/
sl_status_t sl_board_disable_memory(sl_board_memory_t memory);

/***************************************************************************//**
 * @brief Enable an oscillator.
 *
 * @param[in] oscillator Oscillator to enable
 *
 * @return Status code
 * @retval SL_STATUS_OK Oscillator was successfully enabled
 * @retval SL_STATUS_FAIL Enabling oscillator failed
 * @retval SL_STATUS_NOT_AVAILABLE Oscillator control is not available on this board
 ******************************************************************************/
sl_status_t sl_board_enable_oscillator(sl_board_oscillator_t oscillator);

/***************************************************************************//**
 * @brief Disable a oscillator.
 *
 * @param[in] oscillator Oscillator to disable
 *
 * @return Status code
 * @retval SL_STATUS_OK Oscillator was successfully disabled
 * @retval SL_STATUS_FAIL Disabling oscillator failed
 * @retval SL_STATUS_NOT_AVAILABLE Oscillator control is not available on this board
 ******************************************************************************/
sl_status_t sl_board_disable_oscillator(sl_board_oscillator_t oscillator);

/** @} */

#endif // SL_BOARD_CONTROL_H
