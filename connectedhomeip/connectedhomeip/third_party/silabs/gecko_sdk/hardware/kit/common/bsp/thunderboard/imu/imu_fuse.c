/***************************************************************************//**
 * @file
 * @brief Inertial Measurement Unit Fusion driver
 *******************************************************************************
 * # License
 * <b>Copyright 2018 Silicon Laboratories Inc. www.silabs.com</b>
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

#include "thunderboard/imu/imu.h"

/***************************************************************************//**
 * @addtogroup IMU
 * @{
 ******************************************************************************/

/** @cond DO_NOT_INCLUDE_WITH_DOXYGEN */

extern uint8_t IMU_state;

static bool IMU_isAccelerationOK(IMU_SensorFusion *f);

/** @endcond */

/***************************************************************************//**
 * @brief
 *    Initializes a new IMU_SensorFusion structure
 *
 * @param[out] f
 *    Pointer to the IMU_SensorFusion object to be initialized
 *
 * @return
 *    None
 ******************************************************************************/
void IMU_fuseNew(IMU_SensorFusion *f)
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

  return;
}

/***************************************************************************//**
 * @brief
 *    Sets the gyro sample rate and related values in the IMU_SensorFusion
 *    structure
 *
 * @param[out] f
 *    Pointer to the IMU_SensorFusion object
 *
 * @param[in] rate
 *    Sample rate of the gyroscope
 *
 * @return
 *    None
 ******************************************************************************/
void IMU_fuseGyroSetSampleRate(IMU_SensorFusion *f, float rate)
{
  f->gSampleRate      = rate;
  f->gDeltaTime       = 1.0f / rate;
  f->gDeltaTimeScale  = IMU_DEG_TO_RAD_FACTOR / rate;

  return;
}

/***************************************************************************//**
 * @brief
 *    Sets the accelerometer sample rate in the IMU_SensorFusion structure
 *
 * @param[out] f
 *    Pointer to the IMU_SensorFusion object
 *
 * @param[in] rate
 *    Sample rate of the accelerometer
 *
 * @return
 *    None
 ******************************************************************************/
void IMU_fuseAccelerometerSetSampleRate(IMU_SensorFusion *f, float rate)
{
  f->gSampleRate = rate;

  return;
}

/***************************************************************************//**
 * @brief
 *    The current accelerometer data is added to the accumulator
 *
 * @param[out] f
 *    Pointer to the IMU_SensorFusion object
 *
 * @param[in] avec
 *    Accelerometer vector
 *
 * @return
 *    None
 ******************************************************************************/
void IMU_fuseAccelerometerUpdateFilter(IMU_SensorFusion *f, float avec[3])
{
  f->aAccumulator[0] += avec[0];
  f->aAccumulator[1] += avec[1];
  f->aAccumulator[2] += avec[2];

  f->aAccumulatorCount++;

  return;
}

/***************************************************************************//**
 * @brief
 *    Clears the gyro correction vector
 *
 * @param[out] f
 *    Pointer to the IMU_SensorFusion object
 *
 * @return
 *    None
 ******************************************************************************/
void IMU_fuseGyroCorrectionClear(IMU_SensorFusion *f)
{
  IMU_vectorZero(f->angleCorrection);

  return;
}

/***************************************************************************//**
 * @brief
 *    Updates the fusion calculation with a new gyro data
 *
 * @param[out] f
 *    Pointer to the IMU_SensorFusion object
 *
 * @param[in] gvec
 *    Gyroscope vector
 *
 * @return
 *    None
 ******************************************************************************/
void IMU_fuseGyroUpdate(IMU_SensorFusion *f, float gvec[3])
{
  float dgvec[3];
  float rgvec[3];

  /* Calculate 3D rotation over delta-T */
  IMU_vectorScalarMultiplication(dgvec, gvec, f->gDeltaTimeScale);

  /* Add delta-t rotation to fusion correction angle */
  IMU_vectorAdd(rgvec, dgvec, f->angleCorrection);

  /* DCM rotation */
  IMU_dcmRotate(f->dcm, rgvec);
  IMU_dcmNormalize(f->dcm);
  IMU_dcmGetAngles(f->dcm, f->orientation);

  return;
}

