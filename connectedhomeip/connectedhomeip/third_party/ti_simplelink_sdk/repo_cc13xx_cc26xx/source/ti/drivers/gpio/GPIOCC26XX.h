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
 *  @file       GPIOCC26XX.h
 *
 *  @brief      GPIO driver implementation for CC26xx devices
 *
 *  The GPIO header file should be included in an application as follows:
 *  @code
 *  #include <ti/drivers/GPIO.h>
 *  #include <ti/drivers/gpio/GPIOCC26XX.h>
 *  @endcode
 *
 *  Refer to @ref GPIO.h for a complete description of the GPIO
 *  driver APIs provided and examples of their use.
 *
 *  The definitions in this file should not be used directly. All GPIO_CFG
 *  macros should be used as-is from GPIO.h.
 *
 *  There are no additional configuration values or platform-specific
 *  functions for GPIOCC26XX.
 */

#ifndef ti_drivers_GPIOCC26XX__include
#define ti_drivers_GPIOCC26XX__include

#include <ti/drivers/GPIO.h>

#include <ti/devices/DeviceFamily.h>
#include DeviceFamily_constructPath(inc/hw_ioc.h)

#ifdef __cplusplus
extern "C" {
#endif

/* Alternative mux values are defined in hw_ioc.h */
#define GPIO_MUX_GPIO_INTERNAL              IOC_IOCFG0_PORT_ID_GPIO

/* We don't define this value on purpose - any unsupported values will cause a
 * compile-time error. If your compiler tells you that this macro is missing,
 * you are trying to use an unsupported option.
 *
 * See below for which options are unsupported.
 */
#undef GPIOCC26XX_CFG_OPTION_NOT_SUPPORTED

/* Low and high value interrupts are not available on CC26XX hardware */
#define GPIO_CFG_INT_LOW_INTERNAL           GPIOCC26XX_CFG_OPTION_NOT_SUPPORTED
#define GPIO_CFG_INT_HIGH_INTERNAL          GPIOCC26XX_CFG_OPTION_NOT_SUPPORTED

/* Support for DO_NOT_CONFIG would break a major initialisation optimisation */
#define GPIO_CFG_DO_NOT_CONFIG_INTERNAL     GPIOCC26XX_CFG_OPTION_NOT_SUPPORTED

/* See GPIO.h for details about these configuration values */

/* General options remapped directly to IOC defines */
#define GPIO_CFG_NO_DIR_INTERNAL            (IOC_IOCFG0_IOMODE_NORMAL | GPIOCC26XX_CFG_PIN_IS_INPUT_INTERNAL)
#define GPIO_CFG_INPUT_INTERNAL             (IOC_IOCFG0_IOMODE_NORMAL | IOC_IOCFG0_IE | GPIOCC26XX_CFG_PIN_IS_INPUT_INTERNAL)
#define GPIO_CFG_OUTPUT_INTERNAL            (IOC_IOCFG0_IOMODE_NORMAL | IOC_IOCFG0_IE | GPIOCC26XX_CFG_PIN_IS_OUTPUT_INTERNAL)
#define GPIO_CFG_OUTPUT_OPEN_DRAIN_INTERNAL (IOC_IOCFG0_IOMODE_OPENDR | IOC_IOCFG0_IE | GPIOCC26XX_CFG_PIN_IS_OUTPUT_INTERNAL)
#define GPIO_CFG_OUT_OPEN_SOURCE_INTERNAL   (IOC_IOCFG0_IOMODE_OPENSRC | IOC_IOCFG0_IE | GPIOCC26XX_CFG_PIN_IS_OUTPUT_INTERNAL)

#define GPIO_CFG_PULL_NONE_INTERNAL         IOC_IOCFG0_PULL_CTL_DIS
#define GPIO_CFG_PULL_UP_INTERNAL           IOC_IOCFG0_PULL_CTL_UP
#define GPIO_CFG_PULL_DOWN_INTERNAL         IOC_IOCFG0_PULL_CTL_DWN

#define GPIO_CFG_INT_NONE_INTERNAL          IOC_IOCFG0_EDGE_DET_NONE
#define GPIO_CFG_INT_FALLING_INTERNAL       IOC_IOCFG0_EDGE_DET_NEG
#define GPIO_CFG_INT_RISING_INTERNAL        IOC_IOCFG0_EDGE_DET_POS
#define GPIO_CFG_INT_BOTH_EDGES_INTERNAL    IOC_IOCFG0_EDGE_DET_BOTH

#define GPIO_CFG_INT_ENABLE_INTERNAL        IOC_IOCFG0_EDGE_IRQ_EN
#define GPIO_CFG_INT_DISABLE_INTERNAL       0

/* Value inversion is stored in the low bit of IOMODE */
#define GPIO_CFG_INVERT_OFF_INTERNAL        0
#define GPIO_CFG_INVERT_ON_INTERNAL         IOC_IOCFG0_IOMODE_INV

#define GPIO_CFG_HYSTERESIS_OFF_INTERNAL    0
#define GPIO_CFG_HYSTERESIS_ON_INTERNAL     IOC_IOCFG0_HYST_EN

#define GPIO_CFG_SLEW_NORMAL_INTERNAL       0
#define GPIO_CFG_SLEW_REDUCED_INTERNAL      IOC_IOCFG0_SLEW_RED

#define GPIO_CFG_DRVSTR_LOW_INTERNAL        IOC_IOCFG0_IOSTR_AUTO
#define GPIO_CFG_DRVSTR_MED_INTERNAL        IOC_IOCFG0_IOSTR_MED
#define GPIO_CFG_DRVSTR_HIGH_INTERNAL       IOC_IOCFG0_IOSTR_MAX

/* We can hide some settings inside the MUX byte if they are not part of the
 * IOC config, since the mux is applied separately. On CC26XX this is the
 * lowest 8 bits.
 *
 * Do not use these values when calling GPIO_setConfig. They are for
 * internal use only, to provide support for driver functionality that does
 * not map directly into the IO config registers.
 */
#define GPIO_CFG_OUTPUT_DEFAULT_HIGH_INTERNAL   0x1
#define GPIO_CFG_OUTPUT_DEFAULT_LOW_INTERNAL    0

/* Whether GPIO hardware should have the output enable bit set for this pin */
#define GPIOCC26XX_CFG_PIN_IS_INPUT_INTERNAL    0x2
#define GPIOCC26XX_CFG_PIN_IS_OUTPUT_INTERNAL   0

#ifdef __cplusplus
}
#endif

#endif /* ti_drivers_GPIOCC26XX__include */
