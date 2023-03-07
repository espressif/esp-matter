/*******************************************************************************
* @file  rsi_hal.h
* @brief
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

#ifndef _RSIHAL_H_
#define _RSIHAL_H_

#ifndef RSI_SAMPLE_HAL
#include "hal/rsi_hal.h"
#else

#include <stdint.h>
/******************************************************
 * *                      Macros
 * ******************************************************/

//! User can configure this pin based on platform

//! GPIO to reset WiSeConnect Module
#define RSI_HAL_RESET_PIN                 1

// GPIO to receive packet pending interrupt
#define RSI_HAL_MODULE_INTERRUPT_PIN      2

// GPIO to receive module wakeup from power save indication
#define RSI_HAL_WAKEUP_INDICATION_PIN     3

// ULP GPIO to give sleep confirmation to module to goto sleep in power save
#define RSI_HAL_SLEEP_CONFIRM_PIN         4

// LP GPIO to give sleep confirmation to module to goto sleep in power save
#define RSI_HAL_LP_SLEEP_CONFIRM_PIN      6

//!ULP GPIO to receive module wakeup from wake on wireless indication
#define RSI_HAL_WOWLAN_INTR_FROM_DEV_PIN  7

//! ULP GPIO to give confirmation to module to goto wake on wireless mode
#define RSI_HAL_WOWLAN_CONFIRM_TO_DEV_PIN 8

//! GPIO Pins related Macros
//! Macro to configure GPIO in output mode
#define RSI_HAL_GPIO_OUTPUT_MODE          1

// Macro to configure GPIO in input mode
#define RSI_HAL_GPIO_INPUT_MODE           0

// Macro to drive low value on GPIO
#define RSI_HAL_GPIO_LOW                  0

// Macro to drive high value on GPIO
#define RSI_HAL_GPIO_HIGH                 1

//! Timer related macros
//! Macro to configure timer type in single shot
#define RSI_HAL_TIMER_TYPE_SINGLE_SHOT    0

// Macro to configure timer type in periodic
#define RSI_HAL_TIMER_TYPE_PERIODIC       1

// Macro to configure timer in micro seconds mode
#define RSI_HAL_TIMER_MODE_MICRO          0

// Macro to configure timer in milli seconds mode
#define RSI_HAL_TIMER_MODE_MILLI          1

/******************************************************
 * *                    Constants
 * ******************************************************/
/******************************************************
 * *                   Enumerations
 * ******************************************************/

/******************************************************
 * *                 Type Definitions
 * ******************************************************/

/******************************************************
 * *                    Structures
 * ******************************************************/

/******************************************************
 * *                 Global Variables
 * ******************************************************/
/******************************************************
 * *               Function Declarations
 * ******************************************************/

void rsi_hal_board_init(void);
void rsi_switch_to_high_clk_freq(void);
void rsi_hal_intr_config(void (*rsi_interrupt_handler)(void));
void rsi_hal_intr_mask(void);
void rsi_hal_intr_unmask(void);
void rsi_hal_intr_clear(void);
uint8_t rsi_hal_intr_pin_status(void);
void rsi_hal_config_gpio(uint8_t gpio_number, uint8_t mode, uint8_t value);
void rsi_hal_set_gpio(uint8_t gpio_number);
uint8_t rsi_hal_get_gpio(uint8_t gpio_number);
void rsi_hal_clear_gpio(uint8_t gpio_number);
int16_t rsi_spi_transfer(uint8_t *tx_buff, uint8_t *rx_buff, uint16_t transfer_length, uint8_t mode);
int16_t rsi_uart_send(uint8_t *ptrBuf, uint16_t bufLen);
int16_t rsi_uart_recv(uint8_t *ptrBuf, uint16_t bufLen);
int16_t rsi_com_port_send(uint8_t *ptrBuf, uint16_t bufLen);
int16_t rsi_com_port_receive(uint8_t *ptrBuf, uint16_t bufLen);
uint32_t rsi_get_random_number(void);
int32_t rsi_timer_start(uint8_t timer_node,
                        uint8_t mode,
                        uint8_t type,
                        uint32_t duration,
                        void (*rsi_timer_expiry_handler)(void));
int32_t rsi_timer_stop(uint8_t timer_node);
uint32_t rsi_timer_read(uint8_t timer_node);
void rsi_delay_us(uint32_t delay_us);
void rsi_delay_ms(uint32_t delay_ms);
uint32_t rsi_hal_gettickcount(void);
void rsi_hal_enable_uart_irq(void);
void SysTick_Handler(void);
void rsi_hal_enable_uart_irq(void);

void rsi_interrupt_handler(void);
void ABRD(void);

void Error_Handler(void);

/* RTC Related API's */
uint32_t rsi_rtc_get_time(void);
int32_t rsi_rtc_set_time(uint32_t time);
/* End - RTC Related API's */

#ifdef LOGGING_STATS
void rsi_hal_log_stats_intr_config(void (*rsi_give_wakeup_indication)());
#endif
#ifdef RSI_WITH_OS
void rsi_os_delay_ms(uint32_t delay_ms);
void PORTD_IRQHandler(void);

#endif
#endif
#endif
