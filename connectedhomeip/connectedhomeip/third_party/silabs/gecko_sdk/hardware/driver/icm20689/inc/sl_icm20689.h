/***************************************************************************//**
 * @file
 * @brief Driver for the Invensense ICM20689 6-axis motion sensor
 *******************************************************************************
 * # License
 * <b>Copyright 2021 Silicon Laboratories Inc. www.silabs.com</b>
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

#ifndef SL_ICM20689_H
#define SL_ICM20689_H

#include <stdint.h>
#include <stdbool.h>
#include "sl_status.h"

#ifdef __cplusplus
extern "C" {
#endif

/***************************************************************************//**
 * @addtogroup icm20689 ICM20689 - Motion Sensor
 * @brief Driver for the Invensense ICM20689 6-axis motion sensor.
 * @{
 ******************************************************************************/
/***************************************************************************//**
 * @addtogroup icm20689_details Register definitions
 * @brief Register definitions.
 * @{
 ******************************************************************************/

/**************************************************************************//**
* @name Register and associated bit definitions
* @{
******************************************************************************/
#include "sl_icm20689_defs.h"

/// Gyroscope enum bitfield shift.
#define GYRO_BANDWITDH_ENUM_FCHOICE_SHIFT   (0x03U)

/// Gyroscope bandwidth.
typedef enum {
  sl_gyroBandwidth8173Hz          =   _ICM20689_GYRO_CONFIG_FCHOICE_B_8173HZ  << GYRO_BANDWITDH_ENUM_FCHOICE_SHIFT,
    sl_gyroBandwidth3281Hz        =   _ICM20689_GYRO_CONFIG_FCHOICE_B_3281HZ  << GYRO_BANDWITDH_ENUM_FCHOICE_SHIFT,
    sl_gyroBandwidthDlpf250Hz     =   _ICM20689_GYRO_CONFIG_FCHOICE_B_LOWPASS << GYRO_BANDWITDH_ENUM_FCHOICE_SHIFT | _ICM20689_CONFIG_DLPF_CFG_250HZ,
    sl_gyroBandwidthDlpf176Hz     =   _ICM20689_GYRO_CONFIG_FCHOICE_B_LOWPASS << GYRO_BANDWITDH_ENUM_FCHOICE_SHIFT | _ICM20689_CONFIG_DLPF_CFG_176HZ,
    sl_gyroBandwidthDlpf92Hz      =   _ICM20689_GYRO_CONFIG_FCHOICE_B_LOWPASS << GYRO_BANDWITDH_ENUM_FCHOICE_SHIFT | _ICM20689_CONFIG_DLPF_CFG_92HZ,
    sl_gyroBandwidthDlpf41Hz      =   _ICM20689_GYRO_CONFIG_FCHOICE_B_LOWPASS << GYRO_BANDWITDH_ENUM_FCHOICE_SHIFT | _ICM20689_CONFIG_DLPF_CFG_41HZ,
    sl_gyroBandwidthDlpf20Hz      =   _ICM20689_GYRO_CONFIG_FCHOICE_B_LOWPASS << GYRO_BANDWITDH_ENUM_FCHOICE_SHIFT | _ICM20689_CONFIG_DLPF_CFG_20HZ,
    sl_gyroBandwidthDlpf10Hz      =   _ICM20689_GYRO_CONFIG_FCHOICE_B_LOWPASS << GYRO_BANDWITDH_ENUM_FCHOICE_SHIFT | _ICM20689_CONFIG_DLPF_CFG_10HZ,
    sl_gyroBandwidthDlpf5Hz       =   _ICM20689_GYRO_CONFIG_FCHOICE_B_LOWPASS << GYRO_BANDWITDH_ENUM_FCHOICE_SHIFT | _ICM20689_CONFIG_DLPF_CFG_5HZ,
    sl_gyroBandwidthDlpf3281Hz    =   _ICM20689_GYRO_CONFIG_FCHOICE_B_LOWPASS << GYRO_BANDWITDH_ENUM_FCHOICE_SHIFT | _ICM20689_CONFIG_DLPF_CFG_3281HZ
} sl_gyro_bandwidth_t;

