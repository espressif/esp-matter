/***************************************************************************//**
 * @file
 * @brief Driver for the Si1133 Ambient Light and UV sensor
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
#include "i2cspm.h"

#include "thunderboard/util.h"
#include "thunderboard/board.h"
#include "thunderboard/si1133.h"

/**************************************************************************//**
* @addtogroup TBSense_BSP
* @{
******************************************************************************/

/***************************************************************************//**
 * @defgroup Si1133 SI1133 - Ambient Light and UV Index Sensor
 * @{
 * @brief Driver for the Silicon Labs Si1133 Ambient Light and UV sensor
 ******************************************************************************/

/** @cond DO_NOT_INCLUDE_WITH_DOXYGEN */

/***************************************************************************//**
 * @brief
 *    Coefficients for lux calculation
 ******************************************************************************/
static SI1133_LuxCoeff_TypeDef lk = {
  { {     0, 209 },           /**< coeff_high[0]   */
    {  1665, 93  },           /**< coeff_high[1]   */
    {  2064, 65  },           /**< coeff_high[2]   */
    { -2671, 234 } },         /**< coeff_high[3]   */
  { {     0, 0   },           /**< coeff_low[0]    */
    {  1921, 29053 },         /**< coeff_low[1]    */
    { -1022, 36363 },         /**< coeff_low[2]    */
    {  2320, 20789 },         /**< coeff_low[3]    */
    {  -367, 57909 },         /**< coeff_low[4]    */
    { -1774, 38240 },         /**< coeff_low[5]    */
    {  -608, 46775 },         /**< coeff_low[6]    */
    { -1503, 51831 },         /**< coeff_low[7]    */
    { -1886, 58928 } }        /**< coeff_low[8]    */
};

/***************************************************************************//**
 * @brief
 *    Coefficients for UV index calculation
 ******************************************************************************/
static SI1133_Coeff_TypeDef uk[2] = {
  { 1281, 30902 },            /**< coeff[0]        */
  { -638, 46301 }             /**< coeff[1]        */
};

/** @endcond */

/** @cond DO_NOT_INCLUDE_WITH_DOXYGEN */

static int32_t SI1133_calcPolyInner (int32_t input, int8_t fraction, uint16_t mag, int8_t  shift);
static int32_t SI1133_calcEvalPoly  (int32_t x, int32_t y, uint8_t input_fraction, uint8_t output_fraction, uint8_t num_coeff, SI1133_Coeff_TypeDef *kp);

/** @endcond */

/***************************************************************************//**
 * @brief
 *    Reads register from the Si1133 sensor
 *
 * @param[in] reg
 *    The register address to read from in the sensor.
 *
 * @param[out] data
 *    The data read from the sensor
 *
 * @return
 *    Returns zero on OK, non-zero otherwise
 ******************************************************************************/
uint32_t SI1133_registerRead(uint8_t reg, uint8_t *data)
{
  I2C_TransferSeq_TypeDef seq;
  I2C_TransferReturn_TypeDef ret;
  uint8_t i2c_write_data[1];
  uint32_t retval;

  retval = SI1133_OK;

  seq.addr = SI1133_I2C_DEVICE_BUS_ADDRESS;
  seq.flags = I2C_FLAG_WRITE_READ;
  /* Select register to start reading from */
  i2c_write_data[0] = reg;
  seq.buf[0].data = i2c_write_data;
  seq.buf[0].len = 1;
  /* Select length of data to be read */
  seq.buf[1].data = data;
  seq.buf[1].len = 1;

  BOARD_i2cBusSelect(BOARD_I2C_BUS_SELECT_ENV_SENSOR);

  ret = I2CSPM_Transfer(SI1133_I2C_DEVICE, &seq);
  if ( ret != i2cTransferDone ) {
    *data = 0xff;
    retval = SI1133_ERROR_I2C_TRANSACTION_FAILED;
  }

  return retval;
}

