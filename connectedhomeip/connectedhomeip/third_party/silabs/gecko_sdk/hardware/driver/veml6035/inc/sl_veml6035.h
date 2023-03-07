/***************************************************************************//**
 * @file
 * @brief Driver for the VEML6035 ambient light sensor
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

#ifndef SL_VEML6035_H
#define SL_VEML6035_H

#include "sl_i2cspm.h"
#include "sl_status.h"
#include "sl_enum.h"

#ifdef __cplusplus
extern "C" {
#endif

/**************************************************************************//**
* @addtogroup veml6035 VEML6035 - Ambient Light Sensor
* @brief Driver for the Vishay VEML6025 ambient light sensor
* @{
******************************************************************************/

/** @cond DO_NOT_INCLUDE_WITH_DOXYGEN */
#define SL_VEML6035_SENS_SHIFT             12
#define SL_VEML6035_SENS_MASK              0x1000
#define SL_VEML6035_SENS_HIGH              (0x0000 << SL_VEML6035_SENS_SHIFT)
#define SL_VEML6035_SENS_LOW               (0x0001 << SL_VEML6035_SENS_SHIFT)

#define SL_VEML6035_DG_SHIFT               11
#define SL_VEML6035_DG_MASK                0x0800
#define SL_VEML6035_DG_NORMAL              (0x0000  << SL_VEML6035_DG_SHIFT)
#define SL_VEML6035_DG_DOUBLE              (0x0001 << SL_VEML6035_DG_SHIFT)

#define SL_VEML6035_GAIN_MASK              0x0400
#define SL_VEML6035_GAIN_SHIFT             10
#define SL_VEML6035_GAIN_NORMAL            (0x00 << SL_VEML6035_GAIN_SHIFT)
#define SL_VEML6035_GAIN_DOUBLE            (0x01 << SL_VEML6035_GAIN_SHIFT)

#define SL_VEML6035_ALS_IT_SHIFT           6
#define SL_VEML6035_ALS_IT_MASK            0x03C0
#define SL_VEML6035_ALS_IT_25MS            (0x000C << SL_VEML6035_ALS_IT_SHIFT)
#define SL_VEML6035_ALS_IT_50MS            (0x0008 << SL_VEML6035_ALS_IT_SHIFT)
#define SL_VEML6035_ALS_IT_100MS           (0X0000 << SL_VEML6035_ALS_IT_SHIFT)
#define SL_VEML6035_ALS_IT_200MS           (0x0001 << SL_VEML6035_ALS_IT_SHIFT)
#define SL_VEML6035_ALS_IT_400MS           (0x0002 << SL_VEML6035_ALS_IT_SHIFT)
#define SL_VEML6035_ALS_IT_800MS           (0x0003 << SL_VEML6035_ALS_IT_SHIFT)

#define SL_VEML6035_ALS_PERS_SHIFT         4
#define SL_VEML6035_ALS_PERS_MASK          0x0030
#define SL_VEML6035_ALS_PERS_1             (0x0000 << SL_VEML6035_ALS_PERS_SHIFT)
#define SL_VEML6035_ALS_PERS_2             (0x0001 << SL_VEML6035_ALS_PERS_SHIFT)
#define SL_VEML6035_ALS_PERS_4             (0x0002 << SL_VEML6035_ALS_PERS_SHIFT)
#define SL_VEML6035_ALS_PERS_8             (0x0003 << SL_VEML6035_ALS_PERS_SHIFT)

#define SL_VEML6035_INT_CHANNEL_SHIFT      3
#define SL_VEML6035_INT_CHANNEL_MASK       0x0008
#define SL_VEML6035_INT_CHANNEL_ALS        (0X0000 << SL_VEML6035_INT_CHANNEL_SHIFT)
#define SL_VEML6035_INT_CHANNEL_WHITE      (0X0001 << SL_VEML6035_INT_CHANNEL_SHIFT)

