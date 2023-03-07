
/*
#   Copyright (c) 2011-2016, GreenPeak Technologies
#   Copyright (c) 2017-2021, Qorvo Inc
 *
 *
 *   QPG6105 Radio board plugged on QPG6105 Smart Home And Lighting Carrier board 
 *
 * This software is owned by Qorvo Inc
 * and protected under applicable copyright laws.
 * It is delivered under the terms of the license
 * and is intended and supplied for use solely and
 * exclusively with products manufactured by
 * Qorvo Inc.
 *
 *
 * THIS SOFTWARE IS PROVIDED IN AN "AS IS"
 * CONDITION. NO WARRANTIES, WHETHER EXPRESS,
 * IMPLIED OR STATUTORY, INCLUDING, BUT NOT
 * LIMITED TO, IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS FOR A
 * PARTICULAR PURPOSE APPLY TO THIS SOFTWARE.
 * QORVO INC. SHALL NOT, IN ANY
 * CIRCUMSTANCES, BE LIABLE FOR SPECIAL,
 * INCIDENTAL OR CONSEQUENTIAL DAMAGES,
 * FOR ANY REASON WHATSOEVER.
 *
 * $Header$
 * $Change$
 * $DateTime$
 *
 */


#ifndef _GPBSP_QPG6105DK_B01_H_
#define _GPBSP_QPG6105DK_B01_H_

/*****************************************************************************
 *                    Code Extensions
 *****************************************************************************/

#ifdef GP_DIVERSITY_GPHAL_TRIM_XTAL_32M
// Include for 32MHz trimcap temperature calibration configuration file
#include "gpBsp_TrimXtal32M_default.h"
#endif // GP_DIVERSITY_GPHAL_TRIM_XTAL_32M

/*****************************************************************************
 *                    BSP configuration interface
 *****************************************************************************/

#define GP_BSP_INTERNAL_UC
// Is a 32kHz crystal mounted?
#define GP_BSP_32KHZ_CRYSTAL_AVAILABLE() 1
// Is the watchdog timer used?
#define GP_BSP_USE_WDT_TIMER() 1
// Has the board an differential antenna?
#define GP_BSP_HAS_DIFFERENTIAL_SINGLE_ANTENNA() 0
// Settling time for tx monitor in symbols
#define GP_BSP_TXMONITOR_SETTLING_TIME_IN_SYMBOLS() 0

/*****************************************************************************
 *                    Debug
 *****************************************************************************/

//SWJDP interface will be disabled at startup - overlapping pins:
// Pin 23 - GPIO 6 - SWDIO_TMS
// Pin 24 - GPIO 7 - SWCLK_TCK
// Pin 27 - GPIO 8 - TDI, UART0_RX
// Pin 28 - GPIO 9 - SWV_TDO, UART0_TX

#define GP_BSP_DEBUG_SWJDP_AVAILABLE

//SIF mapping used in debug mode:
// Pin 23 - GPIO 6 - SWDIO_TMS
#define GP_BSP_DEBUG_SWJDP_SWDIO_TMS_GPIO           6
#define GP_BSP_DEBUG_SWJDP_SWDIO_TMS_ALTERNATE      GP_WB_ENUM_GPIO_6_ALTERNATES_SWJDP_SWDIO_TMS
#define GP_BSP_DEBUG_SWJDP_SWDIO_TMS_INIT()         do{ GP_WB_WRITE_IOB_GPIO_6_ALTERNATE(GP_WB_ENUM_GPIO_6_ALTERNATES_SWJDP_SWDIO_TMS); GP_WB_WRITE_IOB_GPIO_6_ALTERNATE_ENABLE(1); }while(0)
#define GP_BSP_DEBUG_SWJDP_SWDIO_TMS_DEINIT()       GP_WB_WRITE_IOB_GPIO_6_ALTERNATE_ENABLE(0);
// Pin 24 - GPIO 7 - SWCLK_TCK
#define GP_BSP_DEBUG_SWJDP_SWCLK_TCK_GPIO           7
#define GP_BSP_DEBUG_SWJDP_SWCLK_TCK_ALTERNATE      GP_WB_ENUM_GPIO_7_ALTERNATES_SWJDP_SWCLK_TCK
#define GP_BSP_DEBUG_SWJDP_SWCLK_TCK_INIT()         do{ GP_WB_WRITE_IOB_GPIO_7_ALTERNATE(GP_WB_ENUM_GPIO_7_ALTERNATES_SWJDP_SWCLK_TCK); GP_WB_WRITE_IOB_GPIO_7_ALTERNATE_ENABLE(1); }while(0)
#define GP_BSP_DEBUG_SWJDP_SWCLK_TCK_DEINIT()       GP_WB_WRITE_IOB_GPIO_7_ALTERNATE_ENABLE(0);
// Pin 27 - GPIO 8 - TDI, UART0_RX
#define GP_BSP_DEBUG_SWJDP_TDI_GPIO                 8
#define GP_BSP_DEBUG_SWJDP_TDI_ALTERNATE            GP_WB_ENUM_GPIO_8_ALTERNATES_SWJDP_TDI
#define GP_BSP_DEBUG_SWJDP_TDI_INIT()               do{ GP_WB_WRITE_IOB_GPIO_8_ALTERNATE(GP_WB_ENUM_GPIO_8_ALTERNATES_SWJDP_TDI); GP_WB_WRITE_IOB_GPIO_8_ALTERNATE_ENABLE(1); }while(0)
#define GP_BSP_DEBUG_SWJDP_TDI_DEINIT()             GP_WB_WRITE_IOB_GPIO_8_ALTERNATE_ENABLE(0);
// Pin 28 - GPIO 9 - SWV_TDO, UART0_TX
#define GP_BSP_DEBUG_SWJDP_SWV_TDO_GPIO             9
#define GP_BSP_DEBUG_SWJDP_SWV_TDO_ALTERNATE        GP_WB_ENUM_GPIO_9_ALTERNATES_SWJDP_SWV_TDO
#define GP_BSP_DEBUG_SWJDP_SWV_TDO_INIT()           do{ GP_WB_WRITE_IOB_GPIO_9_ALTERNATE(GP_WB_ENUM_GPIO_9_ALTERNATES_SWJDP_SWV_TDO); GP_WB_WRITE_IOB_GPIO_9_ALTERNATE_ENABLE(1); }while(0)
#define GP_BSP_DEBUG_SWJDP_SWV_TDO_DEINIT()         GP_WB_WRITE_IOB_GPIO_9_ALTERNATE_ENABLE(0);

