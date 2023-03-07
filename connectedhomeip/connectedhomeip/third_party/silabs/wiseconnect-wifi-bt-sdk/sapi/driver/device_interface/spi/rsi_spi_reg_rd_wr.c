/*******************************************************************************
* @file  rsi_spi_reg_rd_wr.c
* @brief
*******************************************************************************
* # License
* <b>Copyright 2020 Silicon Laboratories Inc. www.silabs.com</b>
*******************************************************************************
*
* The licensor of this software is Silicon Laboratories Inc. Your use of this
* software is governed by the terms of Silicon Labs Master Software License
* Agreement (MSLA) available at
* www.silabs.com/about-us/legal/master-software-license-agreement. This
* software is distributed to you in Source Code format and is governed by the
* sections of the MSLA applicable to Source Code.
*
******************************************************************************/
/**
 * @file    rsi_spi_reg_rd_wr.c
 * @version	0.1
 * @date 		15 Aug 2015
 *
 *
 *
 * @brief SPI REGRDWR: SPI Register Read/Write functions
 * Description
 * Contains the SPI based Register R/W functions
 *
 *
 */

/*
  Includes
 */
#include "rsi_driver.h"

#ifdef RSI_SPI_INTERFACE

/*
  Global Variables
 */
/** @addtogroup DRIVER2
* @{
*/
/**
 * @brief       Read from a register in the module from the address specified.
 * @param[in]   regAddr - Address of spi register to read, addr is 6-bits, upper 2 bits must be cleared
 * @param[in]   dBuf    - Pointer to the buffer of data to write, assumed to be at least 2 bytes long
 * @return      0 - SUCCESS \n
 *              -1 - SPI busy / Timeout \n
 *              -2 - SPI Failure
 *
 */
int16_t rsi_reg_rd(uint8_t regAddr, uint8_t *dBuf)
{
  int16_t retval;
  uint8_t c1;
  uint8_t c2;

  // Set the C1/C2 values
  c1 = RSI_C1INTREAD1BYTES;
  c2 = 0x00;
  // And/or the address into C2, addr is 6-bits long, upper two bits must be cleared for byte mode
  c2 |= regAddr;

  // Send C1C2
  retval = rsi_send_c1c2(c1, c2);

  // Check for SPI Busy/Error
  if (retval != RSI_SUCCESS) {
    // Exit with error if timed out waiting for the SPI to get ready
    return retval;
  }

  // Wait for start token
  retval = rsi_spi_wait_start_token(RSI_START_TOKEN_TIMEOUT, RSI_MODE_8BIT);

  if (retval != RSI_SUCCESS) {
    // Exit with error if timed out waiting for start token
    return retval;
  }

  // Start token found now read the two bytes of data
  retval = rsi_spi_transfer(NULL, dBuf, 1, RSI_MODE_8BIT);

  return retval;
}

/**
 * @brief       Read from a register in the wlan module from the address specified.
 * @param[in]   regAddr - Address of spi register to read, addr is 6-bits, upper 2 bits must be cleared
 * @param[in]   dBuf    - Pointer to the buffer of data to write, assumed to be at least 2 bytes long
 * @return     0 - SUCCESS \n
 *              -1 - SPI busy / Timeout \n
 *              -2 - SPI Failure
 */
int16_t rsi_reg_rd2(uint8_t regAddr, uint16_t *dBuf)
{
  int16_t retval;
  uint8_t c1;
  uint8_t c2;

  // Set the C1/C2 values
  c1 = RSI_C1INTREAD2BYTES;
  c2 = 0x00;

  // And/or the address into C2, addr is 6-bits long, upper two bits must be cleared for byte mode
  c2 |= regAddr;

  // Send C1C2
  retval = rsi_send_c1c2(c1, c2);

  // Check for SPI Busy/Error
  if (retval != 0) {
    // Exit with error if timed out waiting for the SPI to get ready
    return retval;
  }

  // Wait for start token
  retval = rsi_spi_wait_start_token(RSI_START_TOKEN_TIMEOUT, RSI_MODE_8BIT);

  if (retval != 0) {
    // Exit with error if timed out waiting for start token
    return retval;
  }

  // Start token found now read the two bytes of data
  retval = rsi_spi_transfer(NULL, (uint8_t *)dBuf, 2, RSI_MODE_8BIT);

  return retval;
}

/**
 * @brief       Write to a register in the module with an address specified.
 * @param[in]   regAddr - Address of spi register to be written
 * @param[in]   dBuf    - Pointer to the buffer of data to write, assumed to be at least 2 bytes long
 * @return      0 - SUCCESS \n
 *              -1 - SPI busy / Timeout \n
 *              -2 - SPI Failure

 */

int16_t rsi_reg_wr(uint8_t regAddr, uint8_t *dBuf)
{
  int16_t retval;
  uint8_t c1;
  uint8_t c2;
  uint8_t localBuf[8];

  // Set the C1/C2 values
  c1 = RSI_C1INTWRITE2BYTES;
  c2 = 0x00;

  // And/or the address into C2, addr is 6-bits long, upper two bits must be cleared for byte mode
  c2 |= regAddr;

  // Send C1C2
  retval = rsi_send_c1c2(c1, c2);

  // Check for SPI Busy/Error
  if (retval != RSI_SUCCESS) {
    // Exit with error if timed out waiting for the SPI to get ready
    return retval;
  }

  // SPI send
  retval = rsi_spi_transfer(dBuf, localBuf, 2, RSI_MODE_8BIT);

  if (retval != RSI_SUCCESS) {
    // Exit with error if timed out waiting for start token
    return retval;
  }
  return retval;
}

#endif
/** @} */
