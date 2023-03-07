/*
 * Copyright 2020 NXP
 * All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include "fsl_pinmux.h"

/*******************************************************************************
 * Definitions
 ******************************************************************************/
/* Component ID definition, used by tools. */
#ifndef FSL_COMPONENT_ID
#define FSL_COMPONENT_ID "platform.drivers.pinmux"
#endif

/*******************************************************************************
 * Variables
 ******************************************************************************/

/*******************************************************************************
 * Prototypes
 ******************************************************************************/

/*******************************************************************************
 * Code
 ******************************************************************************/
/**
 * @brief   Sets I/O Pad Control pin mux
 * @param   pin         : GPIO pin to mux
 * @param   modefunc    : OR'ed values of type PINMUX_*
 */
void PINMUX_PinMuxSet(uint32_t pin, uint32_t modefunc)
{
    volatile uint32_t *reg = &PIN_MUX->_GPIO0 + pin;

    assert(pin < 50U);

    /* For GPIO6~10, GPIO22~26 and GPIO28~33, function 1 is GPIO function, for other GPIOs, function 0 is GPIO function
     */
    if (((pin >= 6U) && (pin <= 10U)) || ((pin >= 22U) && (pin <= 26U)) || ((pin >= 28U) && (pin <= 33U)))
    {
        if ((modefunc & PINMUX_FUNC_MASK) != PINMUX_FUNC1)
        {
            /* Need to set default pull up for non-GPIO before function switch. */
            *reg &= ~PINMUX__GPIO0_PIO_PULL_SEL_MASK;
        }
    }
    else
    {
        if ((modefunc & PINMUX_FUNC_MASK) != PINMUX_FUNC0)
        {
            /* Need to set default pull up for non-GPIO before function switch. */
            *reg &= ~PINMUX__GPIO0_PIO_PULL_SEL_MASK;
        }
    }

    *reg = modefunc;
}