/*****************************************************************************
 *                    GPIO - LED
 *****************************************************************************/

/* No GRN led available, map to RED led */ 
#define GRN RED
#define HAL_LED_SET_GRN() HAL_LED_SET_RED()
#define HAL_LED_CLR_GRN() HAL_LED_CLR_RED()
#define HAL_LED_TST_GRN() HAL_LED_TST_RED()
#define HAL_LED_TGL_GRN() HAL_LED_TGL_RED()


/* RED led - LD4 - GPIO block driven / Push-button GP PB 5 */
#define GP_BSP_LED_RED_PIN 0
#define GP_BSP_LED_RED_LOGIC_LEVEL 0
// HAL helpers
#define RED 0 // GPIO0 - LED Active when low
#define HAL_LED_SET_RED() GP_WB_WRITE_GPIO_GPIO0_DIRECTION(1)
#define HAL_LED_CLR_RED() GP_WB_WRITE_GPIO_GPIO0_DIRECTION(0)
#define HAL_LED_TST_RED() GP_WB_READ_GPIO_GPIO0_DIRECTION()
#define HAL_LED_TGL_RED() do{ if (HAL_LED_TST_RED()) { HAL_LED_CLR_RED(); } else { HAL_LED_SET_RED(); }; }while(false)

#define HAL_LED_INIT_LEDS()                         do{ \
    /*Initialize output value - switching input/output will toggle LED*/ \
    GP_WB_WRITE_GPIO_GPIO0_OUTPUT_VALUE(0); \
    GP_WB_WRITE_IOB_GPIO_0_CFG(GP_WB_ENUM_GPIO_MODE_PULLUP); \
    HAL_LED_CLR_RED(); \
    /*Drive strength*/ \
    GP_WB_WRITE_IOB_GPIO_0_3_DRIVE_STRENGTH(GP_WB_ENUM_DRIVE_STRENGTH_DRIVE_18MA); \
}while(0)

#define GP_BSP_LED_GPIO_MAP                         { 0xff, 0xff, 0xff, 0xff }
#define GP_BSP_LED_ALTERNATE_MAP                    { 0, 0, 0, 0 }

/*****************************************************************************
 *                    GPIO - BTN
 *****************************************************************************/

/* RED led - LD4 - GPIO block driven / Push-button GP PB 5 */
#define GP_BSP_BUTTON_GP_PB5_PIN  0 //GPIO0
#define GP_BSP_BUTTON_GP_PB5_LOGIC_LEVEL 0 //Active low
// HAL helpers
#define GP_PB5  0
#define HAL_BUTTON_GP_PB5_IS_PRESSED()  (!GP_WB_READ_GPIO_GPIO0_INPUT_VALUE()) /*Button pin low = pressed*/

/* Connected to Humidity sensor U11 - I2C SDA / Push-button GP PB 4 */
#define GP_BSP_BUTTON_GP_PB4_PIN  2 //GPIO2
#define GP_BSP_BUTTON_GP_PB4_LOGIC_LEVEL 0 //Active low
// HAL helpers
#define GP_PB4  2
#define HAL_BUTTON_GP_PB4_IS_PRESSED()  (!GP_WB_READ_GPIO_GPIO2_INPUT_VALUE()) /*Button pin low = pressed*/

/* Connected to Humidity sensor U11 - I2C SCLK / Push-button GP PB 3 / PROG_EN for QPG6100 */
#define GP_BSP_BUTTON_GP_PB3_PIN  3 //GPIO3
#define GP_BSP_BUTTON_GP_PB3_LOGIC_LEVEL 0 //Active low
// HAL helpers
#define GP_PB3  3
#define HAL_BUTTON_GP_PB3_IS_PRESSED()  (!GP_WB_READ_GPIO_GPIO3_INPUT_VALUE()) /*Button pin low = pressed*/

