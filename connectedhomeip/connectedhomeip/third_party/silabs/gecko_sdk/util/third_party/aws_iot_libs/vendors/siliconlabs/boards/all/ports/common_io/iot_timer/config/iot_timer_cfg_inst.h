/***************************************************************************//**
 * @file    iot_timer_cfg_inst.h
 * @brief   Common I/O Timer instance configuration.
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

#ifndef _IOT_TIMER_CFG_INSTANCE_H_
#define _IOT_TIMER_CFG_INSTANCE_H_

/*******************************************************************************
 *                        Timer Default Configs
 ******************************************************************************/

// <<< Use Configuration Wizard in Context Menu >>>

// <h> Timer General Options

// <o IOT_TIMER_CFG_INSTANCE_INST_NUM> Instance number
// <i> Instance number used when iot_timer_open() is called.
// <i> Default: 0
#define IOT_TIMER_CFG_INSTANCE_INST_NUM                0

// <o IOT_TIMER_CFG_INSTANCE_DEFAULT_PRIORITY> Default priority
// <i> Default priority that is assigned to the sleep timer.
// <i> Default: 2
#define IOT_TIMER_CFG_INSTANCE_DEFAULT_PRIORITY        2

// </h>

// <<< end of configuration section >>>

/*******************************************************************************
 *                              SAFE GUARD
 ******************************************************************************/

#endif /* _IOT_TIMER_CFG_INSTANCE_H_ */
