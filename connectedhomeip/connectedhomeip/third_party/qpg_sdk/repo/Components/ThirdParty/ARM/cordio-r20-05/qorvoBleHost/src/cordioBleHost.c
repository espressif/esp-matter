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

/*****************************************************************************
 *                    Includes Definitions
 *****************************************************************************/
#define GP_COMPONENT_ID GP_COMPONENT_ID_QORVOBLEHOST

/*****************************************************************************
 *                   Includes
 *****************************************************************************/
#include "cordioBleHost.h"
#include "gpLog.h"
#include "gpSched.h"

#include "wsf_types.h"
#include "wsf_os.h"
#include "wsf_buf.h"
#include "wsf_timer.h"
#include "wsf_msg.h"

/* for Initialization of cordio stack components */
#include "dm_api.h"
#include "dm_handler.h"
#include "dm_adv.h"

#include "l2c_handler.h"
#include "l2c_api.h"

#ifndef CORDIO_BLE_HOST_EXCLUDE_SMPR
#include "smp_handler.h"
#include "smp_api.h"
#endif //CORDIO_BLE_HOST_EXCLUDE_SMPR

#include "att_handler.h"
#include "att_api.h"
#ifndef CORDIO_BLE_HOST_EXCLUDE_CORDIOAPPFW
#include "app_api.h"
#endif //CORDIO_BLE_HOST_EXCLUDE_CORDIOAPPFW

/* For registration of HCI handler */
#include "hci_handler.h"

/* For CPU processing monitoring */
#include "gpUtils.h"

/*****************************************************************************
 *                   Macro's
 *****************************************************************************/
/* Number of WSF buffer pools */
#ifndef CORDIO_BLE_HOST_WSF_BUF_POOLS
#define CORDIO_BLE_HOST_WSF_BUF_POOLS 5
#endif //CORDIO_BLE_HOST_WSF_BUF_POOLS

#define CORDIO_BLE_HOST_TICK_TIME_MS 1000

#ifndef CORDIO_BLE_HOST_BUFPOOLS_1_CHUNK
#define CORDIO_BLE_HOST_BUFPOOLS_1_CHUNK 16
#endif //CORDIO_BLE_HOST_BUFPOOLS_1_SIZE
#ifndef CORDIO_BLE_HOST_BUFPOOLS_1_AMOUNT
#define CORDIO_BLE_HOST_BUFPOOLS_1_AMOUNT 6
#endif //CORDIO_BLE_HOST_BUFPOOLS_1_AMOUNT

#ifndef CORDIO_BLE_HOST_BUFPOOLS_2_CHUNK
#define CORDIO_BLE_HOST_BUFPOOLS_2_CHUNK 32
#endif //CORDIO_BLE_HOST_BUFPOOLS_2_SIZE
#ifndef CORDIO_BLE_HOST_BUFPOOLS_2_AMOUNT
#define CORDIO_BLE_HOST_BUFPOOLS_2_AMOUNT 8
#endif //CORDIO_BLE_HOST_BUFPOOLS_2_AMOUNT

#ifndef CORDIO_BLE_HOST_BUFPOOLS_3_CHUNK
#define CORDIO_BLE_HOST_BUFPOOLS_3_CHUNK 64
#endif //CORDIO_BLE_HOST_BUFPOOLS_3_SIZE
#ifndef CORDIO_BLE_HOST_BUFPOOLS_3_AMOUNT
#define CORDIO_BLE_HOST_BUFPOOLS_3_AMOUNT 10
#endif //CORDIO_BLE_HOST_BUFPOOLS_3_AMOUNT

#ifndef CORDIO_BLE_HOST_BUFPOOLS_4_CHUNK
#define CORDIO_BLE_HOST_BUFPOOLS_4_CHUNK 128
#endif //CORDIO_BLE_HOST_BUFPOOLS_4_SIZE
#ifndef CORDIO_BLE_HOST_BUFPOOLS_4_AMOUNT
#define CORDIO_BLE_HOST_BUFPOOLS_4_AMOUNT 6
#endif //CORDIO_BLE_HOST_BUFPOOLS_4_AMOUNT

