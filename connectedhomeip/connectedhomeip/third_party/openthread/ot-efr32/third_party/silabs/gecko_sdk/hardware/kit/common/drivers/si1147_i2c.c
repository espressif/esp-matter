/***************************************************************************//**
 * @file
 * @brief i2c driver for the Si1147
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
#include "si114x_functions.h"
#include "si1147_i2c.h"
#include "sl_sleeptimer.h"

/*******************************************************************************
 **************************   GLOBAL FUNCTIONS   *******************************
 ******************************************************************************/

/**************************************************************************//**
 * @brief
 *  Reads register from the Si1147 sensor.
 * @param[in] i2c
 *   The I2C peripheral to use (not used).
 * @param[in] addr
 *   The I2C address of the sensor.
 * @param[out] data
 *   The data read from the sensor.
 * @param[in] reg
 *   The register address to read from in the sensor.
 * @return
 *   Returns number of bytes read on success. Otherwise returns error codes
 *   based on the I2CDRV.
 *****************************************************************************/
uint32_t Si1147_Read_Register(I2C_TypeDef *i2c, uint8_t addr, uint8_t reg, uint8_t *data)
{
  I2C_TransferSeq_TypeDef    seq;
  I2C_TransferReturn_TypeDef ret;
  uint8_t i2c_write_data[1];

  seq.addr  = addr;
  seq.flags = I2C_FLAG_WRITE_READ;
  /* Select register to start reading from */
  i2c_write_data[0] = reg;
  seq.buf[0].data = i2c_write_data;
  seq.buf[0].len  = 1;
  /* Select length of data to be read */
  seq.buf[1].data = data;
  seq.buf[1].len  = 1;

  ret = I2CSPM_Transfer(i2c, &seq);
  if (ret != i2cTransferDone) {
    *data = 0xff;
    return (uint32_t)ret;
  }
  return (uint32_t)0;
}

/**************************************************************************//**
 * @brief
 *  Writes register in the Si1147 sensor.
 * @param[in] i2c
 *   The I2C peripheral to use (not used).
 * @param[in] addr
 *   The I2C address of the sensor.
 * @param[in] data
 *   The data to write to the sensor.
 * @param[in] reg
 *   The register address to write to in the sensor.
 * @return
 *   Returns zero on success. Otherwise returns error codes
 *   based on the I2CDRV.
 *****************************************************************************/
uint32_t Si1147_Write_Register(I2C_TypeDef *i2c, uint8_t addr, uint8_t reg, uint8_t data)
{
  I2C_TransferSeq_TypeDef    seq;
  I2C_TransferReturn_TypeDef ret;
  uint8_t i2c_write_data[2];
  uint8_t i2c_read_data[1];

  seq.addr  = addr;
  seq.flags = I2C_FLAG_WRITE;
  /* Select register and data to write */
  i2c_write_data[0] = reg;
  i2c_write_data[1] = data;
  seq.buf[0].data = i2c_write_data;
  seq.buf[0].len  = 2;
  seq.buf[1].data = i2c_read_data;
  seq.buf[1].len  = 0;

  ret = I2CSPM_Transfer(i2c, &seq);
  if (ret != i2cTransferDone) {
    return (uint32_t)ret;
  }
  return (uint32_t)0;
}

/**************************************************************************//**
 * @brief
 *  Writes a block of data to the Si1147 sensor.
 * @param[in] i2c
 *   The I2C peripheral to use (not used).
 * @param[in] addr
 *   The I2C address of the sensor.
 * @param[in] data
 *   The data to write to the sensor.
 * @param[in] length
 *   The number of bytes to write to the sensor.
 * @param[in] reg
 *   The first register to begin writing to.
 * @return
 *   Returns zero on success. Otherwise returns error codes
 *   based on the I2CDRV.
 *****************************************************************************/
uint32_t Si1147_Write_Block_Register(I2C_TypeDef *i2c, uint8_t addr, uint8_t reg, uint8_t length, uint8_t const *data)
{
  I2C_TransferSeq_TypeDef    seq;
  I2C_TransferReturn_TypeDef ret;
  uint8_t i2c_write_data[10];
  uint8_t i2c_read_data[1];
  int i;

  seq.addr  = addr;
  seq.flags = I2C_FLAG_WRITE;
  /* Select register to start writing to*/
  i2c_write_data[0] = reg;
  for (i = 0; i < length; i++) {
    i2c_write_data[i + 1] = data[i];
  }
  seq.buf[0].data = i2c_write_data;
  seq.buf[0].len  = 1 + length;
  seq.buf[1].data = i2c_read_data;
  seq.buf[1].len  = 0;

  ret = I2CSPM_Transfer(i2c, &seq);
  if (ret != i2cTransferDone) {
    return (uint32_t)ret;
  }
  return (uint32_t)0;
}

