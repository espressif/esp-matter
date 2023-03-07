/*
 * Copyright (c) 2015-2021, Texas Instruments Incorporated
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
/*! ============================================================================
 *  @file       GPIOCC32XX.h
 *
 *  @brief      GPIO driver implementation for CC32xx devices
 *
 *  The GPIO header file should be included in an application as follows:
 *  @code
 *  #include <ti/drivers/GPIO.h>
 *  #include <ti/drivers/gpio/GPIOCC32XX.h>
 *  @endcode
 *
 *  Refer to @ref GPIO.h for a complete description of the GPIO
 *  driver APIs provided and examples of their use.
 *
 *  There are some CC32XX-specific configuration values that can be used when
 *  calling GPIO_setConfig, listed below. All other macros should be used
 *  directly from GPIO.h.
 *
 *  \note GPIOCC32XX_GPIO_26 & GPIOCC32XX_GPIO_27 can only be used as output
 *  pins.
 *
 *  ============================================================================
 */

#ifndef ti_drivers_GPIOCC32XX__include
#define ti_drivers_GPIOCC32XX__include

#include <stdint.h>
#include <ti/drivers/GPIO.h>

#include <ti/devices/cc32xx/inc/hw_types.h>
#include <ti/devices/cc32xx/driverlib/gpio.h>
#include <ti/devices/cc32xx/driverlib/pin.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 *  @name CC32xx device specific GPIO_PinConfig macros
 *  @{
 */
/*! @hideinitializer Do not configure this pin. setConfig calls will return immediately */
#define GPIOCC32XX_DO_NOT_CONFIG    0x80000000

/*! @hideinitializer Use statically-defined parking state from Power driver, instead of current pin value */
#define GPIOCC32XX_CFG_USE_STATIC   0x4000
/** @} */

/* We don't define this value on purpose - any unsupported values will cause a
 * compile-time error. If your compiler tells you that this macro is missing,
 * you are trying to use an unsupported option.
 *
 * See below for which options are unsupported.
 */
#undef GPIOCC32XX_CFG_OPTION_NOT_SUPPORTED

/* To mux on CC32XX, use the PinTypeXXX functions from driverlib. */
#define GPIO_MUX_GPIO_INTERNAL              GPIOCC32XX_CFG_OPTION_NOT_SUPPORTED

/* The following options are not supported by CC32XX IO hardware */
#define GPIO_CFG_SLEW_NORMAL_INTERNAL       GPIOCC32XX_CFG_OPTION_NOT_SUPPORTED
#define GPIO_CFG_SLEW_REDUCED_INTERNAL      GPIOCC32XX_CFG_OPTION_NOT_SUPPORTED
#define GPIO_CFG_INVERT_OFF_INTERNAL        GPIOCC32XX_CFG_OPTION_NOT_SUPPORTED
#define GPIO_CFG_INVERT_ON_INTERNAL         GPIOCC32XX_CFG_OPTION_NOT_SUPPORTED

/* See GPIO.h for details about these configuration values */

/* Interrupt config takes bits 0-2.
 * 0x0 is used for FALLING, which makes this setting difficult to detect. The
 * highest value here is 6 and 7 is still only 3 bits, so we add 1 to shift the
 * value range and we can then use 0 for 'no configured interrupts' as usual.
 */
#define GPIO_CFG_INT_NONE_INTERNAL          0
#define GPIO_CFG_INT_LOW_INTERNAL           (GPIO_LOW_LEVEL + 1)
#define GPIO_CFG_INT_HIGH_INTERNAL          (GPIO_HIGH_LEVEL + 1)
#define GPIO_CFG_INT_FALLING_INTERNAL       (GPIO_FALLING_EDGE + 1)
#define GPIO_CFG_INT_RISING_INTERNAL        (GPIO_RISING_EDGE + 1)
#define GPIO_CFG_INT_BOTH_EDGES_INTERNAL    (GPIO_BOTH_EDGES + 1)

/* Int enabled stored in bit 3 */
#define GPIO_CFG_INT_ENABLE_INTERNAL        0x8
#define GPIO_CFG_INT_DISABLE_INTERNAL       0

/* General options, stored in bits 4-11 */
#define GPIO_CFG_INPUT_INTERNAL             (PIN_TYPE_STD | PIN_DIR_MODE_IN)
#define GPIO_CFG_OUTPUT_INTERNAL            (PIN_TYPE_STD | PIN_DIR_MODE_OUT)
#define GPIO_CFG_OUTPUT_OPEN_DRAIN_INTERNAL (PIN_TYPE_OD | PIN_DIR_MODE_OUT)

#define GPIO_CFG_PULL_NONE_INTERNAL         PIN_TYPE_STD
#define GPIO_CFG_PULL_UP_INTERNAL           PIN_TYPE_STD_PU
#define GPIO_CFG_PULL_DOWN_INTERNAL         PIN_TYPE_STD_PD

#define GPIO_CFG_DRVSTR_LOW_INTERNAL        PIN_STRENGTH_2MA
#define GPIO_CFG_DRVSTR_MED_INTERNAL        PIN_STRENGTH_4MA
#define GPIO_CFG_DRVSTR_HIGH_INTERNAL       PIN_STRENGTH_6MA

#define GPIO_CFG_NO_DIR_INTERNAL            PIN_TYPE_ANALOG

/* Hysteresis enable in bit 12 */
#define GPIO_CFG_HYSTERESIS_OFF_INTERNAL    0x0
#define GPIO_CFG_HYSTERESIS_ON_INTERNAL     0x1000

/* Default enable in bit 13 */
#define GPIO_CFG_OUTPUT_DEFAULT_LOW_INTERNAL     0x0
#define GPIO_CFG_OUTPUT_DEFAULT_HIGH_INTERNAL    0x2000

#ifdef __cplusplus
}
#endif

#endif /* ti_drivers_GPIOCC32XX__include */
