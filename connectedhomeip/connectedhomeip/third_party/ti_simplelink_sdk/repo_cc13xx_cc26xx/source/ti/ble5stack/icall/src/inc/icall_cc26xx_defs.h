/******************************************************************************

 Group: WCS, LPC, BTS
 Target Device: cc13xx_cc26xx

 ******************************************************************************
 
 Copyright (c) 2014-2022, Texas Instruments Incorporated
 All rights reserved.

 Redistribution and use in source and binary forms, with or without
 modification, are permitted provided that the following conditions
 are met:

 *  Redistributions of source code must retain the above copyright
    notice, this list of conditions and the following disclaimer.

 *  Redistributions in binary form must reproduce the above copyright
    notice, this list of conditions and the following disclaimer in the
    documentation and/or other materials provided with the distribution.

 *  Neither the name of Texas Instruments Incorporated nor the names of
    its contributors may be used to endorse or promote products derived
    from this software without specific prior written permission.

 THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR
 CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

 ******************************************************************************
 
 
 *****************************************************************************/

/**
 *  @file icall_cc26xx_defs.h
 *  @brief Indirect function Call dispatcher constant definitions specific to
 *  CC26xx platform.
 *
 *  Note that the constants in this file is unique to the CC26xx and are
 *  not generic.
 */

#ifndef ICALLCC26XXDEFS_H
#define ICALLCC26XXDEFS_H

/**
 *  @addtogroup ICall_Constants
 *  @{
 */

/* Note that this header file must not have dependency on actual TI-RTOS
 * header file because the TI-RTOS header file must not be included
 * when building a stack image. */

/**
 * Power state transition to active power state
 * from standby power state.
 */
#define ICALL_PWR_AWAKE_FROM_STANDBY       0

/**
 * Power state transition to standby power state
 * from active power state.
 */
#define ICALL_PWR_ENTER_STANDBY            2

/**
 * Power state transition to shut down power state
 * from active power state
 */
#define ICALL_PWR_ENTER_SHUTDOWN           4

/**
 * Power state transition to active power state
 * where IOs can be accessed, from standby power state.
 */
#define ICALL_PWR_AWAKE_FROM_STANDBY_LATE  5

/**
 * Return value of ICall_pwrGetTransitionState().
 * Power state transition cannot be retrieved.
 */
#define ICALL_PWR_TRANSITION_UNKNOWN        0

/**
 * Return value of ICall_pwrGetTransitionState().
 * Power state transition not happening.
 */
#define ICALL_PWR_TRANSITION_STAY_IN_ACTIVE 1

/**
 * Return value of ICall_pwrGetTransitionState().
 * Transitioning into sleep.
 */
#define ICALL_PWR_TRANSITION_ENTERING_SLEEP 2

/**
 * Return value of ICall_pwrGetTransitionState().
 * Transitioning out of sleep.
 */
#define ICALL_PWR_TRANSITION_EXITING_SLEEP  3


/* Constraints and dependencies */

/**
 * A power API constraint flag that can be added to a bitmap used
 * as the argument to ICall_pwrConfigACAction(), ICall_pwrRequire()
 * and ICall_pwrDispense().
 */
#define ICALL_PWR_C_SB_VIMS_CACHE_RETAIN       0x00000001l

/**
 * A power API constraint flag that can be added to a bitmap used
 * as the argument to ICall_pwrConfigACAction(), ICall_pwrRequire()
 * and ICall_pwrDispense().
 */
#define ICALL_PWR_C_SD_DISALLOW                0x00000002l

/**
 * A power API constraint flag that can be added to a bitmap used
 * as the argument to ICall_pwrConfigACAction(), ICall_pwrRequire()
 * and ICall_pwrDispense().
 */
#define ICALL_PWR_C_SB_DISALLOW                0x00000004l

/**
 * A power API constraint flag that can be added to a bitmap used
 * as the argument to ICall_pwrConfigACAction(), ICall_pwrRequire()
 * and ICall_pwrDispense().
 */
#define ICALL_PWR_C_IDLE_PD_DISALLOW           0x00000008l

/**
 * A power API constraint flag that can be added to a bitmap used
 * as the argument to ICall_pwrConfigACAction(), ICall_pwrRequire()
 * and ICall_pwrDispense().
 */
#define ICALL_PWR_C_NEED_FLASH_IN_IDLE         0x00000010l

/**
 * A power API dependency flag that can be added to a bitmap used
 * as the argument to ICall_pwrConfigACAction(), ICall_pwrRequire()
 * and ICall_pwrDispense().
 */
#define ICALL_PWR_D_PERIPH_GPT0                0x00000020l

/**
 * A power API dependency flag that can be added to a bitmap used
 * as the argument to ICall_pwrConfigACAction(), ICall_pwrRequire()
 * and ICall_pwrDispense().
 */