#ifndef CORDIO_BLE_HOST_BUFPOOLS_5_CHUNK
#define CORDIO_BLE_HOST_BUFPOOLS_5_CHUNK 264
#endif //CORDIO_BLE_HOST_BUFPOOLS_5_SIZE
#ifndef CORDIO_BLE_HOST_BUFPOOLS_5_AMOUNT
#if defined(GP_ECC_DIVERSITY_USE_SLICING)
/* When ECC slicing is enabled, we need more temporary buffer space */
#define CORDIO_BLE_HOST_BUFPOOLS_5_AMOUNT 4
#else
#define CORDIO_BLE_HOST_BUFPOOLS_5_AMOUNT 2
#endif
#endif //CORDIO_BLE_HOST_BUFPOOLS_5_AMOUNT


#ifndef CORDIO_BLE_HOST_ATT_MAX_MTU
#define CORDIO_BLE_HOST_ATT_MAX_MTU ATT_DEFAULT_MTU
#endif /* CORDIO_BLE_HOST_ATT_MAX_MTU */

#if DM_CONN_MAX != GP_DIVERSITY_BLE_MAX_NR_OF_SUPPORTED_CONNECTIONS
#error DM_CONN_MAX and GP_DIVERSITY_BLE_MAX_NR_OF_SUPPORTED_CONNECTIONS are not the same
#endif

#define MS_PER_SEC 1000
#define US_PER_MS  1000

/*****************************************************************************
 *                    Static Data Definitions
 *****************************************************************************/

/*! Free memory for pool buffers. */
/*! Minimum size: SUM(mainPoolDesc_size[n]*mainPoolDesc_num[n]) + sizeof(wsfBufPool_t) * CORDIO_BLE_HOST_WSF_BUF_POOLS */

#if(CORDIO_BLE_HOST_WSF_BUF_POOLS >= 1)
#define WSF_POOL1 CORDIO_BLE_HOST_BUFPOOLS_1_CHUNK* CORDIO_BLE_HOST_BUFPOOLS_1_AMOUNT
#else
#define WSF_POOL1 0
#endif
#if(CORDIO_BLE_HOST_WSF_BUF_POOLS >= 2)
#define WSF_POOL2 CORDIO_BLE_HOST_BUFPOOLS_2_CHUNK* CORDIO_BLE_HOST_BUFPOOLS_2_AMOUNT
#else
#define WSF_POOL2 0
#endif
#if(CORDIO_BLE_HOST_WSF_BUF_POOLS >= 3)
#define WSF_POOL3 CORDIO_BLE_HOST_BUFPOOLS_3_CHUNK* CORDIO_BLE_HOST_BUFPOOLS_3_AMOUNT
#else
#define WSF_POOL3 0
#endif
#if(CORDIO_BLE_HOST_WSF_BUF_POOLS >= 4)
#define WSF_POOL4 CORDIO_BLE_HOST_BUFPOOLS_4_CHUNK* CORDIO_BLE_HOST_BUFPOOLS_4_AMOUNT
#else
#define WSF_POOL4 0
#endif
#if(CORDIO_BLE_HOST_WSF_BUF_POOLS >= 5)
#define WSF_POOL5 CORDIO_BLE_HOST_BUFPOOLS_5_CHUNK* CORDIO_BLE_HOST_BUFPOOLS_5_AMOUNT
#else
#define WSF_POOL5 0
#endif

/* Error if more as 5 pools are defined. */
#if(CORDIO_BLE_HOST_WSF_BUF_POOLS > 5)
#error "Max value of CORDIO_BLE_HOST_WSF_BUF_POOLS = 5"
#endif

/* sizeof(wsfBufPool_t) = 16 */
#define WSF_POOL_MEM (WSF_POOL1 + WSF_POOL2 + WSF_POOL3 + WSF_POOL4 + WSF_POOL5 + 600 /*16 * CORDIO_BLE_HOST_WSF_BUF_POOLS*/)