/***************************************************************************//**
 * @brief
 *    Writes register in the Si1133 sensor
 *
 * @param[in] reg
 *    The register address to write to in the sensor
 *
 * @param[in] data
 *    The data to write to the sensor
 *
 * @return
 *    Returns zero on OK, non-zero otherwise
 ******************************************************************************/
uint32_t SI1133_registerWrite(uint8_t reg, uint8_t data)
{
  I2C_TransferSeq_TypeDef seq;
  I2C_TransferReturn_TypeDef ret;
  uint8_t i2c_write_data[2];
  uint8_t i2c_read_data[1];
  uint32_t retval;

  retval = SI1133_OK;

  seq.addr = SI1133_I2C_DEVICE_BUS_ADDRESS;
  seq.flags = I2C_FLAG_WRITE;
  /* Select register and data to write */
  i2c_write_data[0] = reg;
  i2c_write_data[1] = data;
  seq.buf[0].data = i2c_write_data;
  seq.buf[0].len = 2;
  seq.buf[1].data = i2c_read_data;
  seq.buf[1].len = 0;

  BOARD_i2cBusSelect(BOARD_I2C_BUS_SELECT_ENV_SENSOR);

  ret = I2CSPM_Transfer(SI1133_I2C_DEVICE, &seq);
  if ( ret != i2cTransferDone ) {
    retval = SI1133_ERROR_I2C_TRANSACTION_FAILED;
  }

  return retval;
}

/***************************************************************************//**
 * @brief
 *    Writes a block of data to the Si1133 sensor.
 *
 * @param[in] reg
 *    The first register to begin writing to
 *
 * @param[in] length
 *    The number of bytes to write to the sensor
 *
 * @param[in] data
 *    The data to write to the sensor
 *
 * @return
 *    Returns zero on OK, non-zero otherwise
 ******************************************************************************/
uint32_t SI1133_registerBlockWrite(uint8_t reg, uint8_t length, uint8_t *data)
{
  I2C_TransferSeq_TypeDef seq;
  I2C_TransferReturn_TypeDef ret;
  uint8_t i2c_write_data[10];
  uint8_t i2c_read_data[1];
  uint8_t i;
  uint32_t retval;

  retval = SI1133_OK;

  seq.addr = SI1133_I2C_DEVICE_BUS_ADDRESS;
  seq.flags = I2C_FLAG_WRITE;
  /* Select register to start writing to*/
  i2c_write_data[0] = reg;
  for ( i = 0; i < length; i++ ) {
    i2c_write_data[i + 1] = data[i];
  }
  seq.buf[0].data = i2c_write_data;
  seq.buf[0].len = length + 1;
  seq.buf[1].data = i2c_read_data;
  seq.buf[1].len = 0;

  BOARD_i2cBusSelect(BOARD_I2C_BUS_SELECT_ENV_SENSOR);

  ret = I2CSPM_Transfer(SI1133_I2C_DEVICE, &seq);
  if ( ret != i2cTransferDone ) {
    retval = SI1133_ERROR_I2C_TRANSACTION_FAILED;
  }

  return retval;
}

/***************************************************************************//**
 * @brief
 *    Reads a block of data from the Si1133 sensor.
 *
 * @param[in] reg
 *    The first register to begin reading from
 *
 * @param[in] length
 *    The number of bytes to write to the sensor
 *
 * @param[out] data
 *    The data read from the sensor
 *
 * @return
 *    Returns zero on OK, non-zero otherwise
 ******************************************************************************/
uint32_t SI1133_registerBlockRead(uint8_t reg, uint8_t length, uint8_t *data)
{
  I2C_TransferSeq_TypeDef seq;
  I2C_TransferReturn_TypeDef ret;
  uint8_t i2c_write_data[1];
  uint32_t retval;

  retval = SI1133_OK;

  seq.addr = SI1133_I2C_DEVICE_BUS_ADDRESS;
  seq.flags = I2C_FLAG_WRITE_READ;
  /* Select register to start reading from */
  i2c_write_data[0] = reg;
  seq.buf[0].data = i2c_write_data;
  seq.buf[0].len = 1;
  /* Select length of data to be read */
  seq.buf[1].data = data;
  seq.buf[1].len = length;

  BOARD_i2cBusSelect(BOARD_I2C_BUS_SELECT_ENV_SENSOR);

  ret = I2CSPM_Transfer(SI1133_I2C_DEVICE, &seq);
  if ( ret != i2cTransferDone ) {
    retval = SI1133_ERROR_I2C_TRANSACTION_FAILED;
  }

  return retval;
}

