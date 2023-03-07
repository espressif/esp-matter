/*******************************************************************************
* @file  rsi_sdio_reg_rd_wr.c
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
 * @brief SDIO REGRDWR: SDIO Register Read/Write functions
 *
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
/**
 *
 */
int16_t rsi_reg_rd(uint8_t regAddr, uint8_t *dBuf)
{
  return sdio_reg_readb(regAddr, dBuf);
}

/**
 */

int16_t rsi_reg_wr(uint8_t regAddr, uint8_t *dBuf)
{
  return sdio_reg_writeb(regAddr, dBuf);
}

#endif
/** @} */
