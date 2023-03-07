/*
 * Copyright (c) 2014-2016, GreenPeak Technologies
 * Copyright (c) 2017, Qorvo Inc
 *
 *
 * This software is owned by Qorvo Inc
 * and protected under applicable copyright laws.
 * It is delivered under the terms of the license
 * and is intended and supplied for use solely and
 * exclusively with products manufactured by
 * Qorvo Inc.
 *
 *
 * THIS SOFTWARE IS PROVIDED IN AN "AS IS"
 * CONDITION. NO WARRANTIES, WHETHER EXPRESS,
 * IMPLIED OR STATUTORY, INCLUDING, BUT NOT
 * LIMITED TO, IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS FOR A
 * PARTICULAR PURPOSE APPLY TO THIS SOFTWARE.
 * QORVO INC. SHALL NOT, IN ANY
 * CIRCUMSTANCES, BE LIABLE FOR SPECIAL,
 * INCIDENTAL OR CONSEQUENTIAL DAMAGES,
 * FOR ANY REASON WHATSOEVER.
 *
 * $Header$
 * $Change$
 * $DateTime$
 *
 */

/*****************************************************************************
 *                    Includes Definitions
 *****************************************************************************/

#include "hal.h"
#include "gpHal.h"
#include "gpHal_DEFS.h"
#include "gpAssert.h"
#ifdef GP_COMP_GPHAL_ES
#include "gpHal_kx_ES.h"
#endif

#define GP_COMPONENT_ID GP_COMPONENT_ID_GPHAL

/*****************************************************************************
 *                    Macro Definitions
 *****************************************************************************/

// Definitions for Sleep Clock Accuracies (SCA)
// Average accuracies
#ifndef GP_DIVERSITY_GPHAL_CLOCK_ACCURACY_PPM_RC_AVERAGE
#define GP_DIVERSITY_GPHAL_CLOCK_ACCURACY_PPM_RC_AVERAGE    500
#endif //GP_DIVERSITY_GPHAL_CLOCK_ACCURACY_PPM_RC_AVERAGE

#ifndef GP_DIVERSITY_GPHAL_CLOCK_ACCURACY_PPM_32K_AVERAGE
#define GP_DIVERSITY_GPHAL_CLOCK_ACCURACY_PPM_32K_AVERAGE   100
#endif //GP_DIVERSITY_GPHAL_CLOCK_ACCURACY_PPM_32K_AVERAGE

#ifndef GP_DIVERSITY_GPHAL_CLOCK_ACCURACY_PPM_32M_AVERAGE
#define GP_DIVERSITY_GPHAL_CLOCK_ACCURACY_PPM_32M_AVERAGE   40 // SDP004-2387
#endif //GP_DIVERSITY_GPHAL_CLOCK_ACCURACY_PPM_32M_AVERAGE

// Worst case accuracies (to be used for dimensioning maximum sleep time)
#ifndef GP_DIVERSITY_GPHAL_CLOCK_ACCURACY_PPM_RC_WORST
#define GP_DIVERSITY_GPHAL_CLOCK_ACCURACY_PPM_RC_WORST      1300
#endif //GP_DIVERSITY_GPHAL_CLOCK_ACCURACY_PPM_RC_WORST

#ifndef GP_DIVERSITY_GPHAL_CLOCK_ACCURACY_PPM_32K_WORST
#define GP_DIVERSITY_GPHAL_CLOCK_ACCURACY_PPM_32K_WORST     100
#endif //GP_DIVERSITY_GPHAL_CLOCK_ACCURACY_PPM_32K_WORST

#ifndef GP_DIVERSITY_GPHAL_CLOCK_ACCURACY_PPM_32M_WORST
#define GP_DIVERSITY_GPHAL_CLOCK_ACCURACY_PPM_32M_WORST     40 // SDP004-2387
#endif //GP_DIVERSITY_GPHAL_CLOCK_ACCURACY_PPM_32M_WORST

/*****************************************************************************
 *                    Static Data Definitions
 *****************************************************************************/

UInt8 gpHal_awakeCounter=1;

/*****************************************************************************
 *                    Static Function Prototypes
 *****************************************************************************/

