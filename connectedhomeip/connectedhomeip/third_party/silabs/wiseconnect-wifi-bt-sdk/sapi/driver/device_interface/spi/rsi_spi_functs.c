/*******************************************************************************
* @file  rsi_spi_functs.c
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
 * @file     rsi_spi_functs.c
 * @version  0.1
 * @date     15 Aug 2015
 *
 *
 *
 * @brief Common SPI Functions
 *
 * Description
 * SPI bus Layer function used to transfer spi protocol level commands to module.
 * For more details and for spi commands, refer to the Programming Reference Manual.
 *
 */

/*
 * Includes
 */
#include "rsi_driver.h"

#ifdef RSI_SPI_INTERFACE

/*
  Global Variables
 */
/** @addtogroup DRIVER2
* @{
*/
/*==================================================*/
/**
 * @brief       Send the C1 & C2 commands bytes, should check response for C1 command, if busy, should retry.
 * @param[in]   c1 - SPI c1 command
 * @param[in]   c2 - SPI c2 command
 * @return      0 - SUCCESS \n
 *              -1 - SPI busy / Timeout \n
 *              -2 - SPI Failure
 */
int16_t rsi_send_c1c2(uint8_t c1, uint8_t c2)
{
  int16_t retval;
  uint32_t timeout;
  uint8_t txCmd[2];
  uint8_t localBuf[8];

  timeout = 1 * RSI_TICKS_PER_SECOND;

  // Reset the timeout timer to 0
  RSI_RESET_TIMER1;

  while (1) {
    txCmd[0] = c1;
    txCmd[1] = c2;
    if (RSI_INC_TIMER_1 > timeout) {
      retval = RSI_ERROR_SPI_TIMEOUT;
      break;
    }

    // Send C1 & C2
    retval = rsi_spi_transfer(&txCmd[0], localBuf, 2, RSI_MODE_8BIT);
    if ((localBuf[1] == RSI_SPI_SUCCESS) || (localBuf[1] == 0x00)) {
      // Success, return now
      retval = RSI_SUCCESS;
      break;
    } else if (localBuf[1] == RSI_SPI_FAIL) {
      retval = RSI_ERROR_SPI_FAIL;
      break;
    } else if (localBuf[1] == RSI_SPI_BUSY) {
      // Busy, retry once again
      retval = RSI_ERROR_SPI_BUSY;
    }
  }

  return retval;
}

/*==================================================*/
/**
 * @brief       The C3 & C4 command bytes to send.
 * @pre         A succesful rsi_send_c1c2 is required before this function.
 * @param[in]   c3 - SPI c3 command bytes to be sent
 * @param[in]   c4 - SPI c4 command bytes to be sent
 * @return      0 - SUCCESS \n
 *              -1 - SPI busy / Timeout \n
 *              -2 - SPI Failure
 */
int16_t rsi_send_c3c4(uint8_t c3, uint8_t c4)
{
  int16_t retval;
  // Command buffer
  uint8_t txCmd[2];

  txCmd[0] = c3;
  txCmd[1] = c4;

  // Command should only send 8-bit mode
  retval = rsi_spi_transfer(txCmd, NULL, 2, RSI_MODE_8BIT);
  if (retval != RSI_SUCCESS) {
    return retval;
  }

  return retval;
}

/*==================================================*/
/**
 * @brief         Loop reading the SPI until a start token, 0x55, is received.
 * @pre           Should issue read commands before using this function
 * @param[in]     timeout -  Timeout for start token.
 * @param[in]     mode    -  To indicate 8-bit/32-bit mode.
 * @return        0 - SUCCESS \n
 *               -1 - SPI busy / Timeout \n
 *               -2 - SPI Failure
 */
int16_t rsi_spi_wait_start_token(uint32_t timeout, uint8_t mode)
{
  int16_t retval;
#ifdef RSI_BIT_32_SUPPORT
  // Char to send/receive data in
  uint32_t txChar;
#else
  uint8_t txChar;
#endif
  // Look for start token
  // Send a character, could be any character, and check the response for a start token
  // If not found within the timeout time, error out
  // Timeout value needs to be passed since context is important
  // Reset the timeout timer to 0;
  RSI_RESET_TIMER1;

  while (1) {
    if (RSI_INC_TIMER_1 > timeout) {
      retval = RSI_ERROR_SPI_BUSY;
      // Timeout
      break;
    }

    txChar = 0x00;
#ifdef RSI_BIT_32_SUPPORT
    if (mode == RSI_MODE_8BIT)
#endif
      retval = rsi_spi_transfer(NULL, &txChar, 1, mode);
#ifdef RSI_BIT_32_SUPPORT
    else
      retval = rsi_spi_transfer(NULL, (uint8_t *)&txChar, 4, mode);
#endif
    if (txChar == RSI_SPI_START_TOKEN) {
      // Found the start token
      retval = RSI_SUCCESS;
      break;
    }
  }
  return retval;
}