/***************************************************************************//**
 * @brief
 *    Waits until the Si1133 is sleeping before proceeding
 *
 * @return
 *    Returns zero on OK, non-zero otherwise
 ******************************************************************************/
static uint32_t SI1133_waitUntilSleep(void)
{
  uint32_t ret;
  uint8_t response;
  uint8_t count = 0;
  uint32_t retval;

  retval = SI1133_OK;

  /* This loops until the Si1133 is known to be in its sleep state  */
  /* or if an i2c error occurs                                      */
  while ( count < 5 ) {
    ret = SI1133_registerRead(SI1133_REG_RESPONSE0, &response);
    if ( (response & SI1133_RSP0_CHIPSTAT_MASK) == SI1133_RSP0_SLEEP ) {
      break;
    }

    if ( ret != SI1133_OK ) {
      retval = SI1133_ERROR_SLEEP_FAILED;
      break;
    }

    count++;
  }

  return retval;
}

/***************************************************************************//**
 * @brief
 *    Resets the Si1133
 *
 * @return
 *    Returns zero on OK, non-zero otherwise
 ******************************************************************************/
uint32_t SI1133_reset(void)
{
  uint32_t retval;

  /* Perform the Reset Command */
  retval = SI1133_registerWrite(SI1133_REG_COMMAND, SI1133_CMD_RESET);

  /* Delay for 10 ms. This delay is needed to allow the Si1133   */
  /* to perform internal reset sequence.                         */
  UTIL_delay(30);

  return retval;
}

/***************************************************************************//**
 * @brief
 *    Helper function to send a command to the Si1133
 *
 * @param[in] command
 *    The command to send to the sensor
 *
 * @return
 *    Returns zero on OK, non-zero otherwise
 ******************************************************************************/
static uint32_t SI1133_sendCmd(uint8_t command)
{
  uint8_t response;
  uint8_t response_stored;
  uint8_t count = 0;
  uint32_t ret;

  /* Get the response register contents */
  ret = SI1133_registerRead(SI1133_REG_RESPONSE0, &response_stored);
  if ( ret != SI1133_OK ) {
    return ret;
  }

  response_stored = response_stored & SI1133_RSP0_COUNTER_MASK;

  /* Double-check the response register is consistent */
  while ( count < 5 ) {
    ret = SI1133_waitUntilSleep();
    if ( ret != SI1133_OK ) {
      return ret;
    }
    /* Skip if the command is RESET COMMAND COUNTER */
    if ( command == SI1133_CMD_RESET_CMD_CTR ) {
      break;
    }

    ret = SI1133_registerRead(SI1133_REG_RESPONSE0, &response);

    if ( (response & SI1133_RSP0_COUNTER_MASK) == response_stored ) {
      break;
    } else {
      if ( ret != SI1133_OK ) {
        return ret;
      } else {
        response_stored = response & SI1133_RSP0_COUNTER_MASK;
      }
    }

    count++;
  }

  /* Send the command */
  ret = SI1133_registerWrite(SI1133_REG_COMMAND, command);
  if ( ret != SI1133_OK ) {
    return ret;
  }

  count = 0;
  /* Expect a change in the response register */
  while ( count < 5 ) {
    /* Skip if the command is RESET COMMAND COUNTER */
    if ( command == SI1133_CMD_RESET_CMD_CTR ) {
      break;
    }

    ret = SI1133_registerRead(SI1133_REG_RESPONSE0, &response);
    if ( (response & SI1133_RSP0_COUNTER_MASK) != response_stored ) {
      break;
    } else {
      if ( ret != SI1133_OK ) {
        return ret;
      }
    }

    count++;
  }

  return SI1133_OK;
}