/* Slider switch GP SW */
#define GP_BSP_BUTTON_GP_SW_PIN  4 //GPIO4
#define GP_BSP_BUTTON_GP_SW_LOGIC_LEVEL 0 //Active low
// HAL helpers
#define GP_SW  4
#define HAL_BUTTON_GP_SW_IS_PRESSED()  (!GP_WB_READ_GPIO_GPIO4_INPUT_VALUE()) /*Button pin low = pressed*/

/* Push-button GP PB 1 / PROG_EN QPG6105 */
#define GP_BSP_BUTTON_GP_PB1_PIN  5 //GPIO5
#define GP_BSP_BUTTON_GP_PB1_LOGIC_LEVEL 0 //Active low
// HAL helpers
#define GP_PB1  5
#define HAL_BUTTON_GP_PB1_IS_PRESSED()  (!GP_WB_READ_GPIO_GPIO5_INPUT_VALUE()) /*Button pin low = pressed*/

/* Analog I/O - A1 / Push-button GP PB 2 */
#define GP_BSP_BUTTON_GP_PB2_PIN  22 //GPIO22
#define GP_BSP_BUTTON_GP_PB2_LOGIC_LEVEL 0 //Active low
// HAL helpers
#define GP_PB2  22
#define HAL_BUTTON_GP_PB2_IS_PRESSED()  (!GP_WB_READ_GPIO_GPIO22_INPUT_VALUE()) /*Button pin low = pressed*/


#define HAL_BTN_INIT_BTNS()                         do{ \
    GP_WB_WRITE_IOB_GPIO_0_CFG(GP_WB_ENUM_GPIO_MODE_PULLUP); /*Pull up pin*/ \
    GP_WB_WRITE_GPIO_GPIO0_DIRECTION(0); /*Set as input*/ \
    GP_WB_WRITE_IOB_GPIO_2_CFG(GP_WB_ENUM_GPIO_MODE_PULLUP); /*Pull up pin*/ \
    GP_WB_WRITE_GPIO_GPIO2_DIRECTION(0); /*Set as input*/ \
    GP_WB_WRITE_IOB_GPIO_3_CFG(GP_WB_ENUM_GPIO_MODE_PULLUP); /*Pull up pin*/ \
    GP_WB_WRITE_GPIO_GPIO3_DIRECTION(0); /*Set as input*/ \
    GP_WB_WRITE_IOB_GPIO_4_CFG(GP_WB_ENUM_GPIO_MODE_PULLUP); /*Pull up pin*/ \
    GP_WB_WRITE_GPIO_GPIO4_DIRECTION(0); /*Set as input*/ \
    GP_WB_WRITE_IOB_GPIO_5_CFG(GP_WB_ENUM_GPIO_MODE_PULLUP); /*Pull up pin*/ \
    GP_WB_WRITE_GPIO_GPIO5_DIRECTION(0); /*Set as input*/ \
    GP_WB_WRITE_IOB_GPIO_22_CFG(GP_WB_ENUM_GPIO_MODE_PULLUP); /*Pull up pin*/ \
    GP_WB_WRITE_GPIO_GPIO22_DIRECTION(0); /*Set as input*/ \
}while(0)

/*****************************************************************************
 *                    GPIO - ALTERNATIVE - not used
 *****************************************************************************/


/*****************************************************************************
 *                    UART
 *****************************************************************************/

// Pin 27 - GPIO 8 - TDI, UART0_RX
#define GP_BSP_UART0_RX_GPIO                        8
#define GP_BSP_UART0_RX_ALTERNATE                   GP_WB_ENUM_GPIO_8_ALTERNATES_UART_0_RX
#define GP_BSP_UART0_RX_INIT()                      do{ GP_WB_WRITE_IOB_GPIO_8_ALTERNATE(GP_WB_ENUM_GPIO_8_ALTERNATES_UART_0_RX); GP_WB_WRITE_IOB_GPIO_8_ALTERNATE_ENABLE(1); }while(0)
#define GP_BSP_UART0_RX_DEINIT()                    GP_WB_WRITE_IOB_GPIO_8_ALTERNATE_ENABLE(0);
#define GP_BSP_UART0_RX_DEFINED()                   (1)
#define GP_BSP_UART0_RX_GPIO_CFG()                  GP_WB_WRITE_IOB_GPIO_8_CFG(GP_WB_ENUM_GPIO_MODE_PULLUP)
#define GP_BSP_UART0_RX_ENABLE(en)                  do{ if (en) { GP_WB_WRITE_IOB_GPIO_8_ALTERNATE(GP_WB_ENUM_GPIO_8_ALTERNATES_UART_0_RX); } GP_WB_WRITE_IOB_GPIO_8_ALTERNATE_ENABLE((en)); }while(0)
#define GP_BSP_UART0_RX_ENABLED()                   GP_WB_READ_IOB_GPIO_8_ALTERNATE_ENABLE()

