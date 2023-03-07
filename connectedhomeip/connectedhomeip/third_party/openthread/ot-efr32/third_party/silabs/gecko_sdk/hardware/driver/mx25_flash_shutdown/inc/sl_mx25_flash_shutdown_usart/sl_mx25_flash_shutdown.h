/***************************************************************************//**
 * @file
 * @brief MX25 flash shutdown
 *******************************************************************************
 * # License
 * <b>Copyright 2020 Silicon Laboratories Inc. www.silabs.com</b>
 *******************************************************************************
 *
 * The licensor of this software is Silicon Laboratories Inc.  Your use of this
 * software is governed by the terms of Silicon Labs Master Software License
 * Agreement (MSLA) available at
 * www.silabs.com/about-us/legal/master-software-license-agreement.  This
 * software is distributed to you in Source Code format and is governed by the
 * sections of the MSLA applicable to Source Code.
 *
 ******************************************************************************/

#ifndef  SL_MX25_FLASH_SHUTDOWN_H
#define  SL_MX25_FLASH_SHUTDOWN_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdbool.h>
#include <stdint.h>
#include "em_device.h"
#include "sl_mx25_flash_shutdown_usart_config.h"

/***************************************************************************//**
 * @addtogroup mx25_flash_shutdown MX25 SPI Flash Shutdown
 * @brief  Provide a function to put the MX25 SPI flash into deep power down
 * mode to reduce power consumption.
 * @{
 ******************************************************************************/

/***************************************************************************//**
 * @brief
 *    Put the MX25 SPI flash into deep power down mode.
 *
 * This function initializes SPI communication with the MX25 flash and sends
 * the deep power-down instruction, which sets the device to minimal
 * power consumption. The SPI communication is disabled to free the USART.
 ******************************************************************************/
void sl_mx25_flash_shutdown(void);

/**@}*/

#ifdef __cplusplus
}
#endif
#endif // SL_MX25_FLASH_SHUTDOWN_H
