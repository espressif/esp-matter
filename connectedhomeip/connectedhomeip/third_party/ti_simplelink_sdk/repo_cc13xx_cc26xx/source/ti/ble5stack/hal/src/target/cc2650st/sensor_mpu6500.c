/******************************************************************************

 @file  sensor_mpu6500.c

 @brief Driver for the Invensys MPU6500 Motion Porcessing Unit

 Group: WCS, LPC, BTS
 Target Device: cc13xx_cc26xx

 ******************************************************************************
 
 Copyright (c) 2013-2022, Texas Instruments Incorporated
 All rights reserved.

 Redistribution and use in source and binary forms, with or without
 modification, are permitted provided that the following conditions
 are met:

 *  Redistributions of source code must retain the above copyright
    notice, this list of conditions and the following disclaimer.

 *  Redistributions in binary form must reproduce the above copyright
    notice, this list of conditions and the following disclaimer in the
    documentation and/or other materials provided with the distribution.

 *  Neither the name of Texas Instruments Incorporated nor the names of
    its contributors may be used to endorse or promote products derived
    from this software without specific prior written permission.

 THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR
 CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

 ******************************************************************************
 
 
 *****************************************************************************/

/* ------------------------------------------------------------------------------------------------
*                                          Includes
* ------------------------------------------------------------------------------------------------
*/

#include "sensor_mpu6500.h"
#include "sensor.h"
#include "bsp_i2c.h"

/* ------------------------------------------------------------------------------------------------
*                                           Constants
* ------------------------------------------------------------------------------------------------
*/
// Sensor I2C address
#define SENSOR_I2C_ADDRESS            0x68

// Registers
#define SELF_TEST_X_GYRO              0x00 // R/W
#define SELF_TEST_Y_GYRO              0x01 // R/W
#define SELF_TEST_Z_GYRO              0x02 // R/W
#define SELF_TEST_X_ACCEL             0x0D // R/W
#define SELF_TEST_Z_ACCEL             0x0E // R/W
#define SELF_TEST_Y_ACCEL             0x0F // R/W

#define XG_OFFSET_H                   0x13 // R/W
#define XG_OFFSET_L                   0x14 // R/W
#define YG_OFFSET_H                   0x15 // R/W
#define YG_OFFSET_L                   0x16 // R/W
#define ZG_OFFSET_H                   0x17 // R/W
#define ZG_OFFSET_L                   0x18 // R/W

#define SMPLRT_DIV                    0x19 // R/W
#define CONFIG                        0x1A // R/W
#define GYRO_CONFIG                   0x1B // R/W
#define ACCEL_CONFIG                  0x1C // R/W
#define ACCEL_CONFIG_2                0x1D // R/W
#define LP_ACCEL_ODR                  0x1E // R/W
#define WOM_THR                       0x1F // R/W
#define FIFO_EN                       0x23 // R/W

// .. registers 0x24 - 0x36 are not applicable to the SensorTag HW configuration (IC2 Master)

#define INT_PIN_CFG                   0x37 // R/W
#define INT_ENABLE                    0x38 // R/W
#define INT_STATUS                    0x3A // R
#define ACCEL_XOUT_H                  0x3B // R
#define ACCEL_XOUT_L                  0x3C // R
#define ACCEL_YOUT_H                  0x3D // R
#define ACCEL_YOUT_L                  0x3E // R
#define ACCEL_ZOUT_H                  0x3F // R
#define ACCEL_ZOUT_L                  0x40 // R
#define TEMP_OUT_H                    0x41 // R
#define TEMP_OUT_L                    0x42 // R
#define GYRO_XOUT_H                   0x43 // R
#define GYRO_XOUT_L                   0x44 // R
#define GYRO_YOUT_H                   0x45 // R
#define GYRO_YOUT_L                   0x46 // R
#define GYRO_ZOUT_H                   0x47 // R
#define GYRO_ZOUT_L                   0x48 // R

// .. registers 0x49 - 0x60 are not applicable to the SensorTag HW configuration (external sensor data)
// .. registers 0x63 - 0x67 are not applicable to the SensorTag HW configuration (I2C master)

#define SIGNAL_PATH_RESET             0x68 // R/W
#define ACCEL_INTEL_CTRL              0x69 // R/W
#define USER_CTRL                     0x6A // R/W
#define PWR_MGMT_1                    0x6B // R/W
#define PWR_MGMT_2                    0x6C // R/W
#define FIFO_COUNT_H                  0x72 // R/W
#define FIFO_COUNT_L                  0x73 // R/W
#define FIFO_R_W                      0x74 // R/W
#define WHO_AM_I                      0x75 // R/W

// Acceleromter ranges
#define ACC_REG_CTRL_2G               0x00
#define ACC_REG_CTRL_4G               0x08
#define ACC_REG_CTRL_8G               0x10
#define ACC_REG_CTRL_16G              0x18
#define ACC_REG_CTRL_INVALID          0xFF