// Pin 28 - GPIO 9 - SWV_TDO, UART0_TX
#define GP_BSP_UART0_TX_GPIO                        9
#define GP_BSP_UART0_TX_ALTERNATE                   GP_WB_ENUM_GPIO_9_ALTERNATES_UART_0_TX
#define GP_BSP_UART0_TX_INIT()                      do{ GP_WB_WRITE_IOB_GPIO_9_ALTERNATE(GP_WB_ENUM_GPIO_9_ALTERNATES_UART_0_TX); GP_WB_WRITE_IOB_GPIO_9_ALTERNATE_ENABLE(1); }while(0)
#define GP_BSP_UART0_TX_DEINIT()                    GP_WB_WRITE_IOB_GPIO_9_ALTERNATE_ENABLE(0);
#define GP_BSP_UART0_TX_DEFINED()                   (1)
#define GP_BSP_UART0_TX_GPIO_CFG()                  GP_WB_WRITE_IOB_GPIO_9_CFG(GP_WB_ENUM_GPIO_MODE_FLOAT)
#define GP_BSP_UART0_TX_ENABLE(en)                  do{ if (en) { GP_WB_WRITE_IOB_GPIO_9_ALTERNATE(GP_WB_ENUM_GPIO_9_ALTERNATES_UART_0_TX); } GP_WB_WRITE_IOB_GPIO_9_ALTERNATE_ENABLE((en)); }while(0)
#define GP_BSP_UART0_TX_ENABLED()                   GP_WB_READ_IOB_GPIO_9_ALTERNATE_ENABLE()

#define GP_BSP_UART1_RX_DEFINED()                   (0)
#define GP_BSP_UART1_RX_GPIO_CFG()                  do { } while(0)
#define GP_BSP_UART1_RX_ENABLE(en)                  do { } while(0)
#define GP_BSP_UART1_RX_ENABLED()                   (0)

#define GP_BSP_UART1_TX_DEFINED()                   (0)
#define GP_BSP_UART1_TX_GPIO_CFG()                  do { } while(0)
#define GP_BSP_UART1_TX_ENABLE(en)                  do { } while(0)
#define GP_BSP_UART1_TX_ENABLED()                   (0)

#define GP_BSP_UART_TX_GPIO_MAP                     { 9, 0xff }
#define GP_BSP_UART_TX_ALTERNATE_MAP                { GP_WB_ENUM_GPIO_9_ALTERNATES_UART_0_TX, 0 }
#define GP_BSP_UART_RX_GPIO_MAP                     { 8, 0xff }
#define GP_BSP_UART_RX_ALTERNATE_MAP                { GP_WB_ENUM_GPIO_8_ALTERNATES_UART_0_RX, 0 }

#define GP_BSP_UART_COM1                            0

#ifndef GP_BSP_UART_COM_BAUDRATE
#define GP_BSP_UART_COM_BAUDRATE                    115200
#endif

#ifndef GP_BSP_UART_SCOM_BAUDRATE
#define GP_BSP_UART_SCOM_BAUDRATE                   115200
#endif

/*****************************************************************************
 *                    MSPI
 *****************************************************************************/

/* Connected to SPI Flash U5 (A25SF081) - SPI SCK */
// Pin 29 - GPIO 10 - MSPI_SCLK
#define GP_BSP_MSPI_SCLK_GPIO                       10
#define GP_BSP_MSPI_SCLK_ALTERNATE                  GP_WB_ENUM_GPIO_10_ALTERNATES_SPI_M_SCLK
#define GP_BSP_MSPI_SCLK_INIT()                     do{ GP_WB_WRITE_IOB_GPIO_10_ALTERNATE(GP_WB_ENUM_GPIO_10_ALTERNATES_SPI_M_SCLK); GP_WB_WRITE_IOB_GPIO_10_ALTERNATE_ENABLE(1); }while(0)
#define GP_BSP_MSPI_SCLK_DEINIT()                   GP_WB_WRITE_IOB_GPIO_10_ALTERNATE_ENABLE(0);
/* Connected to SPI Flash U5 (A25SF081) - SPI MOSI */
// Pin 30 - GPIO 11 - MSPI_MOSI
#define GP_BSP_MSPI_MOSI_GPIO                       11
#define GP_BSP_MSPI_MOSI_ALTERNATE                  GP_WB_ENUM_GPIO_11_ALTERNATES_SPI_M_MOSI
#define GP_BSP_MSPI_MOSI_INIT()                     do{ GP_WB_WRITE_IOB_GPIO_11_ALTERNATE(GP_WB_ENUM_GPIO_11_ALTERNATES_SPI_M_MOSI); GP_WB_WRITE_IOB_GPIO_11_ALTERNATE_ENABLE(1); }while(0)
#define GP_BSP_MSPI_MOSI_DEINIT()                   GP_WB_WRITE_IOB_GPIO_11_ALTERNATE_ENABLE(0);
/* Connected to SPI Flash U5 (A25SF081) - SPI MISO */
// Pin 31 - GPIO 12 - MSPI_MISO
#define GP_BSP_MSPI_MISO_GPIO                       12
#define GP_BSP_MSPI_MISO_ALTERNATE                  GP_WB_ENUM_GPIO_12_ALTERNATES_SPI_M_MISO
#define GP_BSP_MSPI_MISO_INIT()                     do{ GP_WB_WRITE_IOB_GPIO_12_ALTERNATE(GP_WB_ENUM_GPIO_12_ALTERNATES_SPI_M_MISO); GP_WB_WRITE_IOB_GPIO_12_ALTERNATE_ENABLE(1); }while(0)
#define GP_BSP_MSPI_MISO_DEINIT()                   GP_WB_WRITE_IOB_GPIO_12_ALTERNATE_ENABLE(0);


/*****************************************************************************
 *                    PWM
 *****************************************************************************/

