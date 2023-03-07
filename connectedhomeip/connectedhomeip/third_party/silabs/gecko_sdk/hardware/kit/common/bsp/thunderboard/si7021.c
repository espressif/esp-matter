/***************************************************************************//**
 * @file
 * @brief Driver for the Si7021 I2C Humidity and Temperature Sensor
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

#include "i2cspm.h"

#include "thunderboard/util.h"
#include "thunderboard/board.h"
#include "thunderboard/si7021.h"

/**************************************************************************//**
* @addtogroup TBSense_BSP
* @{
******************************************************************************/

/***************************************************************************//**
 * @defgroup Si7021 SI7021 - Relative Humidity and Temperature Sensor
 * @{
 * @brief Driver for the Silicon Labs Si7021 I2C Humidity and Temperature Sensor
 ******************************************************************************/

/***************************************************************************//**
 * @brief
 *    Initializes the Si7021 chip
 *
 * @return
 *    Returns zero on OK, non-zero otherwise
 ******************************************************************************/
uint32_t SI7021_init(void)
{
  uint32_t status;
  uint8_t cmdReadId2[2] = SI7021_CMD_READ_ID_BYTE2;
  uint8_t deviceId[8];

  /* Enable the sensor and wait for it to become ready */
  BOARD_rhtempEnable(true);
  UTIL_delay(80);

  /* Read and compare device ID */
  status = SI7021_cmdRead(cmdReadId2, 2, deviceId, 8);
  if ( status != SI7021_OK ) {
    return status;
  }

  if ( deviceId[0] != SI7021_DEVICE_ID ) {
    return SI7021_ERROR_INVALID_DEVICE_ID;
  }

  return SI7021_OK;
}

/***************************************************************************//**
 * @brief
 *    De-initializes the Si7021 chip. Disables the sensor power domain, this
 *    also disables other sensors.
 *
 * @return
 *    None
 ******************************************************************************/
void SI7021_deInit(void)
{
  BOARD_rhtempEnable(false);
  return;
}

/***************************************************************************//**
 * @brief
 *    Performs relative humidity and temperature measurements
 *
 * @param[out] rhData
 *    The measured relative humidity value in milli-percent units
 *
 * @param[out] tData
 *    The measured temperature value in milli-degree Celsius
 *
 * @return
 *    Returns zero on OK, non-zero otherwise
 ******************************************************************************/
uint32_t SI7021_measure(uint32_t *rhData, int32_t *tData)
{
  uint32_t status;
  uint8_t cmd;
  uint8_t readData[2];
  uint32_t timeout;

  /* Start no-hold measurement */
  cmd = SI7021_CMD_MEASURE_RH_NO_HOLD;
  status = SI7021_cmdWrite(&cmd, 1, NULL, 0);
  if ( status != SI7021_OK ) {
    return status;
  }

  /* Wait for data to become ready */
  timeout = 10;
  while ( timeout-- ) {
    status = SI7021_cmdRead(NULL, 0, readData, 2);

    if ( status == SI7021_OK ) {
      /* Data is ready */
      *rhData = ( (uint32_t) readData[0] << 8) + (readData[1] & 0xfc);
      *rhData = ( ( (*rhData) * 15625L) >> 13) - 6000;
      break;
    } else if ( status == SI7021_ERROR_I2C_TRANSFER_NACK ) {
      /* Data not ready, sleep and try again */
      UTIL_delay(10);
    } else {
      /* Something else went wrong! */
      return status;
    }
  }

  if ( timeout == 0 ) {
    return SI7021_ERROR_TIMEOUT;
  }

  cmd = SI7021_CMD_READ_TEMP;
  status = SI7021_cmdRead(&cmd, 1, readData, 2);
  if ( status != SI7021_OK ) {
    return status;
  }

  *tData = ( (uint32_t) readData[0] << 8) + (readData[1] & 0xfc);
  *tData = ( ( (*tData) * 21965L) >> 13) - 46850;

  return SI7021_OK;
}