#if defined(__GNUC__)
static UInt8 mainBufMem[WSF_POOL_MEM] __attribute__((aligned(4))) GP_EXTRAM_SECTION_ATTR;
#elif defined(__IAR_SYSTEMS_ICC__)
#pragma data_alignment = 4
static UInt8 mainBufMem[WSF_POOL_MEM];
#endif /* __IAR_SYSTEMS_ICC__ */

/*! Default pool descriptor. */
static wsfBufPoolDesc_t mainPoolDesc[CORDIO_BLE_HOST_WSF_BUF_POOLS] =
    {
#if(CORDIO_BLE_HOST_WSF_BUF_POOLS >= 1)
        {CORDIO_BLE_HOST_BUFPOOLS_1_CHUNK, CORDIO_BLE_HOST_BUFPOOLS_1_AMOUNT}
#endif
#if(CORDIO_BLE_HOST_WSF_BUF_POOLS >= 2)
        ,
        {CORDIO_BLE_HOST_BUFPOOLS_2_CHUNK, CORDIO_BLE_HOST_BUFPOOLS_2_AMOUNT}
#endif
#if(CORDIO_BLE_HOST_WSF_BUF_POOLS >= 3)
        ,
        {CORDIO_BLE_HOST_BUFPOOLS_3_CHUNK, CORDIO_BLE_HOST_BUFPOOLS_3_AMOUNT}
#endif
#if(CORDIO_BLE_HOST_WSF_BUF_POOLS >= 4)
        ,
        {CORDIO_BLE_HOST_BUFPOOLS_4_CHUNK, CORDIO_BLE_HOST_BUFPOOLS_4_AMOUNT}
#endif
#if(CORDIO_BLE_HOST_WSF_BUF_POOLS >= 5)
        ,
        {CORDIO_BLE_HOST_BUFPOOLS_5_CHUNK, CORDIO_BLE_HOST_BUFPOOLS_5_AMOUNT}
#endif
};


static UInt32 cordioBleHost_tickTime = 0;
static Bool cordioBleHost_isScheduledFromMain = false;

/*****************************************************************************
 *                    Static Function Prototypes
 *****************************************************************************/

static void cordioBleHost_PostProcessing(void*);
static void cordioBleHost_StackInit(void);

/*****************************************************************************
 *                    Static Function Definitions
 *****************************************************************************/

#if WSF_BUF_STATS == TRUE
static void BleHost_PrintWsfPool(void)
{
    WsfBufPoolStat_t wsfBufPoolStat;
    UIntLoop i;

    GP_LOG_SYSTEM_PRINTF("WSF pool  len/avg/max  num/cur/max/total  ovfl", 0);
    for(i = 0; i < CORDIO_BLE_HOST_WSF_BUF_POOLS; i++)
    {
        WsfBufGetPoolStats(&wsfBufPoolStat, i);
        GP_LOG_SYSTEM_PRINTF("WSF  %3d  %3d %3ld %3d  %3d %3d %3d %5ld   %3d", 0, i,
                             wsfBufPoolStat.bufSize,
                             wsfBufPoolStat.cumulativeLen / wsfBufPoolStat.cumulativeCount,
                             wsfBufPoolStat.maxReqLen,
                             wsfBufPoolStat.numBuf,
                             wsfBufPoolStat.numAlloc,
                             wsfBufPoolStat.maxAlloc,
                             wsfBufPoolStat.cumulativeCount,
                             wsfBufPoolStat.overflows);
        gpLog_Flush();
    }
}
#endif // WSF_BUF_STATS == TRUE

static void BleHost_BufDiagnostics(WsfBufDiag_t* pInfo)
{
    if(pInfo->type == WSF_BUF_ALLOC_FAILED)
    {
        GP_LOG_SYSTEM_PRINTF("BLE Host got WSF Buffer Allocation Failure - Task: %d Len: %d", 0,
                             pInfo->param.alloc.taskId, pInfo->param.alloc.len);
#if WSF_BUF_STATS == TRUE
        BleHost_PrintWsfPool();
#endif
    }
}


