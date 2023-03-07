/*******************************************************************************
* File Name: cycfg_pins.h
*
* Description:
* Pin configuration
* This file was automatically generated and should not be modified.
* Tools Package 2.3.0.4276
* 30739A0 CSP
* personalities 1.0.0.31
* udd 3.0.0.1636
*
********************************************************************************
* Copyright 2022 Cypress Semiconductor Corporation
* SPDX-License-Identifier: Apache-2.0
*
* Licensed under the Apache License, Version 2.0 (the "License");
* you may not use this file except in compliance with the License.
* You may obtain a copy of the License at
*
*     http://www.apache.org/licenses/LICENSE-2.0
*
* Unless required by applicable law or agreed to in writing, software
* distributed under the License is distributed on an "AS IS" BASIS,
* WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
* See the License for the specific language governing permissions and
* limitations under the License.
********************************************************************************/

#if !defined(CYCFG_PINS_H)
#define CYCFG_PINS_H

#include "cycfg_notices.h"
#include "wiced_platform.h"
#include "cycfg_routing.h"

#if defined(__cplusplus)
extern "C" {
#endif

#define bluetooth_0_ENABLED 1U
#define ioss_0_ENABLED 1U
#define WICED_GET_PIN_FOR_LED(idx) (*(platform_led[(idx)].gpio))
#define WICED_GET_PIN_FOR_BUTTON(idx) (*(platform_button[(idx)].gpio))
#define WICED_GET_PIN_FOR_IO(idx) (*(platform_gpio[(idx)].gpio))
#define BUTTON_USER_ENABLED 1U
#define BUTTON_USER WICED_P00
#define PUART_TXD_ENABLED 1U
#define PUART_TXD WICED_P10
#define SPI2_CLK_ENABLED 1U
#define SPI2_CLK WICED_P16
#define GPIO2_ENABLED 1U
#define GPIO2 WICED_P17
#define SPI2_CS_ENABLED 1U
#define SPI2_CS WICED_P25
#define GPIO3_ENABLED 1U
#define GPIO3 WICED_P26
#define SPI2_MOSI_ENABLED 1U
#define SPI2_MOSI WICED_P28
#define SPI2_MISO_ENABLED 1U
#define SPI2_MISO WICED_P29
#define DEBUG_UART_TXD_ENABLED 1U
#define DEBUG_UART_TXD WICED_P33
#define LED1_ENABLED 1U
#define LED1 WICED_P34
#define LED2_ENABLED 1U
#define LED2 WICED_P38
#define PWM0_ENABLED 1U
#define PWM0 WICED_P04
#define PUART_RXD_ENABLED 1U
#define PUART_RXD WICED_P06
#define GPIO1_ENABLED 1U
#define GPIO1 WICED_P07
#define pwm_0_ENABLED 1U
#define spi_1_ENABLED 1U
#define uart_1_ENABLED 1U

extern const wiced_platform_gpio_t platform_gpio_pins[];
extern const size_t platform_gpio_pin_count;
extern const wiced_platform_led_config_t platform_led[];
extern const size_t led_count;
extern const wiced_platform_button_config_t platform_button[];
extern const size_t button_count;
extern const wiced_platform_gpio_config_t platform_gpio[];
extern const size_t gpio_count;


#if defined(__cplusplus)
}
#endif


#endif /* CYCFG_PINS_H */
