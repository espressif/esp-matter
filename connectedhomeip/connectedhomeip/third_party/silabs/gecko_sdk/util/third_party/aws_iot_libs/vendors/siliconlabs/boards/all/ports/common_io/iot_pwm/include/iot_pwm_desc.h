/***************************************************************************//**
 * @file    iot_pwm_desc.h
 * @brief   PWM instance descriptor data structure.
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

#ifndef _IOT_PWM_DESC_H_
#define _IOT_PWM_DESC_H_

/*******************************************************************************
 *                               INCLUDES
 ******************************************************************************/

#include "sl_status.h"

#include "em_device.h"
#include "em_gpio.h"
#include "em_timer.h"
#include "em_cmu.h"

#include "iot_pwm.h"

/*******************************************************************************
 *                              DESCRIPTOR
 ******************************************************************************/

typedef struct IotPwmDescriptor {
  /**************************************/
  /*            PWM CONFIG              */
  /**************************************/

  /* instance number */
  int32_t            lInstNum;

  /**************************************/
  /*            PWM DRIVER              */
  /**************************************/

  /* selected peripheral */
  TIMER_TypeDef     *pxPeripheral;
  uint8_t            ucPeripheralNo;

  /* channel 0 */
  GPIO_Port_TypeDef  xCC0Port;
  uint8_t            ucCC0Pin;
  uint8_t            ucCC0Loc;

  /* channel 1 */
  GPIO_Port_TypeDef  xCC1Port;
  uint8_t            ucCC1Pin;
  uint8_t            ucCC1Loc;

  /* channel 2 */
  GPIO_Port_TypeDef  xCC2Port;
  uint8_t            ucCC2Pin;
  uint8_t            ucCC2Loc;

  /**************************************/
  /*             PWM HAL                */
  /**************************************/

  /* PWM flags */
  bool              is_open;
  bool              is_configured;

  /* PWM driver parameters */
  bool              is_running;
  bool              polarity_low;
  uint8_t           dutycycle;
  uint32_t          pwm_frequency;
  uint8_t           channel;

  /* runtime config */
  IotPwmConfig_t    pwm_config;
} IotPwmDescriptor_t;

/*******************************************************************************
 *                              PROTOTYPES
 ******************************************************************************/

IotPwmDescriptor_t *iot_pwm_desc_get(int32_t lInstNum);

/*******************************************************************************
 *                              SAFE GUARD
 ******************************************************************************/

#endif /* _IOT_PWM_DESC_H_ */
