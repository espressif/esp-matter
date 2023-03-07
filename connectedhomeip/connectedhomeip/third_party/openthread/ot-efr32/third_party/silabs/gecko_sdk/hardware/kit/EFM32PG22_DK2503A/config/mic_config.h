/***************************************************************************//**
 * @file
 * @brief SPH0645LM4H-B MEMS Microphone configuration file
 *******************************************************************************
 * # License
 * <b>Copyright 2020 Silicon Laboratories Inc. www.silabs.com</b>
 *******************************************************************************
 *
 * The licensor of this software is Silicon Laboratories Inc. Your use of this
 * software is governed by the terms of Silicon Labs Master Software License
 * Agreement (MSLA) available at
 * www.silabs.com/about-us/legal/master-software-license-agreement. This
 * software is distributed to you in Source Code format and is governed by the
 * sections of the MSLA applicable to Source Code.
 *
 ******************************************************************************/

#ifndef MIC_CONFIG_H
#define MIC_CONFIG_H

#define MIC_PORT_DATA         gpioPortC
#define MIC_PIN_DATA          7
#define MIC_PORT_CLK          gpioPortC
#define MIC_PIN_CLK           6
#define MIC_PORT_PWR_EN       gpioPortA
#define MIC_PIN_PWR_EN        0

#endif // MIC_CONFIG_H
