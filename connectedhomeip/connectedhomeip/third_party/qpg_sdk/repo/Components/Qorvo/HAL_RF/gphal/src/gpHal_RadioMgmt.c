/*
 * Copyright (c) 2021, Qorvo Inc
 *
 * gpHal_RadioMgmt.c
 *   Radio claim management for different modules in NRT.
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
 * $Header:
 * $Change:
 * $DateTime:
 *
 */

// #define GP_LOCAL_LOG
#include "gpHal.h"
#include "gpHal_RadioMgmt.h"
#include "gpUtils.h"
#include "gpSched.h"
#include "gpLog.h"

#define GP_COMPONENT_ID GP_COMPONENT_ID_GPHAL
#define CHECK_GRANT_INTERVAL_US    1000

/* Circular buffer in which the callbacks registered by each radio claim user is stored
till that user releases radio */
radioMgmtUserCallBack radioMgmtUserCbBuffer[GPHAL_RADIOMGMT_MAXUSERS];
/* Circular buffer administration */
gpUtils_CircularBuffer_t  radioMgmt_UserCbCircBuf;

static Bool synchClaim;
static Bool asynchClaim;

static void gpHalRadioMgmt_checkGrant(void);

static INLINE void radioMgmt_claimRadio(Bool claim)
{
    GP_WB_WRITE_RADIO_ARB_MGMT_CLAIM(claim);
}

Bool radioMgmt_IsRadioClaimed(void)
{
    return GP_WB_READ_RADIO_ARB_MGMT_CLAIM();
}

Bool radioMgmt_IsRadioGranted(void)
{
    return GP_WB_READ_RADIO_ARB_MGMT_GRANTED();
}

static gpHalRadioMgmt_status radioMgmt_popUserCallback(radioMgmtUserCallBack* cb)
{
    gpHalRadioMgmt_status stat;
    Bool res;
    HAL_DISABLE_GLOBAL_INT();
    res = gpUtils_CircBReadData (&radioMgmt_UserCbCircBuf, (UInt8 *)cb, sizeof(radioMgmtUserCallBack));
    HAL_ENABLE_GLOBAL_INT();
    stat = (res) ? gpHalRadioMgmt_success : gpHalRadioMgmt_fail;
    return stat;
}

static gpHalRadioMgmt_status radioMgmt_addUserCallback(radioMgmtUserCallBack cb)
{
    gpHalRadioMgmt_status stat;
    Bool res;
    HAL_DISABLE_GLOBAL_INT();
    res = gpUtils_CircBWriteData (&radioMgmt_UserCbCircBuf, (UInt8 *)&cb, sizeof(radioMgmtUserCallBack));
    HAL_ENABLE_GLOBAL_INT();
    stat = (res) ? gpHalRadioMgmt_success : gpHalRadioMgmt_fail;
    return stat;
}

static UInt8 radioMgmt_numPendingClaims(void)
{
    UInt16 availableData;
    HAL_DISABLE_GLOBAL_INT();
    availableData = gpUtils_CircBAvailableData(&radioMgmt_UserCbCircBuf);
    HAL_ENABLE_GLOBAL_INT();
    return availableData;
}

static void radioMgmt_managePendingClaims(void)
{
    if (radioMgmt_numPendingClaims())
    {
        /* raise radio claim */
        radioMgmt_claimRadio(true);
        asynchClaim = true;
        gpSched_ScheduleEvent(CHECK_GRANT_INTERVAL_US, gpHalRadioMgmt_checkGrant);
    }
}

/* API called to release claim of radio and if there are any pending users who want to claim
the radio allocate radio to them */
void gpHalRadioMgmtAsynch_releaseRadio(void)
{
    GP_ASSERT_DEV_EXT(radioMgmt_IsRadioClaimed());
    GP_ASSERT_DEV_EXT(asynchClaim);
    /* release radio claim */
    GP_LOG_PRINTF("[RADIOMGMT]:: nonblocking claim -> 0", 0);
    radioMgmt_claimRadio(false);
    asynchClaim = false;
    GP_LOG_PRINTF("[RADIOMGMT]:: asynch release", 0);
    gpSched_UnscheduleEvent(gpHalRadioMgmt_checkGrant);
}