void cordioBleHost_PostProcessing(void* arg)
{
    NOT_USED(arg);
    UInt32 tmpDiff_us;
    UInt32 nowTime;

    GP_UTILS_CPUMON_HOSTPROCESSING_START();
    nowTime = gpSched_GetCurrentTime();
    tmpDiff_us = nowTime - cordioBleHost_tickTime;
    cordioBleHost_tickTime = nowTime;

    WsfTimerUpdate((tmpDiff_us / US_PER_MS) / WSF_MS_PER_TICK);

    /* run tasks */
    WsfOsDispatcher();

    /* Unschedule possible delayed scheduled event */
    gpSched_UnscheduleEventArg(cordioBleHost_PostProcessing, NULL);

    if(WsfOsReadyToSleep())
    {
        bool_t timerRunning = false;
        wsfTimerTicks_t nextExpirationTime = WsfTimerNextExpiration(&timerRunning);

        if(timerRunning)
        {
            if(nextExpirationTime < ((wsfTimerTicks_t)-1) / WSF_MS_PER_TICK / US_PER_MS)
            {
                gpSched_ScheduleEventArg(US_PER_MS * WSF_MS_PER_TICK * nextExpirationTime, cordioBleHost_PostProcessing, NULL);
            }
            else
            {
                gpSched_ScheduleEventInSecAndUs(nextExpirationTime * WSF_MS_PER_TICK / MS_PER_SEC, (nextExpirationTime * WSF_MS_PER_TICK) % MS_PER_SEC, cordioBleHost_PostProcessing, NULL);
            }
        }

        /* will be scheduled again by cordioBleHost_OnIdle if needed */
        cordioBleHost_isScheduledFromMain = false;
    }
    else
    {
        /* Makes sure we don't go to sleep before cordioBleHost_OnIdle is run again */
        gpSched_ScheduleEventArg(0, cordioBleHost_PostProcessing, NULL);
        cordioBleHost_isScheduledFromMain = true;
    }
    GP_UTILS_CPUMON_HOSTPROCESSING_DONE();
}

/*****************************************************************************
 *                    Public Function Definitions
 *****************************************************************************/

void cordioBleHost_Init(void)
{
    wsfHandlerId_t handlerId;

    cordioBleHost_isScheduledFromMain = false;

    gpSched_RegisterOnIdleCallback(cordioBleHost_OnIdle);


    WsfOsInit();

    /* Initialize HCI handler */
    handlerId = WsfOsSetNextHandler(HciHandler);

    HciHandlerInit(handlerId);

    /* init OS subsystems */
    WsfTimerInit();
    cordioBleHost_tickTime = gpSched_GetCurrentTime();

    WsfBufInit(sizeof(mainBufMem), mainBufMem, CORDIO_BLE_HOST_WSF_BUF_POOLS, mainPoolDesc);
    WsfBufDiagRegister(BleHost_BufDiagnostics);

    /* Initialize cordio stack components for both internal and external host */
    cordioBleHost_StackInit();

    GP_LOG_PRINTF("poolMem claimed = %d bytes", 0, WSF_POOL_MEM);
}

static void cordioBleHost_CommonInit(void)
{
    /* Initialize Generic Security Components */
    SecInit();
    SecAesInit();
    SecCmacInit();
    SecEccInit();
#ifndef GP_COMP_BLEMESH
    /* Initialize Generic Dm Components */
    DmConnInit();
    DmSecInit();
    DmSecLescInit();
    DmPrivInit();
    DmDevPrivInit();
    DmPhyInit();
    /* Initialize L2cap */
    L2cInit();
#endif //GP_COMP_BLEMESH

#ifdef GP_DIVERSITY_DIRECTIONFINDING_SUPPORTED
    DmConnCteInit();
#endif
}

