/*
 * Copyright (c) 2014-2016, GreenPeak Technologies
 * Copyright (c) 2017, Qorvo Inc
 *
 * gpHal_Fll.c
 *
 *  This file defines all functions for periodic recalibration
 *  of FLL.
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
 */
// #define GP_LOCAL_LOG

#include "gpLog.h"
#include "gpHal.h"
#include "gpHal_kx_gpm.h"
#include "gpHal_Calibration.h"
#include "gpHal_kx_Ipc.h"
#include "gpHal_kx_Fll.h"
#include "gpHal_DEFS.h"
#include "gpHal_ES.h"
#include "gpSched.h"
#include "gpHal_RadioMgmt.h"

#define GP_COMPONENT_ID GP_COMPONENT_ID_GPHAL

//#define GP_LOCAL_LOG
static UInt8 calTaskHandle;
// FLL calibrate waiting for radio grant
static Bool FllCalibrationInProgress = false;
// FLL calibrate triggered from interrupt context
static Bool FllCalibrateInterruptContext = false;
// CW turned off so that radio is freed for FLL calibration to proceed
static Bool FllCalibrate_CWToggledOff = false;
// callback triggered once radio is granted
static void FllCalibrateRadioMgmtCallback(void);

// Start FLL calibration (claims radio asynchronously)
void FllCalibrateStart(void);

// clear FLL OOR/OOL interrupts after fll calibration
static void FllCalibrateClearFllInterrupts(void);

static void FllCalibrateClearFllInterrupts(void)
{
    if (FllCalibrate_CWToggledOff)
    {
        // Retrieve CW settings
        gpHalPhy_CurrentCWModeSettings_t *settings = gpHal_GetCurrentContinuousWaveModeSettings();
        // Enable CW - thereby automatically calibrating the channel
        gpHal_SetContinuousWaveMode(settings->mode, settings->channel,settings->txpower, settings->antenna);
    }

    // clear the interrupts
    if(GP_WB_READ_FLL_UNMASKED_TX_OUT_OF_LOCK_INTERRUPT())
    {
        GP_LOG_PRINTF("[FLL]: clear TX OOL", 0);
        GP_WB_FLL_CLR_TX_OUT_OF_LOCK_INTERRUPT();
    }
    if(GP_WB_READ_FLL_UNMASKED_RX_OUT_OF_RANGE_INTERRUPT())
    {
        GP_LOG_PRINTF("[FLL]: clear RX OOR", 0);
        GP_WB_FLL_CLR_RX_OUT_OF_RANGE_INTERRUPT();
    }
    if(GP_WB_READ_FLL_UNMASKED_TX_OUT_OF_RANGE_INTERRUPT())
    {
        GP_LOG_PRINTF("[FLL]: clear TX OOR", 0);
        GP_WB_FLL_CLR_TX_OUT_OF_RANGE_INTERRUPT();
    }
    FllCalibrateInterruptContext = false;
}


void gpHal_FllInit(void)
{
    gpHal_CalibrationTask_t calTask;
    GP_LOG_PRINTF("gpHal_FllInit", 0);
    MEMSET(&calTask, 0, sizeof(gpHal_CalibrationTask_t));
    calTask.flags = GP_HAL_CALIBRATION_FLAG_TEMPERATURE_SENSITIVE;
    /* Trigger calibrations for every 10 C change */
    calTask.temperatureThreshold = UQ_PRECISION_INCR8(10);

    /* Enable for testing */
    /* calTask.flags |=  GP_HAL_CALIBRATION_FLAG_PERIODIC;
     * calTask.calibrationPeriod = 1000000; */
    calTaskHandle = gpHal_CalibrationCreateTask(&calTask, gpHal_FllCalibrate);
    GP_ASSERT_DEV_INT(calTaskHandle != GP_HAL_CALIBRATION_INVALID_TASK_HANDLE);
    FllCalibrationInProgress = false;
    FllCalibrateInterruptContext = false;
    FllCalibrate_CWToggledOff = false;
}




/* Trigger radio claim and wait for grant */
void FllCalibrateStart(void)
{
    // if earlier instance of FLL calibration has not finished (due to pending radio grant)
    // let that complete first.
    if (FllCalibrationInProgress)
    {
        return;
    }


    GP_LOG_PRINTF("[FLL CAL]: start", 0);
    GP_STAT_SAMPLE_TIME();

#if defined(GP_COMP_GPHAL_MAC) || defined(GP_COMP_GPHAL_BLE)
    gpHalRadioMgmt_status claimStatus;
    // fll non blocking waits for radio to be granted
    // FllCalibrateRadioMgmtCallback is called upon radio grant
    claimStatus = gpHalRadioMgmtAsynch_claimRadio(FllCalibrateRadioMgmtCallback);

    if (claimStatus == gpHalRadioMgmt_success)
    {
        FllCalibrationInProgress = true;
        GP_LOG_PRINTF("[FLL CAL]: radio claimed", 0);
    }
    else
    {
        FllCalibrationInProgress = false;
        // if claim fails, radio mgmt buffer size has to be increased.
        GP_ASSERT_DEV_EXT(0);
    }
#endif
}

