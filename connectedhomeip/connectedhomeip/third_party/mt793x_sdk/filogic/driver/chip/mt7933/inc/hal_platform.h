/* Copyright Statement:
 *
 * (C) 2005-2017 MediaTek Inc. All rights reserved.
 *
 * This software/firmware and related documentation ("MediaTek Software") are
 * protected under relevant copyright laws. The information contained herein
 * is confidential and proprietary to MediaTek Inc. ("MediaTek") and/or its licensors.
 * Without the prior written permission of MediaTek and/or its licensors,
 * any reproduction, modification, use or disclosure of MediaTek Software,
 * and information contained herein, in whole or in part, shall be strictly prohibited.
 * You may only use, reproduce, modify, or distribute (as applicable) MediaTek Software
 * if you have agreed to and been bound by the applicable license agreement with
 * MediaTek ("License Agreement") and been granted explicit permission to do so within
 * the License Agreement ("Permitted User"). If you are not a Permitted User,
 * please cease any access or use of MediaTek Software immediately.
 * BY OPENING THIS FILE, RECEIVER HEREBY UNEQUIVOCALLY ACKNOWLEDGES AND AGREES
 * THAT MEDIATEK SOFTWARE RECEIVED FROM MEDIATEK AND/OR ITS REPRESENTATIVES
 * ARE PROVIDED TO RECEIVER ON AN "AS-IS" BASIS ONLY. MEDIATEK EXPRESSLY DISCLAIMS ANY AND ALL
 * WARRANTIES, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE OR NONINFRINGEMENT.
 * NEITHER DOES MEDIATEK PROVIDE ANY WARRANTY WHATSOEVER WITH RESPECT TO THE
 * SOFTWARE OF ANY THIRD PARTY WHICH MAY BE USED BY, INCORPORATED IN, OR
 * SUPPLIED WITH MEDIATEK SOFTWARE, AND RECEIVER AGREES TO LOOK ONLY TO SUCH
 * THIRD PARTY FOR ANY WARRANTY CLAIM RELATING THERETO. RECEIVER EXPRESSLY ACKNOWLEDGES
 * THAT IT IS RECEIVER'S SOLE RESPONSIBILITY TO OBTAIN FROM ANY THIRD PARTY ALL PROPER LICENSES
 * CONTAINED IN MEDIATEK SOFTWARE. MEDIATEK SHALL ALSO NOT BE RESPONSIBLE FOR ANY MEDIATEK
 * SOFTWARE RELEASES MADE TO RECEIVER'S SPECIFICATION OR TO CONFORM TO A PARTICULAR
 * STANDARD OR OPEN FORUM. RECEIVER'S SOLE AND EXCLUSIVE REMEDY AND MEDIATEK'S ENTIRE AND
 * CUMULATIVE LIABILITY WITH RESPECT TO MEDIATEK SOFTWARE RELEASED HEREUNDER WILL BE,
 * AT MEDIATEK'S OPTION, TO REVISE OR REPLACE MEDIATEK SOFTWARE AT ISSUE,
 * OR REFUND ANY SOFTWARE LICENSE FEES OR SERVICE CHARGE PAID BY RECEIVER TO
 * MEDIATEK FOR SUCH MEDIATEK SOFTWARE AT ISSUE.
 */

#ifndef __HAL_PLATFORM_H__
#define __HAL_PLATFORM_H__


#include "hal_define.h"
#include "hal_feature_config.h"
#include "mt7933.h"
//#include "hal_weak.h"
//#include "memory_map.h"

