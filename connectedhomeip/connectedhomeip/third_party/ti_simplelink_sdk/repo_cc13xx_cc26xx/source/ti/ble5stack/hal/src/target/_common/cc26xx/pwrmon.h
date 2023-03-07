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
/** ============================================================================
 *  @file  pwrmon.h
 *
 *  @brief      power monitoring for CC26xx device
 *
 *  # Driver Include #
 *  The header file should be included in an application as follows:
 *  @code
 *  #include "pwrmon.h"
 *  @endcode
 *  the location of the driver needs to be included in the project options:
 *     $SRC_REMOTI_EXAMPLE$/common/cc26xx
 *  # Overview #
 *
 *  The power monitoring driver provides a set of API to easily monitor the
 *  voltage applied to your device. it allows the reading of this voltage at any
 *  time, or to know if the voltage is under a certain threshold, or to register
 *  a callback that will be called if the voltage goes under a specific
 *  threshold.
 *
 *  ## General Behavior #
 *  For code examples, see [Use Cases](@ref PWRMON_USE_CASES) below.
 *  This driver is not thread safe and is not intented to be used by different
 *  threads.
 *
 *  ### Starting up the monitoring #
 *  On CC26xx platform, the power monitoring is always enabled by default. The
 *  call to PWRMON_init() is optional.
 *
 *  ### Reading of the voltage #
 *  There are two ways to check the voltage appllied to the chip:
 *      - ask for a comparison with a threshold
 *      - ask for the direct value of the voltage
 *
 *  #### Threshold comparaison#
 *  calling PWRMON_check(thresh) will return FALSE depending if the current
 *  voltage appllied to the cc26xx if bellow the threshold 'thresh', TRUE if
 *  it's equal or above. in this case, the threshold unit is a fractional value
 *  align with the register definition (@ref PWRMON_REGISTER)

 *  #### Voltage reading #
 *  calling PWRMON_getVoltage() will return the voltage appllied to the cc26xx
 *
 *  On CC26xx platform, the power monitoring is always enabled by default. The
 *  call to PWRMON_init() is optional.
 *
 *  ### Setting up an alert #
 *  Using the APIs PWRMON_setMonitor and PWRMON_startMonitor, the user can
 *  register a callback function that will be called if the voltage goes under a
 *  specified threshold
 *  API PWRMON_stopMonitor can be used to stop the monitoring.
 *  This is not multi-thread, meaning only one monitoring can be setup at any
 *  time.
 *
*/

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
 * constant
 * -----------------------------------------------------------------------------
 */
#define PWRMON_STATUS_SUCCESS 0
#define PWRMON_STATUS_FAILURE 1

#define PWRMON_MODE_LOW       0
#define PWRMON_MODE_HIGH      1
#define PWRMON_MODE_ALWAYS    2
/* -----------------------------------------------------------------------------
 * typedef
 * -----------------------------------------------------------------------------
 */
typedef void (*PWRMON_CbackFn_t)(uint16_t voltage);

/* -----------------------------------------------------------------------------
 * Default Voltage Thresholds
 * -----------------------------------------------------------------------------
 */

// voltage register unit description @anchor PWRMON_REGISTER ##
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
#endif

// This macro converts a register value into millivolts.
#define PWRMON_REG_TO_MV(reg)  ((((reg & 0xFFFF) >> 8) * 1000) + \
                                (((reg & 0xFF)*1000)>>8));

// This macro converts millivolts into a register value.
#define PWRMON_MV_TO_REG(volt) (((volt / 1000) << 8) + \
                                (((volt * 256)/1000) & 0xFF));

/*******************************************************************************
 * @fn      PWRMON_check()
 *
 * @brief   Checks the caller supplied voltage threshold against the value read
 *          from the CC26xx BATMON register.
 *
 * @param   threshold - voltage to compare device Vdd to (AON_BATMON_O_BAT)
 *                      unit is register linear value, see @ref PWRMON_REGISTER
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
 * @fn      PWRMON_setMonitor()
 *
 * @brief   set the monitoring rate of the voltage. when voltage is detected to
 *          be lower than the threasholod set by PWRMON_setThreshold, the
 *          callback is called.
 *          the threshold and the callback needs to be set first by calling the
 *          API PWRMON_setThreshold.
 *
 * @param[in]   mode:  the monitoring mode:
 *                       - PWRMON_MODE_LOW, CB is called when voltage is lower
 *                         than threshold
 *                       - PWRMON_MODE_HIGH, CB is called when voltage is higher
 *                         than threshold
 *                       - PWRMON_MODE_ALWAYS, CB is always called.
 * @param[in]   threshold:  threshold in mV
 * @param[in]   period:     in mSec between each measurement (min 10, max 65535)
 * @param[in]   cBack:      callback to call when voltage goes under the
 *                          threshold
 *
 * @return  None
 *******************************************************************************
 */
extern uint8_t PWRMON_setMonitor(uint8_t mode, uint16_t threshold, uint16_t period,
                                 PWRMON_CbackFn_t cBack);

/*******************************************************************************
 * @fn      PWRMON_getVoltage
 *
 * @brief   return the current voltage appllied to the cc26xx.
 *
 * @return  the voltage measured in mV.
 *******************************************************************************
 */
extern uint16_t PWRMON_getVoltage(void);

/*******************************************************************************
 * @fn      PWRMON_startMonitor()
 *
 * @brief   Start monitoring the voltage, PWRMON_setThreshold must have been
 *          called first
 *
 * @return  Status:  PWRMON_STATUS_SUCCESS:  monitoring has started.
 *                   PWRMON_STATUS_FAILURE:  monitoring failed to start.
 *******************************************************************************
 */
extern uint8_t PWRMON_startMonitor(void);

/*******************************************************************************
 * @fn      PWRMON_stopMonitor()
 *
 * @brief   Stop monitoring the voltage.
 *
 * @return  None
 *******************************************************************************
 */
extern void PWRMON_stopMonitor(void);

/*******************************************************************************
*/

#ifdef __cplusplus
};
#endif

#endif
