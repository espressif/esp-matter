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
#define SL_MIC_I2S_PERIPHERAL                    USART1
#define SL_MIC_I2S_PERIPHERAL_NO                 1

// USART1 RX on PC7
#define SL_MIC_I2S_RX_PORT                       gpioPortC
#define SL_MIC_I2S_RX_PIN                        7
#define SL_MIC_I2S_RX_LOC                        11

// USART1 CLK on PC8
#define SL_MIC_I2S_CLK_PORT                      gpioPortC
#define SL_MIC_I2S_CLK_PIN                       8
#define SL_MIC_I2S_CLK_LOC                       11

// USART1 CS on PC9
#define SL_MIC_I2S_CS_PORT                       gpioPortC
#define SL_MIC_I2S_CS_PIN                        9
#define SL_MIC_I2S_CS_LOC                        11

// [USART_SL_MIC_I2S]$
// <<< sl:end pin_tool >>>

#endif // SL_MIC_I2S_CONFIG_H
