/******************************************************************************

 @file  sensor_sht21.c

 @brief Driver for the Sensirion SHT21 Humidity sensor

 Group: WCS, LPC, BTS
 Target Device: cc13xx_cc26xx

 ******************************************************************************
 
 Copyright (c) 2012-2022, Texas Instruments Incorporated
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
#include "sensor_sht21.h"
#include "sensor.h"
#include "bsp_i2c.h"

/* ------------------------------------------------------------------------------------------------
*                                           Constants
* ------------------------------------------------------------------------------------------------
*/
// Sensor I2C address
#define SENSOR_I2C_ADDRESS         0x40

#define S_REG_LEN                  2
#define DATA_LEN                   3

// Internal commands
#define SHT21_CMD_TEMP_T_NH        0xF3 // command trig. temp meas. no hold master
#define SHT21_CMD_HUMI_T_NH        0xF5 // command trig. humidity meas. no hold master
#define SHT21_CMD_WRITE_U_R        0xE6 // command write user register
#define SHT21_CMD_READ_U_R         0xE7 // command read user register
#define SHT21_CMD_SOFT_RST         0xFE // command soft reset

#define HUMIDITY				           0x00
#define TEMPERATURE			           0x01

#define USR_REG_MASK               0x38  // Mask off reserved bits (3,4,5)
#define USR_REG_DEFAULT            0x02  // Disable OTP reload
#define USR_REG_RES_MASK           0x7E  // Only change bits 0 and 7 (meas. res.)
#define USR_REG_11BITRES           0x81  // 11-bit resolution

#define USR_REG_TEST_VAL           0x83

#define DATA_SIZE                  6

// Sensor selection/deselection
#define SENSOR_SELECT()                     bspI2cAcquire(BSP_I2C_INTERFACE_0,SENSOR_I2C_ADDRESS)
#define SENSOR_DESELECT()                   bspI2cRelease()

/* ------------------------------------------------------------------------------------------------
*                                           Type Definitions
* ------------------------------------------------------------------------------------------------
*/

/* ------------------------------------------------------------------------------------------------
*                                           Local Functions
* ------------------------------------------------------------------------------------------------
*/
static bool sensorSht21ReadData(uint8_t *pBuf,uint8_t nBytes);
static bool sensorSht21WriteCmd(uint8_t cmd);

/* ------------------------------------------------------------------------------------------------
*                                           Local Variables
* ------------------------------------------------------------------------------------------------
*/
static uint8_t usr;                         // Keeps user register value
static uint8_t buf[DATA_SIZE];              // Data buffer
static bool  success;

/**************************************************************************************************
* @fn          sensorSht21Init
*
* @brief       Initialise the humidity sensor driver
*
* @return      none
**************************************************************************************************/
void sensorSht21Init(void)
{
  SENSOR_SELECT();

  // Set 11 bit resolution
  sensorReadReg(SHT21_CMD_READ_U_R,&usr,1);
  usr &= USR_REG_RES_MASK;
  usr |= USR_REG_11BITRES;
  sensorWriteReg(SHT21_CMD_WRITE_U_R,&usr,1);
  success = true;

  SENSOR_DESELECT();
}

/**************************************************************************************************
* @fn          sensorSht21StartTempMeasure
*
* @brief       Execute measurement step
*
* @return      none
*/
void sensorSht21StartTempMeasure(void)
{
  if (success)
  {
    SENSOR_SELECT();
    success = sensorSht21WriteCmd(SHT21_CMD_TEMP_T_NH);
    SENSOR_DESELECT();
  }
}

/**************************************************************************************************
* @fn          sensorSht21LatchTempMeasure
*
* @brief       Execute measurement step
*
* @return      none
*/
void sensorSht21LatchTempMeasure(void)
{
  if (success)
  {
    SENSOR_SELECT();
    success = sensorSht21ReadData(buf, DATA_LEN);
    SENSOR_DESELECT();
  }
}