/// Accelerometer bandwidth.
typedef enum {
  sl_accelBandwidth1046Hz          =   ICM20689_ACCEL_CONFIG2_MASK_ACCEL_FCHOICE_B | ICM20689_ACCEL_CONFIG2_SHIFT_A_DLPF_CFG,
  sl_accelBandwidthDlpf218_1_0Hz   =   ICM20689_ACCEL_CONFIG2_A_DLPF_CFG_218_1_0HZ,
  sl_accelBandwidthDlpf218_1_1Hz   =   ICM20689_ACCEL_CONFIG2_A_DLPF_CFG_218_1_1HZ,
  sl_accelBandwidthDlpf99Hz        =   ICM20689_ACCEL_CONFIG2_A_DLPF_CFG_99HZ,
  sl_accelBandwidthDlpf44_8Hz      =   ICM20689_ACCEL_CONFIG2_A_DLPF_CFG_44HZ,
  sl_accelBandwidthDlpf21_2Hz      =   ICM20689_ACCEL_CONFIG2_A_DLPF_CFG_21_2HZ,
  sl_accelBandwidthDlpf10_2Hz      =   ICM20689_ACCEL_CONFIG2_A_DLPF_CFG_10_2HZ,
  sl_accelBandwidthDlpf5_1Hz       =   ICM20689_ACCEL_CONFIG2_A_DLPF_CFG_5_1HZ,
  sl_accelBandwidthDlpf420Hz       =   ICM20689_ACCEL_CONFIG2_A_DLPF_CFG_420HZ
} sl_accel_bandwidth_t;

/// Gyroscope fullscale.
typedef enum {
  sl_gyroFullscale250Dps  =   ICM20689_GYRO_CONFIG_FS_SEL_250_DPS,
  sl_gyroFullscale500Dps  =   ICM20689_GYRO_CONFIG_FS_SEL_500_DPS,
  sl_gyroFullscale1000Dps =   ICM20689_GYRO_CONFIG_FS_SEL_1000_DPS,
  sl_gyroFullscale2000Dps =   ICM20689_GYRO_CONFIG_FS_SEL_2000_DPS
} sl_gyro_full_scale_t;

/// Accelerometer fullscale.
typedef enum {
  sl_accelFullscale2G   =   ICM20689_ACCEL_CONFIG_ACCEL_FS_SEL_2G,
  sl_accelFullscale4G   =   ICM20689_ACCEL_CONFIG_ACCEL_FS_SEL_4G,
  sl_accelFullscale8G   =   ICM20689_ACCEL_CONFIG_ACCEL_FS_SEL_8G,
  sl_accelFullscale16G  =   ICM20689_ACCEL_CONFIG_ACCEL_FS_SEL_16G
} sl_accel_full_scale_t;

#define ICM20689_WHO_AM_I_MASK_WHOAMI_ID            (0x98)                              /**< Device ID register         */

#define ICM20689_DEVICE_ID                          ICM20689_WHO_AM_I_MASK_WHOAMI_ID   /**< ICM20689 Device ID value    */
/**@}*/
/**@}*/ //icm20689_details

/***************************************************************************//**
 * @brief
 *    Initialize the SPI bus to communicate with the ICM20689.
 *
 * @return
 *    Returns zero on OK, non-zero otherwise
 ******************************************************************************/
sl_status_t sl_icm20689_spi_init(void);

/***************************************************************************//**
 * @brief
 *    De-initialize the ICM20689 sensor.
 *
 * @return
 *    Returns zero on OK, non-zero otherwise
 ******************************************************************************/
sl_status_t sl_icm20689_deinit(void);

/***************************************************************************//**
 * @brief
 *    Read register from the ICM20689 device.
 *
 * @param[in] addr
 *    The register address to read from in the sensor
 *    Bit[7:0] - register address
 *
 * @param[in] num_bytes
 *    The number of bytes to read
 *
 * @param[out] data
 *    The data read from the register
 * 
 * @return
 *    Returns zero on OK, non-zero otherwise
 ******************************************************************************/
sl_status_t sl_icm20689_read_register(uint8_t addr, int num_bytes, uint8_t *data);

/***************************************************************************//**
 * @brief
 *    Write a register in the ICM20689 device.
 *
 * @param[in] addr
 *    The register address to write
 *    Bit[7:0] - register address
 *
 * @param[in] data
 *    The data to write to the register 
 * 
 * @return
 *    Returns zero on OK, non-zero otherwise
 ******************************************************************************/
sl_status_t sl_icm20689_write_register(uint8_t addr, uint8_t data);

/***************************************************************************//**
 * @brief
 *    Perform soft reset on the ICM20689 chip.
 *
 * @return
 *    Returns zero on OK, non-zero otherwise
 ******************************************************************************/
sl_status_t sl_icm20689_reset(void);