/***************************************************************************//**
 * @brief
 *    Sends a RESET COMMAND COUNTER command to the Si1133
 *
 * @return
 *    Returns zero on OK, non-zero otherwise
 ******************************************************************************/
uint32_t SI1133_resetCmdCtr(void)
{
  return SI1133_sendCmd(SI1133_CMD_RESET_CMD_CTR);
}

/***************************************************************************//**
 * @brief
 *    Sends a FORCE command to the Si1133
 *
 * @return
 *    Returns zero on OK, non-zero otherwise
 ******************************************************************************/
uint32_t SI1133_measurementForce(void)
{
  return SI1133_sendCmd(SI1133_CMD_FORCE_CH);
}

/***************************************************************************//**
 * @brief
 *    Sends a START command to the Si1133
 *
 * @return
 *    Returns zero on OK, non-zero otherwise
 ******************************************************************************/
uint32_t SI1133_measurementStart(void)
{
  return SI1133_sendCmd(SI1133_CMD_START);
}

/***************************************************************************//**
 * @brief
 *    Reads a parameter from the Si1133
 *
 * @param[in] address
 *    The address of the parameter.
 *
 * @return
 *    Returns zero on OK, non-zero otherwise
 ******************************************************************************/
uint32_t SI1133_paramRead(uint8_t address)
{
  uint8_t retval;
  uint8_t cmd;

  cmd = 0x40 + (address & 0x3F);

  retval = SI1133_sendCmd(cmd);
  if ( retval != SI1133_OK ) {
    return retval;
  }

  SI1133_registerRead(SI1133_REG_RESPONSE1, &retval);

  return retval;
}

/***************************************************************************//**
 * @brief
 *    Writes a byte to an Si1133 Parameter
 *
 * @param[in] address
 *    The parameter address
 *
 * @param[in] value
 *    The byte value to be written to the Si1133 parameter
 *
 * @return
 *    Returns zero on OK, non-zero otherwise
 *
 * @note
 *    This function ensures that the Si1133 is idle and ready to
 *    receive a command before writing the parameter. Furthermore,
 *    command completion is checked. If setting parameter is not done
 *    properly, no measurements will occur. This is the most common
 *    error. It is highly recommended that host code make use of this
 *    function.
 ******************************************************************************/
uint32_t SI1133_paramSet(uint8_t address, uint8_t value)
{
  uint32_t retval;
  uint8_t buffer[2];
  uint8_t response_stored;
  uint8_t response;
  uint8_t count;

  retval = SI1133_waitUntilSleep();
  if ( retval != SI1133_OK ) {
    return retval;
  }

  SI1133_registerRead(SI1133_REG_RESPONSE0, &response_stored);
  response_stored &= SI1133_RSP0_COUNTER_MASK;

  buffer[0] = value;
  buffer[1] = 0x80 + (address & 0x3F);

  retval = SI1133_registerBlockWrite(SI1133_REG_HOSTIN0, 2, (uint8_t*) buffer);
  if ( retval != SI1133_OK ) {
    return retval;
  }

  /* Wait for command to finish */
  count = 0;
  /* Expect a change in the response register */
  while ( count < 5 ) {
    retval = SI1133_registerRead(SI1133_REG_RESPONSE0, &response);
    if ( (response & SI1133_RSP0_COUNTER_MASK) != response_stored ) {
      break;
    } else {
      if ( retval != SI1133_OK ) {
        return retval;
      }
    }

    count++;
  }

  return SI1133_OK;
}

/***************************************************************************//**
 * @brief
 *    Sends a PAUSE command to the Si1133
 *
 * @return
 *    Returns zero on OK, non-zero otherwise
 ******************************************************************************/
uint32_t SI1133_measurementPause(void)
{
  return SI1133_sendCmd(SI1133_CMD_PAUSE_CH);
}

/**************************************************************************//**
 * @brief
 *    Initializes the Si1133 chip
 *
 * @return
 *    Returns zero on OK, non-zero otherwise
 *****************************************************************************/