#define SL_VEML6035_CHANNEL_EN_SHIFT       2
#define SL_VEML6035_CHANNEL_EN_MASK        0x0004
#define SL_VEML6035_CHANNEL_EN_ALS         (0X0000 << SL_VEML6035_CHANNEL_EN_SHIFT)
#define SL_VEML6035_CHANNEL_EN_ALS_WHITE   (0X0001 << SL_VEML6035_CHANNEL_EN_SHIFT)

#define SL_VEML6035_INT_EN_SHIFT           1
#define SL_VEML6035_INT_EN_MASK            0x0002
#define SL_VEML6035_INT_EN_DIS             (0X0000 << SL_VEML6035_INT_EN_SHIFT)
#define SL_VEML6035_INT_EN_EN              (0X0001 << SL_VEML6035_INT_EN_SHIFT)

#define SL_VEML6035_SD_SHIFT               0
#define SL_VEML6035_SD_MASK                0x0001
#define SL_VEML6035_SD_ON                  (0x0000 << SL_VEML6035_SD_SHIFT)
#define SL_VEML6035_SD_OFF                 (0x0001 << SL_VEML6035_SD_SHIFT)

#define SL_VEML6035_PSM_WAIT_SHIFT         1
#define SL_VEML6035_PSM_WAIT_MASK          0x0006
#define SL_VEML6035_PSM_WAIT_400_MS        (0x0000 << SL_VEML6035_PSM_WAIT_SHIFT)
#define SL_VEML6035_PSM_WAIT_800_MS        (0x0001 << SL_VEML6035_PSM_WAIT_SHIFT)
#define SL_VEML6035_PSM_WAIT_1600_MS       (0x0002 << SL_VEML6035_PSM_WAIT_SHIFT)
#define SL_VEML6035_PSM_WAIT_3200_MS       (0x0003 << SL_VEML6035_PSM_WAIT_SHIFT)

#define SL_VEML6035_PSM_EN_SHIFT           0
#define SL_VEML6035_PSM_EN_MASK            0x0001
#define SL_VEML6035_PSM_EN_EN              (0x0001 << SL_VEML6035_PSM_EN_SHIFT)
#define SL_VEML6035_PSM_EN_DIS             (0x0000 << SL_VEML6035_PSM_EN_SHIFT)
/** @endcond */

/// Integration time settings
SL_ENUM_GENERIC(sl_veml6035_integration_time_t, uint16_t){
  veml6035_integration_time_800_ms  = SL_VEML6035_ALS_IT_800MS,  ///< 800ms integration time
  veml6035_integration_time_400_ms  = SL_VEML6035_ALS_IT_400MS,  ///< 400ms integration time
  veml6035_integration_time_200_ms  = SL_VEML6035_ALS_IT_200MS,  ///< 200ms integration time
  veml6035_integration_time_100_ms  = SL_VEML6035_ALS_IT_100MS,  ///< 100ms integration time
  veml6035_integration_time_50_ms   = SL_VEML6035_ALS_IT_50MS,   ///< 50ms integration time
  veml6035_integration_time_25_ms   = SL_VEML6035_ALS_IT_25MS    ///< 25ms integration time
};

/// Wait time between measurements in power save mode
SL_ENUM_GENERIC(sl_veml6035_psm_wait_t, uint16_t) {
  veml6035_psm_wait_3200_ms = SL_VEML6035_PSM_WAIT_3200_MS,      ///< 3200ms wait time
  veml6035_psm_wait_1600_ms = SL_VEML6035_PSM_WAIT_1600_MS,      ///< 1600ms wait time
  veml6035_psm_wait_800_ms  = SL_VEML6035_PSM_WAIT_800_MS,       ///< 800ms wait time
  veml6035_psm_wait_400_ms  = SL_VEML6035_PSM_WAIT_400_MS        ///< 400ms wait time
};

/// Interrupt persistence settings
SL_ENUM_GENERIC(sl_veml6035_als_pers_t, uint16_t){
  veml6035_als_pers_1 = SL_VEML6035_ALS_PERS_1,                  ///< 1 sensor reading above/under threshold before interrupt trigger
  veml6035_als_pers_2 = SL_VEML6035_ALS_PERS_2,                  ///< 2 sensor readings above/under threshold before interrupt trigger
  veml6035_als_pers_4 = SL_VEML6035_ALS_PERS_4,                  ///< 4 sensor readings above/under threshold before interrupt trigger
  veml6035_als_pers_8 = SL_VEML6035_ALS_PERS_8                   ///< 8 sensor readings above/under threshold before interrupt trigger
};

