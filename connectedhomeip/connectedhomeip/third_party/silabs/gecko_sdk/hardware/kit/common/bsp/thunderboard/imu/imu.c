/***************************************************************************//**
 * @file
 * @brief Inertial Measurement Unit driver
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
#include <stdio.h>

#include "em_device.h"
#include "gpiointerrupt.h"

#ifdef RADIO_BLE
#include "native_gecko.h"
#endif

#include "thunderboard/board.h"
#include "thunderboard/icm20648.h"
#include "thunderboard/imu/imu.h"
#include "thunderboard/util.h"

/** @cond DO_NOT_INCLUDE_WITH_DOXYGEN */

/****************************************************************************/
/* Local function prototypes                                                */
/****************************************************************************/
static void gpioInterrupt(void);

/** @endcond */

/**************************************************************************//**
* @addtogroup TBSense_BSP
* @{
******************************************************************************/

/***************************************************************************//**
 * @defgroup IMU IMU - Inertial Measurement Unit
 * @{
 * @brief Inertial Measurement Unit driver
 ******************************************************************************/

/** @cond DO_NOT_INCLUDE_WITH_DOXYGEN */

uint8_t              IMU_state = IMU_STATE_DISABLED;/**< IMU state variable                                  */
static float         gyroSampleRate;                /**< Gyroscope sample rate                               */
static float         accelSampleRate;               /**< Accelerometer sample rate                           */
static volatile bool dataReady;                     /**< Flag to show if new accel/gyro data ready to read   */
static uint32_t      IMU_interruptCount = 0;        /**< IMU interrupt counter                               */
static uint32_t      IMU_isDataReadyQueryCount = 0; /**< The number of the total data ready queries          */
static uint32_t      IMU_isDataReadyTrueCount = 0;  /**< The number of queries when data is ready            */
IMU_SensorFusion     fuseObj;                       /**< Structure to store the sensor fusion data           */

/** @endcond */

/***************************************************************************//**
 * @brief
 *    Initializes and calibrates the IMU
 *
 * @return
 *    Returns zero on OK, non-zero otherwise
 ******************************************************************************/
uint32_t IMU_init(void)
{
  uint32_t status;
  uint8_t devid;
  float gyroBiasScaled[3];

  /* GPIOINT driver */
  GPIOINT_Init();

  IMU_state = IMU_STATE_INITIALIZING;
  IMU_fuseNew(&fuseObj);

  /* Initialize acc/gyro driver */
  printf("IMU init...");
  status = ICM20648_init();
  if ( status != ICM20648_OK ) {
    printf("Failed! status = %08lXh\r\n", status);
    goto cleanup;
  }
  printf("OK\r\n");

  status = ICM20648_getDeviceID(&devid);
  if ( status != ICM20648_OK ) {
    goto cleanup;
  }
  printf("IMU device ID = %02Xh\r\n", devid);

  /* Gyro calibration */
  IMU_state = IMU_STATE_CALIBRATING;
  printf("IMU gyro calibration...");
  status = ICM20648_gyroCalibrate(gyroBiasScaled);
  if ( status != ICM20648_OK ) {
    printf("Failed! status = %08lXh\r\n", status);
    goto cleanup;
  }
  printf("OK\r\n");

  printf("Gyroscope bias (deg/s)  : ");
  printf("gx: % 6.4f, gy: % 6.4f, gz: % 6.4f\r\n", gyroBiasScaled[0], gyroBiasScaled[1], gyroBiasScaled[2]);

  IMU_state = IMU_STATE_INITIALIZING;

  cleanup:

  if ( status != ICM20648_OK ) {
    ICM20648_deInit();
    IMU_state = IMU_STATE_DISABLED;
  }

  return status;
}

/***************************************************************************//**
 * @brief
 *    De-initializes the IMU chip
 *
 * @return
 *    Returns zero on OK, non-zero otherwise
 ******************************************************************************/
uint32_t IMU_deInit(void)
{
  uint32_t status;

  IMU_state = IMU_STATE_DISABLED;
  status    = ICM20648_deInit();

  return status;
}

/***************************************************************************//**
 * @brief
 *    Returns IMU state
 *
 * @return
 *    Returns zero on OK, non-zero otherwise
 ******************************************************************************/
uint8_t IMU_getState(void)
{
  return IMU_state;
}

