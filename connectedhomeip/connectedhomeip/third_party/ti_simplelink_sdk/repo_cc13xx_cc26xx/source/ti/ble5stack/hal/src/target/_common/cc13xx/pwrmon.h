/******************************************************************************

 @file  pwrmon.h

 @brief Declarations for CC26xx Vdd monitoring subsystem.

 Group: WCS, LPC, BTS
 Target Device: cc13xx_cc26xx

 ******************************************************************************
 
 Copyright (c) 2015-2022, Texas Instruments Incorporated
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
#ifndef PWRMON_H
#define PWRMON_H

#ifdef __cplusplus
extern "C" {
#endif

/* -----------------------------------------------------------------------------
 * Includes
 * -----------------------------------------------------------------------------
 */
#include "hal_types.h"

/* -----------------------------------------------------------------------------
 * Default Voltage Thresholds
 * -----------------------------------------------------------------------------
 */

// Voltage thresholds are defined in "units" compatible with the CC26xx
// AON_BATMON_O_BAT register (see Technical Reference Manual for details).
// The register provides the voltage reading in 11 bits, masked by 0x7FF,
// where 'whole' volts are located in bits 8-10 (0x700) and the 'fraction'
// is located in bits 0-7 (0x0FF). The 'fraction' (0.00-0.99) is scaled
// linearly across the digital values of 0x00-0xFF, with 0x80 = 0.50 volt,
// 0xFF = 0.99 volt, etc.
// The default values provided below indicate settings slightly above and
// somewhat below the CC26xx brown-out threshold. It is intended that the
// user will modify these values appropriately for their system needs.

#if !defined (MIN_VDD_INIT)
#define MIN_VDD_INIT   0x1CE  // 1.80 volts (0.80=206/256 -> 206=0xCE)
#endif

#if !defined (MIN_VDD_POLL)
#define MIN_VDD_POLL   0x1DA  // 1.85 volts (0.85=218/256 -> 218=0xDA)
#endif

#if !defined (MIN_VDD_FLASH)
#define MIN_VDD_FLASH  0x180  // 1.50 volts (0.50=128/256 -> 128=0x80)
//#define MIN_VDD_FLASH  0x200  // 2.00 volts (0.00=  0/256 ->   0=0x00)
#endif

/*******************************************************************************
 * @fn      PWRMON_check()
 *
 * @brief   Checks the caller supplied voltage threshold against the value read
 *          from the CC26xx BATMON register.
 *
 * @param   threshold - voltage to compare device Vdd to (AON_BATMON_O_BAT)
 *
 * @return  false if device voltage less than limit, otherwise true
 *******************************************************************************
 */
extern bool PWRMON_check(uint16_t threshold);

/*******************************************************************************
 * @fn      PWRMON_init()
 *
 * @brief   Enable AON battery monitor and set update rate
 *
 * @param   None
 *
 * @return  None
 *******************************************************************************
 */
extern void PWRMON_init(void);

/*******************************************************************************
*/

#ifdef __cplusplus
};
#endif

#endif