/**************************************************************************//**
 * @brief Initialize and enable the VEML6035 sensor to operate in normal mode.
 *
 * @note This function configures the sensor to a default integration time of
 * 100 ms and the lowest possible sensitivity configuration in order to achieve
 * maximum illumination range. If the lux count is too low, consider configuring
 * sensitivity settings with @ref sl_veml6035_configure_sensitivity.
 *
 * @param[in] i2cspm
 *    The I2C peripheral to use.
 * @param[in] white_enable
 *    Set to true to enable WHITE channel sensor readings in addition to the ALS
 *    channel.
 *
 * @retval SL_STATUS_OK Success
 * @retval SL_STATUS_INITIALIZATION Initialization Failure
 *****************************************************************************/
sl_status_t sl_veml6035_init(sl_i2cspm_t *i2cspm, bool white_enable);

/***************************************************************************//**
 * @brief Retrieve the sample counts from the ALS channel and calculate the
 * lux value.
 *
 * @param[in] i2cspm
 *    The I2C peripheral to use.
 *
 * @param[out] lux
 *    The measured ambient light illuminance in lux.
 *
 * @retval SL_STATUS_OK Success
 * @retval SL_STATUS_TRANSMIT I2C transmit failure
 * @retval SL_STATUS_INVALID_CONFIGURATION Invalid sensor configuration
 ******************************************************************************/
sl_status_t sl_veml6035_get_als_lux(sl_i2cspm_t *i2cspm, float *lux);

/***************************************************************************//**
 * @brief Retrieve the sample counts from the WHITE channel and calculate the
 * lux vaule.
 *
 * @param[in] i2cspm
 *    The I2C peripheral to use.
 *
 * @param[out] lux
 *    The measured white light illuminance in lux.
 *
 * @retval SL_STATUS_OK Success
 * @retval SL_STATUS_TRANSMIT I2C transmit failure
 * @retval SL_STATUS_INVALID_CONFIGURATION Invalid sensor configuration
 ******************************************************************************/
sl_status_t sl_veml6035_get_white_lux(sl_i2cspm_t *i2cspm, float *lux);

/***************************************************************************//**
 * @brief Enable or disable sensor measurements.
 *
 * @note Disabling the sensor does not make sensor lose its configuration and
 * last measurement values.
 *
 * @param[in] i2cspm
 *    The I2C peripheral to use.
 * @param[in] enable
 *    Set to true to enable sensor, set to false to disable sensor.
 *
 * @retval SL_STATUS_OK Success
 * @retval SL_STATUS_TRANSMIT I2C transmit failure
 ******************************************************************************/
sl_status_t sl_veml6035_enable_sensor(sl_i2cspm_t *i2cspm, bool enable);

/***************************************************************************//**
 * @brief Reset sensor configuration to the same state as after a power on reset.
 *
 * @note This function does not clear data output registers, or interrupt status
 * register.
 *
 * @param[in] i2cspm
 *    The I2C peripheral to use.
 *
 * @retval SL_STATUS_OK Success
 * @retval SL_STATUS_TRANSMIT I2C transmit failure
 ******************************************************************************/
sl_status_t sl_veml6035_reset(sl_i2cspm_t *i2cspm);

/**************************************************************************//**
 * @brief Configure sensor sensitivity settings.
 *
 * @note A low sensitivity results in a larger light sensing range, while a high
 *    sensitivity yields a higher resolution at low illumination levels.
 *
 * @param[in] i2cspm
 *    The I2C peripheral to use.
 *
 * @param[in] low_sensitivity
 *    Set to true for low sensitivity (1/8th of normal), false for normal sensitiviy.
 *
 * @param[in] gain
 *    Sensitivity gain, possible values: 1, 2, 4
 *
 * @retval SL_STATUS_OK Success
 * @retval SL_STATUS_TRANSMIT I2C transmit failure
 * @retval SL_STATUS_INVALID_PARAMATER Invalid parameter
 *****************************************************************************/