/***************************************************************************//**
 * @brief
 *    Configures the IMU
 *
 * @param[in] sampleRate
 *    The desired sample rate of the acceleration and gyro sensor
 *
 * @return
 *    None
 ******************************************************************************/
void IMU_config(float sampleRate)
{
  uint32_t itStatus;

  /* Set IMU state */
  IMU_state = IMU_STATE_INITIALIZING;

  /* Register PIC interrupt callback */
  BOARD_imuEnableIRQ(true);
  BOARD_imuSetIRQCallback(gpioInterrupt);

  /* Clear the interrupts */
  BOARD_imuClearIRQ();
  ICM20648_interruptStatusRead(&itStatus);

  /* Enable accel sensor */
  ICM20648_sensorEnable(true, true, false);

  /* Set sample rate */
  gyroSampleRate  = ICM20648_gyroSampleRateSet(sampleRate);
  accelSampleRate = ICM20648_accelSampleRateSet(sampleRate);

  printf("IMU sample rate set to %f Hz (accel), %f Hz (gyro)\r\n", accelSampleRate, gyroSampleRate);

  /* Filter bandwidth: 12kHz, otherwise the results may be off */
  ICM20648_accelBandwidthSet(ICM20648_ACCEL_BW_1210HZ);
  ICM20648_gyroBandwidthSet(ICM20648_GYRO_BW_51HZ);

  /* Accel: 2G full scale */
  ICM20648_accelFullscaleSet(ICM20648_ACCEL_FULLSCALE_2G);

  /* Gyro: 250 degrees per sec full scale */
  ICM20648_gyroFullscaleSet(ICM20648_GYRO_FULLSCALE_250DPS);

  UTIL_delay(50);

  /* Enable the raw data ready interrupt */
  ICM20648_interruptEnable(true, false);

  /* Enter low power mode */
  ICM20648_lowPowerModeEnter(true, true, false);

  /* Clear the interrupts */
  BOARD_imuClearIRQ();
  ICM20648_interruptStatusRead(&itStatus);

  /* IMU fuse config & setup */
  IMU_fuseAccelerometerSetSampleRate(&fuseObj, accelSampleRate);
  IMU_fuseGyroSetSampleRate(&fuseObj, gyroSampleRate);
  IMU_fuseReset(&fuseObj);

  IMU_state = IMU_STATE_READY;

  return;
}

/***************************************************************************//**
 * @brief
 *    Retrieves the processed acceleration data
 *
 * @param[out] avec
 *    Three dimensonal acceleration vector
 *
 * @return
 *    None
 ******************************************************************************/
void IMU_accelerationGet(int16_t avec[3])
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
  return;
}

/***************************************************************************//**
 * @brief
 *    Retrieves the processed orientation data
 *
 * @param[out] ovec
 *    Three dimensonal orientation vector
 *
 * @return
 *    None
 ******************************************************************************/
void IMU_orientationGet(int16_t ovec[3])
{
  ovec[0] = (int16_t) (100.0f * IMU_RAD_TO_DEG_FACTOR * fuseObj.orientation[0]);
  ovec[1] = (int16_t) (100.0f * IMU_RAD_TO_DEG_FACTOR * fuseObj.orientation[1]);
  ovec[2] = (int16_t) (100.0f * IMU_RAD_TO_DEG_FACTOR * fuseObj.orientation[2]);

  return;
}

/***************************************************************************//**
 * @brief
 *    Retrieves the processed gyroscope data
 *
 * @param[out] gvec
 *    Three dimensonal gyro vector
 *
 * @return
 *    None
 ******************************************************************************/
void IMU_gyroGet(int16_t gvec[3])
{
  gvec[0] = (int16_t) (100.0f * fuseObj.gVector[0]);
  gvec[1] = (int16_t) (100.0f * fuseObj.gVector[1]);
  gvec[2] = (int16_t) (100.0f * fuseObj.gVector[2]);

  return;
}

/***************************************************************************//**
 * @brief
 *    Performs gyroscope calibration to cancel gyro bias.
 *
 * @return
 *    None
 ******************************************************************************/
uint32_t IMU_gyroCalibrate(void)
{
  uint32_t status;

  status = IMU_OK;

  /* Disable interrupt */
  ICM20648_interruptEnable(false, false);

  IMU_deInit();
  status = IMU_init();

  /* Restart regular sampling */
  IMU_config(gyroSampleRate);

  return status;
}

