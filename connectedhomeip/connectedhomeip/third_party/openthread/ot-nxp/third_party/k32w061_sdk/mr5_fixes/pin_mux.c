/*
* Copyright (c) 2014 - 2015, Freescale Semiconductor, Inc.
* Copyright 2016-2019 NXP
* All rights reserved.
*
* SPDX-License-Identifier: BSD-3-Clause
 */

/* clang-format off */
/*
 * TEXT BELOW IS USED AS SETTING FOR TOOLS *************************************
!!GlobalInfo
product: Pins v6.0
processor: JN5189
mcu_data: ksdk2_0
processor_version: 0.0.0
 * BE CAREFUL MODIFYING THIS COMMENT - IT IS YAML SETTINGS FOR TOOLS ***********
 */
/* clang-format on */

#include "fsl_common.h"
#include "fsl_iocon.h"
#include "fsl_gpio.h"
#include "pin_mux.h"

/*****************************************************************************
 * Private types/enumerations/variables
 ****************************************************************************/

#ifndef BOARD_USECLKINSRC
#define BOARD_USECLKINSRC (0)
#endif

/*!
 * @brief
 * Select Analog/Digital Mode.
 * 0 Analog mode.
 * 1 Digital mode.
 * When in analog mode, the receiver path in the IO cell is disabled.
 * In this mode, it is essential that the digital function (e.
 * g.
 * GPIO) is not configured as an output.
 * Otherwise it may conflict with analog stuff (loopback of digital on analog input).
 * In other words, the digital output is not automatically disabled when the IO is in analog mode.
 * As a consequence, it is not possible to disable the receiver path when the IO is used for digital output
 * purpose.
 * : Digital mode, digital input is enabled.
 */
#define PIO0_15_DIGIMODE_DIGITAL 0x01u
/*!
 * @brief Select digital function assigned to this pin.: Alternative connection 1. */
#define PIO0_15_FUNC_ALT1 0x01u
/*!
 * @brief
 * Select Analog/Digital Mode.
 * 0 Analog mode.
 * 1 Digital mode.
 * When in analog mode, the receiver path in the IO cell is disabled.
 * In this mode, it is essential that the digital function (e.
 * g.
 * GPIO) is not configured as an output.
 * Otherwise it may conflict with analog stuff (loopback of digital on analog input).
 * In other words, the digital output is not automatically disabled when the IO is in analog mode.
 * As a consequence, it is not possible to disable the receiver path when the IO is used for digital output
 * purpose.
 * : Digital mode, digital input is enabled.
 */
#define PIO0_16_DIGIMODE_DIGITAL 0x01u
/*!
 * @brief Select digital function assigned to this pin.: Alternative connection 1. */
#define PIO0_16_FUNC_ALT1 0x01u
/*!
 * @brief
 * Select Analog/Digital Mode.
 * 0 Analog mode.
 * 1 Digital mode.
 * When in analog mode, the receiver path in the IO cell is disabled.
 * In this mode, it is essential that the digital function (e.
 * g.
 * GPIO) is not configured as an output.
 * Otherwise it may conflict with analog stuff (loopback of digital on analog input).
 * In other words, the digital output is not automatically disabled when the IO is in analog mode.
 * As a consequence, it is not possible to disable the receiver path when the IO is used for digital output
 * purpose.
 * : Digital mode, digital input is enabled.
 */
#define PIO0_17_DIGIMODE_DIGITAL 0x01u
/*!
 * @brief Select digital function assigned to this pin.: Alternative connection 1. */
#define PIO0_17_FUNC_ALT1 0x01u
/*!
 * @brief
 * Select Analog/Digital Mode.
 * 0 Analog mode.
 * 1 Digital mode.
 * When in analog mode, the receiver path in the IO cell is disabled.
 * In this mode, it is essential that the digital function (e.
 * g.
 * GPIO) is not configured as an output.
 * Otherwise it may conflict with analog stuff (loopback of digital on analog input).
 * In other words, the digital output is not automatically disabled when the IO is in analog mode.
 * As a consequence, it is not possible to disable the receiver path when the IO is used for digital output
 * purpose.
 * : Digital mode, digital input is enabled.
 */
#define PIO0_18_DIGIMODE_DIGITAL 0x01u
/*!
 * @brief Select digital function assigned to this pin.: Alternative connection 1. */
#define PIO0_18_FUNC_ALT1 0x01u

/*****************************************************************************
 * Public types/enumerations/variables
 ****************************************************************************/