sl_status_t sl_veml6035_configure_sensitivity(sl_i2cspm_t *i2cspm,
                                              bool low_sensitivity,
                                              uint8_t gain);

/**************************************************************************//**
 * @brief Set integration time.
 *
 * @note The default integration time is 100ms. This can be increased to achieve
 *   a higher resolution, or decreased to achieve a faster measurement rate.
 *
 * @param[in] i2cspm
 *    The I2C peripheral to use.
 * @param[in] integration_time
 *    Integration time
 *
 * @retval SL_STATUS_OK Success
 * @retval SL_STATUS_TRANSMIT I2C transmit failure
 *****************************************************************************/
sl_status_t sl_veml6035_configure_integration_time(sl_i2cspm_t *i2cspm,
                                                   sl_veml6035_integration_time_t integration_time);

/***************************************************************************//**
 * @brief Enable power save mode, with a defined wait time between measurements.
 *
 * @param[in] i2cspm
 *    The I2C peripheral to use.
 *
 * @param[in] psm_wait
 *    The desired interval between measurements
 *
 * @param[in] enable
 *    Set to true to enable power save mode, false to disable power save mode.
 *
 * @retval SL_STATUS_OK Success
 * @retval SL_STATUS_TRANSMIT I2C transmit failure
 ******************************************************************************/
sl_status_t sl_veml6035_configure_psm(sl_i2cspm_t *i2cspm,
                                      sl_veml6035_psm_wait_t psm_wait,
                                      bool enable);

/***************************************************************************//**
 * @brief Configure interrupt mode.
 *
 * @param[in] i2cspm
 *    The I2C peripheral to use.
 *
 * @param[in] high_threshold
 *    Threshold for interrupt to trigger when lux value above
 *
 * @param[in] low_threshold
 *    Interrupt should trigger when lux count below this value
 *
 * @param[in] persistence
 *    How many sensor reads out of threshold window before interrupt triggers
 *
 * @param[in] white_enable
 *    Selection for which channel the interrupt should trigger. True for WHITE
 *    channel, false for ALS channel.
 *
 * @param[in] enable
 *    Set to true to enable interrupts, false to disable interrupts
 *
 * @retval SL_STATUS_OK Success
 * @retval SL_STATUS_TRANSMIT I2C transmit failure
 ******************************************************************************/
sl_status_t sl_veml6035_configure_interrupt_mode(sl_i2cspm_t *i2cspm,
                                                 uint16_t high_threshold,
                                                 uint16_t low_threshold,
                                                 sl_veml6035_als_pers_t persistence,
                                                 bool white_enable,
                                                 bool enable);

/***************************************************************************//**
 * @brief Enable or disable threshold interrupts.
 *
 * @param[in] i2cspm
 *    The I2C peripheral to use.
 *
 * @param[in] enable
 *    Set to true to enable interrupt mode, false to disable interrupt mode
 *
 * @retval SL_STATUS_OK Success
 * @retval SL_STATUS_TRANSMIT I2C transmit failure
 ******************************************************************************/
sl_status_t sl_veml6035_enable_interrupt_mode(sl_i2cspm_t *i2cspm, bool enable);

/**************************************************************************//**
 * @brief Read threshold interrupt status register and return interrupt status
 * for high and low threshold.
 *
 * @param[in] i2cspm
 *    The I2C peripheral to use
 *
 * @param[out] threshold_low
 *    True if crossing low threshold interrupt was triggered
 *
 * @param[out] threshold_high
 *    True if crossing high threshold interrupt was triggered
 *
 * @retval SL_STATUS_OK Success
 * @retval SL_STATUS_TRANSMIT I2C transmit failure
 *****************************************************************************/
sl_status_t sl_veml6035_read_interrupt_status(sl_i2cspm_t *i2cspm,
                                              bool *threshold_low,
                                              bool *threshold_high);

/** @} (end addtogroup veml6035) */

#ifdef __cplusplus
}
#endif

#endif //SL_VEML6035_H