/* Peform actual calibration after radio is granted */
static void FllCalibrateRadioMgmtCallback(void)
{
    GP_LOG_PRINTF("[FLL CAL] radio granted", 0);

#if defined(GP_COMP_GPHAL_MAC) || defined(GP_COMP_GPHAL_BLE)
    gpHal_Result_t result = gpHal_ResultSuccess;
    /* Following IPC command triggers the RT to do the FLL (DTC/DCO) calibration */
    result = gpHal_IpcTriggerCommand(BLE_MGR_FLL_CAL_NRT, 0, NULL);
    GP_ASSERT_DEV_EXT(result == gpHal_ResultSuccess);

#ifdef GP_HAL_DIVERSITY_BLE_AE_RX_PRECALIBRATION
    gpHal_BlePreCalibrateAllChannels();
#endif // GP_HAL_DIVERSITY_BLE_AE_RX_PRECALIBRATION

#endif

    // releasing the claim so that MAC recalibration can go ahead
    gpHalRadioMgmtAsynch_releaseRadio();
#if defined(GP_COMP_GPHAL_MAC)
    /* Recalibrating the FLL table content */
    gpHalMac_RecalibrateChannels();
#endif

    GP_STAT_SAMPLE_TIME();
    gpHal_ClearCalibrationPendingOnWakeup(calTaskHandle);

    FllCalibrationInProgress = false;

    // clear interrupts that triggered calibration
    if (FllCalibrateInterruptContext)
    {
        FllCalibrateClearFllInterrupts();
    }

}


void gpHal_FllCalibrate(const gpHal_CalibrationTask_t* task)
{
    NOT_USED(task);
    gpHalPhy_CurrentCWModeSettings_t *settings = gpHal_GetCurrentContinuousWaveModeSettings();

    if (settings->mode != gpHal_CW_Off)
    {
        // Do not calibrate
        return;
    }

    // start FLL calibration and claim radio asynchronously
    FllCalibrateStart();
}



void gpHal_FllHandleInterrupts()
{
    // Retrieve CW settings
    GP_LOG_PRINTF("[FLL] i ", 0);
    gpHalPhy_CurrentCWModeSettings_t *settings = gpHal_GetCurrentContinuousWaveModeSettings();

    if(settings->mode != gpHal_CW_Off)
    {
        if(GP_WB_READ_FLL_UNMASKED_TX_OUT_OF_LOCK_INTERRUPT())
        {
            // Out of lock == bad
            GP_ASSERT_DEV_INT(false);
            GP_WB_FLL_CLR_TX_OUT_OF_LOCK_INTERRUPT();
        }

        if(GP_WB_READ_FLL_UNMASKED_RX_OUT_OF_RANGE_INTERRUPT())
        {
            // Should not happen during CW
            GP_ASSERT_DEV_INT(false);
            GP_WB_FLL_CLR_RX_OUT_OF_RANGE_INTERRUPT();
        }

        if(GP_WB_READ_FLL_UNMASKED_TX_OUT_OF_RANGE_INTERRUPT())
        {
            GP_LOG_SYSTEM_PRINTF("Recalibrating FLL TX Channel and restarting CW with mode %d channel %d txpower %d antenna %d",0,settings->mode, settings->channel, settings->txpower, settings->antenna);

             // Disable CW
            gpHal_SetContinuousWaveMode( gpHal_CW_Off, 0,0, 0);
            FllCalibrate_CWToggledOff = true;

            // start FLL calibration with asynchronous radio grant
            FllCalibrateStart();
            FllCalibrateInterruptContext = true;
        }
    }
    else
    {
        if(GP_WB_READ_FLL_UNMASKED_TX_OUT_OF_LOCK_INTERRUPT() ||
           GP_WB_READ_FLL_UNMASKED_RX_OUT_OF_RANGE_INTERRUPT() ||
           GP_WB_READ_FLL_UNMASKED_TX_OUT_OF_RANGE_INTERRUPT())
        {
            // start FLL calibration with asynchronous radio grant
            FllCalibrateStart();
            FllCalibrateInterruptContext = true;
        }
    }
}

