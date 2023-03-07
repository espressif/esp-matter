/***************************************************************************//**
 * @file    iot_pwm_cfg_inst.h
 * @brief   Common I/O PWM instance configuration.
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

#ifndef _IOT_PWM_CFG_EXP_H_
#define _IOT_PWM_CFG_EXP_H_

/*******************************************************************************
 *                          PWM Default Configs
 ******************************************************************************/

// <<< Use Configuration Wizard in Context Menu >>>

// <h> PWM General Options

// <o IOT_PWM_CFG_EXP_INST_NUM> Instance number
// <i> Instance number used when iot_pwm_open() is called.
// <i> Default: 0
#define IOT_PWM_CFG_EXP_INST_NUM             0

// </h>

// <<< end of configuration section >>>

/*******************************************************************************
 *                          H/W PERIPHERAL CONFIG
 ******************************************************************************/

// <<< sl:start pin_tool >>>
// <timer signal=(CC0),(CC1),(CC2)> IOT_PWM_CFG_EXP
// $[TIMER_IOT_PWM_CFG_EXP]
#define IOT_PWM_CFG_EXP_PERIPHERAL               TIMER4
#define IOT_PWM_CFG_EXP_PERIPHERAL_NO            4

// TIMER4 CC0 on PA05
#define IOT_PWM_CFG_EXP_CC0_PORT                 gpioPortA
#define IOT_PWM_CFG_EXP_CC0_PIN                  5



// [TIMER_IOT_PWM_CFG_EXP]$
// <<< sl:end pin_tool >>>

/*******************************************************************************
 *                              SAFE GUARD
 ******************************************************************************/

#endif /* _IOT_PWM_CFG_EXP_H_ */