/***************************************************************************//**
 * @brief
 *    Calculates the gyro correction vector
 *
 * @param[out] f
 *    Pointer to the IMU_SensorFusion object
 *
 * @param[in] accValid
 *    True if the acceration value is within the limits
 *
 * @param[in] dirValid
 *    True if the direction value is valid
 *
 * @param[in] dirZ
 *    The direction of the Z axis
 *
 * @return
 *    None
 ******************************************************************************/
void IMU_fuseGyroCorrection(IMU_SensorFusion *f, bool accValid, bool dirValid, float dirZ)
{
  float accAngle[3];
  float accx, accy, accz;

  /* Acceleration components */
  accx = -f->aVector[0];
  accy = -f->aVector[1];
  accz =  f->aVector[2];

  /* Clear correction angles */
  IMU_vectorZero(f->angleCorrection);

  if ( (accValid == true) && IMU_isAccelerationOK(f) ) {
    if ( accz >= 0 ) {
      accAngle[0] = asinf(accy);
      accAngle[1] = -asinf(accx);
      accAngle[2] = dirZ;

      IMU_vectorSubtract(f->angleCorrection, accAngle, f->orientation);
      IMU_vectorNormalizeAngle(f->angleCorrection);
    } else {
      accAngle[0] = IMU_PI - asinf(accy);
      accAngle[1] = -asinf(accx);
      accAngle[2] = IMU_PI + dirZ;

      IMU_vectorNormalizeAngle(accAngle);
      IMU_vectorSubtract(f->angleCorrection, accAngle, f->orientation);
      IMU_vectorNormalizeAngle(f->angleCorrection);

      f->angleCorrection[1] = -f->angleCorrection[1];
    }

    if ( dirValid == false ) {
      f->angleCorrection[2] = 0;
    }

    IMU_vectorScale(f->angleCorrection, 0.5f / (float) f->gSampleRate);
  }

  return;
}

/** @cond DO_NOT_INCLUDE_WITH_DOXYGEN */

/***************************************************************************//**
 * @brief
 *    Checks if the acceleration vector is within the appropriate range
 *
 * @param[out] f
 *    Pointer to the IMU_SensorFusion object
 *
 * @return
 *    True if the acceleration is within the limits, false otherwise
 ******************************************************************************/
static bool IMU_isAccelerationOK(IMU_SensorFusion *f)
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
 * @brief
 *    Clears the values of the sensor fusion object
 *
 * @param[out] f
 *    Pointer to the IMU_SensorFusion object
 *
 * @return
 *    None
 ******************************************************************************/
void IMU_fuseReset(IMU_SensorFusion *f)
{
  IMU_vectorZero(f->aVector);
  IMU_vectorZero(f->aAccumulator);
  f->aAccumulatorCount = 0;
  IMU_vectorZero(f->gVector);
  IMU_vectorZero(f->orientation);
  IMU_vectorZero(f->angleCorrection);

  IMU_dcmReset(f->dcm);

  return;
}

/***************************************************************************//**
 * @brief
 *    Updates the fusion calculation
 *
 * @param[out] f
 *    Pointer to the IMU_SensorFusion object
 *
 * @return
 *    None
 ******************************************************************************/
void IMU_fuseUpdate(IMU_SensorFusion *f)
{
  if ( IMU_state != IMU_STATE_READY ) {
    return;
  }

  /* Get accelerometer data and update Fuse filter */
  IMU_getAccelerometerData(f->aVector);
  IMU_fuseAccelerometerUpdateFilter(f, f->aVector);

  /* Get gyro data and update fuse */
  IMU_getGyroData(f->gVector);
  f->gVector[0] = -f->gVector[0];
  f->gVector[1] = -f->gVector[1];
  IMU_fuseGyroUpdate(f, f->gVector);

  /* Perform fusion to compensate for gyro drift */
  IMU_fuseGyroCorrection(f, true, false, 0);

  return;
}

/** @} */