/*****************************************************************************
 * Local Prototypes
 ****************************************************************************/
/*****************************************************************************
* Private functions
****************************************************************************/

/* clang-format off */
/*
 * TEXT BELOW IS USED AS SETTING FOR TOOLS *************************************
BOARD_InitAppUARTPins:
- options: {callFromInitBoot: 'false', coreID: cm4, enableClock: 'true'}
- pin_list:
  - {pin_num: '11', peripheral: USART0, signal: TXD, pin_signal: PIO0_8/SPI0_MOSI/USART0_TXD/CT32B0_MAT0/PWM8/ANA_COMP_OUT/RFTX/PDM1_DATA, mode: pullUp, invert: disabled,
    open_drain: disabled, filter_off: disabled, slew_rate_0: standard, slew_rate_1: standard, ssel: disabled}
  - {pin_num: '12', peripheral: USART0, signal: RXD, pin_signal: PIO0_9/SPI0_SSELN/USART0_RXD/CT32B1_CAP1/PWM9/USART1_SCK/ADO/PDM1_CLK, mode: pullUp, invert: disabled,
    open_drain: disabled, filter_off: disabled, slew_rate_0: standard, slew_rate_1: standard, ssel: disabled}
 * BE CAREFUL MODIFYING THIS COMMENT - IT IS YAML SETTINGS FOR TOOLS ***********
 */
/* clang-format on */

/* FUNCTION ************************************************************************************************************
 *
 * Function Name : BOARD_InitUART0Pins
 * Description   : Configures pin routing and optionally pin electrical features.
 *
 * END ****************************************************************************************************************/
/* Function assigned for the Cortex-M4 */
void BOARD_InitUART0Pins(void)
{
    /* Enables the clock for the I/O controller block. 0: Disable. 1: Enable.: 0x01u */
    CLOCK_EnableClock(kCLOCK_Iocon);

    const uint32_t port0_pin8_config = (/* Pin is configured as USART0_TXD */
                                        IOCON_PIO_FUNC2 |
                                        /* Selects pull-up function */
                                        IOCON_PIO_MODE_PULLUP |
                                        /* Standard mode, output slew rate control is disabled */
                                        IOCON_PIO_SLEW0_STANDARD |
                                        /* Input function is not inverted */
                                        IOCON_PIO_INV_DI |
                                        /* Enables digital function */
                                        IOCON_PIO_DIGITAL_EN |
                                        /* Input filter disabled */
                                        IOCON_PIO_INPFILT_OFF |
                                        /* Standard mode, output slew rate control is disabled */
                                        IOCON_PIO_SLEW1_STANDARD |
                                        /* Open drain is disabled */
                                        IOCON_PIO_OPENDRAIN_DI |
                                        /* SSEL is disabled */
                                        IOCON_PIO_SSEL_DI);
    /* PORT0 PIN8 (coords: 11) is configured as USART0_TXD */
    IOCON_PinMuxSet(IOCON, 0U, 8U, port0_pin8_config);

    const uint32_t port0_pin9_config = (/* Pin is configured as USART0_RXD */
                                        IOCON_PIO_FUNC2 |
                                        /* Selects pull-up function */
                                        IOCON_PIO_MODE_PULLUP |
                                        /* Standard mode, output slew rate control is disabled */
                                        IOCON_PIO_SLEW0_STANDARD |
                                        /* Input function is not inverted */
                                        IOCON_PIO_INV_DI |
                                        /* Enables digital function */
                                        IOCON_PIO_DIGITAL_EN |
                                        /* Input filter disabled */
                                        IOCON_PIO_INPFILT_OFF |
                                        /* Standard mode, output slew rate control is disabled */
                                        IOCON_PIO_SLEW1_STANDARD |
                                        /* Open drain is disabled */
                                        IOCON_PIO_OPENDRAIN_DI |
                                        /* SSEL is disabled */
                                        IOCON_PIO_SSEL_DI);
    /* PORT0 PIN9 (coords: 12) is configured as USART0_RXD */
    IOCON_PinMuxSet(IOCON, 0U, 9U, port0_pin9_config);
}