/***************************************************************************//**
 * @brief
 *    Set the sample rate both of the accelerometer and the gyroscope.
 *
 * @param[in] sample_rate
 *    The desired sample rate in Hz.
 *
 * @return
 *    The actual sample rate, which may be different than the desired value because
 *    of the finite and discrete number of divider settings.
 ******************************************************************************/
float sl_icm20689_set_sample_rate(float sample_rate);

/***************************************************************************//**
 * @brief
 *    Initialize the ICM20689 sensor. Enable the power supply and SPI lines,
 *    set up the host SPI controller, configure the chip control interface,
 *    clock generator, and interrupt line.
 *
 * @return
 *    Returns zero on OK, non-zero otherwise
 ******************************************************************************/
sl_status_t sl_icm20689_init(void);

/***************************************************************************//**
 * @brief
 *    Set the bandwidth of the gyroscope.
 *
 * @param[in] gyro_bandwidth
 *    The desired bandwidth value. Use the sl_gyro_bandwidth_t enum, which
 *    are defined in the icm20689.h file. The value of x can be
 *    5Hz, 10Hz, 20Hz, 41Hz, 92Hz, 176Hz, 250Hz, 3281Hz or 8173Hz with
 *    or without digital low pass filter(DLPF).
 *
 * @return
 *    Returns zero on OK, non-zero otherwise
 ******************************************************************************/
sl_status_t sl_icm20689_gyro_set_bandwidth(sl_gyro_bandwidth_t gyro_bandwidth);

/***************************************************************************//**
 * @brief
 *    Set the bandwidth of the accelerometer.
 *
 * @param[in] accel_bandwidth
 *    The desired bandwidth value. Use the sl_accel_bandwidth_t enum, which
 *    are defined in the icm20689.h file. The value of y can be
 *    5.1Hz, 10.2Hz, 21.2Hz, 44.8Hz, 99Hz, 218.1Hz(0), 218.1Hz(1), 420Hz or 1046Hz.
 *    1046Hz is the only value without digital low pass filter.
 *
 * @return
 *    Returns zero on OK, non-zero otherwise
 ******************************************************************************/
sl_status_t sl_icm20689_accel_set_bandwidth(sl_accel_bandwidth_t accel_bandwidth);

/***************************************************************************//**
 * @brief
 *    Read the raw acceleration value and convert to g value based on
 *    the actual resolution.
 *
 * @param[out] accel
 *    A 3-element array of float numbers containing the acceleration values
 *    for the x, y and z axes in g units.
 *
 * @return
 *    Returns zero on OK, non-zero otherwise
 ******************************************************************************/
sl_status_t sl_icm20689_accel_read_data(float accel[3]);

/***************************************************************************//**
 * @brief
 *    Read the raw gyroscope value and convert to deg/sec value based on
 *    the actual resolution.
 *
 * @param[out] gyro
 *    A 3-element array of float numbers containing the gyroscope values
 *    for the x, y and z axes in deg/sec units.
 *
 * @return
 *    Returns zero on OK, non-zero otherwise
 ******************************************************************************/
sl_status_t sl_icm20689_gyro_read_data(float gyro[3]);

/***************************************************************************//**
 * @brief
 *    Get the actual resolution of the accelerometer.
 *
 * @param[out] accel_res
 *    The resolution in g/bit units
 *
 * @return
 *    Returns zero on OK, non-zero otherwise
 ******************************************************************************/
sl_status_t sl_icm20689_accel_get_resolution(float *accel_res);

/***************************************************************************//**
 * @brief
 *    Get the actual resolution of the gyroscope.
 *
 * @param[out] gyro_res
 *    The actual resolution in (deg/sec)/bit units
 *
 * @return
 *    Returns zero on OK, non-zero otherwise
 ******************************************************************************/
sl_status_t sl_icm20689_gyro_get_resolution(float *gyro_res);

/***************************************************************************//**
 * @brief
 *    Set the full scale value of the accelerometer.
 *
 * @param[in] accel_fs
 *    The desired full scale value. The value of x can be
 *    2, 4, 8 or 16.
 *
 * @return
 *    Returns zero on OK, non-zero otherwise
 ******************************************************************************/
sl_status_t sl_icm20689_accel_set_full_scale(sl_accel_full_scale_t accel_fs);

/***************************************************************************//**
 * @brief
 *    Set the full scale value of the gyroscope.
 *
 * @param[in] gyro_fs
 *    The desired full scale value. The value of y can be
 *    250, 500, 1000 or 2000.
 *
 * @return
 *    Returns zero on OK, non-zero otherwise
 ******************************************************************************/
