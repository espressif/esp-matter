/***************************************************************************//**
 * @file    iot_flash_desc.h
 * @brief   Flash instance descriptor data structure.
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

#ifndef _IOT_FLASH_DESC_H_
#define _IOT_FLASH_DESC_H_

/*******************************************************************************
 *                               INCLUDES
 ******************************************************************************/

#include "sl_status.h"

#include "spidrv.h"

#include "em_device.h"
#include "em_usart.h"

#include "iot_flash.h"

/*******************************************************************************
 *                              DESCRIPTOR
 ******************************************************************************/

typedef struct IotFlashDescriptor {
  /**************************************/
  /*            FLASH CONFIG            */
  /**************************************/

  /* instance number and type */
  int32_t                     lInstNum;
  int32_t                     lInstType;

  /* spi flash config defaults */
  uint32_t                    ulDefaultBitrate;
  uint32_t                    ulDefaultFrameLen;
  uint32_t                    ulDefaultTXValue;
  SPIDRV_Type_t               xDefaultType;
  SPIDRV_BitOrder_t           xDefaultBitOrder;
  SPIDRV_ClockMode_t          xDefaultClockMode;
  SPIDRV_CsControl_t          xDefaultCsControl;
  SPIDRV_SlaveStart_t         xDefaultSlaveStart;

  /**************************************/
  /*            FLASH DRIVER            */
  /**************************************/

  /* spidrv structures */
  SPIDRV_Init_t               xInit;
  SPIDRV_HandleData_t         xHandleData;

  /* selected peripheral */
  USART_TypeDef              *pxPeripheral;
  uint8_t                     ucPeripheralNo;

  /* ports */
  GPIO_Port_TypeDef           xTxPort;
  GPIO_Port_TypeDef           xRxPort;
  GPIO_Port_TypeDef           xClkPort;
  GPIO_Port_TypeDef           xCsPort;

  /* pins */
  uint8_t                     ucTxPin;
  uint8_t                     ucRxPin;
  uint8_t                     ucClkPin;
  uint8_t                     ucCsPin;

  /* locations */
  uint8_t                     ucTxLoc;
  uint8_t                     ucRxLoc;
  uint8_t                     ucClkLoc;
  uint8_t                     ucCsLoc;

  /**************************************/
  /*             FLASH HAL              */
  /**************************************/

  /* open flag */
  uint8_t                  ucIsOpen;

  /* flash info structure */
  IotFlashInfo_t           xFlashInfo;
} IotFlashDescriptor_t;

/*******************************************************************************
 *                              PROTOTYPES
 ******************************************************************************/

IotFlashDescriptor_t *iot_flash_desc_get(int32_t lInstNum);

/*******************************************************************************
 *                              SAFE GUARD
 ******************************************************************************/

#endif /* _IOT_FLASH_DESC_H_ */
