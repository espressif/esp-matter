/*******************************************************************************
* @file  rsi_sdio_mem_rd_wr.c
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
 * @file       rsi_sdio_mem_rd_wr.c
 * @version    0.1
 * @date       15 Aug 2015
 *
 *
 *
 * @brief SDIO MEMRDWR: SPI Memory Read/Write functions, r/w memory on the module via the SPI interface
 *
 *
 */

/*
  Includes
 */
#include "rsi_driver.h"
#ifdef RSI_SDIO_INTERFACE
/*
  Global Variables
 */
/** @addtogroup DRIVER1
* @{
*/
/*===========================================================================*/
/**
 */

int16_t rsi_mem_wr(uint32_t addr, uint16_t len, uint8_t *dBuf)
{
  uint8_t data;
  int16_t retval        = RSI_SUCCESS;
  uint16_t no_of_blocks = 0;
  data                  = ((addr & 0xff000000) >> 24);
  // write MSB to 0xFB
  retval = rsi_reg_wr(0xfb, &data);
  if (retval != RSI_SUCCESS)
    return retval;

  data = ((addr & 0x00ff0000) >> 16);
  // write MSB to 0xFA
  retval = rsi_reg_wr(0xfa, &data);
  if (retval != RSI_SUCCESS)
    return retval;

  if (len > 256) {
    // Calculate number of blocks
    no_of_blocks = (len / 256);
    if (len % 256) {
      no_of_blocks = no_of_blocks + 1;
    }
    // Transfer packet
    return rsi_sdio_write_multiple(dBuf, (addr & 0xffff) | SD_MASTER_ACCESS, no_of_blocks);
  } else {
    return rsi_sdio_writeb((addr & 0xffff) | SD_MASTER_ACCESS, len, dBuf);
  }
}

/*===========================================================================*/
/**
  */
int16_t rsi_mem_rd(uint32_t addr, uint16_t len, uint8_t *dBuf)
{
  int16_t retval = RSI_SUCCESS;
  uint8_t data;

  data = ((addr & 0xff000000) >> 24);
  // write MSB to 0xFB
  retval = rsi_reg_wr(0xfb, &data);
  if (retval != RSI_SUCCESS)
    return retval;

  data = ((addr & 0x00ff0000) >> 16);
  // write MSB to 0xFA
  retval = rsi_reg_wr(0xfa, &data);
  if (retval != RSI_SUCCESS)
    return retval;

  return rsi_sdio_readb((addr & 0xffff) | SD_MASTER_ACCESS, len, dBuf);
}

/*==================================================*/
/**
 */
int16_t rsi_device_interrupt_status(uint8_t *int_status)
{
  int16_t retval;
  uint32_t timeout;
  timeout = RSI_SDIO_TIMEOUT;
  RSI_RESET_SDIO_TIMER;
  while (1) {
    if (RSI_INC_SDIO_TIMER > timeout) {
      retval = RSI_ERROR_SDIO_TIMEOUT;
      break;
    }
    // Read the interrupt register
    retval = rsi_reg_rd(RSI_SDIO_INTF_STATUS_REG, int_status);

    if (retval == 0) {
      break;
    }
  }

  return retval;
}
#endif
/** @} */