// Masks is mpuConfig valiable
#define ACC_CONFIG_MASK               0x38
#define GYRO_CONFIG_MASK              0x07

// Values PWR_MGMT_1
#define MPU_SLEEP                     0x4F  // Sleep + stop all clocks
#define MPU_WAKE_UP                   0x01  // MPU reset

// Values PWR_MGMT_2
#define ALL_AXES                      0x3F
#define GYRO_AXES                     0x07
#define ACC_AXES                      0x38

// Data sizes
#define DATA_SIZE                     6

/* Sensor selection/deselection */
#define SENSOR_SELECT()                 bspI2cAcquire(BSP_I2C_INTERFACE_1,SENSOR_I2C_ADDRESS)
#define SENSOR_DESELECT()               bspI2cRelease()

/* ------------------------------------------------------------------------------------------------
*                                           Typedefs
* ------------------------------------------------------------------------------------------------
*/

/* ------------------------------------------------------------------------------------------------
*                                           Macros
* ------------------------------------------------------------------------------------------------
*/

/* ------------------------------------------------------------------------------------------------
*                                           Local Functions
* ------------------------------------------------------------------------------------------------
*/
static void sensorMpu6500Sleep(void);
static void sensorMpu6500WakeUp(void);
static void sensorMpu6500SelectAxes(void);

/* ------------------------------------------------------------------------------------------------
*                                           Local Variables
* ------------------------------------------------------------------------------------------------
*/
static uint8_t mpuConfig;
static uint8_t accRange;
static uint8_t accRangeReg;
static uint8_t val;

/**************************************************************************************************
* @fn          sensorMpu6500Init
*
* @brief       This function initializes the HAL Accelerometer abstraction layer.
*
* @return      None.
*/
void sensorMpu6500Init(void)
{
  // Select this sensor
  SENSOR_SELECT();

  // IOCPinTypeGpioOutput(BSP_IOID_MPU_POWER);
  // GPIOPinWrite(BSP_MPU_POWER, 0);
  mpuConfig = 0;   // All axes off
  sensorMpu6500AccSetRange(ACC_RANGE_8G);
  sensorMpu6500Sleep();

  SENSOR_DESELECT();
}

/**************************************************************************************************
* @fn          sensorMpu6500AccEnable
*
* @brief       Enable acceleromter readout
*
* @param       Axes: bitmap [0..2], X = 1, Y = 2, Z = 4. 0 = accelerometer off
*
* @return      None
*/
void sensorMpu6500AccEnable(uint8_t axes)
{
  // Select this sensor
  SENSOR_SELECT();

  if (mpuConfig == 0 && axes != 0)
  {
    // Wake up the sensor if it was off
    sensorMpu6500WakeUp();
  }

  mpuConfig &= ~ACC_CONFIG_MASK;
  mpuConfig |= (axes << 3);

  if (mpuConfig != 0)
  {
    // Enable accelerometer readout
    sensorMpu6500SelectAxes();
  }
  else if (mpuConfig == 0)
  {
    sensorMpu6500Sleep();
  }
  SENSOR_DESELECT();
}

/**************************************************************************************************
* @fn          sensorMpu6500GyroEnable
*
* @brief       Enable gyroscope readout
*
* @param       Axes: bitmap [0..2], X = 1, Y = 2, Z = 4. 0 = gyroscope off
*
* @return      None
*/
void sensorMpu6500GyroEnable(uint8_t axes)
{
  // Select this sensor
  SENSOR_SELECT();

  if (mpuConfig == 0 && axes != 0)
  {
    // Wake up the sensor if it was off
    sensorMpu6500WakeUp();
  }

  mpuConfig &= ~GYRO_CONFIG_MASK;
  mpuConfig |= axes;

  if (mpuConfig != 0)
  {
    // Enable gyro readout
    sensorMpu6500SelectAxes();
  }
  else if (mpuConfig == 0)
  {
    sensorMpu6500Sleep();
  }

  SENSOR_DESELECT();
}

/**************************************************************************************************
* @fn          sensorMpu6500AccSetRange
*
* @brief       Set the range of the accelerometer
*
* @param       range: ACC_RANGE_2G, ACC_RANGE_4G, ACC_RANGE_8G, ACC_RANGE_16G
*
* @return      None
*/
void sensorMpu6500AccSetRange(uint8_t range)
{
  switch (range)
  {
  case ACC_RANGE_2G:
    accRangeReg = ACC_REG_CTRL_2G;
    break;
  case ACC_RANGE_4G:
    accRangeReg = ACC_REG_CTRL_4G;
    break;
  case ACC_RANGE_8G:
    accRangeReg = ACC_REG_CTRL_8G;
    break;
  case ACC_RANGE_16G:
    accRangeReg = ACC_REG_CTRL_16G;
    break;
  default:
    accRangeReg = ACC_REG_CTRL_INVALID;
    // Should not get here
    break;
  }

  if (accRangeReg != ACC_REG_CTRL_INVALID)
  {
    accRange = range;
  }
}

