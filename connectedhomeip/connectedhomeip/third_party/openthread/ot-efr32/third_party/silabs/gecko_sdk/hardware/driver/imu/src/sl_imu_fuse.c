/***************************************************************************//**
 * @file
 * @brief Inertial Measurement Unit Fusion driver
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

#include <math.h>

#include "sl_imu.h"

/***************************************************************************//**
 * @addtogroup IMU
 * @{
 ******************************************************************************/

/** @cond DO_NOT_INCLUDE_WITH_DOXYGEN */

static bool sl_imu_is_acceleration_ok(sl_imu_sensor_fusion_t *f);

/** @endcond */

/***************************************************************************//**
 *    Initializes a new sl_imu_sensor_fusion_t structure
 ******************************************************************************/
void sl_imu_fuse_new(sl_imu_sensor_fusion_t *f)
{
  f->aVector[0]         = 0.0f;
  f->aVector[1]         = 0.0f;
  f->aVector[2]         = 0.0f;

  f->aAccumulator[0]    = 0.0f;
  f->aAccumulator[1]    = 0.0f;
  f->aAccumulator[2]    = 0.0f;
  f->aAccumulatorCount  = 0;
  f->aSampleRate        = 0.0f;

  f->angleCorrection[0] = 0.0f;
  f->angleCorrection[1] = 0.0f;
  f->angleCorrection[2] = 0.0f;

  f->dcm[0][0] = 0.0f; f->dcm[0][1] = 0.0f; f->dcm[0][2] = 0.0f;
  f->dcm[1][0] = 0.0f; f->dcm[1][1] = 0.0f; f->dcm[1][2] = 0.0f;
  f->dcm[2][0] = 0.0f; f->dcm[2][1] = 0.0f; f->dcm[2][2] = 0.0f;

  f->gVector[0]         = 0.0f;
  f->gVector[1]         = 0.0f;
  f->gVector[2]         = 0.0f;
  f->gSampleRate        = 0.0f;
  f->gDeltaTime         = 0.0f;

  f->orientation[0]     = 0.0f;
  f->orientation[1]     = 0.0f;
  f->orientation[2]     = 0.0f;
}

/***************************************************************************//**
 *    Sets the gyro sample rate and related values in the sl_imu_sensor_fusion_t
 *    structure
 ******************************************************************************/
void sl_imu_fuse_gyro_set_sample_rate(sl_imu_sensor_fusion_t *f, float rate)
{
  f->gSampleRate      = rate;
  f->gDeltaTime       = 1.0f / rate;
  f->gDeltaTimeScale  = IMU_DEG_TO_RAD_FACTOR / rate;
}

/***************************************************************************//**
 *    Sets the accelerometer sample rate in the sl_imu_sensor_fusion_t structure
 ******************************************************************************/
void sl_imu_fuse_accelerometer_set_sample_rate(sl_imu_sensor_fusion_t *f, float rate)
{
  f->gSampleRate = rate;
}

/***************************************************************************//**
 *    The current accelerometer data is added to the accumulator
 ******************************************************************************/
void sl_imu_fuse_accelerometer_update_filter(sl_imu_sensor_fusion_t *f, float avec[3])
{
  f->aAccumulator[0] += avec[0];
  f->aAccumulator[1] += avec[1];
  f->aAccumulator[2] += avec[2];

  f->aAccumulatorCount++;
}

/***************************************************************************//**
 *    Clears the gyro correction vector
 ******************************************************************************/
void sl_imu_fuse_gyro_clear_correction_vector(sl_imu_sensor_fusion_t *f)
{
  sl_imu_vector_zero(f->angleCorrection);
}

/***************************************************************************//**
 *    Updates the fusion calculation with a new gyro data
 ******************************************************************************/
void sl_imu_fuse_gyro_update(sl_imu_sensor_fusion_t *f, float gvec[3])
{
  float dgvec[3];
  float rgvec[3];

  /* Calculate 3D rotation over delta-T */
  sl_imu_vector_scalar_multiplication(dgvec, gvec, f->gDeltaTimeScale);

  /* Add delta-t rotation to fusion correction angle */
  sl_imu_vector_add(rgvec, dgvec, f->angleCorrection);

  /* DCM rotation */
  sl_imu_dcm_rotate(f->dcm, rgvec);
  sl_imu_dcm_normalize(f->dcm);
  sl_imu_dcm_get_angles(f->dcm, f->orientation);
}

