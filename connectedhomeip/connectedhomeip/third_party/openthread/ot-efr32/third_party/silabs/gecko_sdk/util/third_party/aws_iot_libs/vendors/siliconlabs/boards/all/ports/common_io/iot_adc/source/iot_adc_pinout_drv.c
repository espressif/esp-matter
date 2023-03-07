/***************************************************************************//**
 * @file    iot_gpio_drv.c
 * @brief   GPIO driver for Silicon Labs boards.
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
 *                               INCLUDES
 ******************************************************************************/

/* Gecko SDK emlib layer */
#include "em_core.h"
#include "em_device.h"
#include "em_gpio.h"

/* GPIO driver layer */
#include "iot_adc_pinout_drv.h"

/*******************************************************************************
 *                              slGetPort()
 ******************************************************************************/
GPIO_Port_TypeDef slGetPort(slPin pin)
{
  GPIO_Port_TypeDef ret = (GPIO_Port_TypeDef) - 1;

  switch (pin & 0xF0) {
    case 0x00:
      ret = (GPIO_Port_TypeDef) - 1;
      break;

    case 0x10:
#if (_GPIO_PORT_I_PIN_COUNT > 0)
      ret = gpioPortI;
#else
      ret = (GPIO_Port_TypeDef) - 1;
#endif
      break;

    case 0x20:
      ret = (GPIO_Port_TypeDef) - 1;
      break;

    case 0x30:
      ret = (GPIO_Port_TypeDef) - 1;
      break;

    case 0x40:
      ret = (GPIO_Port_TypeDef) - 1;
      break;

    case 0x50:
      ret = (GPIO_Port_TypeDef) - 1;
      break;

    case 0x60:
#if (_GPIO_PORT_G_PIN_COUNT > 0)
      ret = gpioPortG;
#else
      ret = (GPIO_Port_TypeDef) - 1;
#endif
      break;

    case 0x70:
#if (_GPIO_PORT_J_PIN_COUNT > 0)
      ret = gpioPortJ;
#else
      ret = (GPIO_Port_TypeDef) - 1;
#endif
      break;

    case 0x80:
#if (_GPIO_PORT_H_PIN_COUNT > 0)
      ret = gpioPortH;
#else
      ret = (GPIO_Port_TypeDef) - 1;
#endif
      break;

    case 0x90:
#if (_GPIO_PORT_K_PIN_COUNT > 0)
      ret = gpioPortK;
#else
      ret = (GPIO_Port_TypeDef) - 1;
#endif
      break;

    case 0xA0:
#if (_GPIO_PORT_A_PIN_COUNT > 0)
      ret = gpioPortA;
#else
      ret = (GPIO_Port_TypeDef) - 1;
#endif
      break;

    case 0xB0:
#if (_GPIO_PORT_B_PIN_COUNT > 0)
      ret = gpioPortB;
#else
      ret = (GPIO_Port_TypeDef) - 1;
#endif
      break;

    case 0xC0:
#if (_GPIO_PORT_C_PIN_COUNT > 0)
      ret = gpioPortC;
#else
      ret = (GPIO_Port_TypeDef) - 1;
#endif
      break;

    case 0xD0:
#if (_GPIO_PORT_D_PIN_COUNT > 0)
      ret = gpioPortD;
#else
      ret = (GPIO_Port_TypeDef) - 1;
#endif
      break;

    case 0xE0:
#if (_GPIO_PORT_E_PIN_COUNT > 0)
      ret = gpioPortE;
#else
      ret = (GPIO_Port_TypeDef) - 1;
#endif
      break;

    case 0xF0:
#if (_GPIO_PORT_F_PIN_COUNT > 0)
      ret = gpioPortF;
#else
      ret = (GPIO_Port_TypeDef) - 1;
#endif
      break;

    default:
      ret = (GPIO_Port_TypeDef) - 1;
      break;
  }
  return ret;
}

/*******************************************************************************
 *                              slGetPin()
 ******************************************************************************/
uint32_t slGetPin(slPin pin)
{
  return ((uint32_t) pin) & 0x0F;
}
