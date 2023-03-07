/*
 * Copyright (c) 2018, Texas Instruments Incorporated
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * *  Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *
 * *  Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * *  Neither the name of Texas Instruments Incorporated nor the names of
 *    its contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 * THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 * OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 * OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */
/** ============================================================================
 *  @file       CC2652RB_LAUNCHXL.h
 *
 *  @brief      CC2652RB_LAUNCHXL Board Specific header file.
 *
 *  The CC2652RB_LAUNCHXL header file should be included in an application as
 *  follows:
 *  @code
 *  #include "CC2652RB_LAUNCHXL.h"
 *  @endcode
 *
 *  ============================================================================
 */
#ifndef __CC2652RB_LAUNCHXL_BOARD_H__
#define __CC2652RB_LAUNCHXL_BOARD_H__

#ifdef __cplusplus
extern "C" {
#endif

/* Includes */
#include <ti/drivers/PIN.h>
#include <ti/devices/cc13x2_cc26x2/driverlib/ioc.h>

/* Externs */
extern const PIN_Config BoardGpioInitTable[];

/* Defines */
#ifndef CC2652RB_LAUNCHXL
#define CC2652RB_LAUNCHXL
#endif
/* Mapping of pins to board signals using general board aliases
 *      <board signal alias>                <pin mapping>
 */

/* Mapping of pins to board signals using general board aliases
 *      <board signal alias>                  <pin mapping>
 */

/* Analog Capable DIOs */
#define CC2652RB_LAUNCHXL_DIO23_ANALOG          IOID_23
#define CC2652RB_LAUNCHXL_DIO24_ANALOG          IOID_24
#define CC2652RB_LAUNCHXL_DIO25_ANALOG          IOID_25
#define CC2652RB_LAUNCHXL_DIO26_ANALOG          IOID_26
#define CC2652RB_LAUNCHXL_DIO27_ANALOG          IOID_27
#define CC2652RB_LAUNCHXL_DIO28_ANALOG          IOID_28
#define CC2652RB_LAUNCHXL_DIO29_ANALOG          IOID_29
#define CC2652RB_LAUNCHXL_DIO30_ANALOG          IOID_30

/* Digital IOs */
#define CC2652RB_LAUNCHXL_DIO0                  IOID_0
#define CC2652RB_LAUNCHXL_DIO1_RFSW             IOID_1
#define CC2652RB_LAUNCHXL_DIO12                 IOID_12
#define CC2652RB_LAUNCHXL_DIO15                 IOID_15
#define CC2652RB_LAUNCHXL_DIO16_TDO             IOID_16
#define CC2652RB_LAUNCHXL_DIO17_TDI             IOID_17
#define CC2652RB_LAUNCHXL_DIO21                 IOID_21
#define CC2652RB_LAUNCHXL_DIO22                 IOID_22

/* Discrete Inputs */
#define CC2652RB_LAUNCHXL_PIN_BTN1              IOID_13
#define CC2652RB_LAUNCHXL_PIN_BTN2              IOID_14

/* GPIO */
#define CC2652RB_LAUNCHXL_GPIO_LED_ON           1
#define CC2652RB_LAUNCHXL_GPIO_LED_OFF          0

/* I2C */
#define CC2652RB_LAUNCHXL_I2C0_SCL0             IOID_4
#define CC2652RB_LAUNCHXL_I2C0_SDA0             IOID_5

/* I2S */
#define CC2652RB_LAUNCHXL_I2S_ADO               IOID_0
#define CC2652RB_LAUNCHXL_I2S_ADI               IOID_1
#define CC2652RB_LAUNCHXL_I2S_BCLK              IOID_30
#define CC2652RB_LAUNCHXL_I2S_MCLK              PIN_UNASSIGNED
#define CC2652RB_LAUNCHXL_I2S_WCLK              IOID_29

/* LEDs */
#define CC2652RB_LAUNCHXL_PIN_LED_ON            1
#define CC2652RB_LAUNCHXL_PIN_LED_OFF           0
#define CC2652RB_LAUNCHXL_PIN_RLED              IOID_6
#define CC2652RB_LAUNCHXL_PIN_GLED              IOID_7

/* PWM Outputs */
#define CC2652RB_LAUNCHXL_PWMPIN0               CC2652RB_LAUNCHXL_PIN_RLED
#define CC2652RB_LAUNCHXL_PWMPIN1               CC2652RB_LAUNCHXL_PIN_GLED
#define CC2652RB_LAUNCHXL_PWMPIN2               PIN_UNASSIGNED
#define CC2652RB_LAUNCHXL_PWMPIN3               PIN_UNASSIGNED
#define CC2652RB_LAUNCHXL_PWMPIN4               PIN_UNASSIGNED
#define CC2652RB_LAUNCHXL_PWMPIN5               PIN_UNASSIGNED
#define CC2652RB_LAUNCHXL_PWMPIN6               PIN_UNASSIGNED
#define CC2652RB_LAUNCHXL_PWMPIN7               PIN_UNASSIGNED

/* SPI */
#define CC2652RB_LAUNCHXL_SPI_FLASH_CS          IOID_20
#define CC2652RB_LAUNCHXL_FLASH_CS_ON           0
#define CC2652RB_LAUNCHXL_FLASH_CS_OFF          1

/* SPI Board */
#define CC2652RB_LAUNCHXL_SPI0_MISO             IOID_8          /* RF1.20 */
#define CC2652RB_LAUNCHXL_SPI0_MOSI             IOID_9          /* RF1.18 */
#define CC2652RB_LAUNCHXL_SPI0_CLK              IOID_10         /* RF1.16 */
#define CC2652RB_LAUNCHXL_SPI0_CSN              PIN_UNASSIGNED
#define CC2652RB_LAUNCHXL_SPI1_MISO             PIN_UNASSIGNED
#define CC2652RB_LAUNCHXL_SPI1_MOSI             PIN_UNASSIGNED
#define CC2652RB_LAUNCHXL_SPI1_CLK              PIN_UNASSIGNED
#define CC2652RB_LAUNCHXL_SPI1_CSN              PIN_UNASSIGNED

/* UART Board */
#define CC2652RB_LAUNCHXL_UART0_RX              IOID_2          /* RXD */
#define CC2652RB_LAUNCHXL_UART0_TX              IOID_3          /* TXD */
#define CC2652RB_LAUNCHXL_UART0_CTS             IOID_19         /* CTS */
#define CC2652RB_LAUNCHXL_UART0_RTS             IOID_18         /* RTS */
#define CC2652RB_LAUNCHXL_UART1_RX              PIN_UNASSIGNED
#define CC2652RB_LAUNCHXL_UART1_TX              PIN_UNASSIGNED
#define CC2652RB_LAUNCHXL_UART1_CTS             PIN_UNASSIGNED
#define CC2652RB_LAUNCHXL_UART1_RTS             PIN_UNASSIGNED
/* For backward compatibility */
#define CC2652RB_LAUNCHXL_UART_RX               CC2652RB_LAUNCHXL_UART0_RX
#define CC2652RB_LAUNCHXL_UART_TX               CC2652RB_LAUNCHXL_UART0_TX
#define CC2652RB_LAUNCHXL_UART_CTS              CC2652RB_LAUNCHXL_UART0_CTS
#define CC2652RB_LAUNCHXL_UART_RTS              CC2652RB_LAUNCHXL_UART0_RTS

/*!
 *  @brief  Initialize the general board specific settings
 *
 *  This function initializes the general board specific settings.
 */
void CC2652RB_LAUNCHXL_initGeneral(void);

/*!
 *  @brief  Shut down the external flash present on the board files
 *
 *  This function bitbangs the SPI sequence necessary to turn off
 *  the external flash on LaunchPads.
 */
void CC2652RB_LAUNCHXL_shutDownExtFlash(void);

/*!
 *  @brief  Wake up the external flash present on the board files
 *
 *  This function toggles the chip select for the amount of time needed
 *  to wake the chip up.
 */
void CC2652RB_LAUNCHXL_wakeUpExtFlash(void);


/*!
 *  @def    CC2652RB_LAUNCHXL_ADCBufName
 *  @brief  Enum of ADCs
 */
typedef enum CC2652RB_LAUNCHXL_ADCBufName {
    CC2652RB_LAUNCHXL_ADCBUF0 = 0,

    CC2652RB_LAUNCHXL_ADCBUFCOUNT
} CC2652RB_LAUNCHXL_ADCBufName;

/*!
 *  @def    CC2652RB_LAUNCHXL_ADCBuf0SourceName
 *  @brief  Enum of ADCBuf channels
 */
typedef enum CC2652RB_LAUNCHXL_ADCBuf0ChannelName {
    CC2652RB_LAUNCHXL_ADCBUF0CHANNEL0 = 0,
    CC2652RB_LAUNCHXL_ADCBUF0CHANNEL1,
    CC2652RB_LAUNCHXL_ADCBUF0CHANNEL2,
    CC2652RB_LAUNCHXL_ADCBUF0CHANNEL3,
    CC2652RB_LAUNCHXL_ADCBUF0CHANNEL4,
    CC2652RB_LAUNCHXL_ADCBUF0CHANNEL5,
    CC2652RB_LAUNCHXL_ADCBUF0CHANNEL6,
    CC2652RB_LAUNCHXL_ADCBUF0CHANNEL7,
    CC2652RB_LAUNCHXL_ADCBUF0CHANNELVDDS,
    CC2652RB_LAUNCHXL_ADCBUF0CHANNELDCOUPL,
    CC2652RB_LAUNCHXL_ADCBUF0CHANNELVSS,

    CC2652RB_LAUNCHXL_ADCBUF0CHANNELCOUNT
} CC2652RB_LAUNCHXL_ADCBuf0ChannelName;

/*!
 *  @def    CC2652RB_LAUNCHXL_ADCName
 *  @brief  Enum of ADCs
 */
typedef enum CC2652RB_LAUNCHXL_ADCName {
    CC2652RB_LAUNCHXL_ADC0 = 0,
    CC2652RB_LAUNCHXL_ADC1,
    CC2652RB_LAUNCHXL_ADC2,
    CC2652RB_LAUNCHXL_ADC3,
    CC2652RB_LAUNCHXL_ADC4,
    CC2652RB_LAUNCHXL_ADC5,
    CC2652RB_LAUNCHXL_ADC6,
    CC2652RB_LAUNCHXL_ADC7,
    CC2652RB_LAUNCHXL_ADCDCOUPL,
    CC2652RB_LAUNCHXL_ADCVSS,
    CC2652RB_LAUNCHXL_ADCVDDS,

    CC2652RB_LAUNCHXL_ADCCOUNT
} CC2652RB_LAUNCHXL_ADCName;

/*!
 *  @def    CC2652RB_LAUNCHXL_ECDHName
 *  @brief  Enum of ECDH names
 */
typedef enum CC2652RB_LAUNCHXL_ECDHName {
    CC2652RB_LAUNCHXL_ECDH0 = 0,

    CC2652RB_LAUNCHXL_ECDHCOUNT
} CC2652RB_LAUNCHXL_ECDHName;

/*!
 *  @def    CC2652RB_LAUNCHXL_ECDSAName
 *  @brief  Enum of ECDSA names
 */
typedef enum CC2652RB_LAUNCHXL_ECDSAName {
    CC2652RB_LAUNCHXL_ECDSA0 = 0,

    CC2652RB_LAUNCHXL_ECDSACOUNT
} CC2652RB_LAUNCHXL_ECDSAName;

/*!
 *  @def    CC2652RB_LAUNCHXL_ECJPAKEName
 *  @brief  Enum of ECJPAKE names
 */
typedef enum CC2652RB_LAUNCHXL_ECJPAKEName {
    CC2652RB_LAUNCHXL_ECJPAKE0 = 0,

    CC2652RB_LAUNCHXL_ECJPAKECOUNT
} CC2652RB_LAUNCHXL_ECJPAKEName;

/*!
 *  @def    CC2652RB_LAUNCHXL_AESCCMName
 *  @brief  Enum of AESCCM names
 */
typedef enum CC2652RB_LAUNCHXL_AESCCMName {
    CC2652RB_LAUNCHXL_AESCCM0 = 0,

    CC2652RB_LAUNCHXL_AESCCMCOUNT
} CC2652RB_LAUNCHXL_AESCCMName;

/*!
 *  @def    CC2652RB_LAUNCHXL_AESGCMName
 *  @brief  Enum of AESGCM names
 */
typedef enum CC2652RB_LAUNCHXL_AESGCMName {
    CC2652RB_LAUNCHXL_AESGCM0 = 0,

    CC2652RB_LAUNCHXL_AESGCMCOUNT
} CC2652RB_LAUNCHXL_AESGCMName;

/*!
 *  @def    CC2652RB_LAUNCHXL_AESCBCName
 *  @brief  Enum of AESCBC names
 */
typedef enum CC2652RB_LAUNCHXL_AESCBCName {
    CC2652RB_LAUNCHXL_AESCBC0 = 0,

    CC2652RB_LAUNCHXL_AESCBCCOUNT
} CC2652RB_LAUNCHXL_AESCBCName;

/*!
 *  @def    CC2652RB_LAUNCHXL_AESCTRName
 *  @brief  Enum of AESCTR names
 */
typedef enum CC2652RB_LAUNCHXL_AESCTRName {
    CC2652RB_LAUNCHXL_AESCTR0 = 0,

    CC2652RB_LAUNCHXL_AESCTRCOUNT
} CC2652RB_LAUNCHXL_AESCTRName;

/*!
 *  @def    CC2652RB_LAUNCHXL_AESECBName
 *  @brief  Enum of AESECB names
 */
typedef enum CC2652RB_LAUNCHXL_AESECBName {
    CC2652RB_LAUNCHXL_AESECB0 = 0,

    CC2652RB_LAUNCHXL_AESECBCOUNT
} CC2652RB_LAUNCHXL_AESECBName;

/*!
 *  @def    CC2652RB_LAUNCHXL_AESCTRDRBGName
 *  @brief  Enum of AESCTRDRBG names
 */
typedef enum CC2652RB_LAUNCHXL_AESCTRDRBGName {
    CC2652RB_LAUNCHXL_AESCTRDRBG0 = 0,

    CC2652RB_LAUNCHXL_AESCTRDRBGCOUNT
} CC2652RB_LAUNCHXL_AESCTRDRBGName;

/*!
 *  @def    CC2652RB_LAUNCHXL_SHA2Name
 *  @brief  Enum of SHA2 names
 */
typedef enum CC2652RB_LAUNCHXL_SHA2Name {
    CC2652RB_LAUNCHXL_SHA20 = 0,

    CC2652RB_LAUNCHXL_SHA2COUNT
} CC2652RB_LAUNCHXL_SHA2Name;

/*!
 *  @def    CC2652RB_LAUNCHXL_TRNGName
 *  @brief  Enum of TRNG names
 */
typedef enum CC2652RB_LAUNCHXL_TRNGName {
    CC2652RB_LAUNCHXL_TRNG0 = 0,

    CC2652RB_LAUNCHXL_TRNGCOUNT
} CC2652RB_LAUNCHXL_TRNGName;

/*!
 *  @def    CC2652RB_LAUNCHXL_GPIOName
 *  @brief  Enum of GPIO names
 */
typedef enum CC2652RB_LAUNCHXL_GPIOName {
    CC2652RB_LAUNCHXL_GPIO_S1 = 0,
    CC2652RB_LAUNCHXL_GPIO_S2,
    CC2652RB_LAUNCHXL_SPI_MASTER_READY,
    CC2652RB_LAUNCHXL_SPI_SLAVE_READY,
    CC2652RB_LAUNCHXL_GPIO_LED_GREEN,
    CC2652RB_LAUNCHXL_GPIO_LED_RED,
    CC2652RB_LAUNCHXL_GPIO_TMP116_EN,
    CC2652RB_LAUNCHXL_GPIO_SPI_FLASH_CS,
    CC2652RB_LAUNCHXL_SDSPI_CS,
    CC2652RB_LAUNCHXL_GPIO_LCD_CS,
    CC2652RB_LAUNCHXL_GPIO_LCD_POWER,
    CC2652RB_LAUNCHXL_GPIO_LCD_ENABLE,
    CC2652RB_LAUNCHXL_GPIOCOUNT
} CC2652RB_LAUNCHXL_GPIOName;

/*!
 *  @def    CC2652RB_LAUNCHXL_GPTimerName
 *  @brief  Enum of GPTimer parts
 */
typedef enum CC2652RB_LAUNCHXL_GPTimerName {
    CC2652RB_LAUNCHXL_GPTIMER0A = 0,
    CC2652RB_LAUNCHXL_GPTIMER0B,
    CC2652RB_LAUNCHXL_GPTIMER1A,
    CC2652RB_LAUNCHXL_GPTIMER1B,
    CC2652RB_LAUNCHXL_GPTIMER2A,
    CC2652RB_LAUNCHXL_GPTIMER2B,
    CC2652RB_LAUNCHXL_GPTIMER3A,
    CC2652RB_LAUNCHXL_GPTIMER3B,

    CC2652RB_LAUNCHXL_GPTIMERPARTSCOUNT
} CC2652RB_LAUNCHXL_GPTimerName;

/*!
 *  @def    CC2652RB_LAUNCHXL_GPTimers
 *  @brief  Enum of GPTimers
 */
typedef enum CC2652RB_LAUNCHXL_GPTimers {
    CC2652RB_LAUNCHXL_GPTIMER0 = 0,
    CC2652RB_LAUNCHXL_GPTIMER1,
    CC2652RB_LAUNCHXL_GPTIMER2,
    CC2652RB_LAUNCHXL_GPTIMER3,

    CC2652RB_LAUNCHXL_GPTIMERCOUNT
} CC2652RB_LAUNCHXL_GPTimers;

/*!
 *  @def    CC2652RB_LAUNCHXL_I2CName
 *  @brief  Enum of I2C names
 */
typedef enum CC2652RB_LAUNCHXL_I2CName {
    CC2652RB_LAUNCHXL_I2C0 = 0,

    CC2652RB_LAUNCHXL_I2CCOUNT
} CC2652RB_LAUNCHXL_I2CName;

/*!
 *  @def    CC2652RB_LAUNCHXL_I2SName
 *  @brief  Enum of I2S names
 */
typedef enum CC2652RB_LAUNCHXL_I2SName {
    CC2652RB_LAUNCHXL_I2S0 = 0,

    CC2652RB_LAUNCHXL_I2SCOUNT
} CC2652RB_LAUNCHXL_I2SName;

/*!
 *  @def    CC2652RB_LAUNCHXL_NVSName
 *  @brief  Enum of NVS names
 */
typedef enum CC2652RB_LAUNCHXL_NVSName {
#if !defined(NO_OSAL_SNV)
#ifndef Board_EXCLUDE_NVS_INTERNAL_FLASH
    CC2652RB_LAUNCHXL_NVSCC26XX0 = 0,
#endif
#endif // NO_OSAL_SNV
#ifdef NVSSPI
#ifndef Board_EXCLUDE_NVS_EXTERNAL_FLASH
    CC2652RB_LAUNCHXL_NVSSPI25X0,
#endif
#endif // NVSSPI
    CC2652RB_LAUNCHXL_NVSCOUNT
} CC2652RB_LAUNCHXL_NVSName;

/*!
 *  @def    CC2652RB_LAUNCHXL_PWM
 *  @brief  Enum of PWM outputs
 */
typedef enum CC2652RB_LAUNCHXL_PWMName {
    CC2652RB_LAUNCHXL_PWM0 = 0,
    CC2652RB_LAUNCHXL_PWM1,
    CC2652RB_LAUNCHXL_PWM2,
    CC2652RB_LAUNCHXL_PWM3,
    CC2652RB_LAUNCHXL_PWM4,
    CC2652RB_LAUNCHXL_PWM5,
    CC2652RB_LAUNCHXL_PWM6,
    CC2652RB_LAUNCHXL_PWM7,

    CC2652RB_LAUNCHXL_PWMCOUNT
} CC2652RB_LAUNCHXL_PWMName;

/*!
 *  @def    CC2652RB_LAUNCHXL_SDName
 *  @brief  Enum of SD names
 */
typedef enum CC2652RB_LAUNCHXL_SDName {
    CC2652RB_LAUNCHXL_SDSPI0 = 0,

    CC2652RB_LAUNCHXL_SDCOUNT
} CC2652RB_LAUNCHXL_SDName;

/*!
 *  @def    CC2652RB_LAUNCHXL_SPIName
 *  @brief  Enum of SPI names
 */
typedef enum CC2652RB_LAUNCHXL_SPIName {
    CC2652RB_LAUNCHXL_SPI0 = 0,
    CC2652RB_LAUNCHXL_SPI1,

    CC2652RB_LAUNCHXL_SPICOUNT
} CC2652RB_LAUNCHXL_SPIName;

/*!
 *  @def    CC2652RB_LAUNCHXL_UARTName
 *  @brief  Enum of UARTs
 */
typedef enum CC2652RB_LAUNCHXL_UARTName {
    CC2652RB_LAUNCHXL_UART0 = 0,
    CC2652RB_LAUNCHXL_UART1,

    CC2652RB_LAUNCHXL_UARTCOUNT
} CC2652RB_LAUNCHXL_UARTName;

/*!
 *  @def    CC2652RB_LAUNCHXL_UDMAName
 *  @brief  Enum of DMA buffers
 */
typedef enum CC2652RB_LAUNCHXL_UDMAName {
    CC2652RB_LAUNCHXL_UDMA0 = 0,

    CC2652RB_LAUNCHXL_UDMACOUNT
} CC2652RB_LAUNCHXL_UDMAName;

/*!
 *  @def    CC2652RB_LAUNCHXL_WatchdogName
 *  @brief  Enum of Watchdogs
 */
typedef enum CC2652RB_LAUNCHXL_WatchdogName {
    CC2652RB_LAUNCHXL_WATCHDOG0 = 0,

    CC2652RB_LAUNCHXL_WATCHDOGCOUNT
} CC2652RB_LAUNCHXL_WatchdogName;

#ifdef __cplusplus
}
#endif

#endif /* __CC2652RB_LAUNCHXL_BOARD_H__ */
