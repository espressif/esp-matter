/***************************************************************************//**
 * @file
 * @brief MIC_I2S config
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

#ifndef SL_MIC_I2S_CONFIG_H
#define SL_MIC_I2S_CONFIG_H

// <<< sl:start pin_tool >>>
// <usart capability=i2s signal=RX,CLK,CS> SL_MIC_I2S
// $[USART_SL_MIC_I2S]
#define SL_MIC_I2S_PERIPHERAL                    USART0
#define SL_MIC_I2S_PERIPHERAL_NO                 0

// USART0 RX on PD04
#define SL_MIC_I2S_RX_PORT                       gpioPortD
#define SL_MIC_I2S_RX_PIN                        4

// USART0 CLK on PD03
#define SL_MIC_I2S_CLK_PORT                      gpioPortD
#define SL_MIC_I2S_CLK_PIN                       3

// USART0 CS on PD05
#define SL_MIC_I2S_CS_PORT                       gpioPortD
#define SL_MIC_I2S_CS_PIN                        5

// [USART_SL_MIC_I2S]$
// <<< sl:end pin_tool >>>

#endif // SL_MIC_I2S_CONFIG_H