sl_status_t sl_icm20689_gyro_set_full_scale(sl_gyro_full_scale_t gyro_fs);

/***************************************************************************//**
 * @brief
 *    Enable or disable the sleep mode of the device.
 *
 * @param[in] enable
 *    If true, sleep mode is enabled. Set to false to disable sleep mode.
 *
 * @return
 *    Returns zero on OK, non-zero otherwise
 ******************************************************************************/
sl_status_t sl_icm20689_enable_sleep_mode(bool enable);

/***************************************************************************//**
 * @brief
 *    Enable or disable the sensors in the ICM20689 chip.
 *
 * @param[in] accel
 *    If true, enables the acceleration sensor
 *
 * @param[in] gyro
 *    If true, enables the gyroscope sensor
 *
 * @param[in] temp
 *    If true, enables the temperature sensor
 *
 * @return
 *    Returns zero on OK, non-zero otherwise
 ******************************************************************************/
sl_status_t sl_icm20689_enable_sensor(bool accel, bool gyro, bool temp);

/***************************************************************************//**
 * @brief
 *    Enable or disable the interrupts in the ICM20689 chip.
 *
 * @param[in] data_ready_enable
 *    If true, enables the Raw Data Ready interrupt, otherwise disables.
 *
 * @param[in] wom_enable
 *    If true, enables the Wake-up On Motion interrupt, otherwise disables.
 *
 * @return
 *    Returns zero on OK, non-zero otherwise
 ******************************************************************************/
sl_status_t sl_icm20689_enable_interrupt(bool data_ready_enable, bool wom_enable);

/***************************************************************************//**
 * @brief
 *    Read the interrupt status registers of the ICM20689 chip.
 *
 * @param[out] int_status
 *    The content the four interrupt registers. LSByte is INT_STATUS, MSByte is
 *    INT_STATUS_3
 *
 * @return
 *    Returns zero on OK, non-zero otherwise
 ******************************************************************************/
sl_status_t sl_icm20689_read_interrupt_status(uint32_t *int_status);

/***************************************************************************//**
 * @brief
 *    Check if new data is available to read.
 *
 * @return
 *    Returns true if the Raw Data Ready interrupt bit set, false otherwise
 ******************************************************************************/
bool sl_icm20689_is_data_ready(void);

/***************************************************************************//**
 * @brief
 *    Accelerometer and gyroscope calibration function. Reads the gyroscope
 *    and accelerometer values while the device is at rest and in level. The
 *    resulting values are loaded to the accel and gyro bias registers to cancel
 *    the static offset error.
 *
 * @param[out] accel_bias_scaled
 *    The mesured acceleration sensor bias in mg
 *
 * @param[out] gyro_bias_scaled
 *    The mesured gyro sensor bias in deg/sec
 *
 * @return
 *    Returns zero on OK, non-zero otherwise
 ******************************************************************************/
sl_status_t sl_icm20689_calibrate_accel_and_gyro(float *accel_bias_scaled, float *gyro_bias_scaled);

/***************************************************************************//**
 * @brief
 *    Gyroscope calibration function. Read the gyroscope
 *    values while the device is at rest and in level. The
 *    resulting values are loaded to the gyro bias registers to cancel
 *    the static offset error.
 *
 * @param[out] gyro_bias_scaled
 *    The mesured gyro sensor bias in deg/sec
 *
 * @return
 *    Returns zero on OK, non-zero otherwise
 ******************************************************************************/
sl_status_t sl_icm20689_calibrate_gyro(float *gyro_bias_scaled);

/***************************************************************************//**
 * @brief
 *    Read the temperature sensor raw value and convert to Celsius.
 *
 * @param[out] temperature
 *    The mesured temperature in Celsius
 *
 * @return
 *    Returns zero on OK, non-zero otherwise
 ******************************************************************************/
sl_status_t sl_icm20689_read_temperature_data(float *temperature);

/***************************************************************************//**
 * @brief
 *    Read the device ID of the ICM20689.
 *
 * @param[out] dev_id
 *    The ID of the device read from the WHO_AM_I register. Expected value 0x98.
 *
 * @return
 *    Returns zero on OK, non-zero otherwise
 ******************************************************************************/
sl_status_t sl_icm20689_get_device_id(uint8_t *dev_id);

/** @} */

#ifdef __cplusplus
}
#endif

#endif // SL_ICM20689_H
