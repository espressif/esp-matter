/***************************************************************************//**
 * @file    iot_spi_desc.h
 * @brief   SPI instance descriptor data structure.
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

#ifndef _IOT_SPI_DESC_H_
#define _IOT_SPI_DESC_H_

/*******************************************************************************
 *                               INCLUDES
 ******************************************************************************/

#include "FreeRTOS.h"
#include "semphr.h"

#include "sl_status.h"

#include "spidrv.h"

#include "em_device.h"
#include "em_usart.h"

/*******************************************************************************
 *                              DESCRIPTOR
 ******************************************************************************/

typedef struct IotSPIDescriptor {
  /**************************************/
  /*            SPI CONFIG              */
  /**************************************/

  /* instance number */
  int32_t                   lInstNum;

  /* defaults */
  uint32_t                  ulDefaultBitrate;
  uint32_t                  ulDefaultFrameLen;
  uint32_t                  ulDefaultTXValue;
  SPIDRV_Type_t             xDefaultType;
  SPIDRV_BitOrder_t         xDefaultBitOrder;
  SPIDRV_ClockMode_t        xDefaultClockMode;
  SPIDRV_CsControl_t        xDefaultCsControl;
  SPIDRV_SlaveStart_t       xDefaultSlaveStart;

  /**************************************/
  /*            SPI DRIVER              */
  /**************************************/

  /* spidrv structures */
  SPIDRV_Init_t             xInit;
  SPIDRV_HandleData_t       xHandleData;

  /* selected peripheral */
  USART_TypeDef            *pxPeripheral;
  uint8_t                   ucPeripheralNo;

  /* ports */
  GPIO_Port_TypeDef         xTxPort;
  GPIO_Port_TypeDef         xRxPort;
  GPIO_Port_TypeDef         xClkPort;
  GPIO_Port_TypeDef         xCsPort;

  /* pins */
  uint8_t                   ucTxPin;
  uint8_t                   ucRxPin;
  uint8_t                   ucClkPin;
  uint8_t                   ucCsPin;

  /* locations */
  uint8_t                   ucTxLoc;
  uint8_t                   ucRxLoc;
  uint8_t                   ucClkLoc;
  uint8_t                   ucCsLoc;

  /**************************************/
  /*             SPI HAL                */
  /**************************************/

  /* open flag */
  uint8_t                   ucIsOpen;

  /* callback */
  void                     *pvCallback;
  void                     *pvContext;

  /* last operation info */
  uint8_t                   ucLastOp;
} IotSPIDescriptor_t;

/*******************************************************************************
 *                              PROTOTYPES
 ******************************************************************************/

IotSPIDescriptor_t *iot_spi_desc_get(int32_t lInstNum);

/*******************************************************************************
 *                              SAFE GUARD
 ******************************************************************************/

#endif /* _IOT_SPI_DESC_H_ */