static void cordioBleHost_Common_HandlerInit(void)
{
    wsfHandlerId_t handlerId;

    handlerId = WsfOsSetNextHandler(DmHandler);
    DmHandlerInit(handlerId);

#ifndef GP_COMP_BLEMESH
#ifndef CORDIO_BLE_HOST_EXCLUDE_SMPR
    handlerId = WsfOsSetNextHandler(SmpHandler);
    SmpHandlerInit(handlerId);
#endif //CORDIO_BLE_HOST_EXCLUDE_SMPR

    handlerId = WsfOsSetNextHandler(AttHandler);
    AttHandlerInit(handlerId);

#ifndef CORDIO_BLE_HOST_EXCLUDE_CORDIOAPPFW
    handlerId = WsfOsSetNextHandler(AppHandler);
    AppHandlerInit(handlerId);
#endif //CORDIO_BLE_HOST_EXCLUDE_CORDIOAPPFW
#endif //GP_COMP_BLEMESH
}

#ifdef GP_DIVERSITY_BLE_MASTER
static void cordioBleHost_MasterInit(void)
{
#ifndef GP_COMP_BLEMESH
    /* Initialize Dm (ext) as a Master */
    DmConnMasterInit();

    /* Initialize L2cap for LE master operation */
    L2cMasterInit();

    /* Initialize ATT client, normally used by Master */
    AttcInit();

    /* Initialize legacy and secure SMP initiator */
    SmpiInit();
    SmpiScInit();
#endif //GP_COMP_BLEMESH
#ifndef CORDIO_BLE_HOST_EXCLUDE_CORDIOAPPFW
    /* Initialize application framework as Master*/
    AppMasterInit();
    AppDiscInit();
#endif //CORDIO_BLE_HOST_EXCLUDE_CORDIOAPPFW
}
#endif //GP_DIVERSITY_BLE_MASTER

#ifdef GP_DIVERSITY_BLE_SLAVE
static void cordioBleHost_SlaveInit(void)
{
#ifndef GP_COMP_BLEMESH
    /* Initialize L2cap for LE slave operation */
    L2cSlaveInit();

#ifndef CORDIO_BLE_HOST_EXCLUDE_SMPR
    /* Initialize legacy and secure SMP responder */
    SmprInit();
    SmprScInit();
#endif //CORDIO_BLE_HOST_EXCLUDE_SMPR

    /* Initialize legacy Dm (ext) slave and private advertising */
    DmConnSlaveInit();
#endif //GP_COMP_BLEMESH

#ifndef GP_COMP_BLEMESH
    /* Initialize ATT server and Indication, normally used by Slave*/
    AttsInit();
    AttsIndInit();
#endif //GP_COMP_BLEMESH

#ifndef CORDIO_BLE_HOST_EXCLUDE_CORDIOAPPFW
    /* Initialize App Framework for slave */
    AppSlaveInit();
#endif //CORDIO_BLE_HOST_EXCLUDE_CORDIOAPPFW
}

static void cordioBleHost_Slave_HandlerInit(void)
{
    wsfHandlerId_t handlerId;

    handlerId = WsfOsSetNextHandler(L2cSlaveHandler);
    L2cSlaveHandlerInit(handlerId);
}
#endif //GP_DIVERSITY_BLE_SLAVE

static void cordioBleHost_StackInit(void)
{
    cordioBleHost_CommonInit();
    cordioBleHost_Common_HandlerInit();

#ifdef GP_DIVERSITY_BLE_SCANNER
    /* Initialize Dm Legacy scanning */
    DmScanInit();
#endif // GP_DIVERSITY_BLE_SCANNER

#ifdef GP_DIVERSITY_BLE_MASTER
    cordioBleHost_MasterInit();
#endif

#ifdef GP_DIVERSITY_BLE_SLAVE
    cordioBleHost_SlaveInit();
    cordioBleHost_Slave_HandlerInit();
#endif

#ifdef GP_DIVERSITY_BLE_ADVERTISER
    DmAdvInit();
#endif // GP_DIVERSITY_BLE_ADVERTISER
}

void cordioBleHost_OnIdle(void)
{
    if(!cordioBleHost_isScheduledFromMain &&
       !WsfOsReadyToSleep())
    {
        gpSched_ScheduleEventArg(0, cordioBleHost_PostProcessing, NULL);
        cordioBleHost_isScheduledFromMain = true;
    }
}

void cordioBleHost_PrintEventString(UInt8 msg_num)
{
}
