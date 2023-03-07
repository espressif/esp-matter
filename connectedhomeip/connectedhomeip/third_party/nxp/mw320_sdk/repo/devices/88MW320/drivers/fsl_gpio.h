/*
 * Copyright 2020 NXP
 * All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef _FSL_GPIO_H_
#define _FSL_GPIO_H_

#include "fsl_common.h"

/*!
 * @addtogroup gpio
 * @{
 */

/*******************************************************************************
 * Definitions
 ******************************************************************************/

/*! @name Driver version */
/*@{*/
/*! @brief GPIO driver version 2.0.0. */
#define FSL_GPIO_DRIVER_VERSION (MAKE_VERSION(2, 0, 0))
/*@}*/

/*! @brief Get GPIO port from pin number */
#define GPIO_PORT(pin) (((uint32_t)(pin)) >> 5U)
/*! @brief Get GPIO pin inside the port from pin number */
#define GPIO_PORT_PIN(pin) (((uint32_t)(pin)) & 0x1FU)

/*! @brief GPIO direction definition */
typedef enum _gpio_pin_direction
{
    kGPIO_DigitalInput  = 0U, /*!< Set current pin as digital input*/
    kGPIO_DigitalOutput = 1U, /*!< Set current pin as digital output*/
} gpio_pin_direction_t;

/*!
 * @brief The GPIO pin configuration structure.
 *
 * Each pin can only be configured as either an output pin or an input pin at a time.
 * If configured as an input pin, leave the outputConfig unused.
 */
typedef struct _gpio_pin_config
{
    gpio_pin_direction_t pinDirection; /*!< GPIO direction, input or output */
    /* Output configurations; ignore if configured as an input pin */
    uint8_t outputLogic; /*!< Set a default output logic, which has no use in input */
} gpio_pin_config_t;

/*! @brief Configures the interrupt generation condition. */
typedef enum _gpio_interrupt_config
{
    kGPIO_InterruptStatusFlagDisabled = 0x00U, /*!< Interrupt status flag is disabled. */
    kGPIO_InterruptRisingEdge         = 0x01U, /*!< Interrupt on rising edge. */
    kGPIO_InterruptFallingEdge        = 0x02U, /*!< Interrupt on falling edge. */
    kGPIO_InterruptEitherEdge         = 0x03U, /*!< Interrupt on either edge. */
} gpio_interrupt_config_t;

/*! @} */

/*******************************************************************************
 * API
 ******************************************************************************/

