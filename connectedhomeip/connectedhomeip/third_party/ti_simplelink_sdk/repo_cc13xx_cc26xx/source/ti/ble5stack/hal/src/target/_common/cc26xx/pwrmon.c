/******************************************************************************

 @file  pwrmon.c

 @brief Implementation for CC26xx Vdd monitoring subsystem.

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

/* -----------------------------------------------------------------------------
 * Includes
 * -----------------------------------------------------------------------------
 */
#include "pwrmon.h"
#include "util.h"

#include <driverlib/aon_batmon.h>
#include <ti/sysbios/knl/Clock.h>

/* -----------------------------------------------------------------------------
 * Macros
 * -----------------------------------------------------------------------------
 */
#define BATMON_ON (HWREG(AON_BATMON_BASE + AON_BATMON_O_CTL) ? true : false)

/* -----------------------------------------------------------------------------
 * Local variables
 * -----------------------------------------------------------------------------
 */
static uint16_t          pwrmon_mode = PWRMON_MODE_LOW;
static uint16_t          pwrmon_thresh = 0;
static uint16_t          pwrmon_period = 0;
static PWRMON_CbackFn_t  pwrmon_cb = NULL;
static Clock_Struct      pwrmon_clock;

/* -----------------------------------------------------------------------------
 * Functions
 * -----------------------------------------------------------------------------
 */

/******************************************************************************
 * @fn      timerEventCB
 *
 * @brief   monitoring timer event
 *
 * @param   a0 - ignored
 *
 * @return  none
 ******************************************************************************/
static void timerEventCB(UArg a0)
{
  if(BATMON_ON)
  {
    uint32_t volt = PWRMON_REG_TO_MV(AONBatMonBatteryVoltageGet());
    switch(pwrmon_mode)
    {
      case PWRMON_MODE_HIGH:
        if(volt >= pwrmon_thresh)
        {
          // The measured device voltage is above the threshold
          pwrmon_cb(volt);
        }
        break;
      case PWRMON_MODE_LOW:
        if(volt < pwrmon_thresh)
        {
          // the measured device voltage is bellow the threshold
          pwrmon_cb(volt);
        }
        break;
      case PWRMON_MODE_ALWAYS:
      default:
        pwrmon_cb(volt);
    }
  }
}

/*******************************************************************************
 * @fn      PWRMON_check()
 *
 * @brief   Checks the caller supplied voltage threshold against the value read
 *          from the CC26xx BATMON register.
 *
 * @param   threshold - voltage to compare measured device Vdd to
 *
 * @return  false if device voltage less than limit, otherwise true
 *******************************************************************************
 */
bool PWRMON_check(uint16_t threshold)
{
  if(BATMON_ON && (AONBatMonBatteryVoltageGet() < threshold))
  {
    // Measured device voltage is below threshold
    return(false);
  }

  return(true);
}

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
void PWRMON_init(void)
{
    // Enable the CC26xx Battery Monitor
    AONBatMonEnable();
}

/*******************************************************************************
 * @fn      PWRMON_setMonitor()
 *
 * @brief   set the monitoring rate of the voltage. when voltage is detected to
 *          be lower than the threshold set by PWRMON_setThreshold, the
 *          callback is call.
 *          the threshold and the callback needs to be set first by calling the
 *          API PWRMON_setThreshold.
 *
 * @param[in]   mode:  the monitoring mode:
 *                       - PWRMON_MODE_LOW, CB is called when voltage is lower
 *                         than threshold
 *                       - PWRMON_MODE_HIGH, CB is called when voltage is higher
 *                         than threshold
 *                       - PWRMON_MODE_ALWAYS, CB is always called.
 * @param[in]   threshold:  threshold in mv
 * @param[in]   period:     in ms between each measurement (min 10, max 65535)
 * @param[in]   cBack:      callback to call when voltage goes under the
 *                          threshold
 *
 * @return  None
 *******************************************************************************
 */
uint8_t PWRMON_setMonitor(uint8_t mode, uint16_t threshold, uint16_t period,
                          PWRMON_CbackFn_t cBack)
{
  if((!cBack) || (period < 10) || (mode > PWRMON_MODE_ALWAYS))
  {
    return PWRMON_STATUS_FAILURE;
  }
  else
  {
    pwrmon_mode = mode;
    pwrmon_thresh = threshold;
    pwrmon_cb = cBack;
    pwrmon_period = period;
    return(PWRMON_STATUS_SUCCESS);
  }
}

/*******************************************************************************
 * @fn      PWRMON_startMonitor()
 *
 * @brief   Start monitoring the voltage, PWRMON_setThreshold must have been
 *          call first
 *
 * @return  Status:  PWRMON_STATUS_SUCCESS:  monitoring has started.
 *                   PWRMON_STATUS_FAILURE:  monitoring fail to start.
 *******************************************************************************
 */
uint8_t PWRMON_startMonitor(void)
{
  if(!pwrmon_cb)
  {
    return PWRMON_STATUS_FAILURE;
  }
  else
  {
    //Setup clock
    // setup debounce timer
    Util_constructClock(&pwrmon_clock, timerEventCB, pwrmon_period,
                        pwrmon_period, true, NULL);
    return(PWRMON_STATUS_SUCCESS);
  }
}

/*******************************************************************************
 * @fn      PWRMON_stopMonitor()
 *
 * @brief   Stop monitoring the voltage.
 *
 * @return  None
 *******************************************************************************
 */
void PWRMON_stopMonitor(void)
{
  Util_stopClock(&pwrmon_clock);
}

/*******************************************************************************
 * @fn      PWRMON_getVoltage
 *
 * @brief   return the current voltage apply to the cc26xx.
 *
 * @return  the voltage measured in mV.
 *******************************************************************************
 */
uint16_t PWRMON_getVoltage(void)
{
  if(BATMON_ON)
  {
    uint32_t volt = PWRMON_REG_TO_MV(AONBatMonBatteryVoltageGet());
    return((uint16_t)volt);
  }
  else
  {
    return(0);
  }
}
/*******************************************************************************
*/