/* clang-format off */
/*
 * TEXT BELOW IS USED AS SETTING FOR TOOLS *************************************
BOARD_InitDebugUARTPins:
- options: {callFromInitBoot: 'false', coreID: cm4, enableClock: 'true'}
- pin_list:
  - {pin_num: '13', peripheral: USART1, signal: TXD, pin_signal: PIO0_10/CT32B0_CAP0/USART1_TXD/RFTX/I2C0_SCL/SPI0_SCK/PDM0_DATA, invert: disabled, open_drain: disabled,
    filter_off: disabled, egp: gpio, ecs: disabled, ehs: low, fsel: disabled, io_clamp: disabled}
  - {pin_num: '14', peripheral: USART1, signal: RXD, pin_signal: PIO0_11/CT32B1_CAP0/USART1_RXD/RFRX/I2C0_SDA/SPI0_MISO/PDM0_CLK, invert: disabled, open_drain: disabled,
    filter_off: disabled, egp: gpio, ecs: disabled, ehs: low, fsel: disabled, io_clamp: disabled}
 * BE CAREFUL MODIFYING THIS COMMENT - IT IS YAML SETTINGS FOR TOOLS ***********
 */
/* clang-format on */

/* FUNCTION ************************************************************************************************************
 *
 * Function Name : BOARD_InitUART1Pins
 * Description   : Configures pin routing and optionally pin electrical features.
 *
 * END ****************************************************************************************************************/
/* Function assigned for the Cortex-M4 */
void BOARD_InitUART1Pins(void)
{
    /* Enables the clock for the I/O controller block. 0: Disable. 1: Enable.: 0x01u */
    CLOCK_EnableClock(kCLOCK_Iocon);

    const uint32_t port0_pin0_config = (/* Pin is configured as USART1_TXD */
                                         IOCON_PIO_FUNC2 |
                                         /* GPIO mode */
                                         IOCON_PIO_EGP_GPIO |
                                         /* IO is an open drain cell */
                                         IOCON_PIO_ECS_DI |
                                         /* High speed IO for GPIO mode, IIC not */
                                         IOCON_PIO_EHS_DI |
                                         /* Input function is not inverted */
                                         IOCON_PIO_INV_DI |
                                         /* Enables digital function */
                                         IOCON_PIO_DIGITAL_EN |
                                         /* Input filter disabled */
                                         IOCON_PIO_INPFILT_OFF |
                                         /* IIC mode:Noise pulses below approximately 50ns are filtered out. GPIO mode:a 3ns filter */
                                         IOCON_PIO_FSEL_DI |
                                         /* Open drain is disabled */
                                         IOCON_PIO_OPENDRAIN_DI |
                                         /* IO_CLAMP disabled */
                                         IOCON_PIO_IO_CLAMP_DI);
    /* PORT0 PIN0 (coords: 13) is configured as USART1_TXD */
    IOCON_PinMuxSet(IOCON, 0U, 0U, port0_pin0_config);

    const uint32_t port0_pin1_config = (/* Pin is configured as USART1_RXD */
                                         IOCON_PIO_FUNC2 |
                                         /* GPIO mode */
                                         IOCON_PIO_EGP_GPIO |
                                         /* IO is an open drain cell */
                                         IOCON_PIO_ECS_DI |
                                         /* High speed IO for GPIO mode, IIC not */
                                         IOCON_PIO_EHS_DI |
                                         /* Input function is not inverted */
                                         IOCON_PIO_INV_DI |
                                         /* Enables digital function */
                                         IOCON_PIO_DIGITAL_EN |
                                         /* Input filter disabled */
                                         IOCON_PIO_INPFILT_OFF |
                                         /* IIC mode:Noise pulses below approximately 50ns are filtered out. GPIO mode:a 3ns filter */
                                         IOCON_PIO_FSEL_DI |
                                         /* Open drain is disabled */
                                         IOCON_PIO_OPENDRAIN_DI |
                                         /* IO_CLAMP disabled */
                                         IOCON_PIO_IO_CLAMP_DI);
    /* PORT0 PIN1 (coords: 14) is configured as USART1_RXD */
    IOCON_PinMuxSet(IOCON, 0U, 1U, port0_pin1_config);
}

static void ConfigureDebugPort(void)
{
    /* SWD SWCLK/SWDIO pins */
    IOCON_PinMuxSet(IOCON, 0, 12, IOCON_FUNC2 | IOCON_MODE_INACT | IOCON_DIGITAL_EN);
    IOCON_PinMuxSet(IOCON, 0, 13, IOCON_FUNC2 | IOCON_MODE_INACT | IOCON_DIGITAL_EN);
#ifdef ENABLE_DEBUG_PORT_SWO
    /* SWD SWO pin (optional) */
    IOCON_PinMuxSet(IOCON, 0, 14, IOCON_FUNC5 | IOCON_MODE_INACT | IOCON_DIGITAL_EN);
    SYSCON->TRACECLKDIV = 0; /* Clear HALT bit */
#endif
}