/***************************************************************************//**
 * @brief
 *    Gets a new set of data from the accel and gyro sensor and updates the
 *    fusion calculation
 *
 * @return
 *    None
 ******************************************************************************/
void IMU_update(void)
{
  IMU_fuseUpdate(&fuseObj);

  return;
}

/***************************************************************************//**
 * @brief
 *    Resets the fusion calculation
 *
 * @return
 *    None
 ******************************************************************************/
void IMU_reset(void)
{
  IMU_fuseReset(&fuseObj);

  return;
}

/***************************************************************************//**
 * @brief
 *    Retrieves the raw acceleration data from the IMU
 *
 * @param[out] avec
 *    Three dimensonal raw acceleration vector
 *
 * @return
 *    None
 ******************************************************************************/
void IMU_getAccelerometerData(float avec[3])
{
  if ( IMU_state != IMU_STATE_READY ) {
    avec[0] = 0;
    avec[1] = 0;
    avec[2] = 0;
    return;
  }

  ICM20648_accelDataRead(avec);

  return;
}

/***************************************************************************//**
 * @brief
 *    Retrieves the processed gyroscope correction angles
 *
 * @param[out] acorr
 *    Three dimensonal gyro correction angle vector
 *
 * @return
 *    None
 ******************************************************************************/
void IMU_getGyroCorrectionAngles(float acorr[3])
{
  acorr[0] = fuseObj.angleCorrection[0];
  acorr[1] = fuseObj.angleCorrection[1];
  acorr[2] = fuseObj.angleCorrection[2];

  return;
}

/***************************************************************************//**
 * @brief
 *    Retrieves the raw gyroscope data from the IMU
 *
 * @param[out] gvec
 *    Three dimensonal raw gyro vector
 *
 * @return
 *    None
 ******************************************************************************/
void IMU_getGyroData(float gvec[3])
{
  if ( IMU_state != IMU_STATE_READY ) {
    gvec[0] = 0;
    gvec[1] = 0;
    gvec[2] = 0;
    return;
  }

  ICM20648_gyroDataRead(gvec);

  return;
}

/***************************************************************************//**
 * @brief
 *    Checks if there is new accel/gyro data available for read
 *
 * @return
 *    True if the measurement data is ready, false otherwise
 ******************************************************************************/
bool IMU_isDataReady(void)
{
  bool ready;

  if ( IMU_state != IMU_STATE_READY ) {
    return false;
  }

  ready = ICM20648_isDataReady();
  IMU_isDataReadyQueryCount++;

  if ( ready ) {
    IMU_isDataReadyTrueCount++;
  }

  return ready;
}

/***************************************************************************//**
 * @brief
 *    Checks if there is new accel/gyro data available for read. In case of ver
 *    0.3.0 and older of the PIC firmware the state of the
 *    PIC_INT_WAKE needs to be read to determinte if the IMU interrupt is
 *    valid.
 *
 * @return
 *    True if data ready flag is set, false otherwise
 ******************************************************************************/
bool IMU_isDataReadyFlag(void)
{
  bool ready;

  if ( IMU_state != IMU_STATE_READY ) {
    return false;
  }

  ready = dataReady;

  return ready;
}

/***************************************************************************//**
 * @brief
 *    Clears the IMU data ready flag. In case of ver 0.3.0 and older of the
 *    PIC firmware the interrupt register also needs to be cleared.
 *
 * @return
 *    None
 ******************************************************************************/
void IMU_clearDataReadyFlag(void)
{
  dataReady = false;

  BOARD_imuClearIRQ();

  return;
}

/** @cond DO_NOT_INCLUDE_WITH_DOXYGEN */

/***************************************************************************//**
 * @brief
 *    GPIO interrupt callback function. It is called when the INT pin of the
 *    IMU asserted and the interrupt functions are enabled.
 *
 * @param[in] pin
 *    The microcontroller pin connected to INT output (not used)
 *
 * @return
 *    Returns zero on OK, non-zero otherwise
 ******************************************************************************/
static void gpioInterrupt(void)
{
  dataReady = true;
  IMU_interruptCount++;

#ifdef RADIO_BLE
  gecko_external_signal(1);
#endif

  return;
}

/** @endcond */

/** @} */
/** @} */
