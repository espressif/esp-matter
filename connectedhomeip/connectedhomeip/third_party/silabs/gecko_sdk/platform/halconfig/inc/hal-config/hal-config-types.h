/***************************************************************************//**
 * @file
 * @brief Defines for hal-config.
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

#ifndef HAL_CONFIG_TYPES_H
#define HAL_CONFIG_TYPES_H

#include "em_gpio.h"

#define HAL_CLK_HFCLK_SOURCE_HFRCODPLL  0
#define HAL_CLK_HFCLK_SOURCE_HFRCO      1
#define HAL_CLK_HFCLK_SOURCE_HFXO       2
#define HAL_CLK_HFCLK_SOURCE_FSRCO      3

#define HAL_CLK_LFCLK_SOURCE_DISABLED   0
#define HAL_CLK_LFCLK_SOURCE_LFRCO      1
#define HAL_CLK_LFCLK_SOURCE_LFXO       2
#define HAL_CLK_LFCLK_SOURCE_ULFRCO     3
#define HAL_CLK_LFCLK_SOURCE_HFLE       4
#define HAL_CLK_LFCLK_SOURCE_PLFRCO     5

#define HAL_CLK_PLL_CONFIGURATION_NONE  1
#define HAL_CLK_PLL_CONFIGURATION_40MHZ 2
#define HAL_CLK_PLL_CONFIGURATION_80MHZ 3

#define HAL_CLK_HFXO_AUTOSTART_NONE     0
#define HAL_CLK_HFXO_AUTOSTART_START    1
#define HAL_CLK_HFXO_AUTOSTART_SELECT   2

#define HAL_DISPLAY_SHARP_LS013B7DH03   1
#define HAL_DISPLAY_SHARP_LS013B7DH06   2

#define HAL_EMU_EM23_VSCALE_FASTWAKEUP  1
#define HAL_EMU_EM23_VSCALE_LOWPOWER    2

#define HAL_GPIO_DOUT_LOW               0
#define HAL_GPIO_DOUT_HIGH              1

#define HAL_GPIO_MODE_INPUT               gpioModeInput
#define HAL_GPIO_MODE_INPUT_PULL          gpioModeInputPull
#define HAL_GPIO_MODE_INPUT_PULL_FILTER   gpioModeInputPullFilter

#define HAL_I2C_PORT_I2C0          	  0x40
#define HAL_I2C_PORT_I2C1          	  0x41
#define HAL_I2C_PORT_I2C2          	  0x42
#define HAL_I2C_PORT_I2C3          	  0x43

#define HAL_LEUART_FLOW_CONTROL_NONE    0
#define HAL_LEUART_FLOW_CONTROL_SW      1
#define HAL_LEUART_FLOW_CONTROL_HW      2
#define HAL_LEUART_FLOW_CONTROL_HWUART  3

#define HAL_PTI_MODE_SPI                1
#define HAL_PTI_MODE_UART               2
#define HAL_PTI_MODE_UART_ONEWIRE       3

typedef uint8_t HalRadioConfig_t;
#define HAL_RADIO_CONFIG_154_2P4_DEFAULT          0
#define HAL_RADIO_CONFIG_154_2P4_ANT_DIV          1
#define HAL_RADIO_CONFIG_154_2P4_COEX             2
#define HAL_RADIO_CONFIG_154_2P4_ANT_DIV_COEX     3
#define HAL_RADIO_CONFIG_154_2P4_FEM              4
#define HAL_RADIO_CONFIG_154_2P4_ANT_DIV_FEM      5
#define HAL_RADIO_CONFIG_154_2P4_COEX_FEM         6
#define HAL_RADIO_CONFIG_154_2P4_ANT_DIV_COEX_FEM 7

#define HAL_SERIAL_PORT_VUART         0x10
#define HAL_SERIAL_PORT_USART0        0x20
#define HAL_SERIAL_PORT_USART1        0x21
#define HAL_SERIAL_PORT_USART2        0x22
#define HAL_SERIAL_PORT_USART3        0x23
#define HAL_SERIAL_PORT_USART4        0x24
#define HAL_SERIAL_PORT_USART5        0x25
#define HAL_SERIAL_PORT_UART0         0x30
#define HAL_SERIAL_PORT_UART1         0x31
#define HAL_SERIAL_PORT_LEUART0       0x40
#define HAL_SERIAL_PORT_LEUART1       0x41
#define HAL_SERIAL_PORT_EUART0        0x50
#define HAL_SERIAL_PORT_EUART1        0x51
#define HAL_SERIAL_PORT_EUSART0       0x60
#define HAL_SERIAL_PORT_EUSART1       0x61
#define HAL_SERIAL_PORT_EUSART2       0x62
#define HAL_SERIAL_PORT_EUSART3       0x63

#define HAL_SPI_PORT_USART0           0x20
#define HAL_SPI_PORT_USART1           0x21
#define HAL_SPI_PORT_USART2           0x22
#define HAL_SPI_PORT_USART3           0x23
#define HAL_SPI_PORT_USART4           0x24
#define HAL_SPI_PORT_USART5           0x25
#define HAL_SPI_PORT_EUSART0          0x60
#define HAL_SPI_PORT_EUSART1          0x61
#define HAL_SPI_PORT_EUSART2          0x62
#define HAL_SPI_PORT_EUSART3          0x63

#define HAL_UART_FLOW_CONTROL_NONE      0
#define HAL_UART_FLOW_CONTROL_SW        1
#define HAL_UART_FLOW_CONTROL_HW        2
#define HAL_UART_FLOW_CONTROL_HWUART    3

#define HAL_USART_FLOW_CONTROL_NONE     0
#define HAL_USART_FLOW_CONTROL_SW       1
#define HAL_USART_FLOW_CONTROL_HW       2
#define HAL_USART_FLOW_CONTROL_HWUART   3

#define HAL_PA_SELECTION_HP             0
#define HAL_PA_SELECTION_MP             1
#define HAL_PA_SELECTION_LP             2
#define HAL_PA_SELECTION_LLP            3
#define HAL_PA_SELECTION_HIGHEST        4

#define HAL_TIMER_TIMER0             0x20
#define HAL_TIMER_TIMER1             0x21
#define HAL_TIMER_TIMER2             0x22
#define HAL_TIMER_TIMER3             0x23
#define HAL_TIMER_TIMER4             0x24
#define HAL_TIMER_TIMER5             0x25
#define HAL_TIMER_TIMER6             0x26
#define HAL_TIMER_WTIMER0            0x30
#define HAL_TIMER_WTIMER1            0x31
#define HAL_TIMER_WTIMER2            0x32
#define HAL_TIMER_WTIMER3            0x33

#endif // HAL_CONFIG_TYPES_H
