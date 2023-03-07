/******************************************************************************

 @file  rcosc_calibration.c

 @brief This file contains the RCOSC calibration routines.

 Group: WCS, BTS
 Target Device: cc13xx_cc26xx

 ******************************************************************************
 
 Copyright (c) 2016-2022, Texas Instruments Incorporated
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

#ifdef USE_RCOSC

/*********************************************************************
 * INCLUDES
 */

#include <ti/drivers/Power.h>
#include <ti/drivers/power/PowerCC26XX.h>
#include <ti/sysbios/knl/Clock.h>

#include "hci.h"

#include "util.h"
#include "rcosc_calibration.h"

/*********************************************************************
 * LOCAL VARIABLES
 */

// Clock instance for Calibration injections
static Clock_Struct  injectCalibrationClock;

// Power Notify Object for wake-up callbacks
Power_NotifyObj injectCalibrationPowerNotifyObj;

static uint8_t isEnabled = FALSE;

/*********************************************************************
 * LOCAL FUNCTIONS
 */
static void rcosc_injectCalibrationClockHandler(UArg arg);

static uint8_t rcosc_injectCalibrationPostNotify(uint8_t eventType,
                                                 uint32_t *eventArg,
                                                 uint32_t *clientArg);

/*********************************************************************
 * PUBLIC FUNCTIONS
 */

/*********************************************************************
 * @fn      RCOSC_enableCalibration
 *
 * @brief   enable calibration.  calibration timer will start immediately.
 *
 * @param   none
 *
 * @return  none
 */
void RCOSC_enableCalibration(void)
{
  if (!isEnabled)
  {
    isEnabled = TRUE;

    // Create RCOSC clock - one-shot clock for calibration injections.
    Util_constructClock(&injectCalibrationClock,
                        rcosc_injectCalibrationClockHandler,
                        RCOSC_CALIBRATION_PERIOD, 0, false, 0);

    // Receive callback when device wakes up from Standby Mode.
    Power_registerNotify(&injectCalibrationPowerNotifyObj, PowerCC26XX_AWAKE_STANDBY,
                         (Power_NotifyFxn)rcosc_injectCalibrationPostNotify,
                         (uintptr_t) NULL);


    // Start clock for the RCOSC calibration injection.  Calibration must be
    // done once every second by either the clock or by waking up from StandyBy
    // Mode. To ensure that the device is always correctly calibrated the clock
    // is started now but is only allowed to expire when a wake up event does
    // not occur within the clock's duration.
    Util_startClock(&injectCalibrationClock);
  }
}

/*********************************************************************
 * @fn      rcosc_injectCalibrationClockHandler
 *
 * @brief   Handler function for RCOSC clock timeouts.  Executes in
 *          SWI context.
 *
 * @param   arg - event type
 *
 * @return  None.
 */
static void rcosc_injectCalibrationClockHandler(UArg arg)
{
  // Restart clock.
  Util_startClock(&injectCalibrationClock);

  // Inject calibration.
  PowerCC26XX_injectCalibration();
}

/*********************************************************************
 * @fn      rcosc_injectCalibrationPostNotify
 *
 * @brief   Callback for Power module state change events.
 *
 * @param   eventType - The state change.
 * @param   clientArg - Not used.
 *
 * @return  Power_NOTIFYDONE
 */
static uint8_t rcosc_injectCalibrationPostNotify(uint8_t eventType,
                                                 uint32_t *eventArg,
                                                 uint32_t *clientArg)
{
  // If clock is active at time of wake up,
  if (Util_isActive(&injectCalibrationClock))
  {
    // Stop injection of calibration - the wakeup has automatically done this.
    Util_stopClock(&injectCalibrationClock);
  }

  // Restart the clock in case delta between now and next wake up is greater
  // than one second.
  Util_startClock(&injectCalibrationClock);

  return Power_NOTIFYDONE;
}
#endif //USE_RCOSC
