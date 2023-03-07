/***************************************************************************//**
 * @file    iot_uart_desc.h
 * @brief   UART instance descriptor data structure.
 *******************************************************************************
 * # License
 * <b>Copyright 2020 Silicon Laboratories Inc. www.silabs.com</b>
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

/*******************************************************************************
 *                              SAFE GUARD
 ******************************************************************************/

#ifndef _IOT_UART_DESC_H_
#define _IOT_UART_DESC_H_

/*******************************************************************************
 *                               INCLUDES
 ******************************************************************************/

#include "sl_status.h"

#include "em_device.h"
#include "em_cmu.h"
#include "em_gpio.h"
#include "em_usart.h"

/*******************************************************************************
 *                              DESCRIPTOR
 ******************************************************************************/

typedef struct IotUARTDescriptor {
  /**************************************/
  /*            UART CONFIG             */
  /**************************************/

  /* instance number */
  int32_t                     lInstNum;

  /* baudrate config */
  uint32_t                    ulDefaultBaudrate;

  /* peripheral settings */
  USART_Databits_TypeDef      xDefaultDataBits;
  USART_Parity_TypeDef        xDefaultParity;
  USART_Stopbits_TypeDef      xDefaultStopBits;

  /* h/w flow control if supported */
#if (_SILICON_LABS_32B_SERIES != 0)
  USART_HwFlowControl_TypeDef xDefaultFlowCtrl;
#endif

  /* loopback configuration */
  uint8_t                     ucLoopbackEn;

  /**************************************/
  /*            UART DRIVER             */
  /**************************************/

  /* initialization structure */
  USART_InitAsync_TypeDef     xInit;

  /* selected peripheral */
  USART_TypeDef              *pxPeripheral;
  uint8_t                     ucPeripheralNo;

  /* enabled ports/pins */
  uint8_t                     ucTxEn;
  uint8_t                     ucRxEn;
  uint8_t                     ucClkEn;
  uint8_t                     ucCsEn;
  uint8_t                     ucRtsEn;
  uint8_t                     ucCtsEn;

  /* ports */
  GPIO_Port_TypeDef           xTxPort;
  GPIO_Port_TypeDef           xRxPort;
  GPIO_Port_TypeDef           xClkPort;
  GPIO_Port_TypeDef           xCsPort;
  GPIO_Port_TypeDef           xRtsPort;
  GPIO_Port_TypeDef           xCtsPort;

  /* pins */
  uint8_t                     ucTxPin;
  uint8_t                     ucRxPin;
  uint8_t                     ucClkPin;
  uint8_t                     ucCsPin;
  uint8_t                     ucRtsPin;
  uint8_t                     ucCtsPin;

  /* locations */
#if (_SILICON_LABS_32B_SERIES < 2)
  uint8_t                     ucTxLoc;
  uint8_t                     ucRxLoc;
  uint8_t                     ucClkLoc;
  uint8_t                     ucCsLoc;
  uint8_t                     ucCtsLoc;
  uint8_t                     ucRtsLoc;
#endif

  /* clocks */
  CMU_Clock_TypeDef           xClock;

  /* IRQ lines */
  IRQn_Type                   xTxIRQn;
  IRQn_Type                   xRxIRQn;

  /**************************************/
  /*             UART HAL               */
  /**************************************/

  /* open flag */
  uint8_t                    ucIsOpen;

  /* callback info */
  void                      *pvCallback;
  void                      *pvContext;

  /* TX buffer */
  uint8_t                    ucTxIsBusy;
  uint8_t                   *pvTxBuf;
  uint32_t                   ulTxMax;
  uint32_t                   ulTxCtr;

  /* RX buffer */
  uint8_t                    ucRxIsBusy;
  uint8_t                   *pvRxBuf;
  uint32_t                   ulRxMax;
  uint32_t                   ulRxCtr;
} IotUARTDescriptor_t;

/*******************************************************************************
 *                              PROTOTYPES
 ******************************************************************************/

IotUARTDescriptor_t *iot_uart_desc_get(int32_t lInstNum);

/*******************************************************************************
 *                              SAFE GUARD
 ******************************************************************************/

#endif /* _IOT_UART_DESC_H_ */