/**************************************************************************************************
* @fn          sensorMpu6500AccRead
*
* @brief       Read data from the accelerometer - X, Y, Z - 3 words
*
* @return      TRUE if valid data, FALSE if not
*/
bool sensorMpu6500AccRead(uint16_t *data )
{
  bool success;

  // Select this sensor
  SENSOR_SELECT();

  // Burst read of all acceleromter values
  success = sensorReadReg(ACCEL_XOUT_H, (uint8_t*)data, DATA_SIZE);
  if (!success)
  {
    sensorSetErrorData((uint8_t*)data,DATA_SIZE);
  }

  SENSOR_DESELECT();

  return success;
}

/**************************************************************************************************
* @fn          sensorMpu6500GyroRead
*
* @brief       Read data from the gyroscope - X, Y, Z - 3 words
*
* @return      TRUE if valid data, FALSE if not
*/
bool sensorMpu6500GyroRead(uint16_t *data )
{
  bool success;

  // Select this sensor
  SENSOR_SELECT();

  // Burst read of all gyroscope values
  success = sensorReadReg(GYRO_XOUT_H, (uint8_t*)data, DATA_SIZE);

  if (!success)
  {
    sensorSetErrorData((uint8_t*)data,DATA_SIZE);
  }

  SENSOR_DESELECT();

  return success;
}

/**************************************************************************************************
 * @fn          sensorMpu6500Test
 *
 * @brief       Run a sensor self-test
 *
 * @return      TRUE if passed, FALSE if failed
 */
bool sensorMpu6500Test(void)
{
  // Select this sensor on the I2C bus
  SENSOR_SELECT();

  // Check the WHO AM I register
  ST_ASSERT(sensorReadReg(WHO_AM_I, &val, 1));
  ST_ASSERT(val == 0x70);

  SENSOR_DESELECT();

  return true;
}

/**************************************************************************************************
 * @fn          sensorMpu6500AccelConvert
 *
 * @brief       Convert raw data to G units
 *
 * @param       rawData - raw data from sensor
 *
 * @return      Converted value
 **************************************************************************************************/
float sensorMpu6500AccelConvert(int16_t rawData)
{
  float v;

  switch (accRange)
  {
  case ACC_RANGE_2G:
    //-- calculate acceleration, unit G, range -2, +2
    v = (rawData * 1.0) / (32768/2);
    break;

  case ACC_RANGE_4G:
    //-- calculate acceleration, unit G, range -4, +4
    v = (rawData * 1.0) / (32768/4);
    break;

  case ACC_RANGE_8G:
    //-- calculate acceleration, unit G, range -8, +8
    v = (rawData * 1.0) / (32768/8);
    break;

  case ACC_RANGE_16G:
    //-- calculate acceleration, unit G, range -16, +16
    v = (rawData * 1.0) / (32768/16);
    break;
  }

  return v;
}

/**************************************************************************************************
 * @fn          sensorMpu6500GyroConvert
 *
 * @brief       Convert raw data to deg/sec units
 *
 * @param       data - raw data from sensor
 *
 * @return      none
 **************************************************************************************************/
float sensorMpu6500GyroConvert(int16_t data)
{
  //-- calculate rotation, unit deg/s, range -250, +250
  return (data * 1.0) / (65536 / 500);
}

/* ------------------------------------------------------------------------------------------------
*                                           Private functions
* -------------------------------------------------------------------------------------------------
*/

/**************************************************************************************************
* @fn          sensorMpu6500Sleep
*
* @brief       Place the MPU in low power mode
*
* @return
*/
static void sensorMpu6500Sleep(void)
{
  val = ALL_AXES;
  sensorWriteReg(PWR_MGMT_2, &val, 1);

  val = MPU_SLEEP;
  sensorWriteReg(PWR_MGMT_1, &val, 1);
}


/**************************************************************************************************
* @fn          sensorMpu6500WakeUp
*
* @brief       Exit low power mode
*
* @return      none
*/
static void sensorMpu6500WakeUp(void)
{
  val = MPU_WAKE_UP;
  sensorWriteReg(PWR_MGMT_1, &val, 1);

  val = ALL_AXES;
  sensorWriteReg(PWR_MGMT_2, &val, 1);
  mpuConfig = 0;

  if (accRangeReg != ACC_REG_CTRL_INVALID)
    sensorWriteReg(ACCEL_CONFIG, &accRangeReg, 1);
}


/**************************************************************************************************
* @fn          sensorMpu6500SelectAxes
*
* @brief       MPU in sleep
*
* @return      none
*/
static void sensorMpu6500SelectAxes(void)
{
  val = ~mpuConfig;
  sensorWriteReg(PWR_MGMT_2, &val, 1);
}


/*********************************************************************
*********************************************************************/