/* RGB led - D1 - Red channel */
// Pin  3 - GPIO 14 - PWM0
#define GP_BSP_PWM0_GPIO                            14
#define GP_BSP_PWM0_ALTERNATE                       GP_WB_ENUM_GPIO_14_ALTERNATES_PWM_DRV_DO_0
#define GP_BSP_PWM0_INIT()                          do{ GP_WB_WRITE_IOB_GPIO_14_ALTERNATE(GP_WB_ENUM_GPIO_14_ALTERNATES_PWM_DRV_DO_0); GP_WB_WRITE_IOB_GPIO_14_ALTERNATE_ENABLE(1); }while(0)
#define GP_BSP_PWM0_DEINIT()                        GP_WB_WRITE_IOB_GPIO_14_ALTERNATE_ENABLE(0);
#define GP_BSP_PWM0_DRIVE                           GP_WB_ENUM_GENERIC_IO_DRIVE_PUSH_PULL
/* RGB led - D1 - Green channel */
// Pin  2 - GPIO 15 - PWM1
#define GP_BSP_PWM1_GPIO                            15
#define GP_BSP_PWM1_ALTERNATE                       GP_WB_ENUM_GPIO_15_ALTERNATES_PWM_DRV_DO_1
#define GP_BSP_PWM1_INIT()                          do{ GP_WB_WRITE_IOB_GPIO_15_ALTERNATE(GP_WB_ENUM_GPIO_15_ALTERNATES_PWM_DRV_DO_1); GP_WB_WRITE_IOB_GPIO_15_ALTERNATE_ENABLE(1); }while(0)
#define GP_BSP_PWM1_DEINIT()                        GP_WB_WRITE_IOB_GPIO_15_ALTERNATE_ENABLE(0);
#define GP_BSP_PWM1_DRIVE                           GP_WB_ENUM_GENERIC_IO_DRIVE_PUSH_PULL
/* RGB led - D1 - Blue channel */
// Pin  1 - GPIO 16 - PWM2
#define GP_BSP_PWM2_GPIO                            16
#define GP_BSP_PWM2_ALTERNATE                       GP_WB_ENUM_GPIO_16_ALTERNATES_PWM_DRV_DO_2
#define GP_BSP_PWM2_INIT()                          do{ GP_WB_WRITE_IOB_GPIO_16_ALTERNATE(GP_WB_ENUM_GPIO_16_ALTERNATES_PWM_DRV_DO_2); GP_WB_WRITE_IOB_GPIO_16_ALTERNATE_ENABLE(1); }while(0)
#define GP_BSP_PWM2_DEINIT()                        GP_WB_WRITE_IOB_GPIO_16_ALTERNATE_ENABLE(0);
#define GP_BSP_PWM2_DRIVE                           GP_WB_ENUM_GENERIC_IO_DRIVE_PUSH_PULL
/* White (Cool) LD1 */
// Pin 15 - GPIO 17 - PWM4
#define GP_BSP_PWM4_GPIO                            17
#define GP_BSP_PWM4_ALTERNATE                       GP_WB_ENUM_GPIO_17_ALTERNATES_PWM_DRV_DO_4
#define GP_BSP_PWM4_INIT()                          do{ GP_WB_WRITE_IOB_GPIO_17_ALTERNATE(GP_WB_ENUM_GPIO_17_ALTERNATES_PWM_DRV_DO_4); GP_WB_WRITE_IOB_GPIO_17_ALTERNATE_ENABLE(1); }while(0)
#define GP_BSP_PWM4_DEINIT()                        GP_WB_WRITE_IOB_GPIO_17_ALTERNATE_ENABLE(0);
#define GP_BSP_PWM4_DRIVE                           GP_WB_ENUM_GENERIC_IO_DRIVE_PUSH_PULL
/* White (Warm) LD2 */
// Pin 16 - GPIO 18 - PWM5
#define GP_BSP_PWM5_GPIO                            18
#define GP_BSP_PWM5_ALTERNATE                       GP_WB_ENUM_GPIO_18_ALTERNATES_PWM_DRV_DO_5
#define GP_BSP_PWM5_INIT()                          do{ GP_WB_WRITE_IOB_GPIO_18_ALTERNATE(GP_WB_ENUM_GPIO_18_ALTERNATES_PWM_DRV_DO_5); GP_WB_WRITE_IOB_GPIO_18_ALTERNATE_ENABLE(1); }while(0)
#define GP_BSP_PWM5_DEINIT()                        GP_WB_WRITE_IOB_GPIO_18_ALTERNATE_ENABLE(0);
#define GP_BSP_PWM5_DRIVE                           GP_WB_ENUM_GENERIC_IO_DRIVE_PUSH_PULL

