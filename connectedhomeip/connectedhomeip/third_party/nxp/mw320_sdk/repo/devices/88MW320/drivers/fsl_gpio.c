/*
 * Copyright 2020 NXP
 * All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include "fsl_gpio.h"

/* Component ID definition, used by tools. */
#ifndef FSL_COMPONENT_ID
#define FSL_COMPONENT_ID "platform.drivers.gpio"
#endif

/*******************************************************************************
 * Variables
 ******************************************************************************/

/*******************************************************************************
 * Prototypes
 ******************************************************************************/
/*!
 * brief Initializes a GPIO pin used by the board.
 *
 * To initialize the GPIO, define a pin configuration, as either input or output, in the user file.
 * Then, call the GPIO_PinInit() function.
 *
 * This is an example to define an input pin or an output pin configuration.
 * code
 * Define a digital input pin configuration,
 * gpio_pin_config_t config =
 * {
 *   kGPIO_DigitalInput,
 *   0,
 * }
 * Define a digital output pin configuration,
 * gpio_pin_config_t config =
 * {
 *   kGPIO_DigitalOutput,
 *   0,
 * }
 * endcode
 *
 * param base   GPIO peripheral base pointer
 * param pin    GPIO port pin number
 * param config GPIO pin configuration pointer
 */
void GPIO_PinInit(GPIO_Type *base, uint32_t pin, const gpio_pin_config_t *config)
{
    assert(NULL != config);
    assert(pin < (uint32_t)FSL_FEATURE_MW_GPIO_IO_NUMBER);

    if (config->pinDirection == kGPIO_DigitalInput)
    {
        base->GCDR_REG[GPIO_PORT(pin)] = 1UL << GPIO_PORT_PIN(pin);
    }
    else
    {
        GPIO_PinWrite(base, pin, config->outputLogic);
        base->GSDR_REG[GPIO_PORT(pin)] = 1UL << GPIO_PORT_PIN(pin);
    }
}

/*!
 * @brief Sets the output level of single GPIO pin to the logic 1 or 0.
 *
 * @param base    GPIO peripheral base pointer
 * @param pin     GPIO pin number
 * @param output  GPIO pin output logic level.
 *        - 0: corresponding pin output low-logic level.
 *        - 1: corresponding pin output high-logic level.
 */
void GPIO_PinWrite(GPIO_Type *base, uint32_t pin, uint8_t output)
{
    assert(pin < (uint32_t)FSL_FEATURE_MW_GPIO_IO_NUMBER);

    if (output != 0U)
    {
        base->GPSR_REG[GPIO_PORT(pin)] = 1UL << GPIO_PORT_PIN(pin);
    }
    else
    {
        base->GPCR_REG[GPIO_PORT(pin)] = 1UL << GPIO_PORT_PIN(pin);
    }
}

/*!
 * @brief Reverses the current output logic of the multiple GPIO pins.
 *
 * @param base GPIO peripheral base pointer
 * @param port GPIO port number. Each port contains 32 GPIOs.
 * @param mask GPIO pin bit mask. Bit position stands for the pin number in the port.
 */
void GPIO_PortToggle(GPIO_Type *base, uint32_t port, uint32_t mask)
{
    uint32_t level;

    assert(port <= GPIO_PORT((uint32_t)FSL_FEATURE_MW_GPIO_IO_NUMBER - 1U));

    level = base->GPLR_REG[port];

    GPIO_PortClear(base, port, level & mask);
    GPIO_PortSet(base, port, (~level) & mask);
}

/*!
 * @brief Configures the gpio pin interrupt.
 *
 * @param base    GPIO peripheral base pointer.
 * @param pin     GPIO pin number.
 * @param config  GPIO pin interrupt configuration.
 */
void GPIO_PinSetInterruptConfig(GPIO_Type *base, uint32_t pin, gpio_interrupt_config_t config)
{
    uint32_t regVal;

    assert(pin < (uint32_t)FSL_FEATURE_MW_GPIO_IO_NUMBER);

    regVal = 1UL << GPIO_PORT_PIN(pin);

    switch (config)
    {
        case kGPIO_InterruptStatusFlagDisabled:
            base->GCRER_REG[GPIO_PORT(pin)] = regVal;
            base->GCFER_REG[GPIO_PORT(pin)] = regVal;
            break;
        case kGPIO_InterruptRisingEdge:
            base->GSRER_REG[GPIO_PORT(pin)] = regVal;
            base->GCFER_REG[GPIO_PORT(pin)] = regVal;
            break;
        case kGPIO_InterruptFallingEdge:
            base->GCRER_REG[GPIO_PORT(pin)] = regVal;
            base->GSFER_REG[GPIO_PORT(pin)] = regVal;
            break;
        case kGPIO_InterruptEitherEdge:
            base->GSRER_REG[GPIO_PORT(pin)] = regVal;
            base->GSFER_REG[GPIO_PORT(pin)] = regVal;
            break;
        default:
            assert(false);
            break;
    }
}