uint32_t SI1133_init(void)
{
  uint32_t retval;

  /* Enable power to the sensor */
  BOARD_alsEnable(true);

  /* Do not access the Si1133 earlier than 25 ms from power-up */
  UTIL_delay(50);

  /* Reset the sensor. The reset function implements the necessary delays after reset. */
  retval = SI1133_reset();

  retval += SI1133_paramSet(SI1133_PARAM_CH_LIST, 0x0f);
  retval += SI1133_paramSet(SI1133_PARAM_ADCCONFIG0, 0x78);
  retval += SI1133_paramSet(SI1133_PARAM_ADCSENS0, 0x71);
  retval += SI1133_paramSet(SI1133_PARAM_ADCPOST0, 0x40);
  retval += SI1133_paramSet(SI1133_PARAM_ADCCONFIG1, 0x4d);
  retval += SI1133_paramSet(SI1133_PARAM_ADCSENS1, 0xe1);
  retval += SI1133_paramSet(SI1133_PARAM_ADCPOST1, 0x40);
  retval += SI1133_paramSet(SI1133_PARAM_ADCCONFIG2, 0x41);
  retval += SI1133_paramSet(SI1133_PARAM_ADCSENS2, 0xe1);
  retval += SI1133_paramSet(SI1133_PARAM_ADCPOST2, 0x50);
  retval += SI1133_paramSet(SI1133_PARAM_ADCCONFIG3, 0x4d);
  retval += SI1133_paramSet(SI1133_PARAM_ADCSENS3, 0x87);
  retval += SI1133_paramSet(SI1133_PARAM_ADCPOST3, 0x40);

  retval += SI1133_registerWrite(SI1133_REG_IRQ_ENABLE, 0x0f);

  return retval;
}

/***************************************************************************//**
 * @brief
 *    Stops the measurements on all channel and waits until the chip
 *    goes to sleep state.
 *
 * @return
 *    Returns zero on OK, non-zero otherwise
 ******************************************************************************/
uint32_t SI1133_deInit(void)
{
  uint32_t retval;

  retval = SI1133_paramSet(SI1133_PARAM_CH_LIST, 0x3f);
  retval += SI1133_measurementPause();
  retval += SI1133_waitUntilSleep();

  /* Turn off the power of the sensor */
  BOARD_alsEnable(false);

  return retval;
}

/***************************************************************************//**
 * @brief
 *    Read samples from the Si1133 chip
 *
 * @param[out] samples
 *    Retrieves interrupt status and measurement data for channel 0..3 and
 *    converts the data to int32_t format
 *
 * @return
 *    Returns zero on OK, non-zero otherwise
 ******************************************************************************/
uint32_t SI1133_measurementGet(SI1133_Samples_TypeDef *samples)
{
  uint8_t buffer[13];
  uint32_t retval;

  retval = SI1133_registerBlockRead(SI1133_REG_IRQ_STATUS, 13, buffer);

  samples->irq_status = buffer[0];

  samples->ch0 = buffer[1] << 16;
  samples->ch0 |= buffer[2] << 8;
  samples->ch0 |= buffer[3];
  if ( samples->ch0 & 0x800000 ) {
    samples->ch0 |= 0xFF000000;
  }

  samples->ch1 = buffer[4] << 16;
  samples->ch1 |= buffer[5] << 8;
  samples->ch1 |= buffer[6];
  if ( samples->ch1 & 0x800000 ) {
    samples->ch1 |= 0xFF000000;
  }

  samples->ch2 = buffer[7] << 16;
  samples->ch2 |= buffer[8] << 8;
  samples->ch2 |= buffer[9];
  if ( samples->ch2 & 0x800000 ) {
    samples->ch2 |= 0xFF000000;
  }

  samples->ch3 = buffer[10] << 16;
  samples->ch3 |= buffer[11] << 8;
  samples->ch3 |= buffer[12];
  if ( samples->ch3 & 0x800000 ) {
    samples->ch3 |= 0xFF000000;
  }

  return retval;
}