#if (defined(GP_COMP_GPHAL_ES_EXT_EVENT) || defined(GP_COMP_GPHAL_ES_ABS_EVENT) || defined(GP_COMP_GPHAL_ES_REL_EVENT) || defined(GP_DIVERSITY_GPHAL_INTERN)) 
static void gpHal_UpdateStandbyMode(gpHal_SleepMode_t mode);
#endif

/*****************************************************************************
 *                    Static Function Definitions
 *****************************************************************************/

#ifndef GP_COMP_CHIPEMU
#endif

#if (defined(GP_COMP_GPHAL_ES_EXT_EVENT) || defined(GP_COMP_GPHAL_ES_ABS_EVENT) || defined(GP_COMP_GPHAL_ES_REL_EVENT) || defined(GP_DIVERSITY_GPHAL_INTERN)) 
void gpHal_UpdateStandbyMode(gpHal_SleepMode_t mode)
{
    GP_ASSERT_SYSTEM(mode != gpHal_SleepMode32kHz || GP_BSP_32KHZ_CRYSTAL_AVAILABLE());
    UInt8 mode_selection;

    // Disable continuous conversion from sleep counter to symbol counter.
    GP_WB_WRITE_ES_ONLY_CONVERT_WHEN_NEEDED(1);

    // Disable automatic halting of time reference.
    // The backup handler needs time reference enabled to realign the sleep counter.
    // Time reference will be halted explicitly in the backup handler.
    GP_WB_WRITE_STANDBY_AUTO_HALT_TIME_REFERENCE(0);

    //Set sleep modes and collaterals
    gpHal_SetDefaultStartupSymbolTimes(mode);
    GP_WB_WRITE_PMUD_STBY_MODE(mode);
    /* Only wait for NVM power down if not in 16Mhz mode */
    mode_selection = (UInt8) (mode != gpHal_SleepMode16MHz);
    GP_WB_WRITE_STANDBY_WAIT_FOR_NVM_DPDOWN(mode_selection);

    // Enable backup handler even when going into 16 MHz sleep.
    GP_WB_WRITE_STANDBY_ALWAYS_TRIGGER_BACKUP_INT(1);

#ifdef GP_COMP_GPHAL_BLE
    // The RT system uses own CA in his calculations. Make sure we always update this when the sleep mode changes
    gpHal_BleSetClockAccuracy(gpHal_GetSleepClockAccuracy());
#endif
}
#endif

/*****************************************************************************
 *                    Public Function Definitions
 *****************************************************************************/

void gpHal_InitSleep(void)
{
    gpHal_awakeCounter=1;

#ifdef GP_DIVERSITY_GPHAL_INTERN
    gpHal_UpdateStandbyMode(gpHal_SleepMode16MHz);
#endif //GP_DIVERSITY_GPHAL_INTERN
}

void gpHal_GoToSleepWhenIdle(Bool enable)
{
    DISABLE_GP_GLOBAL_INT();

    if(enable) //Put chip in sleep if all other awake requests are done
    {
        if(gpHal_awakeCounter > 1)                //Decrement counter
        {
            gpHal_awakeCounter--;
        }
        else
        {

            //When counter is zero the RTGS bit is already set in the chip
            GP_ASSERT_DEV_INT(gpHal_awakeCounter!=0);      //Check on previous sleep set (awake counter only modified internally)

            //Radio chip is allowed to go to sleep
            GP_WB_WRITE_STANDBY_RTGTS_SW(true);

            //External C does not need radio chip, so we can put communication interface
            //in the lowest power mode

#ifndef GP_COMP_CHIPEMU
#endif

            gpHal_awakeCounter=0;                 //Protect counter from negative values
        }
    }
    else //Keep chip awake
    {
        gpHal_awakeCounter++;
        if(gpHal_awakeCounter == 1)
        {
#ifndef GP_COMP_CHIPEMU
#endif
            GP_WB_WRITE_STANDBY_RTGTS_SW(false);
        }
        GP_ASSERT_DEV_INT(gpHal_awakeCounter!=0x00);      //Check on counter overflow
    }

    ENABLE_GP_GLOBAL_INT();
}

#if (defined(GP_COMP_GPHAL_ES_EXT_EVENT) || defined(GP_COMP_GPHAL_ES_ABS_EVENT) || defined(GP_COMP_GPHAL_ES_REL_EVENT)) 

