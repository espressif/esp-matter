/***************************************************************************//**
 * @file    iot_i2c_desc.h
 * @brief   I2C instance descriptor data structure.
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

#ifndef _IOT_I2C_DESC_H_
#define _IOT_I2C_DESC_H_

/*******************************************************************************
 *                               INCLUDES
 ******************************************************************************/

#include "sl_status.h"

#include "em_device.h"
#include "em_cmu.h"
#include "em_gpio.h"
#include "em_i2c.h"

#include "sl_sleeptimer.h"

#include "iot_i2c.h"

/*******************************************************************************
 *                              DESCRIPTOR
 ******************************************************************************/

typedef struct IotI2CDescriptor {
  /**************************************/
  /*            I2C CONFIG              */
  /**************************************/

  /* instance number */
  int32_t                       lInstNum;

  /* default configs */
  uint32_t                      ulDefaultTimeout;
  uint32_t                      ulDefaultFreq;

  /**************************************/
  /*            I2C DRIVER              */
  /**************************************/

  /* i2c operation */
  I2C_TransferSeq_TypeDef       xTransfer;
  I2C_TransferReturn_TypeDef    xTransferRet;

  /* sleep timers */
  sl_sleeptimer_timer_handle_t  xTimeoutTimer;

  /* runtime config */
  uint32_t                      ulTimeout;
  uint32_t                      ulFreq;

  /* accept NAK - for testing the driver */
  uint8_t                       ucAcceptNack;

  /* i2c peripheral */
  I2C_TypeDef                  *pxPeripheral;
  uint8_t                       ucPeripheralNo;

  /* SCL port/pin */
  GPIO_Port_TypeDef             xSclPort;
  uint8_t                       ucSclPin;
  uint8_t                       ucSclLoc;

  /* SDA port/pin */
  GPIO_Port_TypeDef             xSdaPort;
  uint8_t                       ucSdaPin;
  uint8_t                       ucSdaLoc;

  /* Enable signal port/pin*/
  uint8_t                       ucEnMode;
  GPIO_Port_TypeDef             xEnPort;
  uint8_t                       ucEnPin;

  /* clock and IRQ info */
  CMU_Clock_TypeDef             xClock;
  IRQn_Type                     xIRQn;

  /**************************************/
  /*             I2C HAL                */
  /**************************************/

  /* open flag */
  uint8_t                       ucIsOpen;

  /* callback info */
  void                         *pvCallback;
  void                         *pvContext;

  /* slave address */
  uint16_t                      usSlaveAddr;

  /* transfer progress */
  uint8_t                       ucDirection;
  uint16_t                      usTxMax;
  uint16_t                      usRxMax;
  uint16_t                      usTxCount;
  uint16_t                      usRxCount;

  /* I2C status */
  IotI2CBusStatus_t             xBusStatus;
  IotI2COperationStatus_t       xOpStatus;
} IotI2CDescriptor_t;

/*******************************************************************************
 *                              PROTOTYPES
 ******************************************************************************/

IotI2CDescriptor_t *iot_i2c_desc_get(int32_t lInstNum);

/*******************************************************************************
 *                              SAFE GUARD
 ******************************************************************************/

#endif /* _IOT_I2C_DESC_H_ */
