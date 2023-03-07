/***************************************************************************//**
 * @file
 * @brief Configuration Over SWO Component.
 *******************************************************************************
 * # License
 * <b>Copyright 2022 Silicon Laboratories Inc. www.silabs.com</b>
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

#ifndef SL_COS_H
#define SL_COS_H

#include "sl_enum.h"

/***************************************************************************//**
 * @addtogroup cos Configuration Over SWO
 * @brief Configuration Over SWO
 * @details
 * ## Overview
 *
 *   When working with VCOM and/or PTI with a Silicon Labs kit (WSTK or WPK),
 *   it's important that the configuration of the kit's board controller
 *   matches that of the target application.
 *   The Configuration Over SWO component provides a way to automatically
 *   configure the board controller to use the same settings for VCOM and
 *   PTI configuration is sent to the board controller over SWO at startup,
 *   ensuring that the configuration is synchronized.
 * @{
 ******************************************************************************/

/** @} (end addtogroup cos) */

/*******************************************************************************
 *******************************   DEFINES   ***********************************
 ******************************************************************************/
#if defined (SL_CATALOG_UARTDRV_USART_PRESENT) || defined (SL_CATALOG_UARTDRV_EUSART_PRESENT)
#define SLI_COS_UARTDRV_VCOM_PRESENT
#endif // SL_CATALOG_UARTDRV_USART_PRESENT || SL_CATALOG_UARTDRV_EUSART_PRESENT

/***************************************************************************//**
 * PTI mode enumerations.
 ******************************************************************************/
SL_ENUM(COS_PtiMode_t) {
  /** ONEWIRE mode. */
  COS_CONFIG_PTI_MODE_ONEWIRE,
  /** TWOWIRE mode. */
  COS_CONFIG_PTI_MODE_TWOWIRE,
  /** UART mode. */
  COS_CONFIG_PTI_MODE_EFRUART,
  /** SPI mode. */
  COS_CONFIG_PTI_MODE_EFRSPI,
  /** Mode count. */
  COS_CONFIG_PTI_MODE_COUNT
};

/***************************************************************************//**
 * PTI interface enumerations.
 ******************************************************************************/
SL_ENUM(COS_PtiInterface_t) {
  /** PTI INTERFACE 0. */
  COS_CONFIG_PTI_INTERFACE_0,
  /** PTI INTERFACE 1. */
  COS_CONFIG_PTI_INTERFACE_1,
  /** Interface count. */
  COS_CONFIG_PTI_INTERFACE_COUNT
};

// -----------------------------------------------------------------------------
// Prototypes

/***************************************************************************//**
 * Initialize the SWO ITM 8 and initiates VCOM/PTI SWO writes.
 ******************************************************************************/
void sl_cos_send_config(void);

/***************************************************************************//**
 * Custom API, to be used by other software component to write the
 * structured PTI data on SWO ITM channel 8
 ******************************************************************************/
void sl_cos_config_pti(uint32_t baudrate,
                       COS_PtiMode_t mode,
                       COS_PtiInterface_t interface);

/***************************************************************************//**
 * Custom API, to be used by other software component to write the
 * structured VCOM data on SWO ITM channel 8
 ******************************************************************************/
void sl_cos_config_vcom(uint32_t baudrate,
                        uint8_t flow_control);

#endif /* SL_COS_H */