/** @cond DO_NOT_INCLUDE_WITH_DOXYGEN */

/***************************************************************************//**
 * @brief
 *    Helper function to polynomial calculations
 ******************************************************************************/
int32_t SI1133_calcPolyInner(int32_t input, int8_t fraction, uint16_t mag, int8_t shift)
{
  int32_t value;

  if ( shift < 0 ) {
    value = ( (input << fraction) / mag) >> -shift;
  } else {
    value = ( (input << fraction) / mag) << shift;
  }

  return value;
}

/***************************************************************************//**
 * @brief
 *    Polynomial evaluation function to calculate lux and UV index
 *
 * @return
 *    Scaled results
 ******************************************************************************/
int32_t SI1133_calcEvalPoly(int32_t x, int32_t y, uint8_t input_fraction, uint8_t output_fraction, uint8_t num_coeff, SI1133_Coeff_TypeDef *kp)
{
  uint8_t info, x_order, y_order, counter;
  int8_t sign, shift;
  uint16_t mag;
  int32_t output = 0, x1, x2, y1, y2;

  for ( counter = 0; counter < num_coeff; counter++ ) {
    info = kp->info;
    x_order = get_x_order(info);
    y_order = get_y_order(info);

    shift = ( (uint16_t) kp->info & 0xff00) >> 8;
    shift ^= 0x00ff;
    shift += 1;
    shift = -shift;

    mag = kp->mag;

    if ( get_sign(info) ) {
      sign = -1;
    } else {
      sign = 1;
    }

    if ( (x_order == 0) && (y_order == 0) ) {
      output += sign * mag << output_fraction;
    } else {
      if ( x_order > 0 ) {
        x1 = SI1133_calcPolyInner(x, input_fraction, mag, shift);
        if ( x_order > 1 ) {
          x2 = SI1133_calcPolyInner(x, input_fraction, mag, shift);
        } else {
          x2 = 1;
        }
      } else {
        x1 = 1;
        x2 = 1;
      }

      if ( y_order > 0 ) {
        y1 = SI1133_calcPolyInner(y, input_fraction, mag, shift);
        if ( y_order > 1 ) {
          y2 = SI1133_calcPolyInner(y, input_fraction, mag, shift);
        } else {
          y2 = 1;
        }
      } else {
        y1 = 1;
        y2 = 1;
      }

      output += sign * x1 * x2 * y1 * y2;
    }

    kp++;
  }

  if ( output < 0 ) {
    output = -output;
  }

  return output;
}

/** @endcond */

/***************************************************************************//**
 * @brief
 *    Compute UV index
 *
 * @param[in] uv
 *    UV sensor raw data
 *
 * @param[in] uk
 *    UV calculation coefficients
 *
 * @return
 *    UV index scaled by UV_OUPTUT_FRACTION
 ******************************************************************************/
int32_t SI1133_getUv(int32_t uv, SI1133_Coeff_TypeDef *uk)
{
  int32_t uvi;

  uvi = SI1133_calcEvalPoly(0, uv, UV_INPUT_FRACTION, UV_OUTPUT_FRACTION, UV_NUMCOEFF, uk);

  return uvi;
}

/***************************************************************************//**
 * @brief
 *    Compute lux value
 *
 * @param[in] vis_high
 *    Visible light sensor raw data
 *
 * @param[in] vis_low
 *    Visible light sensor raw data
 *
 * @param[in] ir
 *    Infrared sensor raw data
 *
 * @param[in] lk
 *    Lux calculation coefficients
 *
 * @return
 *    Lux value scaled by LUX_OUPTUT_FRACTION
 ******************************************************************************/
int32_t SI1133_getLux(int32_t vis_high, int32_t vis_low, int32_t ir, SI1133_LuxCoeff_TypeDef *lk)
{
  int32_t lux;

  if ( (vis_high > ADC_THRESHOLD) || (ir > ADC_THRESHOLD) ) {
    lux = SI1133_calcEvalPoly(vis_high,
                              ir,
                              INPUT_FRACTION_HIGH,
                              LUX_OUTPUT_FRACTION,
                              NUMCOEFF_HIGH,
                              &(lk->coeff_high[0]) );
  } else {
    lux = SI1133_calcEvalPoly(vis_low,
                              ir,
                              INPUT_FRACTION_LOW,
                              LUX_OUTPUT_FRACTION,
                              NUMCOEFF_LOW,
                              &(lk->coeff_low[0]) );
  }

  return lux;
}

