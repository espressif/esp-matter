/***************************************************************************//**
 * @file    iot_adc_cfg_inst.h
 * @brief   Common I/O ADC instance configuration.
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

#ifndef _IOT_ADC_CFG_INSTANCE_H_
#define _IOT_ADC_CFG_INSTANCE_H_

/*******************************************************************************
 *                          ADC Default Configs
 ******************************************************************************/

// <<< Use Configuration Wizard in Context Menu >>>

// <h>ADC General Options

// <o IOT_ADC_CFG_INSTANCE_INST_NUM> Instance number
// <i> Instance number used when iot_adc_open() is called.
// <i> Default: 0
#define IOT_ADC_CFG_INSTANCE_INST_NUM                0

// <o IOT_ADC_CFG_INSTANCE_DEFAULT_ACQ_TIME> Default sample time
// <i> Default: 4
#define IOT_ADC_CFG_INSTANCE_DEFAULT_ACQ_TIME        4

// <o IOT_ADC_CFG_INSTANCE_DEFAULT_RESOLUTION> Default resolution
// <i> Default: 12
#define IOT_ADC_CFG_INSTANCE_DEFAULT_RESOLUTION      12

// </h>

// <<< end of configuration section >>>

/*******************************************************************************
 *                        H/W PERIPHERAL CONFIG
 ******************************************************************************/

#if (_SILICON_LABS_32B_SERIES < 2)
#define IOT_ADC_CFG_INSTANCE_PERIPHERAL      ADC0
#define IOT_ADC_CFG_INSTANCE_PERIPHERAL_NO   0
#else
#define IOT_ADC_CFG_INSTANCE_PERIPHERAL      IADC0
#define IOT_ADC_CFG_INSTANCE_PERIPHERAL_NO   0
#endif

/*******************************************************************************
 *                            SAFE GUARD
 ******************************************************************************/

#endif /* _IOT_ADC_CFG_INSTANCE_H_ */