#define ICALL_PWR_D_PERIPH_GPT1                0x00000040l

/**
 * A power API dependency flag that can be added to a bitmap used
 * as the argument to ICall_pwrConfigACAction(), ICall_pwrRequire()
 * and ICall_pwrDispense().
 */
#define ICALL_PWR_D_PERIPH_GPT2                0x00000080l

/**
 * A power API dependency flag that can be added to a bitmap used
 * as the argument to ICall_pwrConfigACAction(), ICall_pwrRequire()
 * and ICall_pwrDispense().
 */
#define ICALL_PWR_D_PERIPH_GPT3                0x00000100l

/**
 * A power API dependency flag that can be added to a bitmap used
 * as the argument to ICall_pwrConfigACAction(), ICall_pwrRequire()
 * and ICall_pwrDispense().
 */
#define ICALL_PWR_D_PERIPH_SSI0                0x00000200l

/**
 * A power API dependency flag that can be added to a bitmap used
 * as the argument to ICall_pwrConfigACAction(), ICall_pwrRequire()
 * and ICall_pwrDispense().
 */
#define ICALL_PWR_D_PERIPH_SSI1                0x00000400l

/**
 * A power API dependency flag that can be added to a bitmap used
 * as the argument to ICall_pwrConfigACAction(), ICall_pwrRequire()
 * and ICall_pwrDispense().
 */
#define ICALL_PWR_D_PERIPH_UART0               0x00000800l

/**
 * A power API dependency flag that can be added to a bitmap used
 * as the argument to ICall_pwrConfigACAction(), ICall_pwrRequire()
 * and ICall_pwrDispense().
 */
#define ICALL_PWR_D_PERIPH_I2C0                0x00001000l

/**
 * A power API dependency flag that can be added to a bitmap used
 * as the argument to ICall_pwrConfigACAction(), ICall_pwrRequire()
 * and ICall_pwrDispense().
 */
#define ICALL_PWR_D_PERIPH_TRNG                0x00002000l

/**
 * A power API dependency flag that can be added to a bitmap used
 * as the argument to ICall_pwrConfigACAction(), ICall_pwrRequire()
 * and ICall_pwrDispense().
 */
#define ICALL_PWR_D_PERIPH_GPIO                0x00004000l

/**
 * A power API dependency flag that can be added to a bitmap used
 * as the argument to ICall_pwrConfigACAction(), ICall_pwrRequire()
 * and ICall_pwrDispense().
 */
#define ICALL_PWR_D_PERIPH_UDMA                0x00008000l

/**
 * A power API dependency flag that can be added to a bitmap used
 * as the argument to ICall_pwrConfigACAction(), ICall_pwrRequire()
 * and ICall_pwrDispense().
 */
#define ICALL_PWR_D_PERIPH_CRYPTO              0x00010000l

/**
 * A power API dependency flag that can be added to a bitmap used
 * as the argument to ICall_pwrConfigACAction(), ICall_pwrRequire()
 * and ICall_pwrDispense().
 */
#define ICALL_PWR_D_PERIPH_I2S                 0x00020000l

/**
 * A power API dependency flag that can be added to a bitmap used
 * as the argument to ICall_pwrConfigACAction(), ICall_pwrRequire()
 * and ICall_pwrDispense().
 */
#define ICALL_PWR_D_PERIPH_RFCORE              0x00040000l

/**
 * A power API dependency flag that can be added to a bitmap used
 * as the argument to ICall_pwrConfigACAction(), ICall_pwrRequire()
 * and ICall_pwrDispense().
 */
#define ICALL_PWR_D_XOSC_HF                    0x00080000l

/**
 * A power API dependency flag that can be added to a bitmap used
 * as the argument to ICall_pwrConfigACAction(), ICall_pwrRequire()
 * and ICall_pwrDispense().
 */
#define ICALL_PWR_D_DOMAIN_PERIPH              0x00100000l

/**
 * A power API dependency flag that can be added to a bitmap used
 * as the argument to ICall_pwrConfigACAction(), ICall_pwrRequire()
 * and ICall_pwrDispense().
 */
#define ICALL_PWR_D_DOMAIN_SERIAL              0x00200000l

/**
 * A power API dependency flag that can be added to a bitmap used
 * as the argument to ICall_pwrConfigACAction(), ICall_pwrRequire()
 * and ICall_pwrDispense().
 */
#define ICALL_PWR_D_DOMAIN_RFCORE              0x00400000l

/**
 * A power API dependency flag that can be added to a bitmap used
 * as the argument to ICall_pwrConfigACAction(), ICall_pwrRequire()
 * and ICall_pwrDispense().
 */
#define ICALL_PWR_D_DOMAIN_SYSBUS              0x00800000l

#endif /* ICALLCC26XXDEFS_H */

/** @} */ // end of ICall_Constants