/***************************************************************************//**
 *    Calculates the gyro correction vector
 ******************************************************************************/
void sl_imu_fuse_gyro_calculate_correction_vector(sl_imu_sensor_fusion_t *f, bool accValid, bool dirValid, float dirZ)
{
  float accAngle[3];
  float accx, accy, accz;

  /* Acceleration components */
  accx = -f->aVector[0];
  accy = -f->aVector[1];
  accz =  f->aVector[2];

  /* Clear correction angles */
  sl_imu_vector_zero(f->angleCorrection);

  if ( (accValid == true) && sl_imu_is_acceleration_ok(f) ) {
    if ( accz >= 0 ) {
      accAngle[0] = asinf(accy);
      accAngle[1] = -asinf(accx);
      accAngle[2] = dirZ;

      sl_imu_vector_subtract(f->angleCorrection, accAngle, f->orientation);
      sl_imu_vector_normalize_angle(f->angleCorrection);
    } else {
      accAngle[0] = IMU_PI - asinf(accy);
      accAngle[1] = -asinf(accx);
      accAngle[2] = IMU_PI + dirZ;

      sl_imu_vector_normalize_angle(accAngle);
      sl_imu_vector_subtract(f->angleCorrection, accAngle, f->orientation);
      sl_imu_vector_normalize_angle(f->angleCorrection);

      f->angleCorrection[1] = -f->angleCorrection[1];
    }

    if ( dirValid == false ) {
      f->angleCorrection[2] = 0;
    }

    sl_imu_vector_scale(f->angleCorrection, 0.5f / (float) f->gSampleRate);
  }
}

/** @cond DO_NOT_INCLUDE_WITH_DOXYGEN */

/***************************************************************************//**
 * @brief
 *    Checks if the acceleration vector is within the appropriate range
 *
 * @param[in] f
 *    Pointer to the sl_imu_sensor_fusion_t object
 *
 * @return
 *    True if the acceleration is within the limits, false otherwise
 ******************************************************************************/
static bool sl_imu_is_acceleration_ok(sl_imu_sensor_fusion_t *f)
{
  float accx;
  float accy;
  bool r;

  accx = f->aVector[0];
  accy = f->aVector[1];
  if ( (accx >= -IMU_MAX_ACCEL_FOR_ANGLE) && (accx <= IMU_MAX_ACCEL_FOR_ANGLE)
       && (accy >= -IMU_MAX_ACCEL_FOR_ANGLE) && (accy <= IMU_MAX_ACCEL_FOR_ANGLE) ) {
    r = true;
  } else {
    r = false;
  }

  return r;
}

/** @endcond */

/***************************************************************************//**
 *    Clears the values of the sensor fusion object
 ******************************************************************************/
void sl_imu_fuse_reset(sl_imu_sensor_fusion_t *f)
{
  sl_imu_vector_zero(f->aVector);
  sl_imu_vector_zero(f->aAccumulator);
  f->aAccumulatorCount = 0;
  sl_imu_vector_zero(f->gVector);
  sl_imu_vector_zero(f->orientation);
  sl_imu_vector_zero(f->angleCorrection);

  sl_imu_dcm_reset(f->dcm);
}

/***************************************************************************//**
 *    Updates the fusion calculation
 ******************************************************************************/
void sl_imu_fuse_update(sl_imu_sensor_fusion_t *f)
{
  uint8_t imu_state = sl_imu_get_state();
  if ( imu_state != IMU_STATE_READY ) {
    return;
  }

  /* Get accelerometer data and update Fuse filter */
  sl_imu_get_acceleration_raw_data(f->aVector);
  sl_imu_fuse_accelerometer_update_filter(f, f->aVector);

  /* Get gyro data and update fuse */
  sl_imu_get_gyro_raw_data(f->gVector);
  f->gVector[0] = -f->gVector[0];
  f->gVector[1] = -f->gVector[1];
  sl_imu_fuse_gyro_update(f, f->gVector);

  /* Perform fusion to compensate for gyro drift */
  sl_imu_fuse_gyro_calculate_correction_vector(f, true, false, 0);
}

/** @} */
