/*******************************************************************************
* @file  rsi_spi_iface_init.c
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
 * @file     rsi_spi_iface_init.c
 * @version  0.1
 * @date     15 Aug 2015
 *
 *
 *
 * @brief SPI INIT: Functions to initiaize the SPI hardware interface in the module
 * Description
 * Contains the SPI Initialization function.
 * Enable the SPI interface on the Wi-Fi chip.
 * Only run once - during startup, after power-on, or reset
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
/*=============================================*/
/**
 * @brief       Initialize the wlan module�s Slave SPI interface.
 * @param[in]   void
 * @return      0 - SUCCESS \n
 *              -1 - SPI busy / Timeout \n
 *              -2 - SPI Failure
 */

int16_t rsi_spi_iface_init(void)
{
  SL_PRINTF(SL_SPI_IFACE_INIT_ENTRY, DRIVER, LOG_INFO);

  uint8_t txCmd[4];
  uint8_t localBuf[4] = { 0 };
  int16_t retval      = 0;
  uint16_t timeout;

  // 10ms timeout on command, nothing magic, just a reasonable number
  timeout = 10;

  // Init the timer counter
  RSI_RESET_TIMER1;
  while (1) {
    if (RSI_INC_TIMER_1 > timeout) {
      retval = RSI_ERROR_SPI_TIMEOUT;
      break;
    }

    txCmd[0] = (RSI_RS9116_INIT_CMD & 0xFF);
    txCmd[1] = ((RSI_RS9116_INIT_CMD >> 8) & 0xFF);
    txCmd[2] = ((RSI_RS9116_INIT_CMD >> 16) & 0xFF);
    txCmd[3] = ((RSI_RS9116_INIT_CMD >> 24) & 0xFF);

    retval = rsi_spi_transfer(txCmd, localBuf, sizeof(localBuf), RSI_MODE_8BIT);
    // Retval = rsi_spi_send(txCmd, 2, localBuf, RSI_MODE_8BIT);
    if (localBuf[3] == RSI_SPI_SUCCESS) {
      retval = RSI_SUCCESS;
      break;
    } else {
      retval = RSI_ERROR_SPI_BUSY;
    }
  }
  SL_PRINTF(SL_SPI_IFACE_INIT_EXIT, DRIVER, LOG_INFO, "retval: %d", retval);
  return retval;
}

/*=============================================*/
/**
 * @brief               Initialize the with modules Slave SPI interface on ulp wakeup.
 * @param[in]           void
 * @return              void
 */
void rsi_ulp_wakeup_init(void)
{
  uint8_t txCmd[4];
  uint8_t rxbuff[2];

  txCmd[0] = (RSI_RS9116_INIT_CMD & 0xFF);
  txCmd[1] = ((RSI_RS9116_INIT_CMD >> 8) & 0xFF);
  txCmd[2] = ((RSI_RS9116_INIT_CMD >> 16) & 0xFF);
  txCmd[3] = ((RSI_RS9116_INIT_CMD >> 24) & 0xFF);

  while (1) {
    rsi_spi_transfer(txCmd, rxbuff, 2, RSI_MODE_8BIT);
    if (rxbuff[1] == RSI_SPI_FAIL) {
      return;
    } else if (rxbuff[1] == 0x00) {
      rsi_spi_transfer(&txCmd[2], rxbuff, 2, RSI_MODE_8BIT);
      if (rxbuff[1] == RSI_SPI_SUCCESS) {
        break;
      }
    }
  }
}
#endif
/** @} */
