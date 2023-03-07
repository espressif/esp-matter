/***************************************************************************//**
 * @file
 * @brief Inertial Measurement Unit driver
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

#include <stdint.h>
#include <stdbool.h>

#include "sl_icm20648.h"
#include "sl_imu.h"
#include "sl_sleeptimer.h"

/** @cond DO_NOT_INCLUDE_WITH_DOXYGEN */
static uint8_t IMU_state = IMU_STATE_DISABLED; /**< IMU state variable                                  */
static float gyroSampleRate;                   /**< Gyroscope sample rate                               */
static float accelSampleRate;                  /**< Accelerometer sample rate                           */
static volatile bool dataReady;                /**< Flag to show if new accel/gyro data ready to read   */
static uint32_t IMU_isDataReadyQueryCount = 0; /**< The number of the total data ready queries          */
static uint32_t IMU_isDataReadyTrueCount = 0;  /**< The number of queries when data is ready            */
static sl_imu_sensor_fusion_t fuseObj;         /**< Structure to store the sensor fusion data           */
/** @endcond */

/***************************************************************************//**
 *    Initializes and calibrates the IMU
 ******************************************************************************/
sl_status_t sl_imu_init(void)
{
  sl_status_t status;
  uint8_t devid;
  float gyroBiasScaled[3];

  IMU_state = IMU_STATE_INITIALIZING;
  sl_imu_fuse_new(&fuseObj);

  /* Initialize acc/gyro driver */
  status = sl_icm20648_init();
  if ( status != SL_STATUS_OK ) {
    goto cleanup;
  }

  status = sl_icm20648_get_device_id(&devid);
  if ( status != SL_STATUS_OK ) {
    goto cleanup;
  }

  /* Gyro calibration */
  IMU_state = IMU_STATE_CALIBRATING;
  status = sl_icm20648_calibrate_gyro(gyroBiasScaled);
  if ( status != SL_STATUS_OK ) {
    goto cleanup;
  }

  IMU_state = IMU_STATE_INITIALIZING;

  cleanup:

  if ( status != SL_STATUS_OK ) {
    sl_icm20648_deinit();
    IMU_state = IMU_STATE_DISABLED;
  }

  return status;
}

/***************************************************************************//**
 *    De-initializes the IMU chip
 ******************************************************************************/
sl_status_t sl_imu_deinit(void)
{
  sl_status_t status;

  IMU_state = IMU_STATE_DISABLED;
  status    = sl_icm20648_deinit();

  return status;
}

/***************************************************************************//**
 *    Returns IMU state
 ******************************************************************************/
uint8_t sl_imu_get_state(void)
{
  return IMU_state;
}

/***************************************************************************//**
 *    Configures the IMU
 ******************************************************************************/
void sl_imu_configure(float sampleRate)
{
  uint32_t itStatus;

  /* Set IMU state */
  IMU_state = IMU_STATE_INITIALIZING;

  /* Enable accel sensor */
  sl_icm20648_enable_sensor(true, true, false);

  /* Set sample rate */
  gyroSampleRate  = sl_icm20648_gyro_set_sample_rate(sampleRate);
  accelSampleRate = sl_icm20648_accel_set_sample_rate(sampleRate);

  /* Filter bandwidth: 12kHz, otherwise the results may be off */
  sl_icm20648_accel_set_bandwidth(ICM20648_ACCEL_BW_1210HZ);
  sl_icm20648_gyro_set_bandwidth(ICM20648_GYRO_BW_51HZ);

  /* Accel: 2G full scale */
  sl_icm20648_accel_set_full_scale(ICM20648_ACCEL_FULLSCALE_2G);

  /* Gyro: 250 degrees per sec full scale */
  sl_icm20648_gyro_set_full_scale(ICM20648_GYRO_FULLSCALE_250DPS);

  sl_sleeptimer_delay_millisecond(50);

  /* Enable the raw data ready interrupt */
  sl_icm20648_enable_interrupt(true, false);

  /* Enter low power mode */
  sl_icm20648_enter_low_power_mode(true, true, false);

  /* Clear the interrupts */
  sl_icm20648_read_interrupt_status(&itStatus);

  /* IMU fuse config & setup */
  sl_imu_fuse_accelerometer_set_sample_rate(&fuseObj, accelSampleRate);
  sl_imu_fuse_gyro_set_sample_rate(&fuseObj, gyroSampleRate);
  sl_imu_fuse_reset(&fuseObj);

  IMU_state = IMU_STATE_READY;
}