#if defined(__cplusplus)
extern "C" {
#endif

/*!
 * @addtogroup gpio_driver
 * @{
 */

/*! @name GPIO Configuration */
/*@{*/

/*!
 * @brief Initializes a GPIO pin used by the board.
 *
 * To initialize the GPIO, define a pin configuration, as either input or output, in the user file.
 * Then, call the GPIO_PinInit() function.
 *
 * This is an example to define an input pin or an output pin configuration.
 * @code
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
 * @endcode
 *
 * @param base   GPIO peripheral base pointer
 * @param pin    GPIO pin number
 * @param config GPIO pin configuration pointer
 */
void GPIO_PinInit(GPIO_Type *base, uint32_t pin, const gpio_pin_config_t *config);

/*@}*/

/*! @name GPIO Output Operations */
/*@{*/

/*!
 * @brief Sets the output level of single GPIO pin to the logic 1 or 0.
 *
 * @param base    GPIO peripheral base pointer
 * @param pin     GPIO pin number
 * @param output  GPIO pin output logic level.
 *        - 0: corresponding pin output low-logic level.
 *        - 1: corresponding pin output high-logic level.
 */
void GPIO_PinWrite(GPIO_Type *base, uint32_t pin, uint8_t output);

/*!
 * @brief Sets the output level of the multiple GPIO pins to the logic 1.
 *
 * @param base GPIO peripheral base pointer (GPIOA, GPIOB, GPIOC, and so on.)
 * @param port GPIO port number. Each port contains 32 GPIOs.
 * @param mask GPIO pin bit mask. Bit position stands for the pin number in the port.
 */
__STATIC_INLINE void GPIO_PortSet(GPIO_Type *base, uint32_t port, uint32_t mask)
{
    assert(port <= GPIO_PORT((uint32_t)FSL_FEATURE_MW_GPIO_IO_NUMBER - 1U));

    base->GPSR_REG[port] = mask;
}

/*!
 * @brief Sets the output level of the multiple GPIO pins to the logic 0.
 *
 * @param base GPIO peripheral base pointer
 * @param port GPIO port number. Each port contains 32 GPIOs.
 * @param mask GPIO pin bit mask. Bit position stands for the pin number in the port.
 */
__STATIC_INLINE void GPIO_PortClear(GPIO_Type *base, uint32_t port, uint32_t mask)
{
    assert(port <= GPIO_PORT((uint32_t)FSL_FEATURE_MW_GPIO_IO_NUMBER - 1U));

    base->GPCR_REG[port] = mask;
}

/*!
 * @brief Reverses the current output logic of the multiple GPIO pins.
 *
 * @param base GPIO peripheral base pointer
 * @param port GPIO port number. Each port contains 32 GPIOs.
 * @param mask GPIO pin bit mask. Bit position stands for the pin number in the port.
 */
void GPIO_PortToggle(GPIO_Type *base, uint32_t port, uint32_t mask);

/*@}*/

/*! @name GPIO Input Operations */
/*@{*/

/*!
 * @brief Reads the current input value of the GPIO port.
 *
 * @param base GPIO peripheral base pointer
 * @param pin  GPIO pin number
 * @retval GPIO port input value
 *        - 0: corresponding pin input low-logic level.
 *        - 1: corresponding pin input high-logic level.
 */
__STATIC_INLINE uint32_t GPIO_PinRead(GPIO_Type *base, uint32_t pin)
{
    assert(pin < (uint32_t)FSL_FEATURE_MW_GPIO_IO_NUMBER);

    return (base->GPLR_REG[GPIO_PORT(pin)] >> GPIO_PORT_PIN(pin)) & 0x01U;
}

/*@}*/

/*! @name GPIO Interrupt */
/*@{*/

/*!
 * @brief Configures the gpio pin interrupt.
 *
 * @param base    GPIO peripheral base pointer.
 * @param pin     GPIO pin number.
 * @param config  GPIO pin interrupt configuration.
 */
void GPIO_PinSetInterruptConfig(GPIO_Type *base, uint32_t pin, gpio_interrupt_config_t config);

/*!
 * @brief Enables the specific pin interrupt.
 *
 * @param base GPIO base pointer.
 * @param port GPIO port number. Each port contains 32 GPIOs.
 * @param mask GPIO pin bit mask. Bit position stands for the pin number in the port.
 */
__STATIC_INLINE void GPIO_PortEnableInterrupts(GPIO_Type *base, uint32_t port, uint32_t mask)
{
    assert(port <= GPIO_PORT((uint32_t)FSL_FEATURE_MW_GPIO_IO_NUMBER - 1U));

    base->APMASK_REG[port] |= mask;
}

/*!
 * @brief Disables the specific pin interrupt.
 *
 * @param base GPIO base pointer.
 * @param port GPIO port number. Each port contains 32 GPIOs.
 * @param mask GPIO pin bit mask. Bit position stands for the pin number in the port.
 */
__STATIC_INLINE void GPIO_PortDisableInterrupts(GPIO_Type *base, uint32_t port, uint32_t mask)
{
    assert(port <= GPIO_PORT((uint32_t)FSL_FEATURE_MW_GPIO_IO_NUMBER - 1U));

    base->APMASK_REG[port] &= ~mask;
}

/*!
 * @brief Reads the GPIO port interrupt status flag.
 *
 * The flag remains set until a logic one is written to that flag.
 *
 * @param base GPIO peripheral base pointer
 * @param port GPIO port number. Each port contains 32 GPIOs.
 * @retval The current GPIO port interrupt status flag, for example, 0x00010001 means the
 *         pin 0 and 16 have the interrupt.
 */
__STATIC_INLINE uint32_t GPIO_PortGetInterruptFlags(GPIO_Type *base, uint32_t port)
{
    assert(port <= GPIO_PORT((uint32_t)FSL_FEATURE_MW_GPIO_IO_NUMBER - 1U));

    return base->GEDR_REG[port];
}

/*!
 * @brief Clears multiple GPIO pin interrupt status flags.
 *
 * @param base GPIO peripheral base pointer
 * @param port GPIO port number. Each port contains 32 GPIOs.
 * @param mask GPIO pin bit mask. Bit position stands for the pin number in the port.
 */
__STATIC_INLINE void GPIO_PortClearInterruptFlags(GPIO_Type *base, uint32_t port, uint32_t mask)
{
    assert(port <= GPIO_PORT((uint32_t)FSL_FEATURE_MW_GPIO_IO_NUMBER - 1U));

    base->GEDR_REG[port] = mask;
}

/*@}*/
/*! @} */

#if defined(__cplusplus)
}
#endif

/*!
 * @}
 */

#endif /* _FSL_GPIO_H_*/
