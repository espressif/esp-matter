/***************************************************************************//**
 * @file    iot_adc_desc.h
 * @brief   ADC instance descriptor data structure.
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

#ifndef _IOT_ADC_DESC_H_
#define _IOT_ADC_DESC_H_

/*******************************************************************************
 *                               INCLUDES
 ******************************************************************************/

#include "sl_status.h"

#include "em_device.h"
#include "em_cmu.h"

/*******************************************************************************
 *                            ADC/IADC INCLUDES
 ******************************************************************************/

#if (_SILICON_LABS_32B_SERIES < 2)
#include "em_adc.h"
#else
#include "em_iadc.h"
#endif

/*******************************************************************************
 *                              DESCRIPTOR
 ******************************************************************************/

typedef struct IotAdcDescriptor {
  /**************************************/
  /*            ADC CONFIG              */
  /**************************************/

  /* instance number */
  int32_t                     lInstNum;

  /* default acq time and resolution */
  uint32_t                    ulDefaultAcqTime;
  uint8_t                     ucDefaultResolution;

  /**************************************/
  /*            ADC DRIVER              */
  /**************************************/

  /* driver state */
  uint32_t                    ulAcqTime;
  uint8_t                     ucResolution;

  /* selected peripheral */
#if (_SILICON_LABS_32B_SERIES < 2)
  ADC_TypeDef                *pxPeripheral;
#else
  IADC_TypeDef               *pxPeripheral;
#endif
  uint8_t                     ucPeripheralNo;

  /* clocks and IRQ lines */
  CMU_Clock_TypeDef           xClock;
  IRQn_Type                   xIRQn;

  /**************************************/
  /*             ADC HAL                */
  /**************************************/

  /* open flag */
  uint8_t                     ucIsOpen;

  /* busy status */
  uint8_t                     ucBusyStatus;
  uint8_t                     ucBusyChannel;

  /* callback info */
  void                       *pvCallback;
  void                       *pvContext;

  /* buffer info */
  uint16_t                   *pusBufPtr;
  uint8_t                     ucBufMax;
  uint8_t                     ucBufCtr;
} IotAdcDescriptor_t;

/*******************************************************************************
 *                              PROTOTYPES
 ******************************************************************************/

IotAdcDescriptor_t *iot_adc_desc_get(int32_t lInstNum);

/*******************************************************************************
 *                              SAFE GUARD
 ******************************************************************************/

#endif /* _IOT_ADC_DESC_H_ */