gpHal_Result_t gpHal_SetSleepMode(gpHal_SleepMode_t mode)
{
#if defined(GP_DIVERSITY_ENABLE_125_DEGREE_CELSIUS)
    /* See remark in datasheet "5.13.2 Standby Modes"      */
    /* Applications up to 125C should use XT Standby mode */
    GP_ASSERT_SYSTEM(gpHal_SleepMode16MHz == mode);
#endif //defined(GP_DIVERSITY_ENABLE_125_DEGREE_CELSIUS)

    /* If the 32kHz crystal is broken: fall back to RC sleep mode. */
    if (gpHal_SleepMode32kHz == mode)
    {
        gpHal_SleepClockMeasurementStatus_t status = gpHalEs_Get32kHzBenchmarkStatus();
        if (gpHal_SleepClockMeasurementStatusNotStable == status)
        {
            GP_LOG_SYSTEM_PRINTF("Error: 32kHz XTAL broken!",0);
            GP_ASSERT_DEV_EXT(false);
            return gpHal_ResultInvalidRequest;
        }

/* Look for 'gpHal_cb32kHzCalibrationDone' in gpHal_ES.c for the full story.
 * We disable this error-check here if the applications has not set 'GP_DIVERSITY_GPHAL_32KHZ_CALIBRATION_DONE_CB'
 * so that applications can still set gpHal_SleepMode32kHz before the initial calibration has finished. */
#if defined(GP_DIVERSITY_GPHAL_32KHZ_CALIBRATION_DONE_CB)
        else if (gpHal_SleepClockMeasurementStatusStable != status)
        {
            GP_LOG_SYSTEM_PRINTF("Error: 32kHz XTAL not ready!",0);
            GP_ASSERT_DEV_EXT(false);
            return gpHal_ResultBusy;
        }
#endif
    }

    if(GP_WB_READ_PMUD_STBY_MODE() == mode)
    {
        // Do not write sleep mode when it matches the current mode
        return gpHal_ResultSuccess;
    }

    gpHal_UpdateStandbyMode(mode);

    /* create task to handle sleep clock re-calibrations */
    gpHalES_setupSleepClockRecalibration(mode);

    return gpHal_ResultSuccess;
}

gpHal_SleepMode_t gpHal_GetSleepMode(void)
{
    return GP_WB_READ_PMUD_STBY_MODE();
}

#endif // (defined(GP_COMP_GPHAL_ES_EXT_EVENT) || defined(GP_COMP_GPHAL_ES_ABS_EVENT) || defined(GP_COMP_GPHAL_ES_REL_EVENT)) && !defined(GP_DIVERSITY_GPHAL_COPROC)

UInt16 gpHal_GetSleepClockAccuracy(void)
{
    const UInt16 averageScas[] = {
        GP_DIVERSITY_GPHAL_CLOCK_ACCURACY_PPM_RC_AVERAGE,     /* gpHal_SleepModeRC */
        GP_DIVERSITY_GPHAL_CLOCK_ACCURACY_PPM_32K_AVERAGE,    /* gpHal_SleepMode32kHz */
        GP_DIVERSITY_GPHAL_CLOCK_ACCURACY_PPM_32M_AVERAGE     /* gpHal_SleepMode16MHz */
    };

    gpHal_SleepMode_t sleepMode = GP_WB_READ_PMUD_STBY_MODE();

    if(sleepMode > gpHal_SleepMode16MHz)
    {
        GP_ASSERT_DEV_INT(false);
        return averageScas[gpHal_SleepMode16MHz];
    }

    return averageScas[sleepMode];
}

UInt16 gpHal_GetWorstSleepClockAccuracy(void)
{
    const UInt16 worstScas[] = {
        GP_DIVERSITY_GPHAL_CLOCK_ACCURACY_PPM_RC_WORST,     /* gpHal_SleepModeRC */
        GP_DIVERSITY_GPHAL_CLOCK_ACCURACY_PPM_32K_WORST,    /* gpHal_SleepMode32kHz */
        GP_DIVERSITY_GPHAL_CLOCK_ACCURACY_PPM_32M_WORST     /* gpHal_SleepMode16MHz */
    };

    gpHal_SleepMode_t sleepMode = GP_WB_READ_PMUD_STBY_MODE();

    if(sleepMode > gpHal_SleepMode16MHz)
    {
        GP_ASSERT_DEV_INT(false);
        return worstScas[gpHal_SleepMode16MHz];
    }

    return worstScas[sleepMode];
}