/**************************************************************************//**
 * @brief
 *  Reads a block of data from the Si1147 sensor.
 * @param[in] i2c
 *   The I2C peripheral to use (not used).
 * @param[in] addr
 *   The I2C address of the sensor.
 * @param[out] data
 *   The data read from the sensor.
 * @param[in] length
 *   The number of bytes to write to the sensor.
 * @param[in] reg
 *   The first register to begin reading from.
 * @return
 *   Returns number of bytes read on success. Otherwise returns error codes
 *   based on the I2CDRV.
 *****************************************************************************/
uint32_t Si1147_Read_Block_Register(I2C_TypeDef *i2c, uint8_t addr, uint8_t reg, uint8_t length, uint8_t *data)
{
  I2C_TransferSeq_TypeDef    seq;
  I2C_TransferReturn_TypeDef ret;
  uint8_t i2c_write_data[1];

  seq.addr  = addr;
  seq.flags = I2C_FLAG_WRITE_READ;
  /* Select register to start reading from */
  i2c_write_data[0] = reg;
  seq.buf[0].data = i2c_write_data;
  seq.buf[0].len  = 1;
  /* Select length of data to be read */
  seq.buf[1].data = data;
  seq.buf[1].len  = length;

  ret = I2CSPM_Transfer(i2c, &seq);
  if (ret != i2cTransferDone) {
    return (uint32_t)ret;
  }
  return (uint32_t)0;
}

/**************************************************************************//**
 * @brief  Writes to Si114x Register
 * @param[in] si114x_handle
 *   The programmer toolkit handle.
 * @param[in] address
 *   The register address to write to.
 * @param[in] data
 *   The data to write to the sensor.
 * @return
 *   Returns Error status
 *****************************************************************************/
int16_t Si114xWriteToRegister(HANDLE si114x_handle, uint8_t address, uint8_t data)
{
  return Si1147_Write_Register(((si114x_i2c_t*)si114x_handle)->i2c, ((si114x_i2c_t*)si114x_handle)->addr, address, data);
}

/**************************************************************************//**
 * @brief  Reads from Si114x register
 * @param[in] si114x_handle
 *   The programmer toolkit handle.
 * @param[in] address
 *   The register address to read from.
 * @return
 *   Returns Value read
 *****************************************************************************/
int16_t Si114xReadFromRegister(HANDLE si114x_handle, uint8_t address)
{
  uint8_t data;
  Si1147_Read_Register(((si114x_i2c_t*)si114x_handle)->i2c, ((si114x_i2c_t*)si114x_handle)->addr, address, &data);
  return data;
}

/**************************************************************************//**
 * @brief  Writes block of Si114x registers
 * @param[in] si114x_handle
 *   The programmer toolkit handle.
 * @param[in] address
 *   The register address to write to.
 * @param[in] length
 *   The number of bytes to write.
 * @param[in] values
 *   The data to write to the sensor.
 * @return
 *   Returns Error status
 *****************************************************************************/
int16_t Si114xBlockWrite(HANDLE si114x_handle,
                         uint8_t address, uint8_t length, uint8_t *values)
{
  return Si1147_Write_Block_Register(((si114x_i2c_t*)si114x_handle)->i2c, ((si114x_i2c_t*)si114x_handle)->addr, address, length, values);
}

/**************************************************************************//**
 * @brief  Reads block of Si114x registers
 * @param[in] si114x_handle
 *   The programmer toolkit handle.
 * @param[in] address
 *   The register address to read from.
 * @param[in] length
 *   The number of bytes to read.
 * @param[in] values
 *   The data read from the sensor.
 * @return
 *   Returns Error status
 *****************************************************************************/
int16_t Si114xBlockRead(HANDLE si114x_handle,
                        uint8_t address, uint8_t length, uint8_t *values)
{
  return Si1147_Read_Block_Register(((si114x_i2c_t*)si114x_handle)->i2c, ((si114x_i2c_t*)si114x_handle)->addr, address, length, values);
}

/**************************************************************************//**
 * @brief  Implements 10ms delay
 *****************************************************************************/
void delay_10ms()
{
  // This is needed immediately after a reset command to the Si114x
  // In the PGM_Toolkit, there is sufficient latency, so none is added
  // here. This is a reminder that when porting code, that this must
  // be implemented.
  sl_sleeptimer_delay_millisecond(10);
}

/**************************************************************************//**
 * @brief  Implements 1ms delay
 *****************************************************************************/
void delay_1ms()
{
  // This can be used to save power while polling Si114x status
  // registers by using a sleep delay instead of busy wait.
  sl_sleeptimer_delay_millisecond(1);
}
