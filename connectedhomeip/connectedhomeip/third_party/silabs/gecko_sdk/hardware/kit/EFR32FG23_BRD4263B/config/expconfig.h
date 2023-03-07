/***************************************************************************//**
 * @file
 * @brief Provide EXP header configuration parameters.
 *******************************************************************************
 * # License
 * <b>Copyright 2018 Silicon Laboratories Inc. www.silabs.com</b>
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

#ifndef EXPCONFIG_H
#define EXPCONFIG_H

#define BSP_EXP_HEADER3_PIN    10
#define BSP_EXP_HEADER3_PORT   gpioPortA
#define BSP_EXP_HEADER4_PIN    1
#define BSP_EXP_HEADER4_PORT   gpioPortC
#define BSP_EXP_HEADER5_PIN    0
#define BSP_EXP_HEADER5_PORT   gpioPortA
#define BSP_EXP_HEADER6_PIN    2
#define BSP_EXP_HEADER6_PORT   gpioPortC
#define BSP_EXP_HEADER7_PIN    5
#define BSP_EXP_HEADER7_PORT   gpioPortA
#define BSP_EXP_HEADER8_PIN    3
#define BSP_EXP_HEADER8_PORT   gpioPortC
#define BSP_EXP_HEADER9_PIN    2
#define BSP_EXP_HEADER9_PORT   gpioPortD
#define BSP_EXP_HEADER10_PIN   0
#define BSP_EXP_HEADER10_PORT  gpioPortC
#define BSP_EXP_HEADER11_PIN   6
#define BSP_EXP_HEADER11_PORT  gpioPortA
#define BSP_EXP_HEADER12_PIN   8
#define BSP_EXP_HEADER12_PORT  gpioPortA
#define BSP_EXP_HEADER13_PIN   7
#define BSP_EXP_HEADER13_PORT  gpioPortA
#define BSP_EXP_HEADER14_PIN   9
#define BSP_EXP_HEADER14_PORT  gpioPortA
#define BSP_EXP_HEADER15_PIN   5
#define BSP_EXP_HEADER15_PORT  gpioPortC
#define BSP_EXP_HEADER16_PIN   7
#define BSP_EXP_HEADER16_PORT  gpioPortC

#define BSP_EXP_I2C            I2C0
#define BSP_EXP_I2C_SCL_PIN    BSP_EXP_HEADER15_PIN
#define BSP_EXP_I2C_SCL_PORT   BSP_EXP_HEADER15_PORT
#define BSP_EXP_I2C_SDA_PIN    BSP_EXP_HEADER16_PIN
#define BSP_EXP_I2C_SDA_PORT   BSP_EXP_HEADER16_PORT

#define BSP_EXP_SPI            USART0
#define BSP_EXP_SPI_CLK_PIN    BSP_EXP_HEADER8_PIN
#define BSP_EXP_SPI_CLK_PORT   BSP_EXP_HEADER8_PORT
#define BSP_EXP_SPI_CS_PIN     BSP_EXP_HEADER10_PIN
#define BSP_EXP_SPI_CS_PORT    BSP_EXP_HEADER10_PORT
#define BSP_EXP_SPI_MISO_PIN   BSP_EXP_HEADER6_PIN
#define BSP_EXP_SPI_MISO_PORT  BSP_EXP_HEADER6_PORT
#define BSP_EXP_SPI_MOSI_PIN   BSP_EXP_HEADER4_PIN
#define BSP_EXP_SPI_MOSI_PORT  BSP_EXP_HEADER4_PORT

#endif /* EXPCONFIG_H */
