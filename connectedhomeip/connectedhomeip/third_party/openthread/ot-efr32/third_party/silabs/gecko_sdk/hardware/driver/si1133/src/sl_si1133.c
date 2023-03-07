/***************************************************************************//**
 * @file
 * @brief Driver for the Si1133 Ambient Light and UV sensor
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
#include "sl_i2cspm.h"
#include "sl_sleeptimer.h"
#include "sl_si1133.h"

/***************************************************************************//**
 * @defgroup Si1133 SI1133 - Ambient Light and UV Index Sensor
 * @{
 * @brief Driver for the Silicon Labs Si1133 Ambient Light and UV sensor
 ******************************************************************************/

/** @cond DO_NOT_INCLUDE_WITH_DOXYGEN */

/***************************************************************************//**
 * @brief
 *    Structure to store the calculation coefficients
 ******************************************************************************/
typedef struct {
  int16_t     info;           /**< Info                              */
  uint16_t    mag;            /**< Magnitude                         */
} sl_si1133_coeff_t;

/***************************************************************************//**
 * @brief
 *    Structure to store the coefficients used for Lux calculation
 ******************************************************************************/
typedef struct {
  sl_si1133_coeff_t   coeff_high[4];   /**< High amplitude coefficients */
  sl_si1133_coeff_t   coeff_low[9];    /**< Low amplitude coefficients  */
} sl_si1133_lux_coeff_t;

/***************************************************************************//**
 * @brief
 *    Coefficients for lux calculation
 ******************************************************************************/