/*==================================================*/
/**
 * @brief       Set the INTERRUPT MASK REGISTER of the module.
 * @param[in]   interruptMask - The value to set the mask register to
 * @return      0 - SUCCESS \n
 *              -1 - SPI busy / Timeout \n
 *              -2 - SPI Failure
*/
int16_t rsi_set_intr_mask(uint8_t interruptMask)
{
  int16_t retval;
  int16_t i;
#ifdef RSI_BIT_32_SUPPORT
  uint8_t dBuf[4] = { 0, 0, 0, 0 };
#else
  uint8_t dBuf[2] = { 0, 0 };
#endif

  // Create the data to write to the interrupt mask register
  for (i = 0; (uint16_t)i < sizeof(dBuf); i++) {
    dBuf[i] = 0;
  }

  dBuf[0] = interruptMask;

  // Memory write the mask value
  retval = rsi_mem_wr(RSI_INT_MASK_REG_ADDR, sizeof(dBuf), dBuf);
  if (retval != RSI_SUCCESS) {
    // Exit with error if timed out waiting for the SPI to get ready
    return retval;
  }

  return retval;
}

/*==================================================*/
/**
 * @brief       Set the INTERRUPT TYPE of the module based on selected LOAD_IMAGE.
 * @param[in]   interruptMaskVal - The value to set the mask register
 * @return      0 - SUCCESS \n
 *              -1 - SPI busy / Timeout \n
 *              -2 - SPI Failure
*/

int16_t rsi_set_intr_type(uint32_t interruptMaskVal)
{
  int16_t retval;
#ifdef RSI_BIT_32_SUPPORT
  uint8_t dBuf[4] = { 0, 0, 0, 0 };
#else
  uint8_t dBuf[2] = { 0, 0 };
#endif

  retval = rsi_mem_rd(RSI_INT_MASK_REG_ADDR, sizeof(dBuf), dBuf);

  if (!retval) {
    dBuf[1] = dBuf[1] & 0xFC;
    dBuf[1] = dBuf[1] | interruptMaskVal;
    // Memory write the mask value
    retval = rsi_mem_wr(RSI_INT_MASK_REG_ADDR, sizeof(dBuf), dBuf);
    if (retval != RSI_SUCCESS) {
      // exit with error if we timed out waiting for the SPI to get ready
      return retval;
    }
  }

  return retval;
}

/*==================================================*/
/**
 * @brief       Clear the interrupt register.
 * @param[in]   interrutClear - The value to set the interrupt clear register to
 * @return      0 - SUCCESS \n
 *              -1 - SPI busy / Timeout \n
 *              -2 - SPI Failure
 */
int16_t rsi_clear_interrupt(uint8_t interruptClear)
{
  int16_t retval;
#ifdef RSI_BIT_32_SUPPORT
  uint8_t dBuf[4] = { 0, 0, 0, 0 };
#else
  uint8_t dBuf[2] = { 0, 0 };
#endif
  // Read in the register to set bit 5 and write back out
  // Retval = rsi_memRd(RSI_INT_CLR_REG_ADDR, sizeof(dBuf), dBuf);
  // 0x74, 0x00, 0x22000010
  // Set bit 5, interrupt clear
  dBuf[0] |= interruptClear;

  // Memory write the mask value
  retval = rsi_mem_wr(RSI_INT_CLR_REG_ADDR, sizeof(dBuf), dBuf);
  if (retval != RSI_SUCCESS) {
    // Exit with error if timed out waiting for the SPI to get ready
    return retval;
  }

  return retval;
}

/*==================================================*/
/**
 * @brief       Return the module SPI interrupt register status
 * @param[in]   int_status - Pointer to the buffer of data to be read, assumed to be at least a byte
 * @return      0 - SUCCESS \n
 *              -1 - SPI busy / Timeout \n
 *              -2 - SPI Failure
 */
int16_t rsi_device_interrupt_status(uint8_t *int_status)
{
  int16_t retval;
  uint32_t timeout;

  timeout = 1 * RSI_TICKS_PER_SECOND;

  RSI_RESET_TIMER2;
  while (1) {
    if (RSI_INC_TIMER_2 > timeout) {
      // Timeout
      retval = RSI_ERROR_SPI_TIMEOUT;
      break;
    }

    // Read the interrupt register
    retval = rsi_reg_rd(RSI_SPI_INT_REG_ADDR, int_status);

    if (retval != RSI_ERROR_SPI_BUSY) {
      break;
    }
  }

  return retval;
}

/*==================================================*/
/**
 * @brief       Return the length of the incoming packet
 * @param[in]   length - Pointer to the buffer of data to write, assumed to be at least 2 bytes long
 * @return      0 - SUCCESS \n
 *              -1 - SPI busy / Timeout \n
 *              -2 - SPI Failure
 */
int16_t rsi_spi_pkt_len(uint16_t *length)
{
  int16_t retval;
  uint32_t timeout;

  timeout = 1 * RSI_TICKS_PER_SECOND;

  RSI_RESET_TIMER2;
  while (1) {
    if (RSI_INC_TIMER_2 > timeout) {
      // Timeout
      retval = RSI_ERROR_SPI_BUSY;
      break;
    }

    // Read the interrupt register
    retval = rsi_reg_rd2(RSI_SPI_LENGTH_REG_ADDR, length);

    if (retval != RSI_ERROR_SPI_BUSY) {
      break;
    }
  }

  return retval;
}

/*==================================================================*/
/**
 * @brief      Configure the module SPI interface to high speed mode
 * @param[in]  void
 * @return     0 - SUCCESS \n
 *             -1 - SPI busy / Timeout \n
 *             -2 - SPI Failure
 */

int16_t rsi_spi_high_speed_enable(void)
{
  uint8_t data = 0x3;
  uint8_t return_val;
  return_val = rsi_reg_wr(0x08, &data);
  return return_val;
}
#endif
/** @} */