/***************************************************************************//**
 * @brief
 *    Measure lux and UV index using the Si1133 sensor
 *
 * @param[out] lux
 *    The measured ambient light illuminace in lux
 *
 * @param[out] uvi
 *    UV index
 *
 * @return
 *    Returns zero on OK, non-zero otherwise
 ******************************************************************************/
uint32_t SI1133_measureLuxUvi(float *lux, float *uvi)
{
  SI1133_Samples_TypeDef samples;
  uint32_t retval;
  uint8_t response;

  /* Force measurement */
  retval = SI1133_measurementForce();

  /* Go to sleep while the sensor does the conversion */
  UTIL_delay(200);

  /* Check if the measurement finished, if not then wait */
  retval += SI1133_registerRead(SI1133_REG_IRQ_STATUS, &response);
  while ( response != 0x0F ) {
    UTIL_delay(5);
    retval += SI1133_registerRead(SI1133_REG_IRQ_STATUS, &response);
  }

  /* Get the results */
  SI1133_measurementGet(&samples);

  /* Convert the readings to lux */
  *lux = (float) SI1133_getLux(samples.ch1, samples.ch3, samples.ch2, &lk);
  *lux = *lux / (1 << LUX_OUTPUT_FRACTION);

  /* Convert the readings to UV index */
  *uvi = (float) SI1133_getUv(samples.ch0, uk);
  *uvi = *uvi / (1 << UV_OUTPUT_FRACTION);

  return retval;
}

/***************************************************************************//**
 * @brief
 *    Reads Hardware ID from the SI1133 sensor
 *
 * @param[out] hardwareID
 *    The Hardware ID of the chip (should be 0x33)
 *
 * @return
 *    Returns zero on OK, non-zero otherwise
 ******************************************************************************/
uint32_t SI1133_getHardwareID(uint8_t *hardwareID)
{
  uint32_t retval;

  retval = SI1133_registerRead(SI1133_REG_PART_ID, hardwareID);

  return retval;
}

/***************************************************************************//**
 * @brief
 *    Retrieve the sample values from the chip and convert them
 *    to lux and UV index values
 *
 * @param[out] lux
 *    The measured ambient light illuminace in lux
 *
 * @param[out] uvi
 *    UV index
 *
 * @return
 *    Returns zero on OK, non-zero otherwise
 ******************************************************************************/
uint32_t SI1133_getMeasurement(float *lux, float *uvi)
{
  SI1133_Samples_TypeDef samples;
  uint32_t retval;

  /* Get the results */
  retval = SI1133_measurementGet(&samples);

  /* Convert the readings to lux */
  *lux = (float) SI1133_getLux(samples.ch1, samples.ch3, samples.ch2, &lk);
  *lux = *lux / (1 << LUX_OUTPUT_FRACTION);

  /* Convert the readings to UV index */
  *uvi = (float) SI1133_getUv(samples.ch0, uk);
  *uvi = *uvi / (1 << UV_OUTPUT_FRACTION);

  return retval;
}

/***************************************************************************//**
 * @brief
 *    Reads the interrupt status register of the device
 *
 * @param[out] irqStatus
 *    The contentof the IRQ status register
 *
 * @return
 *    Returns zero on OK, non-zero otherwise
 ******************************************************************************/
uint32_t SI1133_getIrqStatus(uint8_t *irqStatus)
{
  uint32_t retval;

  /* Read the IRQ status register */
  retval = SI1133_registerRead(SI1133_REG_IRQ_STATUS, irqStatus);

  return retval;
}

/** @} {end defgroup Si1133} */
/** @} {end addtogroup TBSense_BSP} */