static sl_si1133_lux_coeff_t lk = {
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
static sl_si1133_coeff_t uk[2] = {
  { 1281, 30902 },            /**< coeff[0]        */
  { -638, 46301 }             /**< coeff[1]        */
};

/***************************************************************************//**
 * @brief
 *    SI1133 i2c address
 ******************************************************************************/
#define SI1133_I2C_DEVICE_BUS_ADDRESS   (0x55)

/***************************************************************************//**
 * @brief
 *    Helper functions and values required to calculate lux and UV index
 ******************************************************************************/
#define X_ORDER_MASK            0x0070
#define Y_ORDER_MASK            0x0007
#define SIGN_MASK               0x0080
#define get_x_order(m)          ( (m & X_ORDER_MASK) >> 4)
#define get_y_order(m)          ( (m & Y_ORDER_MASK)      )
#define get_sign(m)             ( (m & SIGN_MASK) >> 7)

#define UV_INPUT_FRACTION       15
#define UV_OUTPUT_FRACTION      12
#define UV_NUMCOEFF             2

#define ADC_THRESHOLD           16000
#define INPUT_FRACTION_HIGH     7
#define INPUT_FRACTION_LOW      15
#define LUX_OUTPUT_FRACTION     12
#define NUMCOEFF_LOW            9
#define NUMCOEFF_HIGH           4

// Local prototypes
static sl_status_t sl_si1133_wait_until_sleep(sl_i2cspm_t *i2cspm);
static sl_status_t sl_si1133_send_command(sl_i2cspm_t *i2cspm, uint8_t command);
static int32_t sl_si1133_compute_uvi(int32_t uv, sl_si1133_coeff_t *uk);
static int32_t sl_si1133_compute_lux(int32_t vis_high, int32_t vis_low, int32_t ir, sl_si1133_lux_coeff_t *lk);
static int32_t sl_si1133_calculate_inner_polynomial(int32_t input, int8_t fraction, uint16_t mag, int8_t  shift);
static int32_t sl_si1133_calculate_evaluation_polynomial(int32_t x, int32_t y, uint8_t input_fraction, uint8_t output_fraction, uint8_t num_coeff, sl_si1133_coeff_t *kp);
/** @endcond */

/**************************************************************************//**
 *    Initializes the Si1133 chip
 *****************************************************************************/
sl_status_t sl_si1133_init(sl_i2cspm_t *i2cspm)
{
  sl_status_t retval;

  /* Do not access the Si1133 earlier than 25 ms from power-up */
  sl_sleeptimer_delay_millisecond(50);

  /* Reset the sensor. The reset function implements the necessary delays after reset. */
  retval = sl_si1133_reset(i2cspm);

  retval += sl_si1133_set_parameter(i2cspm, SI1133_PARAM_CH_LIST, 0x0f);
  retval += sl_si1133_set_parameter(i2cspm, SI1133_PARAM_ADCCONFIG0, 0x78);
  retval += sl_si1133_set_parameter(i2cspm, SI1133_PARAM_ADCSENS0, 0x71);
  retval += sl_si1133_set_parameter(i2cspm, SI1133_PARAM_ADCPOST0, 0x40);
  retval += sl_si1133_set_parameter(i2cspm, SI1133_PARAM_ADCCONFIG1, 0x4d);
  retval += sl_si1133_set_parameter(i2cspm, SI1133_PARAM_ADCSENS1, 0xe1);
  retval += sl_si1133_set_parameter(i2cspm, SI1133_PARAM_ADCPOST1, 0x40);
  retval += sl_si1133_set_parameter(i2cspm, SI1133_PARAM_ADCCONFIG2, 0x41);
  retval += sl_si1133_set_parameter(i2cspm, SI1133_PARAM_ADCSENS2, 0xe1);
  retval += sl_si1133_set_parameter(i2cspm, SI1133_PARAM_ADCPOST2, 0x50);
  retval += sl_si1133_set_parameter(i2cspm, SI1133_PARAM_ADCCONFIG3, 0x4d);
  retval += sl_si1133_set_parameter(i2cspm, SI1133_PARAM_ADCSENS3, 0x87);
  retval += sl_si1133_set_parameter(i2cspm, SI1133_PARAM_ADCPOST3, 0x40);

  retval += sl_si1133_write_register(i2cspm, SI1133_REG_IRQ_ENABLE, 0x0f);

  if (retval != SL_STATUS_OK) {
    retval = SL_STATUS_INITIALIZATION;
  }

  return retval;
}

/***************************************************************************//**
 *    Stops the measurements on all channel and waits until the chip
 *    goes to sleep state.
 ******************************************************************************/
sl_status_t sl_si1133_deinit(sl_i2cspm_t *i2cspm)
{
  sl_status_t retval;

  retval = sl_si1133_set_parameter(i2cspm, SI1133_PARAM_CH_LIST, 0x3f);
  if (retval != SL_STATUS_OK) {
    return retval;
  }
  retval = sl_si1133_pause_measurement(i2cspm);
  if (retval != SL_STATUS_OK) {
    return retval;
  }
  retval = sl_si1133_wait_until_sleep(i2cspm);
  return retval;
}

/***************************************************************************//**
 *    Measure lux and UV index using the Si1133 sensor
 ******************************************************************************/
sl_status_t sl_si1133_measure_lux_uvi(sl_i2cspm_t *i2cspm, float *lux, float *uvi)
{
  sl_si1133_samples_t samples;
  sl_status_t retval;
  uint8_t response;

  /* Force measurement */
  retval = sl_si1133_force_measurement(i2cspm);

  /* Wait while the sensor does the conversion */
  sl_sleeptimer_delay_millisecond(200);

  /* Check if the measurement finished, if not then wait */
  retval += sl_si1133_read_register(i2cspm, SI1133_REG_IRQ_STATUS, &response);
  while ( response != 0x0F ) {
    sl_sleeptimer_delay_millisecond(5);
    retval += sl_si1133_read_register(i2cspm, SI1133_REG_IRQ_STATUS, &response);
  }

  /* Get the results */
  sl_si1133_read_samples(i2cspm, &samples);

  /* Convert the readings to lux */
  *lux = (float) sl_si1133_compute_lux(samples.ch1, samples.ch3, samples.ch2, &lk);
  *lux = *lux / (1 << LUX_OUTPUT_FRACTION);

  /* Convert the readings to UV index */
  *uvi = (float) sl_si1133_compute_uvi(samples.ch0, uk);
  *uvi = *uvi / (1 << UV_OUTPUT_FRACTION);

  return retval;
}

/***************************************************************************//**
 *    Reads register from the Si1133 sensor
 ******************************************************************************/
sl_status_t sl_si1133_read_register(sl_i2cspm_t *i2cspm, uint8_t reg, uint8_t *data)
{
  I2C_TransferSeq_TypeDef seq;
  I2C_TransferReturn_TypeDef ret;
  uint8_t i2c_write_data[1];
  sl_status_t retval;

  retval = SL_STATUS_OK;

  seq.addr = SI1133_I2C_DEVICE_BUS_ADDRESS << 1;
  seq.flags = I2C_FLAG_WRITE_READ;
  /* Select register to start reading from */
  i2c_write_data[0] = reg;
  seq.buf[0].data = i2c_write_data;
  seq.buf[0].len = 1;
  /* Select length of data to be read */
  seq.buf[1].data = data;
  seq.buf[1].len = 1;

  ret = I2CSPM_Transfer(i2cspm, &seq);
  if ( ret != i2cTransferDone ) {
    *data = 0xff;
    retval = SL_STATUS_TRANSMIT;
  }

  return retval;
}

/***************************************************************************//**
 *    Writes register in the Si1133 sensor
 ******************************************************************************/
sl_status_t sl_si1133_write_register(sl_i2cspm_t *i2cspm, uint8_t reg, uint8_t data)
{
  I2C_TransferSeq_TypeDef seq;
  I2C_TransferReturn_TypeDef ret;
  uint8_t i2c_write_data[2];
  uint8_t i2c_read_data[1];
  sl_status_t retval;

  retval = SL_STATUS_OK;

  seq.addr = SI1133_I2C_DEVICE_BUS_ADDRESS << 1;
  seq.flags = I2C_FLAG_WRITE;
  /* Select register and data to write */
  i2c_write_data[0] = reg;
  i2c_write_data[1] = data;
  seq.buf[0].data = i2c_write_data;
  seq.buf[0].len = 2;
  seq.buf[1].data = i2c_read_data;
  seq.buf[1].len = 0;

  ret = I2CSPM_Transfer(i2cspm, &seq);
  if ( ret != i2cTransferDone ) {
    retval = SL_STATUS_TRANSMIT;
  }

  return retval;
}

/***************************************************************************//**
 *    Writes a block of data to the Si1133 sensor.
 ******************************************************************************/
sl_status_t sl_si1133_write_register_block(sl_i2cspm_t *i2cspm, uint8_t reg, uint8_t length, const uint8_t *data)
{
  I2C_TransferSeq_TypeDef seq;
  I2C_TransferReturn_TypeDef ret;
  uint8_t i2c_write_data[10];
  uint8_t i2c_read_data[1];
  uint8_t i;
  sl_status_t retval;

  retval = SL_STATUS_OK;

  seq.addr = SI1133_I2C_DEVICE_BUS_ADDRESS << 1;
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

  ret = I2CSPM_Transfer(i2cspm, &seq);
  if ( ret != i2cTransferDone ) {
    retval = SL_STATUS_TRANSMIT;
  }

  return retval;
}

/***************************************************************************//**
 *    Reads a block of data from the Si1133 sensor.
 ******************************************************************************/
sl_status_t sl_si1133_read_register_block(sl_i2cspm_t *i2cspm, uint8_t reg, uint8_t length, uint8_t *data)
{
  I2C_TransferSeq_TypeDef seq;
  I2C_TransferReturn_TypeDef ret;
  uint8_t i2c_write_data[1];
  sl_status_t retval;

  retval = SL_STATUS_OK;

  seq.addr = SI1133_I2C_DEVICE_BUS_ADDRESS << 1;
  seq.flags = I2C_FLAG_WRITE_READ;
  /* Select register to start reading from */
  i2c_write_data[0] = reg;
  seq.buf[0].data = i2c_write_data;
  seq.buf[0].len = 1;
  /* Select length of data to be read */
  seq.buf[1].data = data;
  seq.buf[1].len = length;

  ret = I2CSPM_Transfer(i2cspm, &seq);
  if ( ret != i2cTransferDone ) {
    retval = SL_STATUS_TRANSMIT;
  }

  return retval;
}

/***************************************************************************//**
 * @brief
 *    Waits until the Si1133 is sleeping before proceeding
 *
 * @return
 *    @retval SL_STATUS_OK Success
 *    @retval SL_STATUS_TRANSMIT I2C transmit failure
 ******************************************************************************/
sl_status_t sl_si1133_wait_until_sleep(sl_i2cspm_t *i2cspm)
{
  uint32_t ret;
  uint8_t response;
  uint8_t count = 0;
  sl_status_t retval;

  retval = SL_STATUS_OK;

  /* This loops until the Si1133 is known to be in its sleep state  */
  /* or if an i2c error occurs                                      */
  while ( count < 5 ) {
    ret = sl_si1133_read_register(i2cspm, SI1133_REG_RESPONSE0, &response);
    if ( (response & SI1133_RSP0_CHIPSTAT_MASK) == SI1133_RSP0_SLEEP ) {
      break;
    }

    if ( ret != SL_STATUS_OK ) {
      return ret;
    }

    count++;
  }

  return retval;
}

/***************************************************************************//**
 *    Resets the Si1133
 ******************************************************************************/
sl_status_t sl_si1133_reset(sl_i2cspm_t *i2cspm)
{
  sl_status_t retval;

  /* Perform the Reset Command */
  retval = sl_si1133_write_register(i2cspm, SI1133_REG_COMMAND, SI1133_CMD_RESET);

  /* Allow a minimum of 25ms for Si1133 sensor to perform initial       */
  /* startup sequence                                                   */
  sl_sleeptimer_delay_millisecond(30);

  return retval;
}

/***************************************************************************//**
 *    Helper function to send a command to the Si1133
 ******************************************************************************/
sl_status_t sl_si1133_send_command(sl_i2cspm_t *i2cspm, uint8_t command)
{
  uint8_t response;
  uint8_t response_stored;
  uint8_t count = 0;
  uint32_t ret;

  /* Get the response register contents */
  ret = sl_si1133_read_register(i2cspm, SI1133_REG_RESPONSE0, &response_stored);
  if ( ret != SL_STATUS_OK ) {
    return ret;
  }

  response_stored = response_stored & SI1133_RSP0_COUNTER_MASK;

  /* Double-check the response register is consistent */
  while ( count < 5 ) {
    ret = sl_si1133_wait_until_sleep(i2cspm);
    if ( ret != SL_STATUS_OK ) {
      return ret;
    }
    /* Skip if the command is RESET COMMAND COUNTER */
    if ( command == SI1133_CMD_RESET_CMD_CTR ) {
      break;
    }

    ret = sl_si1133_read_register(i2cspm, SI1133_REG_RESPONSE0, &response);

    if ( (response & SI1133_RSP0_COUNTER_MASK) == response_stored ) {
      break;
    } else {
      if ( ret != SL_STATUS_OK ) {
        return ret;
      } else {
        response_stored = response & SI1133_RSP0_COUNTER_MASK;
      }
    }

    count++;
  }

  /* Send the command */
  ret = sl_si1133_write_register(i2cspm, SI1133_REG_COMMAND, command);
  if ( ret != SL_STATUS_OK ) {
    return ret;
  }

  count = 0;
  /* Expect a change in the response register */
  while ( count < 5 ) {
    /* Skip if the command is RESET COMMAND COUNTER */
    if ( command == SI1133_CMD_RESET_CMD_CTR ) {
      break;
    }

    ret = sl_si1133_read_register(i2cspm, SI1133_REG_RESPONSE0, &response);
    if ( (response & SI1133_RSP0_COUNTER_MASK) != response_stored ) {
      break;
    } else {
      if ( ret != SL_STATUS_OK ) {
        return ret;
      }
    }

    count++;
  }

  return SL_STATUS_OK;
}

/***************************************************************************//**
 *    Sends a RESET COMMAND COUNTER command to the Si1133
 ******************************************************************************/
sl_status_t sl_si1133_reset_command_counter(sl_i2cspm_t *i2cspm)
{
  return sl_si1133_send_command(i2cspm, SI1133_CMD_RESET_CMD_CTR);
}

/*************************************************************************//**
 *    Sends a FORCE command to the Si1133
 ******************************************************************************/
sl_status_t sl_si1133_force_measurement(sl_i2cspm_t *i2cspm)
{
  return sl_si1133_send_command(i2cspm, SI1133_CMD_FORCE_CH);
}

/***************************************************************************//**
 *    Sends a START command to the Si1133
 ******************************************************************************/
sl_status_t sl_si1133_start_measurement(sl_i2cspm_t *i2cspm)
{
  return sl_si1133_send_command(i2cspm, SI1133_CMD_START);
}

/***************************************************************************//**
 *    Reads a parameter from the Si1133
 ******************************************************************************/
sl_status_t sl_si1133_read_parameter(sl_i2cspm_t *i2cspm, uint8_t address)
{
  uint8_t retval;
  uint8_t cmd;

  cmd = 0x40 + (address & 0x3F);

  retval = sl_si1133_send_command(i2cspm, cmd);
  if ( retval != SL_STATUS_OK ) {
    return retval;
  }

  sl_si1133_read_register(i2cspm, SI1133_REG_RESPONSE1, &retval);

  return retval;
}

/***************************************************************************//**
 *    Writes a byte to an Si1133 Parameter
 ******************************************************************************/
sl_status_t sl_si1133_set_parameter(sl_i2cspm_t *i2cspm, uint8_t address, uint8_t value)
{
  sl_status_t retval;
  uint8_t buffer[2];
  uint8_t response_stored;
  uint8_t response;
  uint8_t count;

  retval = sl_si1133_wait_until_sleep(i2cspm);
  if ( retval != SL_STATUS_OK ) {
    return retval;
  }

  sl_si1133_read_register(i2cspm, SI1133_REG_RESPONSE0, &response_stored);
  response_stored &= SI1133_RSP0_COUNTER_MASK;

  buffer[0] = value;
  buffer[1] = 0x80 + (address & 0x3F);

  retval = sl_si1133_write_register_block(i2cspm, SI1133_REG_HOSTIN0, 2, (uint8_t*) buffer);
  if ( retval != SL_STATUS_OK ) {
    return retval;
  }

  /* Wait for command to finish */
  count = 0;
  /* Expect a change in the response register */
  while ( count < 5 ) {
    retval = sl_si1133_read_register(i2cspm, SI1133_REG_RESPONSE0, &response);
    if ( (response & SI1133_RSP0_COUNTER_MASK) != response_stored ) {
      break;
    } else {
      if ( retval != SL_STATUS_OK ) {
        return retval;
      }
    }

    count++;
  }

  return SL_STATUS_OK;
}

/***************************************************************************//**
 *    Sends a PAUSE command to the Si1133
 ******************************************************************************/
sl_status_t sl_si1133_pause_measurement(sl_i2cspm_t *i2cspm)
{
  return sl_si1133_send_command(i2cspm, SI1133_CMD_PAUSE_CH);
}

/***************************************************************************//**
 *    Read samples from the Si1133 chip
 ******************************************************************************/
sl_status_t sl_si1133_read_samples(sl_i2cspm_t *i2cspm, sl_si1133_samples_t *samples)
{
  uint8_t buffer[13];
  sl_status_t retval;

  retval = sl_si1133_read_register_block(i2cspm, SI1133_REG_IRQ_STATUS, 13, buffer);

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

/***************************************************************************//**
 *    Reads Hardware ID from the SI1133 sensor
 ******************************************************************************/
sl_status_t sl_si1133_get_hardware_id(sl_i2cspm_t *i2cspm, uint8_t *hardwareID)
{
  sl_status_t retval;

  retval = sl_si1133_read_register(i2cspm, SI1133_REG_PART_ID, hardwareID);

  return retval;
}

/***************************************************************************//**
 *    Retrieve the sample values from the chip and convert them
 *    to lux and UV index values
 ******************************************************************************/
sl_status_t sl_si1133_get_measurement(sl_i2cspm_t *i2cspm, float *lux, float *uvi)
{
  sl_si1133_samples_t samples;
  sl_status_t retval;

  /* Get the results */
  retval = sl_si1133_read_samples(i2cspm, &samples);

  /* Convert the readings to lux */
  *lux = (float) sl_si1133_compute_lux(samples.ch1, samples.ch3, samples.ch2, &lk);
  *lux = *lux / (1 << LUX_OUTPUT_FRACTION);

  /* Convert the readings to UV index */
  *uvi = (float) sl_si1133_compute_uvi(samples.ch0, uk);
  *uvi = *uvi / (1 << UV_OUTPUT_FRACTION);

  return retval;
}

/***************************************************************************//**
 *    Reads the interrupt status register of the device
 ******************************************************************************/
sl_status_t sl_si1133_get_irq_status(sl_i2cspm_t *i2cspm, uint8_t *irqStatus)
{
  sl_status_t retval;

  /* Read the IRQ status register */
  retval = sl_si1133_read_register(i2cspm, SI1133_REG_IRQ_STATUS, irqStatus);

  return retval;
}

/** @cond DO_NOT_INCLUDE_WITH_DOXYGEN */

/***************************************************************************//**
 * @brief
 *    Helper function to polynomial calculations
 * @return
 *    The calculated innter polynomial
 ******************************************************************************/
static int32_t sl_si1133_calculate_inner_polynomial(int32_t input, int8_t fraction, uint16_t mag, int8_t shift)
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
static int32_t sl_si1133_calculate_evaluation_polynomial(int32_t x, int32_t y, uint8_t input_fraction, uint8_t output_fraction, uint8_t num_coeff, sl_si1133_coeff_t *kp)
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
        x1 = sl_si1133_calculate_inner_polynomial(x, input_fraction, mag, shift);
        if ( x_order > 1 ) {
          x2 = sl_si1133_calculate_inner_polynomial(x, input_fraction, mag, shift);
        } else {
          x2 = 1;
        }
      } else {
        x1 = 1;
        x2 = 1;
      }

      if ( y_order > 0 ) {
        y1 = sl_si1133_calculate_inner_polynomial(y, input_fraction, mag, shift);
        if ( y_order > 1 ) {
          y2 = sl_si1133_calculate_inner_polynomial(y, input_fraction, mag, shift);
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
static int32_t sl_si1133_compute_uvi(int32_t uv, sl_si1133_coeff_t *uk)
{
  int32_t uvi;

  uvi = sl_si1133_calculate_evaluation_polynomial(0, uv, UV_INPUT_FRACTION, UV_OUTPUT_FRACTION, UV_NUMCOEFF, uk);

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
static int32_t sl_si1133_compute_lux(int32_t vis_high, int32_t vis_low, int32_t ir, sl_si1133_lux_coeff_t *lk)
{
  int32_t lux;

  if ( (vis_high > ADC_THRESHOLD) || (ir > ADC_THRESHOLD) ) {
    lux = sl_si1133_calculate_evaluation_polynomial(vis_high,
                                                    ir,
                                                    INPUT_FRACTION_HIGH,
                                                    LUX_OUTPUT_FRACTION,
                                                    NUMCOEFF_HIGH,
                                                    &(lk->coeff_high[0]) );
  } else {
    lux = sl_si1133_calculate_evaluation_polynomial(vis_low,
                                                    ir,
                                                    INPUT_FRACTION_LOW,
                                                    LUX_OUTPUT_FRACTION,
                                                    NUMCOEFF_LOW,
                                                    &(lk->coeff_low[0]) );
  }

  return lux;
}
/** @endcond */
/** @} {end defgroup Si1133} */
