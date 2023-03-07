/*
 * Copyright (c) 2016, GreenPeak Technologies
 * Copyright (c) 2017, Qorvo Inc
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

//#define GP_LOCAL_LOG

#define GP_COMPONENT_ID GP_COMPONENT_ID_BLELLCP

/*****************************************************************************
 *                    Includes Definitions
 *****************************************************************************/

#include "gpBle.h"
#include "gpSched.h"
#include "gpBleComps.h"
#include "gpBleConfig.h"
#include "gpBle_defs.h"
#include "gpBleLlcp.h"
#include "gpBleLlcpProcedures.h"
#include "gpLog.h"
#include "gpHal.h"
#include "hal.h"

#ifdef GP_DIVERSITY_BLE_SLAVE
#include "gpBleAdvertiser.h"
#endif //GP_DIVERSITY_BLE_SLAVE

#ifdef GP_DIVERSITY_BLE_MASTER
#include "gpBleScanner.h"
#endif //GP_DIVERSITY_BLE_MASTER

/*****************************************************************************
 *                    Macro Definitions
 *****************************************************************************/

/*****************************************************************************
 *                    Functional Macro Definitions
 *****************************************************************************/

/*****************************************************************************
 *                    Type Definitions
 *****************************************************************************/

/*****************************************************************************
 *                    Static Data Definitions
 *****************************************************************************/

/*****************************************************************************
 *                    Static Function Prototypes
 *****************************************************************************/

// VSD helpers
static gpHci_Result_t gpBle_SetVsdDualModeTimeFor15Dot4Helper( gpHci_CommandParameters_t* pParams, gpBle_EventBuffer_t* pEventBuf);
static gpHci_Result_t gpBle_SetVsdProcessorClockSpeedHelper(gpHci_CommandParameters_t* pParams, gpBle_EventBuffer_t* pEventBuf);


static gpHci_Result_t Ble_SetSleepAction(gpHci_SleepMode_t hciSleepMode, Bool enable);

/*****************************************************************************
 *                    Static Function Definitions
 *****************************************************************************/



gpHci_Result_t gpBle_SetVsdDualModeTimeFor15Dot4Helper( gpHci_CommandParameters_t* pParams, gpBle_EventBuffer_t* pEventBuf)
{
    UInt32 timeFor15Dot4;
    BLE_SET_RESPONSE_EVENT_COMMAND_COMPLETE(pEventBuf->eventCode);

    MEMCPY(&timeFor15Dot4, pParams->SetVsdTestParams.value, sizeof(UInt32));
    Ble_SetTimeFor15Dot4(timeFor15Dot4);

    return gpHci_ResultSuccess;
}

gpHci_Result_t gpBle_SetVsdProcessorClockSpeedHelper(gpHci_CommandParameters_t* pParams, gpBle_EventBuffer_t* pEventBuf)
{
    UInt8 clockSpeed;

    BLE_SET_RESPONSE_EVENT_COMMAND_COMPLETE(pEventBuf->eventCode);

    clockSpeed = pParams->SetVsdTestParams.value[0];

    GP_LOG_SYSTEM_PRINTF("setting clock speed to setting %x",0,clockSpeed);

#ifdef GP_DIVERSITY_CORTEXM4
    HAL_SET_MCU_CLOCK_SPEED(clockSpeed);
#endif

    return gpHci_ResultSuccess;
}


gpHci_Result_t Ble_SetSleepAction(gpHci_SleepMode_t hciSleepMode, Bool enable)
{
    gpHal_SleepMode_t sleepMode = 0xFF;

    if(hciSleepMode == gpHci_SleepMode_RC)
    {
        GP_LOG_SYSTEM_PRINTF("RC",0);
        sleepMode = gpHal_SleepModeRC;
    }
    else if(hciSleepMode == gpHci_SleepMode_32kHzXtal)
    {
        GP_LOG_SYSTEM_PRINTF("32 Xtal",0);
        sleepMode = gpHal_SleepMode32kHz;
    }
    else if(hciSleepMode == gpHci_SleepMode_16Mhz)
    {
        GP_LOG_SYSTEM_PRINTF("16 Xtal",0);
        sleepMode = gpHal_SleepMode16MHz;
    }
    else
    {
        // Sleep mode disabled
        GP_LOG_SYSTEM_PRINTF("Set sleep disable",0);
        gpSched_SetGotoSleepEnable(false);
        return gpHci_ResultSuccess;
    }

    GP_LOG_SYSTEM_PRINTF("set sleep mode HCI %x hal: %x",0,hciSleepMode,sleepMode);

    gpHal_SetSleepMode(sleepMode);

    if(enable)
    {
        gpSched_SetGotoSleepEnable(true);
    }

    return gpHci_ResultSuccess;
}

gpHci_Result_t gpBle_SetVsdfixedRxWindowThresholdHelper( gpHci_CommandParameters_t* pParams, gpBle_EventBuffer_t* pEventBuf)
{
    UInt16 threshold;
    if (pParams->SetVsdTestParams.length != sizeof(threshold) )
    {
        return gpHci_ResultInvalidHCICommandParameters;
    }

    MEMCPY(&threshold, pParams->SetVsdTestParams.value, sizeof(threshold));
    BLE_SET_RESPONSE_EVENT_COMMAND_COMPLETE(pEventBuf->eventCode);

    gpBle_SetVsdfixedRxWindowThresholdParam( threshold );

    return gpHci_ResultSuccess;
}


/*****************************************************************************
 *                    Public Function Definitions
 *****************************************************************************/



/*****************************************************************************
 *                    Service Function Definitions
 *****************************************************************************/

gpHci_Result_t gpBle_SetVsdTestParams(gpHci_CommandParameters_t* pParams, gpBle_EventBuffer_t* pEventBuf)
{
    gpHci_Result_t result = gpHci_ResultUnsupportedFeatureOrParameterValue;
    switch(pParams->SetVsdTestParams.type)
    {
        case gpBle_SetVsdDualModeTimeFor15Dot4Type: return gpBle_SetVsdDualModeTimeFor15Dot4Helper(pParams, pEventBuf);
        case gpBle_SetVsdProcessorClockSpeed: return gpBle_SetVsdProcessorClockSpeedHelper(pParams, pEventBuf);
        case gpBle_SetVsdfixedRxWindowThreshold: return gpBle_SetVsdfixedRxWindowThresholdHelper(pParams, pEventBuf);
        default:
        {
            BLE_SET_RESPONSE_EVENT_COMMAND_COMPLETE(pEventBuf->eventCode);
            result=gpHci_ResultUnsupportedFeatureOrParameterValue;
        }
    }
    return result;
}

gpHci_Result_t gpBle_VsdSetSleep( gpHci_CommandParameters_t* pParams, gpBle_EventBuffer_t* pEventBuf)
{
    BLE_SET_RESPONSE_EVENT_COMMAND_COMPLETE(pEventBuf->eventCode);

    if(pParams->VsdSetSleep.mode >= gpHci_SleepMode_Invalid)
    {
        GP_LOG_PRINTF("sleep mode %x invalid", 0, pParams->VsdSetSleep.mode);
        return gpHci_ResultInvalidHCICommandParameters;
    }

    return Ble_SetSleepAction(pParams->VsdSetSleep.mode, pParams->VsdSetSleep.enable);
}



