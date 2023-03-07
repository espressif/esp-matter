/*
** ###################################################################
**     Processor:           MIMXRT595SFFOA_cm33
**     Compilers:           GNU C Compiler
**                          IAR ANSI C/C++ Compiler for ARM
**                          Keil ARM C/C++ Compiler
**                          MCUXpresso Compiler
**
**     Reference manual:    RT500 Reference Manual. Rev.0,07/2019
**     Version:             rev. 2.0, 2019-07-22
**     Build:               b190723
**
**     Abstract:
**         Provides a system configuration function and a global variable that
**         contains the system frequency. It configures the device and initializes
**         the oscillator (PLL) that is part of the microcontroller device.
**
**     Copyright 2016 Freescale Semiconductor, Inc.
**     Copyright 2016-2019 NXP
**     All rights reserved.
**
**     SPDX-License-Identifier: BSD-3-Clause
**
**     http:                 www.nxp.com
**     mail:                 support@nxp.com
**
**     Revisions:
**     - rev. 1.0 (2019-04-19)
**         Initial version.
**     - rev. 2.0 (2019-07-22)
**         Base on rev 0.5 RM.
**
** ###################################################################
*/

/*!
 * @file MIMXRT595S_cm33
 * @version 2.0
 * @date 2019-07-22
 * @brief Device specific configuration file for MIMXRT595S_cm33 (header file)
 *
 * Provides a system configuration function and a global variable that contains
 * the system frequency. It configures the device and initializes the oscillator
 * (PLL) that is part of the microcontroller device.
 */

#ifndef _SYSTEM_MIMXRT595S_cm33_H_
#define _SYSTEM_MIMXRT595S_cm33_H_               /**< Symbol preventing repeated inclusion */

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>


#define DEFAULT_SYSTEM_CLOCK            1000000u           /* Default System clock value */
#ifndef CLK_XTAL_OSC_CLK
#define CLK_XTAL_OSC_CLK               24000000u           /* Default XTAL OSC clock */
#endif
#define CLK_RTC_32K_CLK                   32768u           /* RTC oscillator 32 kHz (32k_clk) */
#define CLK_LPOSC_1MHZ                  1000000u           /* Low power oscillator 1 MHz (1m_lposc) */
#define CLK_FRO_192MHZ                192000000u           /* FRO 192 MHz (fro_192m) */
#define CLK_FRO_96MHZ       (CLK_FRO_192MHZ / 2)           /* FRO 96 MHz (fro_96m) */
#define CLK_FRO_48MHZ        (CLK_FRO_96MHZ / 2)           /* FRO 48 MHz (fro_48m) */
#define CLK_FRO_24MHZ        (CLK_FRO_48MHZ / 2)           /* FRO 24 MHz (fro_24m) */
#define CLK_FRO_12MHZ        (CLK_FRO_24MHZ / 2)           /* FRO 12 MHz (fro_12m) */
#ifndef CLK_CLK_IN
#define CLK_CLK_IN                            0u           /* Default CLK_IN pin clock */
#endif


/**
 * @brief System clock frequency (core clock)
 *
 * The system clock frequency supplied to the SysTick timer and the processor
 * core clock. This variable can be used by the user application to setup the
 * SysTick timer or configure other parameters. It may also be used by debugger to
 * query the frequency of the debug timer or configure the trace clock speed
 * SystemCoreClock is initialized with a correct predefined value.
 */
extern uint32_t SystemCoreClock;

/**
 * @brief Setup the microcontroller system.
 *
 * Typically this function configures the oscillator (PLL) that is part of the
 * microcontroller device. For systems with variable clock speed it also updates
 * the variable SystemCoreClock. SystemInit is called from startup_device file.
 */
void SystemInit (void);

/**
 * @brief Updates the SystemCoreClock variable.
 *
 * It must be called whenever the core clock is changed during program
 * execution. SystemCoreClockUpdate() evaluates the clock register settings and calculates
 * the current core clock.
 */
void SystemCoreClockUpdate (void);

/**
 * @brief SystemInit function hook.
 *
 * This weak function allows to call specific initialization code during the
 * SystemInit() execution.This can be used when an application specific code needs
 * to be called as close to the reset entry as possible (for example the Multicore
 * Manager MCMGR_EarlyInit() function call).
 * NOTE: No global r/w variables can be used in this hook function because the
 * initialization of these variables happens after this function.
 */
void SystemInitHook (void);

#ifdef __cplusplus
}
#endif

#endif  /* _SYSTEM_MIMXRT595S_cm33_H_ */