/***************************************************************************//**
 *    Retrieves the processed acceleration data
 ******************************************************************************/
void sl_imu_get_acceleration(int16_t avec[3])
{
  if ( fuseObj.aAccumulatorCount > 0 ) {
    avec[0] = (int16_t) (1000.0f * fuseObj.aAccumulator[0] / fuseObj.aAccumulatorCount);
    avec[1] = (int16_t) (1000.0f * fuseObj.aAccumulator[1] / fuseObj.aAccumulatorCount);
    avec[2] = (int16_t) (1000.0f * fuseObj.aAccumulator[2] / fuseObj.aAccumulatorCount);
    fuseObj.aAccumulator[0] = 0;
    fuseObj.aAccumulator[1] = 0;
    fuseObj.aAccumulator[2] = 0;
    fuseObj.aAccumulatorCount = 0;
  } else {
    avec[0] = 0;
    avec[1] = 0;
    avec[2] = 0;
  }
}

/***************************************************************************//**
 *    Retrieves the processed orientation data
 ******************************************************************************/
void sl_imu_get_orientation(int16_t ovec[3])
{
  ovec[0] = (int16_t) (100.0f * IMU_RAD_TO_DEG_FACTOR * fuseObj.orientation[0]);
  ovec[1] = (int16_t) (100.0f * IMU_RAD_TO_DEG_FACTOR * fuseObj.orientation[1]);
  ovec[2] = (int16_t) (100.0f * IMU_RAD_TO_DEG_FACTOR * fuseObj.orientation[2]);
}

/***************************************************************************//**
 *    Retrieves the processed gyroscope data
 ******************************************************************************/
void sl_imu_get_gyro(int16_t gvec[3])
{
  gvec[0] = (int16_t) (100.0f * fuseObj.gVector[0]);
  gvec[1] = (int16_t) (100.0f * fuseObj.gVector[1]);
  gvec[2] = (int16_t) (100.0f * fuseObj.gVector[2]);
}

/***************************************************************************//**
 *    Performs gyroscope calibration to cancel gyro bias.
 ******************************************************************************/
sl_status_t sl_imu_calibrate_gyro(void)
{
  sl_status_t status;

  status = SL_STATUS_OK;

  /* Disable interrupt */
  sl_icm20648_enable_interrupt(false, false);

  sl_imu_deinit();
  status = sl_imu_init();

  /* Restart regular sampling */
  sl_imu_configure(gyroSampleRate);

  return status;
}

/***************************************************************************//**
 *    Gets a new set of data from the accel and gyro sensor and updates the
 *    fusion calculation
 ******************************************************************************/
void sl_imu_update(void)
{
  sl_imu_fuse_update(&fuseObj);
}

/***************************************************************************//**
 *    Resets the fusion calculation
 ******************************************************************************/
void sl_imu_reset(void)
{
  sl_imu_fuse_reset(&fuseObj);
}

/***************************************************************************//**
 *    Retrieves the raw acceleration data from the IMU
 ******************************************************************************/
void sl_imu_get_acceleration_raw_data(float avec[3])
{
  if ( IMU_state != IMU_STATE_READY ) {
    avec[0] = 0;
    avec[1] = 0;
    avec[2] = 0;
    return;
  }

  sl_icm20648_accel_read_data(avec);
}

/***************************************************************************//**
 *    Retrieves the processed gyroscope correction angles
 ******************************************************************************/
void sl_imu_get_gyro_correction_angles(float acorr[3])
{
  acorr[0] = fuseObj.angleCorrection[0];
  acorr[1] = fuseObj.angleCorrection[1];
  acorr[2] = fuseObj.angleCorrection[2];
}

/***************************************************************************//**
 *    Retrieves the raw gyroscope data from the IMU
 ******************************************************************************/
void sl_imu_get_gyro_raw_data(float gvec[3])
{
  if ( IMU_state != IMU_STATE_READY ) {
    gvec[0] = 0;
    gvec[1] = 0;
    gvec[2] = 0;
    return;
  }

  sl_icm20648_gyro_read_data(gvec);
}

/***************************************************************************//**
 *    Checks if there is new accel/gyro data available for read
 ******************************************************************************/
bool sl_imu_is_data_ready(void)
{
  bool ready;

  if ( IMU_state != IMU_STATE_READY ) {
    return false;
  }

  ready = sl_icm20648_is_data_ready();
  IMU_isDataReadyQueryCount++;

  if ( ready ) {
    IMU_isDataReadyTrueCount++;
  }

  return ready;
}
