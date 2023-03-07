/***************************************************************************//**
 * @file
 * @brief Implementation specific functions for HRM code
 *******************************************************************************
 * # License
 * <b>Copyright 2018 Silicon Laboratories Inc. www.silabs.com</b>
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
#ifndef SI114X_SYS_OUT
#define SI114X_SYS_OUT

#include "em_i2c.h"
#include "em_gpio.h"

typedef struct Si114xPortConfig{
  I2C_TypeDef       *i2cPort;   /**< I2C port Si114x is connected to */
  uint8_t           i2cAddress; /**< I2C address of Si114x */
  GPIO_Port_TypeDef irqPort;    /**< Port for Si114x INT pin */
  int               irqPin;     /**< Pin for Si114x INT pin */
} Si114xPortConfig_t;

int16_t Si114xProcessIrq(void* si114x_handle, uint16_t timestamp);

#endif