#define GP_BSP_PWM_GPIO_MAP                         { 14, 15, 16, 0xff, 17, 18, 0xff, 0xff }
#define GP_BSP_PWM_ALTERNATE_MAP                    { GP_WB_ENUM_GPIO_14_ALTERNATES_PWM_DRV_DO_0, GP_WB_ENUM_GPIO_15_ALTERNATES_PWM_DRV_DO_1, GP_WB_ENUM_GPIO_16_ALTERNATES_PWM_DRV_DO_2, 0, GP_WB_ENUM_GPIO_17_ALTERNATES_PWM_DRV_DO_4, GP_WB_ENUM_GPIO_18_ALTERNATES_PWM_DRV_DO_5, 0, 0 }
#define GP_BSP_PWM_DRIVE_MAP                        { GP_WB_ENUM_GENERIC_IO_DRIVE_PUSH_PULL, GP_WB_ENUM_GENERIC_IO_DRIVE_PUSH_PULL, GP_WB_ENUM_GENERIC_IO_DRIVE_PUSH_PULL, 0, GP_WB_ENUM_GENERIC_IO_DRIVE_PUSH_PULL, GP_WB_ENUM_GENERIC_IO_DRIVE_PUSH_PULL, 0, 0 }

/*****************************************************************************
 *                    ADC
 *****************************************************************************/

// Pin 13 - GPIO 21 - ANIO0
#define GP_BSP_ANIO0_GPIO                           21
#define GP_BSP_ANIO0_INIT()                         GP_WB_WRITE_IOB_GPIO_21_ALTERNATE_ENABLE(0)
#define GP_BSP_ANIO0_DEINIT()                       do{ }while(0)
// Pin 14 - GPIO 22 - GPIO22, ANIO1
#define GP_BSP_ANIO1_GPIO                           22
#define GP_BSP_ANIO1_INIT()                         GP_WB_WRITE_IOB_GPIO_22_ALTERNATE_ENABLE(0)
#define GP_BSP_ANIO1_DEINIT()                       do{ }while(0)

#define GP_BSP_ADC_GPIO_MAP                         { 21, 22, 17, 18, 0xff, 0xff, 0xff, 0xff }

/*****************************************************************************
 *                    MTWI
 *****************************************************************************/

/* Connected to Humidity sensor U11 - I2C SDA / Push-button GP PB 4 */
// Pin 19 - GPIO 2 - MTWI_SDA, GPIO2
#define GP_BSP_MTWI_SDA_GPIO                        2
#define GP_BSP_MTWI_SDA_ALTERNATE                   GP_WB_ENUM_GPIO_2_ALTERNATES_I2C_M_SDA
#define GP_BSP_MTWI_SDA_INIT()                      do{ GP_WB_WRITE_IOB_GPIO_2_ALTERNATE(GP_WB_ENUM_GPIO_2_ALTERNATES_I2C_M_SDA); GP_WB_WRITE_IOB_GPIO_2_ALTERNATE_ENABLE(1); }while(0)
#define GP_BSP_MTWI_SDA_DEINIT()                    GP_WB_WRITE_IOB_GPIO_2_ALTERNATE_ENABLE(0);
/* Connected to Humidity sensor U11 - I2C SCLK / Push-button GP PB 3 / PROG_EN for QPG6100 */
// Pin 20 - GPIO 3 - MTWI_SCLK, GPIO3
#define GP_BSP_MTWI_SCLK_GPIO                       3
#define GP_BSP_MTWI_SCLK_ALTERNATE                  GP_WB_ENUM_GPIO_3_ALTERNATES_I2C_M_SCL
#define GP_BSP_MTWI_SCLK_INIT()                     do{ GP_WB_WRITE_IOB_GPIO_3_ALTERNATE(GP_WB_ENUM_GPIO_3_ALTERNATES_I2C_M_SCL); GP_WB_WRITE_IOB_GPIO_3_ALTERNATE_ENABLE(1); }while(0)
#define GP_BSP_MTWI_SCLK_DEINIT()                   GP_WB_WRITE_IOB_GPIO_3_ALTERNATE_ENABLE(0);

/*****************************************************************************
 *                    GPIO - Unused
 *****************************************************************************/

/* Pull down unused pins */
#define GP_BSP_UNUSED_INIT()                        do{ \
    GP_WB_WRITE_IOB_GPIO_6_CFG(GP_WB_ENUM_GPIO_MODE_PULLDOWN); \
    GP_WB_WRITE_IOB_GPIO_7_CFG(GP_WB_ENUM_GPIO_MODE_PULLDOWN); \
    GP_WB_WRITE_IOB_GPIO_13_CFG(GP_WB_ENUM_GPIO_MODE_PULLDOWN); \
}while(0)

/*****************************************************************************
 *                    IO Pending
 *****************************************************************************/

#define HAL_BSP_IO_ACTIVITY_PENDING()               (false)

/*****************************************************************************
 *                    Generic
 *****************************************************************************/


/* Enable one of the defines below to enable the correct antenna tuning parameters and TX power table*/
/* #define GP_BSP_ANTENNATUNECONFIG_10DBM_DIFFERENTIAL */
 #define GP_BSP_ANTENNATUNECONFIG_10DBM_SINGLE_ENDED
/* #define GP_BSP_ANTENNATUNECONFIG_7DBM_DIFFERENTIAL */
/* #define GP_BSP_ANTENNATUNECONFIG_7DBM_SINGLE_ENDED */
/* #define GP_BSP_ANTENNATUNECONFIG_FEM_SINGLE_ENDED */

#include "gpBsp_k8e_antenna_tune_parameters.h"

