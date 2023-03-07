/***************************************************************************//**
 * @file    iot_gpio_cfg_inst.h
 * @brief   Common I/O GPIO instance configuration.
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

#ifndef _IOT_GPIO_CFG_INSTANCE_H_
#define _IOT_GPIO_CFG_INSTANCE_H_

/*******************************************************************************
 *                         GPIO Default Configs
 ******************************************************************************/

// <<< Use Configuration Wizard in Context Menu >>>

// <h>GPIO General Options

// <o IOT_GPIO_CFG_INSTANCE_INST_NUM> Instance number
// <i> Instance number used when iot_gpio_open() is called.
// <i> Default: 0
#define IOT_GPIO_CFG_INSTANCE_INST_NUM                         0

// <o IOT_GPIO_CFG_INSTANCE_DIRECTION> GPIO Direction
// <eGpioDirectionInput=> Input
// <eGpioDirectionOutput=> Output
// <i> Default: eGpioDirectionInput
#define IOT_GPIO_CFG_INSTANCE_DIRECTION               eGpioDirectionInput

// <o IOT_GPIO_CFG_INSTANCE_OUTPUT> GPIO Output mode
// <eGpioOpenDrain=> None
// <eGpioPushPull=> Pull Up
// <i> Default: eGpioPushPull
#define IOT_GPIO_CFG_INSTANCE_OUTPUT               eGpioPushPull

// <o IOT_GPIO_CFG_INSTANCE_PULL> GPIO Pull Mode
// <eGpioPullNone=> None
// <eGpioPullUp=> Pull Up
// <eGpioPullDown=> Pull Down
// <i> Default: eGpioPullNone
#define IOT_GPIO_CFG_INSTANCE_PULL                  eGpioPullNone

// <o IOT_GPIO_CFG_INSTANCE_INTERRUPT_NUMBER> GPIO Interrupt number
// <i> Interrupt number
// <i> Default: 0
#define IOT_GPIO_CFG_INSTANCE_INTERRUPT_NUMBER                 0

// <o IOT_GPIO_CFG_INSTANCE_INTERRUPT_TRIGGER> GPIO Trigger Mode
// <eGpioInterruptNone=> None
// <eGpioInterruptRising=> Generate an interrupt when signal rises
// <eGpioInterruptFalling=> Generate an interrupt when signal falls
// <eGpioInterruptEdge=> Generate an interrupt when either rising or falling
// <eGpioInterruptLow=> Generate an interrupt when signal is low
// <eGpioInterruptHigh=> Generate an interrupt when signal is high
// <i> Default: eGpioPullNone
#define IOT_GPIO_CFG_INSTANCE_INTERRUPT_TRIGGER     eGpioInterruptNone

// </h>

// <<< end of configuration section >>>

/*******************************************************************************
 *                        H/W PERIPHERAL CONFIG
 ******************************************************************************/

// <<< sl:start pin_tool >>>
// <gpio> IOT_GPIO_CFG_INSTANCE
// $[GPIO_IOT_GPIO_CFG_INSTANCE]

#warning "Common I/O GPIO instance not configured"
//#define IOT_GPIO_CFG_TEST_PORT         gpioPortA
//#define IOT_GPIO_CFG_TEST_PIN          0

// <<< sl:end pin_tool >>>

/*******************************************************************************
 *                            SAFE GUARD
 ******************************************************************************/

#endif /* _IOT_GPIO_CFG_INSTANCE_H_ */
