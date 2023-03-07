/***************************************************************************//**
 * @file    iot_gpio_desc.h
 * @brief   GPIO instance descriptor data structure.
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

/* SDK emlib layer */
#include "em_device.h"
#include "em_gpio.h"
#include "em_cmu.h"

/* SDK emdrv layer */
#include "gpiointerrupt.h"

/* Common I/O interface layer */
#include "iot_gpio.h"

/*******************************************************************************
 *                              DESCRIPTOR
 ******************************************************************************/

/* gpio structures */
typedef struct IotGpioDescriptor{
  bool is_open;
  uint8_t instance_id;
  GPIO_Port_TypeDef port;
  unsigned int pin;
  IotGpioDirection_t direction;
  IotGpioOutputMode_t out_mode;
  IotGpioPull_t pull;
  unsigned int int_number;
  IotGpioInterrupt_t interrupt;
  IotGpioCallback_t callback;
  void *context;
} IotGpioDescriptor;

extern IotGpioDescriptor iot_gpio_desc[];

/*******************************************************************************
 *                              PROTOTYPES
 ******************************************************************************/

uint32_t iot_gpio_desc_get_table_size(void);

IotGpioDescriptor *iot_gpio_desc_get(int32_t instance_id);

/*******************************************************************************
 *                              SAFE GUARD
 ******************************************************************************/

#endif /* _IOT_FLASH_DESC_H_ */