#define GP_BSP_GENERIC_INIT()                       do{ \
    /*Disable bus keeper/PU/PD on UART0_RX*/ \
    GP_WB_WRITE_IOB_GPIO_8_CFG(GP_WB_ENUM_GPIO_MODE_FLOAT); \
    /*Disable bus keeper/PU/PD on UART0_TX*/ \
    GP_WB_WRITE_IOB_GPIO_9_CFG(GP_WB_ENUM_GPIO_MODE_FLOAT); \
    /*Set MTWI pins floating - externally pulled up*/ \
    GP_WB_WRITE_IOB_GPIO_2_CFG(GP_WB_ENUM_GPIO_MODE_FLOAT); /*MTWI SDA pin*/ \
    GP_WB_WRITE_IOB_GPIO_3_CFG(GP_WB_ENUM_GPIO_MODE_FLOAT); /*MTWI SCLK pin*/ \
    GP_BSP_ANTENNATUNECONFIG_INIT(); \
}while(0)

#define GP_BSP_SLEEPMODERETENTIONLIST \
    /* *** plme *** */\
    /* *** trx *** */\
    0x0080, /* ext_mode_ctrl_for_trx_off|ext_mode_ctrl_for_rx_on_att_ctrl_low|ext_mode_ctrl_for_rx_on_att_ctrl_high|ext_channel_ctrl_for_trx_off|mask_channel_ctrl_for_trx_off */ \
    0x0081, /* external_mode_control_1 */ \
    0x0082, /* external_mode_control_2 */ \
    /* *** rx *** */\
    0x0222, /* bt_sample_rssi_delay|bt_sample_rssi_on_early_sfd */ \
    0x0223, /* unnamedpb_rx_0x0022_1 */ \
    0x028A, /* bt_delay_early_birdy|en_zb_packet_found */ \
    0x02BC, /* fail_timeout_value|fail_timeout_en|cw_det_depends_on_fine */ \
    0x02BD, /* dbg_22_1 */ \
    /* *** tx *** */\
    0x0307, /* tx_pa_biastrim_mult */ \
    0x0309, /* pa_slope_zb|pa_slope_ble|fll_start_of_ramp|enable_external_pa_biasing */ \
    0x030C, /* tx_ldo_refbits */ \
    0x030E, /* tx_dca_n|tx_dca_p|tx_ldo_bleed_off|tx_ldo_pup|tx_pa_biastrim|tx_peak_lvl|tx_ldo_res_bypass */ \
    0x030F, /* unnamedpb_tx_0x000e_1 */ \
    /* *** rib *** */\
    /* *** qta *** */\
    /* *** prg *** */\
    /* *** pmud *** */\
    /* *** iob *** */\
    0x0710, /* gpio_0_3_drive_strength|gpio_0_3_schmitt_trigger|gpio_4_7_drive_strength|gpio_4_7_schmitt_trigger|gpio_8_11_drive_strength|gpio_8_11_schmitt_trigger|gpio_12_15_drive_strength|gpio_12_15_schmitt_trigger|gpio_16_19_drive_strength|gpio_16_19_schmitt_trigger|gpio_20_23_schmitt_trigger|gpio_20_23_drive_strength */ \
    0x0711, /* gpio_pin_config_b_1 */ \
    0x0712, /* gpio_pin_config_b_2 */ \
    /* *** standby *** */\
    /* *** es *** */\
    /* *** msi *** */\
    /* *** int_ctrl *** */\
    /* *** cortexm4 *** */\
    0x0C48, /* icode_dcode_block_0_remap|icode_dcode_block_1_remap|icode_dcode_block_2_remap|icode_dcode_block_3_remap */ \
    0x0C49, /* unnamedpb_cortexm4_0x0008_1 */ \
    0x0C4C, /* flash_virt_window_0_offset|flash_virt_window_1_offset|flash_virt_window_2_offset|flash_virt_window_3_offset */ \
    0x0C4D, /* unnamedpb_cortexm4_0x000c_1 */ \
    0x0C4E, /* unnamedpb_cortexm4_0x000c_2 */ \
    0x0C4F, /* unnamedpb_cortexm4_0x000c_3 */ \
    0x0C50, /* flash_virt_window_4_offset|flash_virt_window_5_offset|flash_virt_window_6_offset|flash_virt_window_7_offset */ \
    0x0C51, /* unnamedpb_cortexm4_0x0010_1 */ \
    0x0C52, /* unnamedpb_cortexm4_0x0010_2 */ \
    0x0C53, /* unnamedpb_cortexm4_0x0010_3 */ \
    /* *** mm *** */\
    0x0E94, /* conv_0_buffer_disable|conv_1_buffer_disable|conv_2_buffer_disable|conv_3_buffer_disable|conv_4_buffer_disable|conv_4_prefetcher_disable|conv_4_brchstat1_halt_prefetch_disable|conv_4_brchstat2_halt_prefetch_disable|conv_4_brchstat3_halt_prefetch_disable|conv_4_replacement_config */ \
    0x0E95, /* flash_conv_config_1 */ \
    /* *** pbm_adm *** */\
    /* *** gpio *** */\
    0x100A, /* exti0_port_sel|exti1_port_sel|exti2_port_sel|exti3_port_sel|exti4_port_sel|exti5_port_sel|exti6_port_sel|exti7_port_sel */ \
    0x100B, /* exti_port_sel_1 */ \
    0x100C, /* exti0_expected_value|exti1_expected_value|exti2_expected_value|exti3_expected_value|exti4_expected_value|exti5_expected_value|exti6_expected_value|exti7_expected_value */ \
    /* *** i2c_m *** */\
    /* *** adcif *** */\
    0x1068, /* adc_ldo_refbits|adc_vref_refbits|adc_scaler_bias_cgm_res|adc_scaler_vcm_refbits|adc_spare|smux_resload|adc_clk_mux_pup|adc_clk_select|adc_clk_speed|adc_comp_bias_boost|adc_comp_bias_pup|adc_ldo_bleed_off|adc_ldo_pup|adc_ldo_resbypass|adc_ovp_pup|adc_scaler_bypass|adc_scaler_filter_enable|adc_scaler_pup|adc_test_vref|adc_vcm_buf_pup|adc_vref_buf_pup|adc_vref_resbypass|smux_adc_buf_n_fullscale|smux_adc_buf_n_pup|smux_adc_buf_p_fullscale|smux_adc_buf_p_pup|smux_adc_channel_sel_pup|smux_external_reference|smux_resload_en_n|smux_resload_en_p|smux_selftest_mode|smux_tsensor_pup|xo_clk_4m_pup */ \
    0x1069, /* unnamedpb_adcif_0x0028_1 */ \
    0x106A, /* unnamedpb_adcif_0x0028_2 */ \
    0x106B, /* unnamedpb_adcif_0x0028_3 */ \
    0x106C, /* unnamedpb_adcif_0x0028_4 */ \
    0x106D, /* unnamedpb_adcif_0x0028_5 */ \
    /* *** uart_0 *** */\
    /* *** spi_m *** */\
    /* *** watchdog *** */\
    /* *** timers *** */\
    /* 0x1380 tmr0_prescaler_div|tmr0_clk_sel */ \
    0x1382, /* tmr0_threshold */ \
    0x1383, /* unnamedpb_timers_0x0002_1 */ \
    /* 0x138C tmr1_prescaler_div|tmr1_clk_sel */ \
    0x138E, /* tmr1_threshold */ \
    0x138F, /* unnamedpb_timers_0x000e_1 */ \
    0x1398, /* tmr2_prescaler_div|tmr2_clk_sel */ \
    0x139A, /* tmr2_threshold */ \
    0x139B, /* unnamedpb_timers_0x001a_1 */ \
    0x13A4, /* tmr3_prescaler_div|tmr3_clk_sel */ \
    0x13A6, /* tmr3_threshold */ \
    0x13A7, /* unnamedpb_timers_0x0026_1 */ \
    0x13B0, /* tmr4_prescaler_div|tmr4_clk_sel */ \
    0x13B2, /* tmr4_threshold */ \
    0x13B3, /* unnamedpb_timers_0x0032_1 */ \
    /* *** pwms *** */\
    0x1400, /* timestamp0_enable|timestamp0_fifo_size */ \
    0x1401, /* timestamp0_stable_low_power|timestamp0_stable_high_power */ \
    0x1408, /* timestamp1_enable|timestamp1_fifo_size */ \
    0x1409, /* timestamp1_stable_low_power|timestamp1_stable_high_power */ \
    0x1410, /* timestamp2_enable|timestamp2_fifo_size */ \
    0x1411, /* timestamp2_stable_low_power|timestamp2_stable_high_power */ \
    0x1418, /* timestamp3_enable|timestamp3_fifo_size */ \
    0x1419, /* timestamp3_stable_low_power|timestamp3_stable_high_power */ \
    0x1420, /* pwm0_up_down_enable|pwm0_output_drive|pwm0_output_invert */ \
    0x1421, /* pwm0_threshold_update_on_carrier_counter_wrap */ \
    0x1428, /* pwm1_up_down_enable|pwm1_output_drive|pwm1_output_invert */ \
    0x1429, /* pwm1_threshold_update_on_carrier_counter_wrap */ \
    0x1430, /* pwm2_up_down_enable|pwm2_output_drive|pwm2_output_invert */ \
    0x1431, /* pwm2_threshold_update_on_carrier_counter_wrap */ \
    0x1438, /* pwm3_up_down_enable|pwm3_output_drive|pwm3_output_invert */ \
    0x1439, /* pwm3_threshold_update_on_carrier_counter_wrap */ \
    0x1440, /* pwm4_up_down_enable|pwm4_output_drive|pwm4_output_invert */ \
    0x1441, /* pwm4_threshold_update_on_carrier_counter_wrap */ \
    0x1448, /* pwm5_up_down_enable|pwm5_output_drive|pwm5_output_invert */ \
    0x1449, /* pwm5_threshold_update_on_carrier_counter_wrap */ \
    0x1450, /* pwm6_up_down_enable|pwm6_output_drive|pwm6_output_invert */ \
    0x1451, /* pwm6_threshold_update_on_carrier_counter_wrap */ \
    0x1458, /* pwm7_up_down_enable|pwm7_output_drive|pwm7_output_invert */ \
    0x1459, /* pwm7_threshold_update_on_carrier_counter_wrap */ \
    0x1460, /* main_tmr|carrier_tmr|timestamp_tmr|next_threshold_fifo_size|auto_shift_threshold_update_on_carrier_counter_wrap */ \
    0x1461, /* config_1 */ \
    0
#endif //_GPBSP_QPG6105DK_B01_H_

