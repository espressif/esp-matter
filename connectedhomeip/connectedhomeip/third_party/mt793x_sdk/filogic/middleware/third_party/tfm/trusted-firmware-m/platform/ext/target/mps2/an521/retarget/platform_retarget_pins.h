/*
 * Copyright (c) 2016-2018 ARM Limited
 *
 * Licensed under the Apache License Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing software
 * distributed under the License is distributed on an "AS IS" BASIS
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

/**
 * \file platform_retarget_pins.h
 * \brief This file defines all the pins for this platform.
 */

#ifndef __ARM_LTD_AN521_RETARGET_PINS_H__
#define __ARM_LTD_AN521_RETARGET_PINS_H__

/* AHB GPIO pin names */
enum arm_gpio_pin_name_t {
  AHB_GPIO0_0 = 0U,
  AHB_GPIO0_1 = 1U,
  AHB_GPIO0_2 = 2U,
  AHB_GPIO0_3 = 3U,
  AHB_GPIO0_4 = 4U,
  AHB_GPIO0_5 = 5U,
  AHB_GPIO0_6 = 6U,
  AHB_GPIO0_7 = 7U,
  AHB_GPIO0_8 = 8U,
  AHB_GPIO0_9 = 9U,
  AHB_GPIO0_10 = 10U,
  AHB_GPIO0_11 = 11U,
  AHB_GPIO0_12 = 12U,
  AHB_GPIO0_13 = 13U,
  AHB_GPIO0_14 = 14U,
  AHB_GPIO0_15 = 15U,
  AHB_GPIO1_0 = 0U,
  AHB_GPIO1_1 = 1U,
  AHB_GPIO1_2 = 2U,
  AHB_GPIO1_3 = 3U,
  AHB_GPIO1_4 = 4U,
  AHB_GPIO1_5 = 5U,
  AHB_GPIO1_6 = 6U,
  AHB_GPIO1_7 = 7U,
  AHB_GPIO1_8 = 8U,
  AHB_GPIO1_9 = 9U,
  AHB_GPIO1_10 = 10U,
  AHB_GPIO1_11 = 11U,
  AHB_GPIO1_12 = 12U,
  AHB_GPIO1_13 = 13U,
  AHB_GPIO1_14 = 14U,
  AHB_GPIO1_15 = 15U,
  AHB_GPIO2_0 = 0U,
  AHB_GPIO2_1 = 1U,
  AHB_GPIO2_2 = 2U,
  AHB_GPIO2_3 = 3U,
  AHB_GPIO2_4 = 4U,
  AHB_GPIO2_5 = 5U,
  AHB_GPIO2_6 = 6U,
  AHB_GPIO2_7 = 7U,
  AHB_GPIO2_8 = 8U,
  AHB_GPIO2_9 = 9U,
  AHB_GPIO2_10 = 10U,
  AHB_GPIO2_11 = 11U,
  AHB_GPIO2_12 = 12U,
  AHB_GPIO2_13 = 13U,
  AHB_GPIO2_14 = 14U,
  AHB_GPIO2_15 = 15U,
  AHB_GPIO3_0 = 0U,
  AHB_GPIO3_1 = 1U,
  AHB_GPIO3_2 = 2U,
  AHB_GPIO3_3 = 3U,
};

/* Pin definitions for the MPS2 Arduino adapter shields.
 * Reference: Application Note AN502 */

/*
 * Shield buttons adaptor
 * The user buttons on the shield are linked to the pins 5 and 6 of GPIO1.
 */
#define SHIELD_ADAPTOR_PB0  AHB_GPIO1_5
#define SHIELD_ADAPTOR_PB1  AHB_GPIO1_6

/* GPIO shield 0 definition */
#define SH0_UART_RX    AHB_GPIO0_0
#define SH0_UART_TX    AHB_GPIO0_4
#define SH0_I2C_SCL    AHB_GPIO0_5
#define SH0_I2C_SDA    AHB_GPIO0_15
#define SH0_SPI_SCK    AHB_GPIO0_11
#define SH0_SPI_SS     AHB_GPIO0_12
#define SH0_SPI_MOSI   AHB_GPIO0_13
#define SH0_SPI_MISO   AHB_GPIO0_14
#define SH0_LED_RED    AHB_GPIO0_6
#define SH0_LED_GREEN  AHB_GPIO0_10
#define SH0_LED_BLUE   AHB_GPIO0_9
#define SH0_SPEAKER    AHB_GPIO0_7
#define SH0_LCD_CS     SH0_SPI_SS
#define SH0_LCD_A0     AHB_GPIO0_8
#define SH0_LCD_RST    SH0_SPI_MISO
#define SH0_JT         AHB_GPIO0_1

/* GPIO shield 1 definition */
#define SH1_UART_RX    AHB_GPIO1_10
#define SH1_UART_TX    AHB_GPIO1_14
#define SH1_I2C_SCL    AHB_GPIO1_15
#define SH1_I2C_SDA    AHB_GPIO2_9
#define SH1_SPI_SS     AHB_GPIO2_6
#define SH1_SPI_MOSI   AHB_GPIO2_7
#define SH1_SPI_MISO   AHB_GPIO2_8
#define SH1_SPI_SCK    AHB_GPIO2_12
#define SH1_LED_RED    AHB_GPIO2_0
#define SH1_LED_GREEN  AHB_GPIO2_4
#define SH1_LED_BLUE   AHB_GPIO2_3
#define SH1_SPEAKER    AHB_GPIO2_1
#define SH1_LCD_CS     SH1_SPI_SS
#define SH1_LCD_A0     AHB_GPIO2_2
#define SH1_LCD_RST    SH1_SPI_MISO
#define SH1_JT         AHB_GPIO1_11

/* GPIO ADC SPI */
#define ADC_SPI_SS    AHB_GPIO1_0
#define ADC_SPI_MOSI  AHB_GPIO1_1
#define ADC_SPI_MISO  AHB_GPIO1_2
#define ADC_SPI_SCK   AHB_GPIO1_3

/* GPIO BlueTooth/XBEE UART */
#define BT_UART_RX    AHB_GPIO1_7
#define BT_UART_TX    AHB_GPIO1_8
#define BT_BOOT       AHB_GPIO1_9

#endif  /* __ARM_LTD_AN521_RETARGET_PINS_H__ */