static void gpHalRadioMgmt_checkGrant(void)
{
    GP_ASSERT_DEV_EXT(radioMgmt_IsRadioClaimed());
    GP_ASSERT_DEV_EXT(asynchClaim == true);
    GP_ASSERT_DEV_EXT(radioMgmt_numPendingClaims() != 0);
    /* if radio claimed earlier by radio mgmt, check if radio has been granted by radio arbiter */
    if (radioMgmt_IsRadioGranted())
    {
        radioMgmtUserCallBack cb;
        radioMgmt_popUserCallback(&cb);
        GP_LOG_PRINTF("[RADIOMGMT]: exec cb %lx", 0, (UInt32)cb);
        gpLog_Flush();
        GP_ASSERT_DEV_EXT(cb != 0);
        /* call user callback */
        cb();
        /* Assert that callback has released radio */
        GP_ASSERT_DEV_EXT(!radioMgmt_IsRadioClaimed());

        /* Catch wrong usage of sync release i.s.o async release */
        GP_ASSERT_DEV_EXT(asynchClaim == false);

        radioMgmt_managePendingClaims();
    }
    else
    {
        GP_LOG_PRINTF("[RADIOMGMT]: radio not granted!", 0);
        gpSched_ScheduleEvent(CHECK_GRANT_INTERVAL_US, gpHalRadioMgmt_checkGrant);
    }
}

void gpHalRadioMgmt_init(void)
{
    HAL_DISABLE_GLOBAL_INT();
    gpUtils_CircBInit(&radioMgmt_UserCbCircBuf, radioMgmtUserCbBuffer, sizeof(radioMgmtUserCbBuffer));
    HAL_ENABLE_GLOBAL_INT();
    synchClaim = false;
    asynchClaim = false;
}

gpHalRadioMgmt_status gpHalRadioMgmtAsynch_claimRadio(radioMgmtUserCallBack cb)
{
    GP_LOG_PRINTF("[RADIOMGMT]:: nonblocking claim -> 1", 0);

    if (cb)
    {
        gpHalRadioMgmt_status st;
        st = radioMgmt_addUserCallback(cb);
        if (st == gpHalRadioMgmt_fail)
        {
          /* Note: increase GPHAL_RADIOMGMT_MAXUSERS in this case */
          GP_LOG_SYSTEM_PRINTF("[RADIOMGMT]::claim(): failed, exceeded max users", 0);
          return gpHalRadioMgmt_fail;
        }
    }

    if (!radioMgmt_IsRadioClaimed() && (radioMgmt_numPendingClaims() != 0))
    {
        radioMgmt_claimRadio(true);
        asynchClaim = true;
        gpSched_ScheduleEvent(CHECK_GRANT_INTERVAL_US, gpHalRadioMgmt_checkGrant);
    }
    return gpHalRadioMgmt_success;
}

void gpHalRadioMgmtSynch_claimRadio(void)
{
    /* if radio is claimed, and checkGrant is scheduled that would be a pending async claim */
    if (radioMgmt_IsRadioClaimed() && gpSched_ExistsEvent(gpHalRadioMgmt_checkGrant))
    {
        // if asynchClaim not executed, cb still in queue and will be executed once radio is claimed on release of synch claim
        // check for grant will be rescheduled on release of synch radio claim after claiming radio asynch
        gpSched_UnscheduleEvent(gpHalRadioMgmt_checkGrant);
    }
    else
    {
        // multple synch claims not allowed
        GP_ASSERT_DEV_EXT(GP_WB_READ_RADIO_ARB_MGMT_CLAIM() != 1);
        // Each previous synch claimRadio call requires a synch releaseRadio call as well
        GP_ASSERT_DEV_EXT(synchClaim == false);
        synchClaim = true;

    }
    GP_LOG_PRINTF("[RADIOMGMT]:: blocking claim -> 1", 0);
    GP_WB_WRITE_RADIO_ARB_MGMT_CLAIM(1);
    GP_DO_WHILE_TIMEOUT_ASSERT(!GP_WB_READ_RADIO_ARB_MGMT_GRANTED(), 1000000); // can take a while, needs to wait until BLE event/ZB activity is done...
}

void gpHalRadioMgmtSynch_releaseRadio(void)
{
    // make sure that synch claim and release are symmetric
    GP_ASSERT_DEV_EXT(synchClaim == true);
    GP_ASSERT_DEV_EXT(radioMgmt_IsRadioClaimed());
    synchClaim = false;
    GP_LOG_PRINTF("[RADIOMGMT]:: blocking claim -> 0", 0);

    GP_WB_WRITE_RADIO_ARB_MGMT_CLAIM(0);

    radioMgmt_managePendingClaims();
}