static void ConfigureDongleLEDs(void)
{
    const uint32_t port0_pin4_config = (/* Pin is configured as PIO0_4 */
                                        IOCON_PIO_FUNC0 |
                                        /* Selects pull-up function */
                                        IOCON_PIO_MODE_PULLUP |
                                        /* Standard mode, output slew rate control is disabled */
                                        IOCON_PIO_SLEW0_STANDARD |
                                        /* Input function is not inverted */
                                        IOCON_PIO_INV_DI |
                                        /* Enables digital function */
                                        IOCON_PIO_DIGITAL_EN |
                                        /* Input filter disabled */
                                        IOCON_PIO_INPFILT_OFF |
                                        /* Standard mode, output slew rate control is disabled */
                                        IOCON_PIO_SLEW1_STANDARD |
                                        /* Open drain is disabled */
                                        IOCON_PIO_OPENDRAIN_DI |
                                        /* SSEL is disabled */
                                        IOCON_PIO_SSEL_DI);
    /* PORT0 PIN4 (coords: 7) is configured as PIO0_4 */
    IOCON_PinMuxSet(IOCON, 0U, 4U, port0_pin4_config);

    const uint32_t port0_pin10_config = (/* Pin is configured as PIO0_10 */
                                         IOCON_PIO_FUNC0 |
                                         /* GPIO mode */
                                         IOCON_PIO_EGP_GPIO |
                                         /* IO is an open drain cell */
                                         IOCON_PIO_ECS_DI |
                                         /* High speed IO for GPIO mode, IIC not */
                                         IOCON_PIO_EHS_DI |
                                         /* Input function is not inverted */
                                         IOCON_PIO_INV_DI |
                                         /* Enables digital function */
                                         IOCON_PIO_DIGITAL_EN |
                                         /* Input filter disabled */
                                         IOCON_PIO_INPFILT_OFF |
                                         /* IIC mode:Noise pulses below approximately 50ns are filtered out. GPIO mode:a 3ns filter */
                                         IOCON_PIO_FSEL_DI |
                                         /* Open drain is disabled */
                                         IOCON_PIO_OPENDRAIN_DI |
                                         /* IO_CLAMP disabled */
                                         IOCON_PIO_IO_CLAMP_DI);
    /* PORT0 PIN10 (coords: 13) is configured as PIO0_10 */
    IOCON_PinMuxSet(IOCON, 0U, 10U, port0_pin10_config);
}

/*******************************************************************************
 * Code
 ******************************************************************************/
void BOARD_InitPins(void)
{
    /* Define the init structure for the output LED pin*/
    gpio_pin_config_t led_config = {
        kGPIO_DigitalOutput,
        0,
    };

    /* Enable IOCON clock */
    CLOCK_EnableClock(kCLOCK_Iocon);
    CLOCK_EnableClock(kCLOCK_InputMux);

    /* Debugger signals */
    ConfigureDebugPort();

#if USE_USB_DONGLE
    ConfigureDongleLEDs();
#endif
}

