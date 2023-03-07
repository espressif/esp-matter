/*******************************************************************************
* @file  rsi_spi_mem_rd_wr.c
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
 * @file       rsi_spi_mem_rd_wr.c
 * @version    0.1
 * @date       15 Aug 2015
 *
 *
 *
 * @brief SPI MEMRDWR: SPI Memory Read/Write functions, r/w memory on the module via the SPI interface
 * Description
 * Contains the SPI Memory Read/Write functions to the module
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
/*===========================================================================*/
/**
 * @brief       Perform a memory write to the module.
 * @param[in]   addr  - Address to write to
 * @param[in]   len   - Number of bytes to write
 * @param[in]   dBuf  - Pointer to the buffer of data to write
 * @return      0 - SUCCESS \n
 *              -1 - SPI busy / Timeout \n
 *              -2 - SPI Failure
 *
 *
 */
int16_t rsi_mem_wr(uint32_t addr, uint16_t len, uint8_t *dBuf)
{
  uint8_t txCmd[4];
  int16_t retval;
  uint8_t c1;
  uint8_t c2;
  uint8_t c3;
  uint8_t c4;

  c1 = RSI_C1MEMWR16BIT4BYTE;
#ifdef RSI_BIT_32_SUPPORT
  c2 = RSI_C2SPIADDR4BYTE;
#else
  c2     = RSI_C2SPIADDR1BYTE;
#endif
  // C3, LSB of length
  c3 = (int8_t)(len & 0x00ff);

  // C4, MSB of length
  c4 = (int8_t)((len >> 8) & 0x00ff);

  rsi_uint32_to_4bytes(txCmd, addr);

  // Send C1/C2
  retval = rsi_send_c1c2(c1, c2);

  // Check for SPI Busy/Error
  if (retval != RSI_SUCCESS) {
    return retval;
  }

  // Send C3/C4
  retval = rsi_send_c3c4(c3, c4);
  if (retval != RSI_SUCCESS) {
    return retval;
  }

  // Send the 4 address bytes
  retval = rsi_spi_transfer(txCmd, NULL, sizeof(txCmd), RSI_MODE_8BIT);
  if (retval != RSI_SUCCESS) {
    return retval;
  }

  // Send the Data
#ifdef RSI_BIT_32_SUPPORT
  retval = rsi_spi_transfer(dBuf, NULL, len, RSI_MODE_32BIT);
  if (retval != RSI_SUCCESS) {
    return retval;
  }
#else
  retval = rsi_spi_transfer(dBuf, NULL, len, RSI_MODE_8BIT);
  if (retval != RSI_SUCCESS) {
    return retval;
  }
#endif
  return retval;
}

/*===========================================================================*/
/**
 * @brief       Perform a memory read from the module.
 * @param[in]   addr - Address to read from
 * @param[in]   len  - Number of bytes to read
 * @param[in]   dBuf - Pointer to the buffer to receive the data into
 * @return      0 - SUCCESS \n
 *              -1 - SPI busy / Timeout \n
 *              -2 - SPI Failure
 *
 *
 */
int16_t rsi_mem_rd(uint32_t addr, uint16_t len, uint8_t *dBuf)
{
  uint8_t txCmd[4];
  int16_t retval;
  uint8_t c1;
  uint8_t c2;
  uint8_t c3;
  uint8_t c4;
  uint8_t localBuf[8];

  c1 = RSI_C1MEMRD16BIT4BYTE;
#ifdef RSI_BIT_32_SUPPORT
  c2 = RSI_C2SPIADDR4BYTE;
#else
  c2 = RSI_C2MEMRDWRNOCARE;
#endif
  c3 = len & 0x00ff;        // C3, LSB of length
  c4 = (len >> 8) & 0x00ff; // C4, MSB of length
  // Put the address bytes into the buffer to send
  txCmd[0] = addr & 0x000000ff;         // A0, Byte 0 of address (LSB)
  txCmd[1] = (addr >> 8) & 0x000000ff;  // A1, Byte 1 of address
  txCmd[2] = (addr >> 16) & 0x000000ff; // A2, Byte 2 of address
  txCmd[3] = (addr >> 24) & 0x000000ff; // A3, Byte 3 of address (MSB)
  // Send C1/C2
  retval = rsi_send_c1c2(c1, c2);

  // Check for SPI busy
  if (retval != RSI_SUCCESS) {
    return retval;
  }

  // Send C3/C4
  retval = rsi_send_c3c4(c3, c4);
  if (retval != RSI_SUCCESS) {
    return retval;
  }
  // Send the 4 address bytes
  retval = rsi_spi_transfer(txCmd, localBuf, sizeof(txCmd), RSI_MODE_8BIT);
  if (retval != RSI_SUCCESS) {
    return retval;
  }
  // Wait for the start token
  retval = rsi_spi_wait_start_token(RSI_START_TOKEN_TIMEOUT, RSI_MODE_8BIT);
  if (retval != RSI_SUCCESS) {
    return retval;
  }
  // Read in the memory data
  retval = rsi_spi_transfer(NULL, dBuf, len, RSI_MODE_8BIT);
  if (retval != RSI_SUCCESS) {
    return retval;
  }

  return retval;
}

#endif

/** @} */