/***************************************************************************//**
 * @brief
 *    Reads the firmware revision of the Si7021 chip
 *
 * @param[out] fwRev
 *    The firmware revision of the chip
 *
 * @return
 *    Returns zero on OK, non-zero otherwise
 ******************************************************************************/
uint32_t SI7021_getFwRev(uint8_t *fwRev)
{
  uint32_t status;
  uint8_t cmdReadFwRev[2] = SI7021_CMD_READ_FW_REV;

  status = SI7021_cmdRead(cmdReadFwRev, 2, fwRev, 1);

  return status;
}

/***************************************************************************//**
 * @brief
 *    Sends a command and reads the result byte(s) over the I2C bus
 *
 * @param[in] cmd
 *    The command to be sent
 *
 * @param[in] cmdLen
 *    The length of the command in bytes
 *
 * @param[out] result
 *    The received bytes sent by the chip
 *
 * @param[in] resultLen
 *    The number of the bytes to read from the chip
 *
 * @return
 *    Returns zero on OK, non-zero otherwise
 ******************************************************************************/
uint32_t SI7021_cmdRead(uint8_t *cmd, size_t cmdLen, uint8_t *result, size_t resultLen)
{
  I2C_TransferSeq_TypeDef seq;
  I2C_TransferReturn_TypeDef ret;

  seq.addr = SI7021_I2C_BUS_ADDRESS << 1;

  if ( cmdLen > 0 ) {
    seq.flags = I2C_FLAG_WRITE_READ;

    seq.buf[0].data = cmd;
    seq.buf[0].len  = cmdLen;
    seq.buf[1].data = result;
    seq.buf[1].len  = resultLen;
  } else {
    seq.flags = I2C_FLAG_READ;

    seq.buf[0].data = result;
    seq.buf[0].len  = resultLen;
  }

  BOARD_i2cBusSelect(BOARD_I2C_BUS_SELECT_ENV_SENSOR);

  ret = I2CSPM_Transfer(SI7021_I2C_DEVICE, &seq);

  if ( ret == i2cTransferNack ) {
    return SI7021_ERROR_I2C_TRANSFER_NACK;
  } else if ( ret != i2cTransferDone ) {
    return SI7021_ERROR_I2C_TRANSFER_FAILED;
  }

  return SI7021_OK;
}

/***************************************************************************//**
 * @brief
 *    Sends a command and data to the chip over the I2C bus
 *
 * @param[in] cmd
 *    The command to be sent
 *
 * @param[in] cmdLen
 *    The length of the command in bytes
 *
 * @param[out] data
 *    The data byte(s) to be sent to the chip
 *
 * @param[in] dataLen
 *    The number of the bytes to be sent to the chip
 *
 * @return
 *    Returns zero on OK, non-zero otherwise
 ******************************************************************************/
uint32_t SI7021_cmdWrite(uint8_t *cmd, size_t cmdLen, uint8_t *data, size_t dataLen)
{
  I2C_TransferSeq_TypeDef seq;
  I2C_TransferReturn_TypeDef ret;

  seq.addr = SI7021_I2C_BUS_ADDRESS << 1;
  seq.buf[0].data = cmd;
  seq.buf[0].len = cmdLen;

  if ( dataLen > 0 ) {
    seq.flags = I2C_FLAG_WRITE_WRITE;
    seq.buf[1].data = data;
    seq.buf[1].len = dataLen;
  } else {
    seq.flags = I2C_FLAG_WRITE;
  }

  BOARD_i2cBusSelect(BOARD_I2C_BUS_SELECT_ENV_SENSOR);

  ret = I2CSPM_Transfer(SI7021_I2C_DEVICE, &seq);

  if ( ret == i2cTransferNack ) {
    return SI7021_ERROR_I2C_TRANSFER_NACK;
  } else if ( ret != i2cTransferDone ) {
    return SI7021_ERROR_I2C_TRANSFER_FAILED;
  }

  return SI7021_OK;
}

/** @} */
/** @} */