#ifdef __cplusplus
extern "C" {
#endif /* #ifdef __cplusplus */

#ifdef HAL_AUDIO_MODULE_ENABLED
#define HAL_AUDIO_SUPPORT_MULTIPLE_STREAM_OUT    /**/
#endif /* #ifdef HAL_AUDIO_MODULE_ENABLED */

#ifndef ATTR_HAL_DEPRECATED
#define  ATTR_HAL_DEPRECATED                  __attribute__((deprecated))
#endif /* #ifndef ATTR_HAL_DEPRECATED */

#ifdef HAL_GPT_MODULE_ENABLED
#define HAL_GPT_FEATURE_US_TIMER               /* Supports a microsecond timer. */
#define HAL_GPT_SW_GPT_FEATURE                 /* Supports software GPT timer. */
#define HAL_GPT_PORT_ALLOCATE                  /* Allocates GPT communication port. */
#endif /* #ifdef HAL_GPT_MODULE_ENABLED */

#ifdef HAL_SPI_MASTER_MODULE_ENABLED
#define HAL_SPI_MASTER_FEATURE_ADVANCED_CONFIG       /* Enable SPI master advanced configuration feature. For more details, please refer to hal_spi_master.h.*/
#define HAL_SPI_MASTER_FEATURE_DEASSERT_CONFIG       /* Enable SPI master deassert configuration feature to deassert the chip select signal after each byte data transfer is complete. For more details, please refer to hal_spi_master.h.*/
#define HAL_SPI_MASTER_FEATURE_CHIP_SELECT_TIMING    /* Enable SPI master chip select timing configuration feature to set timing value for chip select signal. For more details, please refer to hal_spi_master.h.*/
#define HAL_SPI_MASTER_FEATURE_DMA_MODE              /* Enable SPI master DMA mode feature to do data transfer. For more details, please refer to hal_spi_master.h.*/
#endif /* #ifdef HAL_SPI_MASTER_MODULE_ENABLED */

#ifdef HAL_SPI_SLAVE_MODULE_ENABLED
#define HAL_SPI_SLAVE_FEATURE_SW_CONTROL        /* Supports SPI slave to communicate with SPI master using software control. */
#endif /* #ifdef HAL_SPI_SLAVE_MODULE_ENABLED */

//#ifdef HAL_UART_MODULE_ENABLED
/**
 * @addtogroup HAL
 * @{
 * @addtogroup UART
 * @{
 * @addtogroup hal_uart_enum Enum
 * @{
 */
/*****************************************************************************
* UART
*****************************************************************************/
/** @brief UART port index
 * There are total of four UART ports. Only UART0 and UART1 support hardware flow control.
 * | UART port | Hardware Flow Control |
 * |-----------|-----------------------|
 * |  UART0    |           V           |
 * |  UART1    |           V           |
 * |  UART2    |           X           |
 * |  UART3    |           X           |
 */
typedef enum {
    HAL_UART_PORT_INVALID = -1,
    HAL_UART_0 = 0,                            /**< UART port 0. */
    HAL_UART_1 = 1,                            /**< UART port 1. */
    HAL_UART_2 = 2,                            /**< UART port 2. */
    HAL_UART_3 = 3,                            /**< UART port 3. */
    HAL_UART_MAX                               /**< The total number of UART ports (invalid UART port number). */
} hal_uart_port_t;

/**
  * @}
  */

/**@addtogroup hal_uart_define Define
 * @{
  */

/** @brief  The maximum timeout value for UART timeout interrupt, unit is ms.
  */
#define HAL_UART_TIMEOUT_VALUE_MAX  (2500)

/**
  * @}
  */

/**
 * @}
 * @}
 */
//#endif

/** @brief GPT port */
typedef enum {
    HAL_GPT_PORT_INVALID = -1,
    HAL_GPT_0 = 0,                          /**< GPT port 0. */
    HAL_GPT_1 = 1,                          /**< GPT port 1. */
    HAL_GPT_2 = 2,                          /**< GPT port 2. */
    HAL_GPT_3 = 3,                          /**< GPT port 3. */
    HAL_GPT_4 = 4,                          /**< GPT port 4. */
    HAL_GPT_5 = 5,                          /**< GPT port 5. */
    HAL_GPT_MAX_PORT = 6                    /**< The total number of GPT ports (invalid GPT port). */
} hal_gpt_port_t;

/** @brief GPT clock source */
typedef enum {
    HAL_GPT_CLOCK_SOURCE_32K = 0,            /**< Set the GPT clock source to 32kHz, 1 tick = 1/32768 seconds. */
    HAL_GPT_CLOCK_SOURCE_1M  = 1             /**< Set the GPT clock source to 1MHz, 1 tick = 1 microsecond.*/
} hal_gpt_clock_source_t;

/** @brief IRQ type */
typedef enum {
    HAL_NVIC_IRQ_TYPE_LEVEL_HIGH = 0,
    HAL_NVIC_IRQ_TYPE_EDGE_RISING = 1
} hal_nvic_irq_type_t;

#define HAL_NVIC_IRQ_SET_TYPE_FEATURE   1

#ifdef HAL_RTC_MODULE_ENABLED
/**
 * @addtogroup HAL
 * @{
 * @addtogroup RTC
 * @{
 * @addtogroup hal_rtc_define Define
 * @{
 */

/** @brief  This macro defines a maximum number for backup data that used in #hal_rtc_set_data(),
  * #hal_rtc_get_data(), #hal_rtc_clear_data functions.
  */
#define HAL_RTC_BACKUP_BYTE_NUM_MAX     (512)

/** @brief  This macro defines a maximum number for RTC retention sram cells.
  */
#define HAL_RTC_RETENTION_SRAM_NUMBER_MAX     (0)

/** @brief  This macro defines the mask value for all RTC retention sram cells.
  */
#define HAL_RTC_RETENTION_SRAM_NUMBER_MASK     (0xff)

/**
 * @}
 */

/** @defgroup hal_rtc_struct Struct
  * @{
  */

/** @brief RTC time structure definition. */
typedef struct {
    uint8_t rtc_sec;                                  /**< Seconds after minutes   - [0,59]  */
    uint8_t rtc_min;                                  /**< Minutes after the hour  - [0,59]  */
    uint8_t rtc_hour;                                 /**< Hours after midnight    - [0,23]  */
    uint8_t rtc_day;                                  /**< Day of the month        - [1,31]  */
    uint8_t rtc_mon;                                  /**< Months                  - [1,12]  */
    uint8_t rtc_week;                                 /**< Days in a week          - [0,6]   */
    uint8_t rtc_year;                                 /**< Years                   - [0,127] */
} hal_rtc_time_t;
/**
  * @}
  */

/**
 * @defgroup hal_rtc_enum Enum
 * @{
 */

/** @brief This enum defines the mode of rtc retention sram. */
typedef enum {
    HAL_RTC_SRAM_NORMAL_MODE = 0,     /**< Change SRAM mode to normal mode. */
    HAL_RTC_SRAM_SLEEP_MODE = 1,      /**< Change SRAM mode to sleep mode. */
    HAL_RTC_SRAM_PD_MODE = 2,         /**< Change SRAM mode power down mode. */
} hal_rtc_sram_mode_t;

/** @brief RTC current time change notification period selections. */
typedef enum {
    HAL_RTC_TIME_NOTIFICATION_NONE = 0,                     /**< No need for a time notification. */
    HAL_RTC_TIME_NOTIFICATION_EVERY_SECOND = 1,             /**< Execute a callback function set by #hal_rtc_set_time_notification_period() for every second. */
    HAL_RTC_TIME_NOTIFICATION_EVERY_MINUTE = 2,             /**< Execute a callback function set by #hal_rtc_set_time_notification_period() for every minute. */
    HAL_RTC_TIME_NOTIFICATION_EVERY_HOUR = 3,               /**< Execute a callback function set by #hal_rtc_set_time_notification_period() for every hour. */
    HAL_RTC_TIME_NOTIFICATION_EVERY_DAY = 4,                /**< Execute a callback function set by #hal_rtc_set_time_notification_period() for every day. */
    HAL_RTC_TIME_NOTIFICATION_EVERY_MONTH = 5,              /**< Execute a callback function set by #hal_rtc_set_time_notification_period() for every month. */
    HAL_RTC_TIME_NOTIFICATION_EVERY_YEAR = 6,               /**< Execute a callback function set by #hal_rtc_set_time_notification_period() for every year. */
    HAL_RTC_TIME_NOTIFICATION_EVERY_SECOND_1_2 = 7,         /**< Execute a callback function set by #hal_rtc_set_time_notification_period() for every one-half of a second. */
    HAL_RTC_TIME_NOTIFICATION_EVERY_SECOND_1_4 = 8,         /**< Execute a callback function set by #hal_rtc_set_time_notification_period() for every one-fourth of a second. */
    HAL_RTC_TIME_NOTIFICATION_EVERY_SECOND_1_8 = 9          /**< Execute a callback function set by #hal_rtc_set_time_notification_period() for every one-eighth of a second. */
} hal_rtc_time_notification_period_t;

/**
 * @}
 */

/**
 * @}
 * @}
 * @}
 */
#endif /* #ifdef HAL_RTC_MODULE_ENABLED */

//Temp, For Porting Only
#ifndef HAL_UART_MODULE_ENABLED
/** @defgroup hal_uart_enum Enum
  * @{
  */

#define HAL_UART_FEATURE_3M_BAUDRATE
/** @brief This enum defines baud rate of the UART frame. */
typedef enum {
    HAL_UART_BAUDRATE_110 = 0,           /**< Defines UART baudrate as 110 bps. */
    HAL_UART_BAUDRATE_300 = 1,           /**< Defines UART baudrate as 300 bps. */
    HAL_UART_BAUDRATE_1200 = 2,          /**< Defines UART baudrate as 1200 bps. */
    HAL_UART_BAUDRATE_2400 = 3,          /**< Defines UART baudrate as 2400 bps. */
    HAL_UART_BAUDRATE_4800 = 4,          /**< Defines UART baudrate as 4800 bps. */
    HAL_UART_BAUDRATE_9600 = 5,          /**< Defines UART baudrate as 9600 bps. */
    HAL_UART_BAUDRATE_19200 = 6,         /**< Defines UART baudrate as 19200 bps. */
    HAL_UART_BAUDRATE_38400 = 7,         /**< Defines UART baudrate as 38400 bps. */
    HAL_UART_BAUDRATE_57600 = 8,         /**< Defines UART baudrate as 57600 bps. */
    HAL_UART_BAUDRATE_115200 = 9,        /**< Defines UART baudrate as 115200 bps. */
    HAL_UART_BAUDRATE_230400 = 10,       /**< Defines UART baudrate as 230400 bps. */
    HAL_UART_BAUDRATE_460800 = 11,       /**< Defines UART baudrate as 460800 bps. */
    HAL_UART_BAUDRATE_921600 = 12,       /**< Defines UART baudrate as 921600 bps. */
#ifdef HAL_UART_FEATURE_3M_BAUDRATE
    HAL_UART_BAUDRATE_3000000 = 13,      /**< Defines UART baudrate as 3000000 bps. */
#endif /* #ifdef HAL_UART_FEATURE_3M_BAUDRATE */
    HAL_UART_BAUDRATE_MAX                /**< Defines maximum enum value of UART baudrate. */
} hal_uart_baudrate_t;


/** @brief This enum defines word length of the UART frame. */
typedef enum {
    HAL_UART_WORD_LENGTH_5 = 0,           /**< Defines UART word length as 5 bits per frame. */
    HAL_UART_WORD_LENGTH_6 = 1,           /**< Defines UART word length as 6 bits per frame. */
    HAL_UART_WORD_LENGTH_7 = 2,           /**< Defines UART word length as 7 bits per frame. */
    HAL_UART_WORD_LENGTH_8 = 3            /**< Defines UART word length as 8 bits per frame. */
} hal_uart_word_length_t;


/** @brief This enum defines stop bit of the UART frame. */
typedef enum {
    HAL_UART_STOP_BIT_1 = 0,              /**< Defines UART stop bit as 1 bit per frame. */
    HAL_UART_STOP_BIT_2 = 1,              /**< Defines UART stop bit as 2 bits per frame. */
} hal_uart_stop_bit_t;


/** @brief This enum defines parity of the UART frame. */
typedef enum {
    HAL_UART_PARITY_NONE = 0,            /**< Defines UART parity as none. */
    HAL_UART_PARITY_ODD = 1,             /**< Defines UART parity as odd. */
    HAL_UART_PARITY_EVEN = 2             /**< Defines UART parity as even. */
} hal_uart_parity_t;


/** @brief This enum defines return status of the UART HAL public API. User should check return value after calling these APIs. */
typedef enum {
    HAL_UART_STATUS_ERROR_PARAMETER = -4,      /**< Invalid user input parameter. */
    HAL_UART_STATUS_ERROR_BUSY = -3,           /**< UART port is currently in use. */
    HAL_UART_STATUS_ERROR_UNINITIALIZED = -2,  /**< UART port has not been initialized. */
    HAL_UART_STATUS_ERROR = -1,                /**< UART driver detected a common error. */
    HAL_UART_STATUS_OK = 0                     /**< UART function executed successfully. */
} hal_uart_status_t;


/** @brief This enum defines the UART event when an interrupt occurs. */
typedef enum {
    HAL_UART_EVENT_TRANSACTION_ERROR = -1,          /**< Indicates if there is a transaction error when receiving data. */
    HAL_UART_EVENT_READY_TO_READ = 1,               /**< Indicates if there is enough data available in the RX buffer for the user to read from. */
    HAL_UART_EVENT_READY_TO_WRITE = 2               /**< Indicates if there is enough free space available in the TX buffer for the user to write into. */
} hal_uart_callback_event_t;


/**
  * @}
  */


/** @defgroup hal_uart_struct Struct
  * @{
  */


/** @brief This struct defines UART configure parameters. */
typedef struct {
    hal_uart_baudrate_t baudrate;              /**< This field represents the baudrate of the UART frame. */
    hal_uart_word_length_t word_length;        /**< This field represents the word length of the UART frame. */
    hal_uart_stop_bit_t stop_bit;              /**< This field represents the stop bit of the UART frame. */
    hal_uart_parity_t parity;                  /**< This field represents the parity of the UART frame. */
} hal_uart_config_t;


/** @brief This struct defines configuration parameters and TX/RX buffers for the VFIFO DMA associated with a specific UART channel. */
typedef struct {
    uint8_t *send_vfifo_buffer;                /**< This field represents the transmitting user allocated VFIFO buffer. It will only be used by the UART driver to send data, must be non-cacheable and aligned to 4bytes. */
    uint32_t send_vfifo_buffer_size;           /**< This field represents the size of the transmitting VFIFO buffer. */
    uint32_t send_vfifo_threshold_size;        /**< This field represents the threshold of the transmitting VFIFO buffer. VFIFO DMA will trigger an interrupt when the available bytes in VFIFO buffer are lower than the threshold. */
    uint8_t *receive_vfifo_buffer;             /**< This field represents the receiving user allocated VFIFO buffer. It will only be used by the UART driver for receiving data, and must be non-cacheable and align to 4bytes. */
    uint32_t receive_vfifo_buffer_size;        /**< This field represents size of the receiving VFIFO buffer. */
    uint32_t receive_vfifo_threshold_size;     /**< This field represents the threshold of the receiving VFIFO buffer. VFIFO DMA will trigger receive interrupt when available bytes in VFIFO buffer are more than the threshold. */
    uint32_t receive_vfifo_alert_size;         /**< This field represents the threshold size of free space left in the VFIFO buffer that activates the UART's flow control system. */
} hal_uart_dma_config_t;
#endif /* #ifndef HAL_UART_MODULE_ENABLED */

#ifdef HAL_GPIO_MODULE_ENABLED

#define HAL_GPIO_FEATURE_PUPD              /* Pull state of the pin can be configured with different resistors through different combinations of GPIO_PUPD_x,GPIO_RESEN0_x and GPIO_RESEN1_x. For more details, please refer to hal_gpio.h. */
#define HAL_GPIO_FEATURE_HIGH_Z            /* The pin can be configured to provide high impedance state to prevent possible electric leakage. For more details, please refer to hal_gpio.h. */
#define HAL_GPIO_FEATURE_SET_DRIVING       /* The pin can be configured to enhance driving. For more details, please refer to hal_gpio.h. */

/**
* @addtogroup HAL
* @{
* @addtogroup GPIO
* @{
*
* @addtogroup hal_gpio_enum Enum
* @{
*/

/*****************************************************************************
* GPIO
*****************************************************************************/
/** @brief This enum defines the GPIO port.
 *  The platform supports a total of 49 GPIO pins with various functionality.
 *
*/

typedef enum {
    HAL_GPIO_0 = 0,                            /**< GPIO pin0 */
    HAL_GPIO_1 = 1,                            /**< GPIO pin1 */
    HAL_GPIO_2 = 2,                            /**< GPIO pin2 */
    HAL_GPIO_3 = 3,                            /**< GPIO pin3 */
    HAL_GPIO_4 = 4,                            /**< GPIO pin4 */
    HAL_GPIO_5 = 5,                            /**< GPIO pin5 */
    HAL_GPIO_6 = 6,                            /**< GPIO pin6 */
    HAL_GPIO_7 = 7,                            /**< GPIO pin7 */
    HAL_GPIO_8 = 8,                            /**< GPIO pin8 */
    HAL_GPIO_9 = 9,                            /**< GPIO pin9 */
    HAL_GPIO_10 = 10,                          /**< GPIO pin10 */
    HAL_GPIO_11 = 11,                          /**< GPIO pin11 */
    HAL_GPIO_12 = 12,                          /**< GPIO pin12 */
    HAL_GPIO_13 = 13,                          /**< GPIO pin13 */
    HAL_GPIO_14 = 14,                          /**< GPIO pin14 */
    HAL_GPIO_15 = 15,                          /**< GPIO pin15 */
    HAL_GPIO_16 = 16,                          /**< GPIO pin16 */
    HAL_GPIO_17 = 17,                          /**< GPIO pin17 */
    HAL_GPIO_18 = 18,                          /**< GPIO pin18 */
    HAL_GPIO_19 = 19,                          /**< GPIO pin19 */
    HAL_GPIO_20 = 20,                          /**< GPIO pin20 */
    HAL_GPIO_21 = 21,                          /**< GPIO pin21 */
    HAL_GPIO_22 = 22,                          /**< GPIO pin22 */
    HAL_GPIO_23 = 23,                          /**< GPIO pin23 */
    HAL_GPIO_24 = 24,                          /**< GPIO pin24 */
    HAL_GPIO_25 = 25,                          /**< GPIO pin25 */
    HAL_GPIO_26 = 26,                          /**< GPIO pin26 */
    HAL_GPIO_27 = 27,                          /**< GPIO pin27 */
    HAL_GPIO_28 = 28,                          /**< GPIO pin28 */
    HAL_GPIO_29 = 29,                          /**< GPIO pin29 */
    HAL_GPIO_30 = 30,                          /**< GPIO pin30 */
    HAL_GPIO_31 = 31,                          /**< GPIO pin31 */
    HAL_GPIO_32 = 32,                          /**< GPIO pin32 */
    HAL_GPIO_33 = 33,                          /**< GPIO pin33 */
    HAL_GPIO_34 = 34,                          /**< GPIO pin34 */
    HAL_GPIO_35 = 35,                          /**< GPIO pin35 */
    HAL_GPIO_36 = 36,                          /**< GPIO pin36 */
    HAL_GPIO_37 = 37,                          /**< GPIO pin37 */
    HAL_GPIO_38 = 38,                          /**< GPIO pin38 */
    HAL_GPIO_39 = 39,                          /**< GPIO pin39 */
    HAL_GPIO_40 = 40,                          /**< GPIO pin40 */
    HAL_GPIO_41 = 41,                          /**< GPIO pin41 */
    HAL_GPIO_42 = 42,                          /**< GPIO pin42 */
    HAL_GPIO_43 = 43,                          /**< GPIO pin43 */
    HAL_GPIO_44 = 44,                          /**< GPIO pin44 */
    HAL_GPIO_45 = 45,                          /**< GPIO pin45 */
    HAL_GPIO_46 = 46,                          /**< GPIO pin46 */
    HAL_GPIO_47 = 47,                          /**< GPIO pin47 */
    HAL_GPIO_48 = 48,                          /**< GPIO pin48 */
    HAL_GPIO_49 = 49,                          /**< GPIO pin49 */
    HAL_GPIO_50 = 50,                          /**< GPIO pin50 */
    HAL_GPIO_51 = 51,                          /**< GPIO pin51 */
    HAL_GPIO_52 = 52,                          /**< GPIO pin52 */
    HAL_GPIO_MAX                               /**< The total number of GPIO pins (invalid GPIO pin number). */
} hal_gpio_pin_t;
/**
* @}
*/


/**
 * @}
 * @}
 */
#endif /* #ifdef HAL_GPIO_MODULE_ENABLED */

#ifdef HAL_I2C_MASTER_MODULE_ENABLED
#define HAL_I2C_MASTER_FEATURE_SEND_TO_RECEIVE
#define HAL_I2C_MASTER_FEATURE_EXTENDED_DMA


/**
 * @addtogroup HAL
 * @{
 * @addtogroup I2C_MASTER
 * @{
 * @defgroup hal_i2c_master_define Define
 * @{
 */

/** @brief  The maximum polling mode transaction size.
  */
#define HAL_I2C_MAXIMUM_POLLING_TRANSACTION_SIZE  16

/** @brief  The maximum DMA mode transaction size.
  */
#define HAL_I2C_MAXIMUM_DMA_TRANSACTION_SIZE   0xFF
/**
* @}
*/

/**
 * @}
 * @}
 */


/** @addtogroup hal_i2c_master_enum Enum
  * @{
  */

/*****************************************************************************
* I2C master
*****************************************************************************/
/** @brief This enum defines the I2C port.
 *  The platform supports two I2C masters. Both of them support polling mode and DMA mode.
 *  For more information about the polling mode and DMA mode, please refer to @ref HAL_I2C_Features_Chapter.
 *  The details are shown below:
 *  - Supported features of I2C masters \n
 *    V : supported;  X : not supported.
 * |I2C Master   | Polling mode | DMA mode | Extended DMA mode |
 * |-------------|--------------|----------|-------------------|
 * |I2C0         |      V       |    V     |         x         |
 * |I2C1         |      V       |    V     |         x         |
 *
 *
*/
typedef enum {
    HAL_I2C_MASTER_0 = 0,                /**< I2C master 0. */
    HAL_I2C_MASTER_1 = 1,                /**< I2C master 1. */
    HAL_I2C_MASTER_MAX                   /**< The total number of I2C masters (invalid I2C master number). */
} hal_i2c_port_t;

/**
  * @}
  */

#endif /* #ifdef HAL_I2C_MASTER_MODULE_ENABLED */

#ifdef HAL_EINT_MODULE_ENABLED
/**
 * @addtogroup HAL
 * @{
 * @addtogroup EINT
 * @{
 * @addtogroup hal_eint_enum Enum
 * @{
 */

/*****************************************************************************
* EINT
*****************************************************************************/
/** @brief EINT pins. */
typedef enum {
    HAL_EINT_NUMBER_0 = 0,
    HAL_EINT_NUMBER_1 = 1,
    HAL_EINT_NUMBER_2 = 2,
    HAL_EINT_NUMBER_3 = 3,
    HAL_EINT_NUMBER_4 = 4,
    HAL_EINT_NUMBER_5 = 5,
    HAL_EINT_NUMBER_6 = 6,
    HAL_EINT_NUMBER_7 = 7,
    HAL_EINT_NUMBER_8 = 8,
    HAL_EINT_NUMBER_9 = 9,
    HAL_EINT_NUMBER_10 = 10,
    HAL_EINT_NUMBER_11 = 11,
    HAL_EINT_NUMBER_12 = 12,
    HAL_EINT_NUMBER_13 = 13,
    HAL_EINT_NUMBER_14 = 14,
    HAL_EINT_NUMBER_15 = 15,
    HAL_EINT_NUMBER_16 = 16,
    HAL_EINT_NUMBER_17 = 17,
    HAL_EINT_NUMBER_18 = 18,
    HAL_EINT_NUMBER_19 = 19,
    HAL_EINT_NUMBER_20 = 20,
    HAL_EINT_NUMBER_21 = 21,
    HAL_EINT_NUMBER_22 = 22,
    HAL_EINT_NUMBER_23 = 23,
    HAL_EINT_NUMBER_24 = 24,
    HAL_EINT_NUMBER_25 = 25,
    HAL_EINT_NUMBER_26 = 26,
    HAL_EINT_NUMBER_27 = 27,
    HAL_EINT_NUMBER_28 = 28,
    HAL_EINT_NUMBER_29 = 29,
    HAL_EINT_NUMBER_30 = 30,
    HAL_EINT_NUMBER_MAX           /**< The total number of EINT channels (invalid EINT channel). */
} hal_eint_number_t;

/**
  * @}
  */
/**
 * @}
 * @}
 */
#endif /* #ifdef HAL_EINT_MODULE_ENABLED */


#ifdef HAL_ADC_MODULE_ENABLED
/**
* @addtogroup HAL
* @{
* @addtogroup ADC
* @{
*
* @addtogroup hal_adc_enum Enum
* @{
*/

/*****************************************************************************
* ADC
*****************************************************************************/
/** @brief adc channel */
typedef enum {
    HAL_ADC_CHANNEL_0 = 0,                        /**< ADC channel 0. */
    HAL_ADC_CHANNEL_1 = 1,                        /**< ADC channel 1. */
    HAL_ADC_CHANNEL_2 = 2,                        /**< ADC channel 2. */
    HAL_ADC_CHANNEL_3 = 3,                        /**< ADC channel 3. */
    HAL_ADC_CHANNEL_4 = 4,                        /**< ADC channel 4. */
    HAL_ADC_CHANNEL_5 = 5,                        /**< ADC channel 5. */
    HAL_ADC_CHANNEL_6 = 6,                        /**< ADC channel 6. */
    HAL_ADC_CHANNEL_7 = 7,                        /**< ADC channel 7. */
    HAL_ADC_CHANNEL_8 = 8,                        /**< ADC channel 8. */
    HAL_ADC_CHANNEL_9 = 9,                        /**< ADC channel 9. */
    HAL_ADC_CHANNEL_10 = 10,                     /**< ADC channel 10. */
    HAL_ADC_CHANNEL_11 = 11,                     /**< ADC channel 11. */
    HAL_ADC_CHANNEL_MAX                           /**< The total number of ADC channels (invalid ADC channel).*/
} hal_adc_channel_t;

/**
  * @}
  */


/**
 * @}
 * @}
 */

#endif /* #ifdef HAL_ADC_MODULE_ENABLED */

#ifdef HAL_GDMA_MODULE_ENABLED
/**
 * @addtogroup HAL
 * @{
 * @addtogroup GDMA
 * @{
 * @addtogroup hal_gdma_enum Enum
 * @{
 */
/*****************************************************************************
* gdma
*****************************************************************************/
/** @brief gdma channel */
typedef enum {
    HAL_GDMA_CHANNEL_0 = 0,                        /**< GDMA channel 0. */
    HAL_GDMA_CHANNEL_1,
    HAL_GDMA_CHANNEL_2,
    HAL_GDMA_CHANNEL_MAX                           /**< The total number of GDMA channels (invalid GDMA channel). */
} hal_gdma_channel_t;

/**
  * @}
  */


/**
 * @}
 * @}
 */
#endif /* #ifdef HAL_GDMA_MODULE_ENABLED */

#ifdef HAL_PWM_MODULE_ENABLED
/**
 * @addtogroup HAL
 * @{
 * @addtogroup PWM
 * @{
 * @addtogroup hal_pwm_enum Enum
 * @{
 */
/*****************************************************************************
* pwm
*****************************************************************************/
/** @defgroup os_hal_pwm_enum Enum
  * @{
  * OS-HAL PWM enumeration value list information, including
  * define of  the PWM group number enumeration, the PWM
  * channel number enumeration and  define of the PWM
  * running status enumeration.
  */

/** @brief Defines the PWM group number */
typedef enum {
    PWM_GROUP0  = 0,
    /**< PWM group0*/
    PWM_GROUP1  = 1,
    /**< PWM group1*/
    PWM_GROUP2  = 2,
    /**< PWM group2*/
    PWM_GROUP_MAX
    /**< PWM max group <invalid>*/
}   pwm_groups;

/** @brief Defines the PWM channel bit map */
typedef enum {
    PWM_0_BIT = 1,
    /**< PWM channel0 bit map*/
    PWM_1_BIT = 2,
    /**< PWM channel1 bit map*/
    PWM_2_BIT = 4,
    /**< PWM channel2 bit map*/
    PWM_3_BIT = 8,
    /**< PWM channel3 bit map*/
    PWM_MAP_MAX
    /**< PWM max channel  bit map<invalid>*/
}   pwms_bit_map;
/**
  * @}
  */

/** @brief pwm channel */
typedef enum {
    HAL_PWM_0  = 0,                        /**< pwm channe0 */
    HAL_PWM_1  = 1,                        /**< pwm channe1 */
    HAL_PWM_2  = 2,                        /**< pwm channe2 */
    HAL_PWM_3  = 3,                        /**< pwm channe3 */
    HAL_PWM_4  = 4,                        /**< pwm channe4 */
    HAL_PWM_5  = 5,                        /**< pwm channe5 */
    HAL_PWM_6  = 6,                        /**< pwm channe6 */
    HAL_PWM_7  = 7,                        /**< pwm channe7 */
    HAL_PWM_8  = 8,                        /**< pwm channe8 */
    HAL_PWM_9  = 9,                        /**< pwm channe9 */
    HAL_PWM_10 = 10,                       /**< pwm channe10*/
    HAL_PWM_11 = 11,                       /**< pwm channe11*/
    HAL_PWM_MAX                            /**< pwm max channel <invalid>*/
} hal_pwm_channel_t;

/** @brief pwm clock source seletion */
typedef enum {
    HAL_PWM_CLOCK_32KHZ  = 0,               /**< pwm clock source: Embedded 32KHz clock */
    HAL_PWM_CLOCK_2MHZ   = 1,               /**< pwm clock srouce: Embedded 2MHz  clock */
    HAL_PWM_CLOCK_26MHZ  = 2,               /**< pwm clock srouce: External 26MHz clock */
    HAL_PWM_CLOCK_MAX                       /**< pwm max clock <invalid>*/
} hal_pwm_source_clock_t ;

/**
  * @}
  */


/**
 * @}
 * @}
 */
#endif /* #ifdef HAL_PWM_MODULE_ENABLED */

#ifdef HAL_SPI_MASTER_MODULE_ENABLED
/**
 * @addtogroup HAL
 * @{
 * @addtogroup SPI_MASTER
 * @{
 * @defgroup hal_spi_master_define Define
 * @{
 */

/** @brief  The maximum polling mode transaction size in bytes.
 */
#define HAL_SPI_MAXIMUM_POLLING_TRANSACTION_SIZE  32

/** @brief  The minimum clock frequency.
 */
#define  HAL_SPI_MASTER_CLOCK_MIN_FREQUENCY  198

/** @brief  The maximum clock frequency.
 */
#define  HAL_SPI_MASTER_CLOCK_MAX_FREQUENCY  50000000

/**
 * @}
 */

/**
 * @addtogroup hal_spi_master_enum Enum
 * @{
 */

/*****************************************************************************
* SPI master
*****************************************************************************/
/** @brief This enum defines the SPI master port.
 *  The chip supports total of 4 SPI master ports, each of them supports polling mode
 *  and DMA mode. For more details about polling mode and DMA mode, please refer to @ref
 *  HAL_SPI_MASTER_Features_Chapter.
 */
typedef enum {
    HAL_SPI_MASTER_INVALID = -1,
    HAL_SPI_MASTER_0 = 0,                              /**< SPI master port 0. */
    HAL_SPI_MASTER_1 = 1,                              /**< SPI master port 1. */
    HAL_SPI_MASTER_MAX                                 /**< The total number of SPI master ports (invalid SPI master port). */
} hal_spi_master_port_t;

/** @brief This enum defines the options to connect the SPI slave device to the SPI master's CS pins. */
typedef enum {
    HAL_SPI_MASTER_SLAVE_0 = 0,                       /**< The SPI slave device is connected to the SPI master's CS0 pin. */
    HAL_SPI_MASTER_SLAVE_MAX                          /**< The total number of SPI master CS pins (invalid SPI master CS pin). */
} hal_spi_master_slave_port_t;

/** @brief SPI master transaction bit order definition. */
typedef enum {
    HAL_SPI_MASTER_LSB_FIRST = 0,                       /**< Both send and receive data transfer LSB first. */
    HAL_SPI_MASTER_MSB_FIRST = 1                        /**< Both send and receive data transfer MSB first. */
} hal_spi_master_bit_order_t;

/** @brief SPI master clock polarity definition. */
typedef enum {
    HAL_SPI_MASTER_CLOCK_POLARITY0 = 0,                     /**< Clock polarity is 0. */
    HAL_SPI_MASTER_CLOCK_POLARITY1 = 1                      /**< Clock polarity is 1. */
} hal_spi_master_clock_polarity_t;

/** @brief SPI master clock format definition. */
typedef enum {
    HAL_SPI_MASTER_CLOCK_PHASE0 = 0,                         /**< Clock format is 0. */
    HAL_SPI_MASTER_CLOCK_PHASE1 = 1                          /**< Clock format is 1. */
} hal_spi_master_clock_phase_t;

/**
 * @}
 */

/**
 * @}
 * @}
 */
#endif /* #ifdef HAL_SPI_MASTER_MODULE_ENABLED */

#ifdef HAL_SPI_SLAVE_MODULE_ENABLED
/**
 * @addtogroup HAL
 * @{
 * @addtogroup SPI_SLAVE
 * @{
 * @addtogroup hal_spi_slave_enum Enum
 * @{
 */

/*****************************************************************************
* SPI slave
*****************************************************************************/
/** @brief This enum defines the SPI slave port. This chip supports only one
 *  SPI slave port.
 */
typedef enum {
    HAL_SPI_SLAVE_INVALID = -1,
    HAL_SPI_SLAVE_0 = 0,                             /**< SPI slave port 0. */
    HAL_SPI_SLAVE_MAX                                /**< The total number of SPI slave ports (invalid SPI slave port number). */
} hal_spi_slave_port_t;

/** @brief SPI slave transaction bit order definition. */
typedef enum {
    HAL_SPI_SLAVE_LSB_FIRST = 0,                       /**< Both send and receive data transfer is the LSB first. */
    HAL_SPI_SLAVE_MSB_FIRST = 1                        /**< Both send and receive data transfer is the MSB first. */
} hal_spi_slave_bit_order_t;

/** @brief SPI slave clock polarity definition. */
typedef enum {
    HAL_SPI_SLAVE_CLOCK_POLARITY0 = 0,                 /**< Clock polarity is 0. */
    HAL_SPI_SLAVE_CLOCK_POLARITY1 = 1                  /**< Clock polarity is 1. */
} hal_spi_slave_clock_polarity_t;

/** @brief SPI slave clock format definition. */
typedef enum {
    HAL_SPI_SLAVE_CLOCK_PHASE0 = 0,                    /**< Clock format is 0. */
    HAL_SPI_SLAVE_CLOCK_PHASE1 = 1                     /**< Clock format is 1. */
} hal_spi_slave_clock_phase_t;

/** @brief SPI slave transaction byte order definition. */
typedef enum {
    HAL_SPI_SLAVE_LITTLE_ENDIAN = 0,                    /**< Both send and receive data transfer is the little endian first */
    HAL_SPI_SLAVE_BIG_ENDIAN = 1                     /**< Both send and receive data transfer is the big endian first */
} hal_spi_slave_endian_t;

/** @brief This enum defines the SPI slave event when an interrupt occurs.
 *MT7933 not support, just for build pass.
 */
typedef enum {
    HAL_SPI_SLAVE_EVENT_POWER_ON        = 0,    /**< Power on command is received. */
    HAL_SPI_SLAVE_EVENT_POWER_OFF       = 1,    /**< Power off command is received. */
    HAL_SPI_SLAVE_EVENT_CRD_FINISH      = 2,    /**< Configure read command is received. */
    HAL_SPI_SLAVE_EVENT_RD_FINISH       = 3,    /**< Read command is received. */
    HAL_SPI_SLAVE_EVENT_CWR_FINISH      = 4,    /**< Configure write command is received. */
    HAL_SPI_SLAVE_EVENT_WR_FINISH       = 5,    /**< Write command is received. */
    HAL_SPI_SLAVE_EVENT_RD_ERR          = 6,    /**< An error occurred during a read command. */
    HAL_SPI_SLAVE_EVENT_WR_ERR          = 7,    /**< An error occurred during a write command. */
    HAL_SPI_SLAVE_EVENT_TIMEOUT_ERR     = 8,    /**< A timeout is detected between configure read command and read command or configure write command and write command. */
    HAL_SPI_SLAVE_EVENT_DMA_DONE        = 9     /**< All tx or rx data transfer done. */
} hal_spi_slave_callback_event_t;

/** @brief This enum defines the SPI slave commands.
 *MT7933 not support, just for build pass.
 */
typedef enum {
    HAL_SPI_SLAVE_CMD_WS        = 0,       /**< Write Status command. */
    HAL_SPI_SLAVE_CMD_RS        = 1,       /**< Read Status command. */
    HAL_SPI_SLAVE_CMD_WR        = 2,       /**< Write Data command. */
    HAL_SPI_SLAVE_CMD_RD        = 3,       /**< Read Data command. */
    HAL_SPI_SLAVE_CMD_POWEROFF  = 4,       /**< POWER OFF command. */
    HAL_SPI_SLAVE_CMD_POWERON   = 5,       /**< POWER ON command. */
    HAL_SPI_SLAVE_CMD_CW        = 6,       /**< Configure Write command. */
    HAL_SPI_SLAVE_CMD_CR        = 7,        /**< Configure Read command. */
    HAL_SPI_SLAVE_CMD_CT        = 8        /**< Configure Type command. */
} hal_spi_slave_command_type_t;
/**
 * @}
 */


/**
 * @}
 * @}
 */
#endif /* #ifdef HAL_SPI_SLAVE_MODULE_ENABLED */

#ifdef HAL_CLOCK_MODULE_ENABLED

/*****************************************************************************
* Clock
*****************************************************************************/

/**
 * @addtogroup HAL
 * @{
 * @addtogroup CLOCK
 * @{
 * @addtogroup hal_clock_enum Enum
 * @{
 */

/**
* @section CLOCK_CG_ID_Usage_Chapter HAL_CLOCK_CG_ID descriptions
*
* Each #hal_clock_cg_id is related to one CG. Please check the following parameters before controlling the clock.
*
* The description of API parameters for HAL_CLOCK_CG_ID is listed below:
* | HAL_CLOCK_CG_ID            |Details                                                                            |
* |----------------------------|-----------------------------------------------------------------------------------|
*/
typedef enum {
    HAL_CLOCK_CG_CM33_XTAL = 0,                         /**< CG of CM33 xtal clock*/
    HAL_CLOCK_CG_DSP_XTAL = 1,                          /**< CG of DSP xtal clock*/
    HAL_CLOCK_CG_AUDIO_XTAL = 2,                        /**< CG of AUDIO xtal clock*/
    HAL_CLOCK_CG_USB_CTRL_XTAL = 3,                     /**< CG of USB CTRL xtal clock*/
    HAL_CLOCK_CG_USB_PHY_XTAL = 4,                      /**< CG of USB PHY xtal clock*/
    HAL_CLOCK_CG_UART0_XTAL = 5,                        /**< CG of TOP UART0 xtal clock*/
    HAL_CLOCK_CG_UART1_XTAL = 6,                        /**< CG of TOP UART1 xtal clock*/
    HAL_CLOCK_CG_PWM0_XTAL = 7,                         /**< CG of PWM 0 xtal clock*/
    HAL_CLOCK_CG_PWM1_XTAL = 8,                         /**< CG of PWM 1 xtal clock*/
    HAL_CLOCK_CG_PWM2_XTAL = 9,                         /**< CG of PWM 2 xtal clock*/
    HAL_CLOCK_CG_CONNAC_XTAL = 10,                      /**< CG of CONNAC xtal clock*/
    HAL_CLOCK_CG_EFUSE1_XTAL = 11,                      /**< CG of EFUSE GROUP1 xtal clock*/
    HAL_CLOCK_CG_EFUSE2_XTAL = 12,                      /**< CG of EFUSE GROUP2 xtal clock*/
    HAL_CLOCK_CG_EFUSE3_XTAL = 13,                      /**< CG of EFUSE GROUP3 xtal clock*/
    HAL_CLOCK_CG_RTC_XTAL = 14,                         /**< CG of RTC xtal clock*/
    HAL_CLOCK_CG_UHS_PSRAM_XTAL = 15,                   /**< CG of UHS PSRAM xtal clock*/
    HAL_CLOCK_CG_FLASH_XTAL = 16,                       /**< CG of FLASH xtal clock*/
    HAL_CLOCK_CG_AUD_ADC0_XTAL = 17,                    /**< CG of AUDIO ADC0 xtal clock*/
    HAL_CLOCK_CG_AUD_ADC1_XTAL = 18,                    /**< CG of AUDIO ADC1 xtal clock*/
    HAL_CLOCK_CG_AUD_DAC_XTAL = 19,                     /**< CG of AUDIO DAC xtal clock*/
    HAL_CLOCK_CG_TOP_AON_XTAL = 20,                     /**< CG of TOP AON xtal clock*/
    HAL_CLOCK_CG_TOP_MBIST_XTAL = 21,                   /**< CG of TOP MBIST xtal clock*/
    HAL_CLOCK_CG_USB_MBIST_XTAL = 22,                   /**< CG of USB MBIST xtal clock*/
    HAL_CLOCK_CG_CRYPTO_DIV2_XTAL = 28,                 /**< DIV2 CG of Crypto xtal clock*/
    HAL_CLOCK_CG_CM33_DIV2_XTAL = 29,                   /**< DIV2 CG of CM33 xtal clock*/
    HAL_CLOCK_CG_XTAL_END = HAL_CLOCK_CG_CM33_DIV2_XTAL,

    HAL_CLOCK_CG_PLL_BEGIN,
    HAL_CLOCK_CG_TOP_PLL = HAL_CLOCK_CG_PLL_BEGIN,      /**< CG of TOP PLL clock*/
    HAL_CLOCK_CG_DSP_PLL,                               /**< CG of DSP PLL clock*/
    HAL_CLOCK_CG_USB_PLL,                               /**< CG of USB PLL clock*/
    HAL_CLOCK_CG_XPLL,                                  /**< CG of XPLL clock*/
    HAL_CLOCK_CG_PLL_END = HAL_CLOCK_CG_XPLL,

    HAL_CLOCK_CG_CTL_BEGIN,
    HAL_CLOCK_CG_DSP_HCLK = HAL_CLOCK_CG_CTL_BEGIN,     /**< CG of CM33 HCLK clock*/
    HAL_CLOCK_CG_INFRA_BUS,                             /**< CG of Infra bus clock*/
    HAL_CLOCK_CG_AUDSYS_BUS,                            /**< CG of AUDSYS bus clock*/
    HAL_CLOCK_CG_PSRAM,                                 /**< CG of PSRAM clock*/
    HAL_CLOCK_CG_TRACE,                                 /**< CG of TRACE clock*/
    HAL_CLOCK_CG_PSRAM_AXI,                             /**< CG of PSRAM AXI bus clock*/
    HAL_CLOCK_CG_FLASH,                                 /**< CG of FLASH clock*/
    HAL_CLOCK_CG_GCPU,                                  /**< CG of GCPU clock*/
    HAL_CLOCK_CG_ECC,                                   /**< CG of ECC clock*/
    HAL_CLOCK_CG_SPIM0,                                 /**< CG of SPIM0 clock*/
    HAL_CLOCK_CG_SPIM1,                                 /**< CG of SPIM1 clock*/
    HAL_CLOCK_CG_SPIS,                                  /**< CG of SPIS clock*/
    HAL_CLOCK_CG_I2C0,                                  /**< CG of I2C0 clock*/
    HAL_CLOCK_CG_I2C1,                                  /**< CG of I2C1 clock*/
    HAL_CLOCK_CG_DBG,                                   /**< CG of DBG clock*/
    HAL_CLOCK_CG_SDIOM,                                 /**< CG of SDIOM clock*/
    HAL_CLOCK_CG_USB_SYS,                               /**< CG of USB SYS clock*/
    HAL_CLOCK_CG_USB_XHCI,                              /**< CG of USB XHCI clock*/
    HAL_CLOCK_CG_AUX_ADC_DIV,                           /**< CG of AUX ADC divider clock*/
    HAL_CLOCK_CG_PWM_DIV,                               /**< CG of PWM divier clock*/
    HAL_CLOCK_CG_APLL12_CK_DIV0,                        /**< CG of APLL12_CK_DIV0*/
    HAL_CLOCK_CG_APLL12_CK_DIV3,                        /**< CG of APLL12_CK_DIV3*/
    HAL_CLOCK_CG_APLL12_CK_DIV6,                        /**< CG of APLL12_CK_DIV6*/
    HAL_CLOCK_CG_CTL_END = HAL_CLOCK_CG_APLL12_CK_DIV6,

    HAL_CLOCK_CG_MAX
} hal_clock_cg_id;

/**
 * @section CLOCK_SEL_ID_Usage_Chapter HAL_CLOCK_SEL_ID descriptions
 *
 * Each #hal_clock_sel_id is related to one clock source. Please check the following parameters before controlling the clock.
 *
 * The description of API parameters for HAL_CLOCK_CG_ID is listed below:
 * | HAL_CLOCK_SEL_ID            |Details                                                                           |
 * |----------------------------|-----------------------------------------------------------------------------------|
 */

typedef enum {
    HAL_CLOCK_SEL_TBL_BEGIN = 1,
    HAL_CLOCK_SEL_AUDIO_FAUDIO = HAL_CLOCK_SEL_TBL_BEGIN, /**< Clksel for AUDIO FAUDIO clock*/
    HAL_CLOCK_SEL_AUDIO_FASM,                   /**< Clksel for AUDIO FASM clock*/
    HAL_CLOCK_SEL_AUDIO_FASYS,                  /**< Clksel for AUDIO FASYS clock*/
    HAL_CLOCK_SEL_AUDIO_HAPLL,                  /**< Clksel for AUDIO FAPLL*/
    HAL_CLOCK_SEL_AUDIO_FAUD_INTBUS,            /**< Clksel for AUDIO FAUD internal bus clock*/
    HAL_CLOCK_SEL_DSP_HCLK,                     /**< Clksel for DSP HCLK*/
    HAL_CLOCK_SEL_INFRA_BUS,                    /**< Clksel for Infra(AXI) bus clock*/
    HAL_CLOCK_SEL_AUDSYS_BUS,                   /**< Clksel for AUDSYS bus clock*/
    HAL_CLOCK_SEL_PSRAM,                        /**< Clksel for PSRAM clock*/
    HAL_CLOCK_SEL_TRACE,                        /**< Clksel for TRACE clock*/
    HAL_CLOCK_SEL_PSRAM_AXI,                    /**< Clksel for PSRAM AXI bus clock*/
    HAL_CLOCK_SEL_FLASH,                        /**< Clksel for FLASH clock*/
    HAL_CLOCK_SEL_GCPU,                         /**< Clksel for GCPU clock*/
    HAL_CLOCK_SEL_ECC,                          /**< Clksel for ECC clock*/
    HAL_CLOCK_SEL_SPIM0,                        /**< Clksel for SPIM0 clock*/
    HAL_CLOCK_SEL_SPIM1,                        /**< Clksel for SPIM1 clock*/
    HAL_CLOCK_SEL_SPIS,                         /**< Clksel for SPIS clock*/
    HAL_CLOCK_SEL_I2C,                          /**< Clksel for I2C clock*/
    HAL_CLOCK_SEL_DBG,                          /**< Clksel for DBG clock*/
    HAL_CLOCK_SEL_SDIOM,                        /**< Clksel for SDIOM clock*/
    HAL_CLOCK_SEL_USB_SYS,                      /**< Clksel for USB SYS clock*/
    HAL_CLOCK_SEL_USB_XHCI,                     /**< Clksel for USB XHCI clock*/
    HAL_CLOCK_SEL_AUX_ADC_DIV,                  /**< Clksel for AUX ADC divider*/
    HAL_CLOCK_SEL_PWM_DIV,                      /**< Clksel for PWM divider*/
    HAL_CLOCK_SEL_F32K,                         /**< Clksel for F32K clock*/
    HAL_CLOCK_SEL_CM33_HCLK,                    /**< Clksel for CM33 HCLK*/
    HAL_CLOCK_SEL_TBL_END = HAL_CLOCK_SEL_CM33_HCLK,

    HAL_CLOCK_SEL_APLL12_CK_DIV0,               /**< Clksel for APLL12_CK_DIV0*/
    HAL_CLOCK_SEL_APLL12_CK_DIV3,               /**< Clksel for APLL12_CK_DIV3*/
    HAL_CLOCK_SEL_APLL12_CK_DIV6,               /**< Clksel for APLL12_CK_DIV6*/
    HAL_CLOCK_SEL_DSP_PLL,                      /**< Clksel for DSP PLL*/
    HAL_CLOCK_SEL_TOP_PLL,                      /**< Clksel for TOP PLL*/

    HAL_CLOCK_SEL_MAX
} hal_clock_sel_id;

/** @cond FOO */
#define CLKSEL_FIELD(_clock_id, _divsel, _clksel) (((_clock_id) & 0xFF) << 24 | ((_divsel) & 0xFFFF) << 8 | ((_clksel) & 0xFF))
#define CLKSEL_FIELD_CLOCK_ID(_fld) (((_fld) >> 24) & 0xFF)
#define CLKSEL_FIELD_DIVSEL(_fld) (((_fld) >> 8) & 0xFFFF)
#define CLKSEL_FIELD_CLKSEL(_fld) ((_fld) & 0xFF)
/** #endcond */

/** @brief Clock select options for <code>HAL_CLOCK_SEL_AUDIO_FAUDIO</code>*/
typedef enum {
    CLK_AUDIO_FAUDIO_CLKSEL_AUDIO_XTAL  = CLKSEL_FIELD(HAL_CLOCK_SEL_AUDIO_FAUDIO, 0, 0), /**< @hideinitializer */
    CLK_AUDIO_FAUDIO_CLKSEL_60M         = CLKSEL_FIELD(HAL_CLOCK_SEL_AUDIO_FAUDIO, 0, 1), /**< @hideinitializer */
    CLK_AUDIO_FAUDIO_CLKSEL_XPLL_DIV4   = CLKSEL_FIELD(HAL_CLOCK_SEL_AUDIO_FAUDIO, 0, 2)  /**< @hideinitializer */
} clk_audio_faudio_clksel_t;

/** @brief Clock select options for <code>HAL_CLOCK_SEL_AUDIO_FASM</code>*/
typedef enum {
    CLK_AUDIO_FASM_CLKSEL_AUDIO_XTAL    = CLKSEL_FIELD(HAL_CLOCK_SEL_AUDIO_FASM, 0, 0), /**< @hideinitializer */
    CLK_AUDIO_FASM_CLKSEL_120M          = CLKSEL_FIELD(HAL_CLOCK_SEL_AUDIO_FASM, 0, 1)  /**< @hideinitializer */
} clk_audio_fasm_clksel_t;

/** @brief Clock select options for <code>HAL_CLOCK_SEL_AUDIO_FASYS</code>*/
typedef enum {
    CLK_AUDIO_FASYS_CLKSEL_AUDIO_XTAL   = CLKSEL_FIELD(HAL_CLOCK_SEL_AUDIO_FASYS, 0, 0), /**< @hideinitializer */
    CLK_AUDIO_FASYS_CLKSEL_XPLL_DIV2    = CLKSEL_FIELD(HAL_CLOCK_SEL_AUDIO_FASYS, 0, 2), /**< @hideinitializer */
    CLK_AUDIO_FASYS_CLKSEL_XPLL_DIV4    = CLKSEL_FIELD(HAL_CLOCK_SEL_AUDIO_FASYS, 0, 3)  /**< @hideinitializer */
} clk_audio_fasys_clksel_t;

/** @brief Clock select options for <code>HAL_CLOCK_SEL_AUDIO_HAPLL</code>*/
typedef enum {
    CLK_AUDIO_HAPLL_CLKSEL_AUDIO_XTAL   = CLKSEL_FIELD(HAL_CLOCK_SEL_AUDIO_HAPLL, 0, 0), /**< @hideinitializer */
    CLK_AUDIO_HAPLL_CLKSEL_XPLL_DIV2    = CLKSEL_FIELD(HAL_CLOCK_SEL_AUDIO_HAPLL, 0, 2), /**< @hideinitializer */
    CLK_AUDIO_HAPLL_CLKSEL_XPLL_DIV4    = CLKSEL_FIELD(HAL_CLOCK_SEL_AUDIO_HAPLL, 0, 3)  /**< @hideinitializer */
} clk_audio_hapll_clksel_t;

/** @brief Clock select options for <code>HAL_CLOCK_SEL_AUDIO_FAUD_INTBUS</code>*/
typedef enum {
    CLK_AUDIO_FAUD_INTBUS_CLKSEL_AUDIO_XTAL = CLKSEL_FIELD(HAL_CLOCK_SEL_AUDIO_FAUD_INTBUS, 0, 0), /**< @hideinitializer */
    CLK_AUDIO_FAUD_INTBUS_CLKSEL_266M       = CLKSEL_FIELD(HAL_CLOCK_SEL_AUDIO_FAUD_INTBUS, 0, 1), /**< @hideinitializer */
    CLK_AUDIO_FAUD_INTBUS_CLKSEL_133M       = CLKSEL_FIELD(HAL_CLOCK_SEL_AUDIO_FAUD_INTBUS, 0, 2), /**< @hideinitializer */
    CLK_AUDIO_FAUD_INTBUS_CLKSEL_XPLL_DIV4  = CLKSEL_FIELD(HAL_CLOCK_SEL_AUDIO_FAUD_INTBUS, 0, 3)  /**< @hideinitializer */
} clk_audio_faud_intbus_clksel_t;

/** @brief Clock select options for <code>HAL_CLOCK_SEL_DSP_HCLK</code>*/
typedef enum {
    CLK_DSP_HCLK_CLKSEL_XTAL            = CLKSEL_FIELD(HAL_CLOCK_SEL_DSP_HCLK, 0, 0), /**< @hideinitializer */
    CLK_DSP_HCLK_CLKSEL_XTAL_DIV2       = CLKSEL_FIELD(HAL_CLOCK_SEL_DSP_HCLK, 0, 1), /**< @hideinitializer */
    CLK_DSP_HCLK_CLKSEL_F32K            = CLKSEL_FIELD(HAL_CLOCK_SEL_DSP_HCLK, 0, 2), /**< @hideinitializer */
    CLK_DSP_HCLK_CLKSEL_DSPPLL          = CLKSEL_FIELD(HAL_CLOCK_SEL_DSP_HCLK, 0, 3), /**< @hideinitializer */
    CLK_DSP_HCLK_CLKSEL_DSPPLL_DIV2     = CLKSEL_FIELD(HAL_CLOCK_SEL_DSP_HCLK, 0, 4), /**< @hideinitializer */
    CLK_DSP_HCLK_CLKSEL_DSPPLL_DIV4     = CLKSEL_FIELD(HAL_CLOCK_SEL_DSP_HCLK, 0, 5), /**< @hideinitializer */
    CLK_DSP_HCLK_CLKSEL_DSPPLL_DIV8     = CLKSEL_FIELD(HAL_CLOCK_SEL_DSP_HCLK, 0, 6), /**< @hideinitializer */
    CLK_DSP_HCLK_CLKSEL_XPLL_DIV4       = CLKSEL_FIELD(HAL_CLOCK_SEL_DSP_HCLK, 0, 7)  /**< @hideinitializer */
} clk_dsp_hclk_clksel_t;

/** @brief Clock select options for <code>HAL_CLOCK_SEL_INFRA_BUS</code>*/
typedef enum {
    CLK_INFRA_BUS_CLKSEL_XTAL           = CLKSEL_FIELD(HAL_CLOCK_SEL_INFRA_BUS, 0, 0), /**< @hideinitializer */
    CLK_INFRA_BUS_CLKSEL_F32K           = CLKSEL_FIELD(HAL_CLOCK_SEL_INFRA_BUS, 0, 1), /**< @hideinitializer */
    CLK_INFRA_BUS_CLKSEL_133M           = CLKSEL_FIELD(HAL_CLOCK_SEL_INFRA_BUS, 0, 2), /**< @hideinitializer */
    CLK_INFRA_BUS_CLKSEL_DIV_120M       = CLKSEL_FIELD(HAL_CLOCK_SEL_INFRA_BUS, 4, 3), /**< @hideinitializer */
    CLK_INFRA_BUS_CLKSEL_DIV_100M       = CLKSEL_FIELD(HAL_CLOCK_SEL_INFRA_BUS, 5, 3)  /**< @hideinitializer */
} clk_infra_bus_clksel_t;

/** @brief Clock select options for <code>HAL_CLOCK_SEL_AUDSYS_BUS</code>*/
typedef enum {
    CLK_AUDSYS_BUS_CLKSEL_XTAL          = CLKSEL_FIELD(HAL_CLOCK_SEL_AUDSYS_BUS, 0, 0), /**< @hideinitializer */
    CLK_AUDSYS_BUS_CLKSEL_F32K          = CLKSEL_FIELD(HAL_CLOCK_SEL_AUDSYS_BUS, 0, 1), /**< @hideinitializer */
    CLK_AUDSYS_BUS_CLKSEL_266M          = CLKSEL_FIELD(HAL_CLOCK_SEL_AUDSYS_BUS, 0, 2), /**< @hideinitializer */
    CLK_AUDSYS_BUS_CLKSEL_DIV_200M      = CLKSEL_FIELD(HAL_CLOCK_SEL_AUDSYS_BUS, 2, 3), /**< @hideinitializer */
    CLK_AUDSYS_BUS_CLKSEL_DIV_120M      = CLKSEL_FIELD(HAL_CLOCK_SEL_AUDSYS_BUS, 4, 3)  /**< @hideinitializer */
} clk_audsys_bus_clksel_t;

/** @brief Clock select options for <code>HAL_CLOCK_SEL_PSRAM</code>*/
typedef enum {
    CLK_PSRAM_CLKSEL_400M               = CLKSEL_FIELD(HAL_CLOCK_SEL_PSRAM, 0, 0), /**< @hideinitializer */
    CLK_PSRAM_CLKSEL_DIV_300M           = CLKSEL_FIELD(HAL_CLOCK_SEL_PSRAM, 1, 1), /**< @hideinitializer */
    CLK_PSRAM_CLKSEL_DIV_200M           = CLKSEL_FIELD(HAL_CLOCK_SEL_PSRAM, 2, 1),  /**< @hideinitializer */
    CLK_PSRAM_CLKSEL_DIV_150M           = CLKSEL_FIELD(HAL_CLOCK_SEL_PSRAM, 3, 1),  /**< @hideinitializer */
    CLK_PSRAM_CLKSEL_DIV_120M           = CLKSEL_FIELD(HAL_CLOCK_SEL_PSRAM, 4, 1),  /**< @hideinitializer */
    CLK_PSRAM_CLKSEL_DIV_100M           = CLKSEL_FIELD(HAL_CLOCK_SEL_PSRAM, 5, 1),  /**< @hideinitializer */
    CLK_PSRAM_CLKSEL_DIV_85p7M          = CLKSEL_FIELD(HAL_CLOCK_SEL_PSRAM, 6, 1),  /**< @hideinitializer */
    CLK_PSRAM_CLKSEL_DIV_75M            = CLKSEL_FIELD(HAL_CLOCK_SEL_PSRAM, 7, 1)   /**< @hideinitializer */
} clk_psram_clksel_t;

/** @brief Clock select options for <code>HAL_CLOCK_SEL_TRACE</code>*/
typedef enum {
    CLK_TRACE_CLKSEL_XTAL               = CLKSEL_FIELD(HAL_CLOCK_SEL_TRACE, 0, 0), /**< @hideinitializer */
    CLK_TRACE_CLKSEL_DIV_300M           = CLKSEL_FIELD(HAL_CLOCK_SEL_TRACE, 1, 1)  /**< @hideinitializer */
} clk_trace_clksel_t;

/** @brief Clock select options for <code>HAL_CLOCK_SEL_PSRAM_AXI</code>*/
typedef enum {
    CLK_PSRAM_AXI_CLKSEL_XTAL           = CLKSEL_FIELD(HAL_CLOCK_SEL_PSRAM_AXI, 0, 0), /**< @hideinitializer */
    CLK_PSRAM_AXI_CLKSEL_PSRAM          = CLKSEL_FIELD(HAL_CLOCK_SEL_PSRAM_AXI, 0, 1)  /**< @hideinitializer */
} clk_psram_axi_clksel_t;

/** @brief Clock select options for <code>HAL_CLOCK_SEL_FLASH</code>*/
typedef enum {
    CLK_FLASH_CLKSEL_XTAL               = CLKSEL_FIELD(HAL_CLOCK_SEL_FLASH, 0, 0), /**< @hideinitializer */
    CLK_FLASH_CLKSEL_DIV_120M           = CLKSEL_FIELD(HAL_CLOCK_SEL_FLASH, 4, 1), /**< @hideinitializer */
    CLK_FLASH_CLKSEL_DIV_60M            = CLKSEL_FIELD(HAL_CLOCK_SEL_FLASH, 9, 1)  /**< @hideinitializer */
} clk_flash_clksel_t;

/** @brief Clock select options for <code>HAL_CLOCK_SEL_GCPU</code>*/
typedef enum {
    CLK_GCPU_CLKSEL_XTAL                = CLKSEL_FIELD(HAL_CLOCK_SEL_GCPU, 0, 0), /**< @hideinitializer */
    CLK_GCPU_CLKSEL_DIV_300M            = CLKSEL_FIELD(HAL_CLOCK_SEL_GCPU, 1, 1)  /**< @hideinitializer */
} clk_gcpu_clksel_t;

/** @brief Clock select options for <code>HAL_CLOCK_SEL_ECC</code>*/
typedef enum {
    CLK_ECC_CLKSEL_XTAL                 = CLKSEL_FIELD(HAL_CLOCK_SEL_ECC, 0, 0), /**< @hideinitializer */
    CLK_ECC_CLKSEL_DIV_300M             = CLKSEL_FIELD(HAL_CLOCK_SEL_ECC, 1, 1)  /**< @hideinitializer */
} clk_ecc_clksel_t;

/** @brief Clock select options for <code>HAL_CLOCK_SEL_SPIM0</code>*/
typedef enum {
    CLK_SPIM0_CLKSEL_XTAL               = CLKSEL_FIELD(HAL_CLOCK_SEL_SPIM0, 0, 0), /**< @hideinitializer */
    CLK_SPIM0_CLKSEL_DIV_200M           = CLKSEL_FIELD(HAL_CLOCK_SEL_SPIM0, 2, 1), /**< @hideinitializer */
    CLK_SPIM0_CLKSEL_171p43M            = CLKSEL_FIELD(HAL_CLOCK_SEL_SPIM0, 0, 2), /**< @hideinitializer */
    CLK_SPIM0_CLKSEL_133M               = CLKSEL_FIELD(HAL_CLOCK_SEL_SPIM0, 0, 3)  /**< @hideinitializer */
} clk_spim0_clksel_t;

/** @brief Clock select options for <code>HAL_CLOCK_SEL_SPIM1</code>*/
typedef enum {
    CLK_SPIM1_CLKSEL_XTAL               = CLKSEL_FIELD(HAL_CLOCK_SEL_SPIM1, 0, 0), /**< @hideinitializer */
    CLK_SPIM1_CLKSEL_DIV_200M           = CLKSEL_FIELD(HAL_CLOCK_SEL_SPIM1, 2, 1), /**< @hideinitializer */
    CLK_SPIM1_CLKSEL_171p43M            = CLKSEL_FIELD(HAL_CLOCK_SEL_SPIM1, 0, 2), /**< @hideinitializer */
    CLK_SPIM1_CLKSEL_133M               = CLKSEL_FIELD(HAL_CLOCK_SEL_SPIM1, 0, 3)  /**< @hideinitializer */
} clk_spim1_clksel_t;

/** @brief Clock select options for <code>HAL_CLOCK_SEL_SPIS</code>*/
typedef enum {
    CLK_SPIS_CLKSEL_XTAL                = CLKSEL_FIELD(HAL_CLOCK_SEL_SPIS, 0, 0), /**< @hideinitializer */
    CLK_SPIS_CLKSEL_DIV_400M            = CLKSEL_FIELD(HAL_CLOCK_SEL_SPIS, 2, 1)  /**< @hideinitializer */
} clk_spis_clksel_t;

/** @brief Clock select options for <code>HAL_CLOCK_SEL_I2C</code>*/
typedef enum {
    CLK_I2C_CLKSEL_XTAL                 = CLKSEL_FIELD(HAL_CLOCK_SEL_I2C, 0, 0), /**< @hideinitializer */
    CLK_I2C_CLKSEL_DIV_120M             = CLKSEL_FIELD(HAL_CLOCK_SEL_I2C, 4, 1)  /**< @hideinitializer */
} clk_i2c_clksel_t;

/** @brief Clock select options for <code>HAL_CLOCK_SEL_DBG</code>*/
typedef enum {
    CLK_DBG_CLKSEL_XTAL                 = CLKSEL_FIELD(HAL_CLOCK_SEL_DBG, 0, 0), /**< @hideinitializer */
    CLK_DBG_CLKSEL_F32K                 = CLKSEL_FIELD(HAL_CLOCK_SEL_DBG, 0, 1), /**< @hideinitializer */
    CLK_DBG_CLKSEL_100M                 = CLKSEL_FIELD(HAL_CLOCK_SEL_DBG, 0, 2), /**< @hideinitializer */
    CLK_DBG_CLKSEL_133M                 = CLKSEL_FIELD(HAL_CLOCK_SEL_DBG, 0, 3)  /**< @hideinitializer */
} clk_dbg_clksel_t;

/** @brief Clock select options for <code>HAL_CLOCK_SEL_SDIOM</code>*/
typedef enum {
    CLK_SDIOM_CLKSEL_XTAL               = CLKSEL_FIELD(HAL_CLOCK_SEL_SDIOM, 0, 0), /**< @hideinitializer */
    CLK_SDIOM_CLKSEL_50M                = CLKSEL_FIELD(HAL_CLOCK_SEL_SDIOM, 0, 1)  /**< @hideinitializer */
} clk_sdiom_clksel_t;

/** @brief Clock select options for <code>HAL_CLOCK_SEL_USB_SYS</code>*/
typedef enum {
    CLK_USB_SYS_CLKSEL_XTAL             = CLKSEL_FIELD(HAL_CLOCK_SEL_USB_SYS, 0, 0), /**< @hideinitializer */
    CLK_USB_SYS_CLKSEL_50M              = CLKSEL_FIELD(HAL_CLOCK_SEL_USB_SYS, 0, 1), /**< @hideinitializer */
    CLK_USB_SYS_CLKSEL_100M             = CLKSEL_FIELD(HAL_CLOCK_SEL_USB_SYS, 0, 2), /**< @hideinitializer */
    CLK_USB_SYS_CLKSEL_120M             = CLKSEL_FIELD(HAL_CLOCK_SEL_USB_SYS, 0, 3)  /**< @hideinitializer */
} clk_usb_sys_clksel_t;

/** @brief Clock select options for <code>HAL_CLOCK_SEL_USB_XHCI</code>*/
typedef enum {
    CLK_USB_XHCI_CLKSEL_XTAL            = CLKSEL_FIELD(HAL_CLOCK_SEL_USB_XHCI, 0, 0), /**< @hideinitializer */
    CLK_USB_XHCI_CLKSEL_50M             = CLKSEL_FIELD(HAL_CLOCK_SEL_USB_XHCI, 0, 1), /**< @hideinitializer */
    CLK_USB_XHCI_CLKSEL_100M            = CLKSEL_FIELD(HAL_CLOCK_SEL_USB_XHCI, 0, 2), /**< @hideinitializer */
    CLK_USB_XHCI_CLKSEL_133M            = CLKSEL_FIELD(HAL_CLOCK_SEL_USB_XHCI, 0, 3)  /**< @hideinitializer */
} clk_usb_xhci_clksel_t;

/** @brief Clock select options for <code>HAL_CLOCK_SEL_AUX_ADC_DIV</code>*/
typedef enum {
    CLK_AUX_ADC_DIV_CLKSEL_XTAL_DIV_2M  = CLKSEL_FIELD(HAL_CLOCK_SEL_AUX_ADC_DIV, 12, 0) /**< @hideinitializer */
} clk_aux_adc_div_clksel_t;

/** @brief Clock select options for <code>HAL_CLOCK_SEL_PWM_DIV</code>*/
typedef enum {
    CLK_PWM_DIV_CLKSEL_XTAL_DIV_2M      = CLKSEL_FIELD(HAL_CLOCK_SEL_PWM_DIV, 12, 0) /**< @hideinitializer */
} clk_pwm_div_clksel_t;

/** @brief Clock select options for <code>HAL_CLOCK_SEL_F32K</code>*/
typedef enum {
    CLK_F32K_CLKSEL_PMU                 = CLKSEL_FIELD(HAL_CLOCK_SEL_F32K, 0, 0),  /**< @hideinitializer */
    CLK_F32K_CLKSEL_RTC                 = CLKSEL_FIELD(HAL_CLOCK_SEL_F32K, 0, 1),  /**< @hideinitializer */
    CLK_F32K_CLKSEL_XTAL_DIV_32K        = CLKSEL_FIELD(HAL_CLOCK_SEL_F32K, 793, 2) /**< @hideinitializer */
} clk_f32k_clksel_t;

/** @brief Clock select options for <code>HAL_CLOCK_SEL_CM33_HCLK</code>*/
typedef enum {
    CLK_CM33_HCLK_CLKSEL_XTAL           = CLKSEL_FIELD(HAL_CLOCK_SEL_CM33_HCLK, 0, 0), /**< @hideinitializer */
    CLK_CM33_HCLK_CLKSEL_DIV_300M       = CLKSEL_FIELD(HAL_CLOCK_SEL_CM33_HCLK, 1, 1), /**< @hideinitializer */
    CLK_CM33_HCLK_CLKSEL_DIV_200M       = CLKSEL_FIELD(HAL_CLOCK_SEL_CM33_HCLK, 2, 1), /**< @hideinitializer */
    CLK_CM33_HCLK_CLKSEL_F32K           = CLKSEL_FIELD(HAL_CLOCK_SEL_CM33_HCLK, 1, 0)  /**< @hideinitializer */
} clk_cm33_hclk_clksel_t;

/** @brief Clock select options for <code>HAL_CLOCK_SEL_APLL12_CK_DIV0</code>*/
typedef enum {
    CLK_APLL12_CK_DIV0_CLKSEL_DIV_SET   = CLKSEL_FIELD(HAL_CLOCK_SEL_APLL12_CK_DIV0, 0, 0),  /**< @hideinitializer */
} clk_apll12_ck_div0_clksel_t;

/** @brief Clock select options for <code>HAL_CLOCK_SEL_APLL12_CK_DIV3</code>*/
typedef enum {
    CLK_APLL12_CK_DIV3_CLKSEL_DIV_SET   = CLKSEL_FIELD(HAL_CLOCK_SEL_APLL12_CK_DIV3, 0, 0),  /**< @hideinitializer */
} clk_apll12_ck_div3_clksel_t;

/** @brief Clock select options for <code>HAL_CLOCK_SEL_APLL12_CK_DIV6</code>*/
typedef enum {
    CLK_APLL12_CK_DIV6_CLKSEL_DIV_SET   = CLKSEL_FIELD(HAL_CLOCK_SEL_APLL12_CK_DIV6, 0, 0),  /**< @hideinitializer */
} clk_apll12_ck_div6_clksel_t;

/** @brief Clock select options for <code>HAL_CLOCK_SEL_DSP_PLL</code>*/
typedef enum {
    CLK_DSPPLL_CLKSEL_300M              = CLKSEL_FIELD(HAL_CLOCK_SEL_DSP_PLL, 0, 0),  /**< @hideinitializer */
    CLK_DSPPLL_CLKSEL_400M              = CLKSEL_FIELD(HAL_CLOCK_SEL_DSP_PLL, 0, 1),  /**< @hideinitializer */
    CLK_DSPPLL_CLKSEL_500M              = CLKSEL_FIELD(HAL_CLOCK_SEL_DSP_PLL, 0, 2),  /**< @hideinitializer */
    CLK_DSPPLL_CLKSEL_600M              = CLKSEL_FIELD(HAL_CLOCK_SEL_DSP_PLL, 0, 3)   /**< @hideinitializer */
} clk_dsppll_clksel_t;

/** @brief Clock select options for <code>HAL_CLOCK_SEL_TOP_PLL</code>*/
typedef enum {
    CLK_TOPPLL_CLKSEL_1200M             = CLKSEL_FIELD(HAL_CLOCK_SEL_TOP_PLL, 0, 4),  /**< @hideinitializer */
    CLK_TOPPLL_CLKSEL_1194M             = CLKSEL_FIELD(HAL_CLOCK_SEL_TOP_PLL, 0, 5),  /**< @hideinitializer */
} clk_toppll_clksel_t;

/**
 * @}
 */


/**
 * @}
 * @}
 */

#endif /* #ifdef HAL_CLOCK_MODULE_ENABLED */

#ifdef HAL_SDIO_SLAVE_MODULE_ENABLED
/**
 * @addtogroup HAL
 * @{
 * @addtogroup SDIO
 * @{
 * @addtogroup hal_sdio_enum Enum
 * @{
 */
/*****************************************************************************
* SDIO
*****************************************************************************/
/** @brief  This enum defines the SDIO port.  */
typedef enum {
    HAL_SDIO_SLAVE_PORT_0 = 0,                                             /**< SDIO slave port 0. */
} hal_sdio_slave_port_t;

/**
  * @}
  */
/**
 * @}
 * @}
 */
#endif /* #ifdef HAL_SDIO_SLAVE_MODULE_ENABLED */

#ifdef HAL_SD_MODULE_ENABLED
/**
 * @addtogroup HAL
 * @{
 * @addtogroup SD
 * @{
 * @addtogroup hal_sd_enum Enum
 * @{
 */
/*****************************************************************************
* SD
*****************************************************************************/
/** @brief  This enum defines the SD port.  */
typedef enum {
    HAL_SD_PORT_0 = 0,                                             /**< SD port 0. */
    HAL_SD_PORT_1 = 1                                              /**< SD port 1. */
} hal_sd_port_t;


/**
  * @}
  */
/**
 * @}
 * @}
 */
#endif /* #ifdef HAL_SD_MODULE_ENABLED */

#ifdef HAL_SDIO_MODULE_ENABLED
/**
 * @addtogroup HAL
 * @{
 * @addtogroup SDIO
 * @{
 * @addtogroup hal_sdio_enum Enum
 * @{
 */
/*****************************************************************************
* SDIO
*****************************************************************************/
/** @brief  This enum defines the SDIO port.  */
typedef enum {
    HAL_SDIO_PORT_0 = 0,                                             /**< SDIO port 0. */
    HAL_SDIO_PORT_1 = 1                                              /**< SDIO port 1. */
} hal_sdio_port_t;


/**
  * @}
  */
/**
 * @}
 * @}
 */
#endif /* #ifdef HAL_SDIO_MODULE_ENABLED */

#define audio_message_type_t hal_audio_message_type_t
#ifdef HAL_AUDIO_MODULE_ENABLED
/**
 * @addtogroup HAL
 * @{
 * @addtogroup AUDIO
 * @{
 * @addtogroup hal_audio_enum Enum
 * @{
 */
/** @brief AUDIO port */
typedef enum {
    HAL_AUDIO_STREAM_OUT1    = 0, /**<  stream out HWGAIN1 only. */
    HAL_AUDIO_STREAM_OUT2    = 1, /**<  stream out HWGAIN2 only. */
#ifdef MTK_PORTING_AB
    HAL_AUDIO_STREAM_OUT3    = 2, /**<  stream out HWGAIN3 only. */
    HAL_AUDIO_STREAM_OUT_ALL = 3, /**<  stream out HWGAIN1, HWGAIN2 and HWGAIN3. */
#else /* #ifdef MTK_PORTING_AB */
    HAL_AUDIO_STREAM_OUT_ALL = 2, /**<  stream out HWGAIN1 and HWGAIN2. */
#endif /* #ifdef MTK_PORTING_AB */
} hal_audio_hw_stream_out_index_t;

/** @brief Audio message type */
typedef enum {
    AUDIO_MESSAGE_TYPE_COMMON,            /**< Audio basic scenario. */
    AUDIO_MESSAGE_TYPE_BT_AUDIO_UL  = 1,  /**< BT audio UL scenario. */
    AUDIO_MESSAGE_TYPE_BT_AUDIO_DL  = 2,  /**< BT audio DL scenario. */
    AUDIO_MESSAGE_TYPE_BT_VOICE_UL  = 3,  /**< BT aoice UL scenario. */
    AUDIO_MESSAGE_TYPE_BT_VOICE_DL  = 4,  /**< BT aoice DL scenario. */
    AUDIO_MESSAGE_TYPE_PLAYBACK     = 5,  /**< Local playback scenario. */
    AUDIO_MESSAGE_TYPE_RECORD       = 6,  /**< Mic record scenario. */
    AUDIO_MESSAGE_TYPE_PROMPT       = 7,  /**< Voice prompt scenario. */
    AUDIO_MESSAGE_TYPE_LINEIN       = 8,  /**< LineIN & loopback scenario. */
    AUDIO_MESSAGE_TYPE_BLE_AUDIO_UL = 9,  /**< BLE audio UL scenario. */
    AUDIO_MESSAGE_TYPE_BLE_AUDIO_DL = 10, /**< BLE audio DL scenario. */
    AUDIO_MESSAGE_TYPE_SIDETONE,          /**< Sidetone scenario. */
    AUDIO_MESSAGE_TYPE_ANC,               /**< ANC scenario. */
    AUDIO_MESSAGE_TYPE_AFE,               /**< DSP AFE dummy type. */
    AUDIO_MESSAGE_TYPE_MAX,               /**< Audio scenario type MAX. */

    AUDIO_RESERVE_TYPE_QUERY_RCDC,        /**< Query Message: RCDC. Different from above audio main scenario messages. Only for query purpose.*/
} audio_message_type_t;

/*****************************************************************************
* Audio setting
*****************************************************************************/

#ifdef HAL_AUDIO_SUPPORT_MULTIPLE_STREAM_OUT
/** @brief Audio device. */
typedef enum {
    HAL_AUDIO_DEVICE_NONE               = 0x0000,  /**<  No audio device is on. */
    HAL_AUDIO_DEVICE_MAIN_MIC_L         = 0x0001,  /**<  Stream in: main mic L. */
    HAL_AUDIO_DEVICE_MAIN_MIC_R         = 0x0002,  /**<  Stream in: main mic R. */
    HAL_AUDIO_DEVICE_MAIN_MIC_DUAL      = 0x0003,  /**<  Stream in: main mic L+R. */
    HAL_AUDIO_DEVICE_LINEINPLAYBACK_L   = 0x0004,  /**<  Stream in: line in playback L. */
    HAL_AUDIO_DEVICE_LINEINPLAYBACK_R   = 0x0008,  /**<  Stream in: line in playback R. */
    HAL_AUDIO_DEVICE_LINEINPLAYBACK_DUAL = 0x000c, /**<  Stream in: line in playback L+R. */
    HAL_AUDIO_DEVICE_DIGITAL_MIC_L      = 0x0010,  /**<  Stream in: digital mic L. */
    HAL_AUDIO_DEVICE_DIGITAL_MIC_R      = 0x0020,  /**<  Stream in: digital mic R. */
    HAL_AUDIO_DEVICE_DIGITAL_MIC_DUAL   = 0x0030,  /**<  Stream in: digital mic L+R. */

    HAL_AUDIO_DEVICE_DAC_L              = 0x0100,  /**<  Stream out:speaker L. */
    HAL_AUDIO_DEVICE_DAC_R              = 0x0200,  /**<  Stream out:speaker R. */
    HAL_AUDIO_DEVICE_DAC_DUAL           = 0x0300,  /**<  Stream out:speaker L+R. */

    HAL_AUDIO_DEVICE_I2S_MASTER         = 0x1000,  /**<  Stream in/out: I2S master role */
    HAL_AUDIO_DEVICE_I2S_SLAVE          = 0x2000,  /**<  Stream in/out: I2S slave role */
    HAL_AUDIO_DEVICE_EXT_CODEC          = 0x3000,   /**<  Stream out: external amp.&codec, stereo/mono */

    HAL_AUDIO_DEVICE_MAIN_MIC           = 0x0001,       /**<  OLD: Stream in: main mic. */
    HAL_AUDIO_DEVICE_HEADSET_MIC        = 0x0002,       /**<  OLD: Stream in: earphone mic. */
    HAL_AUDIO_DEVICE_HANDSET            = 0x0004,       /**<  OLD: Stream out:receiver. */
    HAL_AUDIO_DEVICE_HANDS_FREE_MONO    = 0x0008,       /**<  OLD: Stream out:loudspeaker, mono. */
    HAL_AUDIO_DEVICE_HANDS_FREE_STEREO  = 0x0010,       /**<  OLD: Stream out:loudspeaker, stereo to mono L=R=(R+L)/2. */
    HAL_AUDIO_DEVICE_HEADSET            = 0x0020,       /**<  OLD: Stream out:earphone, stereo */
    HAL_AUDIO_DEVICE_HEADSET_MONO       = 0x0040,       /**<  OLD: Stream out:earphone, mono to stereo. L=R. */
    HAL_AUDIO_DEVICE_LINE_IN            = 0x0080,       /**<  OLD: Stream in/out: line in. */
    HAL_AUDIO_DEVICE_DUAL_DIGITAL_MIC   = 0x0100,       /**<  OLD: Stream in: dual digital mic. */
    HAL_AUDIO_DEVICE_SINGLE_DIGITAL_MIC = 0x0200,       /**<  OLD: Stream in: single digital mic. */

    HAL_AUDIO_DEVICE_DUMMY              = 0xFFFFFFFF,   /**<  for DSP structrue alignment */
} hal_audio_device_t;
#endif /* #ifdef HAL_AUDIO_SUPPORT_MULTIPLE_STREAM_OUT */

/** @brief audio channel selection define */
typedef enum {
    HAL_AUDIO_DIRECT                     = 0, /**< A single interconnection, output equal to input. */
    HAL_AUDIO_SWAP_L_R                   = 2, /**< L and R channels are swapped. That is (L, R) -> (R, L). */
    HAL_AUDIO_BOTH_L                     = 3, /**< only output L channel. That is (L, R) -> (L, L). */
    HAL_AUDIO_BOTH_R                     = 4, /**< only output R channel. That is (L, R) -> (R, R). */
    HAL_AUDIO_MIX_L_R                    = 5, /**< L and R channels are mixed. That is (L, R) -> (L+R, L+R). */
    HAL_AUDIO_MIX_SHIFT_L_R              = 6, /**< L and R channels are mixed and shift. That is (L, R) -> (L/2+R/2, L/2+R/2). */
    HAL_AUDIO_CHANNEL_SELECTION_DUMMY    = 0xFFFFFFFF,   /**<  for DSP structrue alignment */
} hal_audio_channel_selection_t;

/** @brief i2s clk source define */
typedef enum {
    I2S_CLK_SOURCE_APLL                         = 0, /**< Low jitter mode. */
    I2S_CLK_SOURCE_DCXO                         = 1, /**< Normal mode. */
    I2S_CLK_SOURCE_TYPE_DUMMY                   = 0xFFFFFFFF,   /**<  for DSP structrue alignment */
} I2S_CLK_SOURCE_TYPE;

/** @brief micbias source define */
typedef enum {
    MICBIAS_SOURCE_0                            = 1, /**< Open micbias0. */
    MICBIAS_SOURCE_1                            = 2, /**< Open micbias1. */
    MICBIAS_SOURCE_ALL                          = 3, /**< Open micbias0 and micbias1. */
    MICBIAS_SOURCE_TYPE_DUMMY                   = 0xFFFFFFFF,   /**<  for DSP structrue alignment */
} MICBIAS_SOURCE_TYPE;

/** @brief micbias out voltage define */
typedef enum {
    MICBIAS3V_OUTVOLTAGE_1p8v                   = 1 << 2,   /**< 1.8V */
    MICBIAS3V_OUTVOLTAGE_1p85v                  = 1 << 3,   /**< 1.85V (Default) */
    MICBIAS3V_OUTVOLTAGE_1p9v                   = 1 << 4,   /**< 1.9V */
    MICBIAS3V_OUTVOLTAGE_2p0v                   = 1 << 5,   /**< 2.0V */
    MICBIAS3V_OUTVOLTAGE_2p1v                   = 1 << 6,   /**< 2.1V */
    MICBIAS3V_OUTVOLTAGE_2p2v                   = 1 << 7,   /**< 2.2V (Not support in 2V) */
    MICBIAS3V_OUTVOLTAGE_2p4v                   = 1 << 8,   /**< 2.4V (Not support in 2V) */
    MICBIAS3V_OUTVOLTAGE_VCC                    = 0x7f << 2, /**< BYPASSEN  */
    MICBIAS3V_OUTVOLTAGE_TYPE_DUMMY             = 0xFFFFFFFF,   /**<  for DSP structrue alignment */
} MICBIAS3V_OUTVOLTAGE_TYPE;

/** @brief micbias0 amic type define */
typedef enum {
    MICBIAS0_AMIC_MEMS                          = 0 << 9,  /**< MEMS (Default)*/
    MICBIAS0_AMIC_ECM_DIFFERENTIAL              = 1 << 9,  /**< ECM Differential*/
    MICBIAS0_AMIC_ECM_SINGLE                    = 3 << 9,  /**< ECM Single*/
    MICBIAS0_AMIC_TYPE_DUMMY                    = 0xFFFFFFFF,   /**<  for DSP structrue alignment */
} MICBIAS0_AMIC_TYPE;

/** @brief micbias1 amic type define */
typedef enum {
    MICBIAS1_AMIC_MEMS                          = 0 << 11,  /**< MEMS (Default)*/
    MICBIAS1_AMIC_ECM_DIFFERENTIAL              = 1 << 11,  /**< ECM Differential*/
    MICBIAS1_AMIC_ECM_SINGLE                    = 3 << 11,  /**< ECM Single*/
    MICBIAS1_AMIC_TYPE_DUMMY                    = 0xFFFFFFFF,   /**<  for DSP structrue alignment */
} MICBIAS1_AMIC_TYPE;

/** @brief uplink performance type define */
typedef enum {
    UPLINK_PERFORMANCE_NORMAL                   = 0 << 13, /**< Normal mode (Default)*/
    UPLINK_PERFORMANCE_HIGH                     = 1 << 13, /**< High performance mode*/
    UPLINK_PERFORMANCE_TYPE_DUMMY               = 0xFFFFFFFF,   /**<  for DSP structrue alignment */
} UPLINK_PERFORMANCE_TYPE;

/** @brief amic mic type define */
typedef enum {
    AMIC_DCC                                    = 0 << 14, /**< AMIC DCC mode.*/
    AMIC_ACC                                    = 1 << 14, /**< AMIC ACC mode.*/
    AMIC_TYPE_DUMMY                             = 0xFFFFFFFF,   /**<  for DSP structrue alignment */
} AMIC_TYPE;

/** @brief downlink performance type define */
typedef enum {
    DOWNLINK_PERFORMANCE_NORMAL                 = 0, /**< Normal mode (Default)*/
    DOWNLINK_PERFORMANCE_HIGH                   = 1, /**< High performance mode*/
    DOWNLINK_PERFORMANCE_TYPE_DUMMY             = 0xFFFFFFFF,   /**<  for DSP structrue alignment */
} DOWNLINK_PERFORMANCE_TYPE;

/** @brief audio MCLK pin select define */
typedef enum {
    AFE_MCLK_PIN_FROM_I2S0 = 0,     /**< MCLK from I2S0's mclk pin */
    AFE_MCLK_PIN_FROM_I2S1,         /**< MCLK from I2S1's mclk pin */
    AFE_MCLK_PIN_FROM_I2S2,         /**< MCLK from I2S2's mclk pin */
    AFE_MCLK_PIN_FROM_I2S3,         /**< MCLK from I2S3's mclk pin */
} afe_mclk_out_pin_t;

/** @brief audio APLL define */
typedef enum {
    AFE_APLL_NOUSE = 0,
    AFE_APLL1 = 1,                  /**< APLL1:45.1584M, 44.1K base */
    AFE_APLL2 = 2,                  /**< APLL2:49.152M, 48K base */
} afe_apll_source_t;

/** @brief audio MCLK status define */
typedef enum {
    MCLK_DISABLE = 0,               /**< Turn off MCLK */
    MCLK_ENABLE  = 1,               /**< Turn on MCLK */
} afe_mclk_status_t;

/** @brief amp performance define */
typedef enum {
    AUDIO_AMP_PERFORMANCE_NORMAL                = 0, /**< Normal mode. */
    AUDIO_AMP_PERFORMANCE_HIGH                  = 1, /**< High performance mode. */
    AUDIO_AMP_PERFORMANCE_TYPE_DUMMY            = 0xFFFFFFFF,   /**<  for DSP structrue alignment */
} AUDIO_AMP_PERFORMANCE_TYPE;

/** @brief DSP streaming source channel define */
typedef enum {
    AUDIO_DSP_CHANNEL_SELECTION_STEREO          = 0, /**< DSP streaming output L and R will be it own. */
    AUDIO_DSP_CHANNEL_SELECTION_MONO            = 1, /**< DSP streaming output L and R will be (L+R)/2. */
    AUDIO_DSP_CHANNEL_SELECTION_BOTH_L          = 2, /**< DSP streaming output both L. */
    AUDIO_DSP_CHANNEL_SELECTION_BOTH_R          = 3, /**< DSP streaming output both R. */
    AUDIO_DSP_CHANNEL_SELECTION_NUM,
} AUDIO_DSP_CHANNEL_SELECTION;

/** @brief audio MCLK status structure */
typedef struct {
    bool                        status;                 /**< Audio mclk on/off status*/
    int16_t                     mclk_cntr;              /**< Audio mclk user count*/
    afe_apll_source_t           apll;                   /**< Specifies the apll of mclk source.*/
    uint8_t                     divider;                /**< Specifies the divider of mclk source, MCLK = clock_source/(1+Divider), Divider = [6:0].*/
} hal_audio_mclk_status_t;

/**
  * @}
  */
/**
 * @}
 * @}
 */
#endif /* #ifdef HAL_AUDIO_MODULE_ENABLED */



#ifdef __cplusplus
}
#endif /* #ifdef __cplusplus */

#endif /* #ifndef __HAL_PLATFORM_H__ */