/**************************************************************************************************
* @fn          sensorSht21StartHumMeasure
*
* @brief       Execute measurement step
*
* @return      none
*/
void sensorSht21StartHumMeasure(void)
{
  if (success)
  {
    SENSOR_SELECT();
    success = sensorSht21WriteCmd(SHT21_CMD_HUMI_T_NH);
    SENSOR_DESELECT();
  }
}

/**************************************************************************************************
* @fn          sensorSht21LatchHumMeasure
*
* @brief       Latch humidity measurement
*
* @return      none
*/
void sensorSht21LatchHumMeasure(void)
{
  if (success)
  {
    SENSOR_SELECT();
    success = sensorSht21ReadData(buf+DATA_LEN, DATA_LEN);
    SENSOR_DESELECT();
  }
}



/**************************************************************************************************
* @fn          sensorSht21Read
*
* @brief       Get humidity sensor data
*
* @return      none
*/
bool sensorSht21Read(uint16_t *rawTemp, uint16_t *rawHum)
{
  bool valid;

  valid = success;
  if (!success)
  {
    sensorSetErrorData(buf,DATA_SIZE);
  }

  // Store temperature
  *rawTemp = buf[0]<<8 | buf[1];
  // [2] ignore CRC
  // Store humidity
  *rawHum = buf[3]<<8 | buf[4];
  // [5] ignore CRC

  success = true; // Ready for next cycle

  return valid;
}

/**************************************************************************************************
 * @fn          sensorSht21Convert
 *
 * @brief       Convert raw data to temperature and humidity
 *
 * @param       data - raw data from sensor (little endian)
 *
 * @param       temp - converted temperature
 *
 * @param       hum - converted humidity
 *
 * @return      none
 **************************************************************************************************/
void sensorSht21Convert(uint16_t rawTemp, uint16_t rawHum,  float *temp, float *hum)
{
  //-- calculate temperature [°C] --
  rawTemp &= ~0x0003; // clear bits [1..0] (status bits)
  *temp = -46.85 + 175.72/65536 *(double)(int16_t)rawTemp;

  rawHum &= ~0x0003; // clear bits [1..0] (status bits)
  //-- calculate relative humidity [%RH] --
  *hum = -6.0 + 125.0/65536 * (double)rawHum; // RH= -6 + 125 * SRH/2^16
}


/**************************************************************************************************
* @fn          sensorSht21Test
*
* @brief       Humidity sensor self test
*
* @return      none
**************************************************************************************************/
bool sensorSht21Test(void)
{
  uint8_t val;

  SENSOR_SELECT();

  // Verify write and read
  val = USR_REG_TEST_VAL;
  ST_ASSERT(sensorWriteReg(SHT21_CMD_WRITE_U_R,&val,1));
  ST_ASSERT(sensorReadReg(SHT21_CMD_READ_U_R,&val,1));
  ST_ASSERT(val == USR_REG_TEST_VAL);

  SENSOR_DESELECT();

  return true;
}


/* ------------------------------------------------------------------------------------------------
*                                           Private functions
* -------------------------------------------------------------------------------------------------
*/

/**************************************************************************************************
* @fn          halHumiWriteCmd
*
* @brief       Write a command to the humidity sensor
*
* @param       cmd - command to write
*
* @return      TRUE if the command has been transmitted successfully
**************************************************************************************************/
static bool sensorSht21WriteCmd(uint8_t cmd)
{
  /* Send command */
  return bspI2cWriteSingle(cmd);
}


/**************************************************************************************************
* @fn          sensorSht21ReadData
*
* @brief       This function implements the I2C protocol to read from the SHT21.
*
* @param       pBuf - pointer to buffer to place data
*
* @param       nBytes - number of bytes to read
*
* @return      TRUE if the required number of bytes are received
**************************************************************************************************/
static bool sensorSht21ReadData(uint8_t *pBuf, uint8_t nBytes)
{
  /* Read data */
  return bspI2cRead(pBuf,nBytes);
}


/*********************************************************************
*********************************************************************/