void BOARD_InitSPI1Pins(void)
{
    IOCON->PIO[0][15] = ((IOCON->PIO[0][15] &
                          /* Mask bits to zero which are setting */
                          (~(IOCON_PIO_FUNC_MASK | IOCON_PIO_DIGIMODE_MASK)))



                         /* Select digital function assigned to this pin.
                          * : PORT015 (pin 18) is configured as SPI1_SCK. */
                         | IOCON_PIO_FUNC(PIO0_15_FUNC_ALT1)



                         /* Select Analog/Digital Mode.
                          * 0 Analog mode.
                          * 1 Digital mode.
                          * When in analog mode, the receiver path in the IO cell is disabled.
                          * In this mode, it is essential that the digital function (e.
                          * g.
                          * GPIO) is not configured as an output.
                          * Otherwise it may conflict with analog stuff (loopback of digital on analog input).
                          * In other words, the digital output is not automatically disabled when the IO is in
                          * analog mode.
                          * As a consequence, it is not possible to disable the receiver path when the IO is used
                          * for digital output purpose.
                          * : Digital mode, digital input is enabled. */
                         | IOCON_PIO_DIGIMODE(PIO0_15_DIGIMODE_DIGITAL));



    IOCON->PIO[0][16] = ((IOCON->PIO[0][16] &
                          /* Mask bits to zero which are setting */
                          (~(IOCON_PIO_FUNC_MASK | IOCON_PIO_DIGIMODE_MASK)))



                         /* Select digital function assigned to this pin.
                          * : PORT016 (pin 19) is configured as SPI1_SSELN0. */
                         | IOCON_PIO_FUNC(PIO0_16_FUNC_ALT1)



                         /* Select Analog/Digital Mode.
                          * 0 Analog mode.
                          * 1 Digital mode.
                          * When in analog mode, the receiver path in the IO cell is disabled.
                          * In this mode, it is essential that the digital function (e.
                          * g.
                          * GPIO) is not configured as an output.
                          * Otherwise it may conflict with analog stuff (loopback of digital on analog input).
                          * In other words, the digital output is not automatically disabled when the IO is in
                          * analog mode.
                          * As a consequence, it is not possible to disable the receiver path when the IO is used
                          * for digital output purpose.
                          * : Digital mode, digital input is enabled. */
                         | IOCON_PIO_DIGIMODE(PIO0_16_DIGIMODE_DIGITAL));



    IOCON->PIO[0][17] = ((IOCON->PIO[0][17] &
                          /* Mask bits to zero which are setting */
                          (~(IOCON_PIO_FUNC_MASK | IOCON_PIO_DIGIMODE_MASK)))



                         /* Select digital function assigned to this pin.
                          * : PORT017 (pin 21) is configured as SPI1_MOSI. */
                         | IOCON_PIO_FUNC(PIO0_17_FUNC_ALT1)



                         /* Select Analog/Digital Mode.
                          * 0 Analog mode.
                          * 1 Digital mode.
                          * When in analog mode, the receiver path in the IO cell is disabled.
                          * In this mode, it is essential that the digital function (e.
                          * g.
                          * GPIO) is not configured as an output.
                          * Otherwise it may conflict with analog stuff (loopback of digital on analog input).
                          * In other words, the digital output is not automatically disabled when the IO is in
                          * analog mode.
                          * As a consequence, it is not possible to disable the receiver path when the IO is used
                          * for digital output purpose.
                          * : Digital mode, digital input is enabled. */
                         | IOCON_PIO_DIGIMODE(PIO0_17_DIGIMODE_DIGITAL));



    IOCON->PIO[0][18] = ((IOCON->PIO[0][18] &
                          /* Mask bits to zero which are setting */
                          (~(IOCON_PIO_FUNC_MASK | IOCON_PIO_DIGIMODE_MASK)))



                         /* Select digital function assigned to this pin.
                          * : PORT018 (pin 22) is configured as SPI1_MISO. */
                         | IOCON_PIO_FUNC(PIO0_18_FUNC_ALT1)



                         /* Select Analog/Digital Mode.
                          * 0 Analog mode.
                          * 1 Digital mode.
                          * When in analog mode, the receiver path in the IO cell is disabled.
                          * In this mode, it is essential that the digital function (e.
                          * g.
                          * GPIO) is not configured as an output.
                          * Otherwise it may conflict with analog stuff (loopback of digital on analog input).
                          * In other words, the digital output is not automatically disabled when the IO is in
                          * analog mode.
                          * As a consequence, it is not possible to disable the receiver path when the IO is used
                          * for digital output purpose.
                          * : Digital mode, digital input is enabled. */
                         | IOCON_PIO_DIGIMODE(PIO0_18_DIGIMODE_DIGITAL));
    /* SPI Interrupt pin */
    gpio_pin_config_t gpioSpi_int_config = {
      .pinDirection = kGPIO_DigitalOutput,
      .outputLogic = 0U,
    };
    IOCON_PinMuxSet(IOCON, 0U, 19, IOCON_PIO_FUNC(0) | IOCON_MODE_PULLUP | IOCON_DIGITAL_EN);

    GPIO_PinInit(GPIO, 0, 19, &gpioSpi_int_config);
}

void BOARD_LedDongleToggle(void)
{
#if USE_USB_DONGLE
    GPIO_PortToggle(BOARD_LED_USB_DONGLE_GPIO, BOARD_LED_USB_DONGLE_GPIO_PORT, 1u << BOARD_LED_USB_DONGLE1_GPIO_PIN);
    GPIO_PortToggle(BOARD_LED_USB_DONGLE_GPIO, BOARD_LED_USB_DONGLE_GPIO_PORT, 1u << BOARD_LED_USB_DONGLE2_GPIO_PIN);
#endif
}
