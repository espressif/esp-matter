/*
 * Copyright (c) 2010-2016, GreenPeak Technologies
 * Copyright (c) 2017-2021, Qorvo Inc
 *
 * gpBaseComps_StackInit.c
 *
 * The file contains the function that initializes all the base components that are used.
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
 * Alternatively, this software may be distributed under the terms of the
 * modified BSD License or the 3-clause BSD License as published by the Free
 * Software Foundation @ https://directory.fsf.org/wiki/License:BSD-3-Clause
 *
 * $Header$
 * $Change$
 * $DateTime$
 *
 */


#define GP_COMPONENT_ID GP_COMPONENT_ID_BASECOMPS

/*****************************************************************************
 *                    Includes Definitions
 *****************************************************************************/

#include "global.h"
#include "gpBaseComps.h"

#include "hal.h"

#if defined(GP_DIVERSITY_JUMPTABLES) && !defined(GP_DIVERSITY_KEEP_NRT_IN_FLASH) 
#include "gpJumpTables.h"

//Adding symbol pointing to Jump table to avoid optimizing out during linking
#include "gpJumpTables_DataTable.h"
const void* gpBaseComps_ForceDataJumpTableInclude = &JumpTables_DataTable;
#endif

#ifdef GP_DIVERSITY_ROMUSAGE_FOR_MATTER
#include "gpJumpTablesMatter.h"
#endif //GP_DIVERSITY_ROMUSAGE_FOR_MATTER

#ifdef GP_COMP_SCHED
#include "gpSched.h"
#endif //GP_COMP_SCHED


#ifdef GP_COMP_RESET
#include "gpReset.h"
#endif //GP_COMP_RESET

#ifdef GP_COMP_UNIT_TEST
#include "gpUnitTest.h"
#endif //GP_COMP_UNIT_TEST


#ifdef GP_COMP_COM
#include "gpCom.h"
#endif //GP_COMP_COM

#ifdef GP_COMP_LOG
#include "gpLog.h"
#endif //GP_COMP_LOG



#ifdef GP_COMP_GPHAL
#include "gpHal.h"
#endif //GP_COMP_GPHAL

#ifdef GP_COMP_RADIO
#include "gpRadio.h"
#endif //GP_COMP_RADIO

#ifdef GP_COMP_MACCORE
#include "gpMacCore.h"
#endif //GP_COMP_MACCORE
#ifdef GP_COMP_MACDISPATCHER
#include "gpMacDispatcher.h"
#endif // GP_COMP_MACDISPATCHER

#ifdef GP_COMP_RANDOM
#include "gpRandom.h"
#endif //GP_COMP_RANDOM
#ifdef GP_COMP_ENCRYPTION
#include "gpEncryption.h"
#endif //GP_COMP_ENCRYPTION


#ifdef GP_COMP_POOLMEM
#include "gpPoolMem.h"
#endif //GP_COMP_POOLMEM

#ifdef GP_COMP_PD
#include "gpPd.h"
#endif //GP_COMP_PD
#ifdef GP_COMP_PAD
#include "gpPad.h"
#endif //GP_COMP_PAD

#ifdef GP_COMP_NVM
#include "gpNvm.h"
#endif //GP_COMP_NVM

#ifdef GP_COMP_BSPRECFG
#include "gpBspReCfg.h"
#endif //GP_COMP_BSPRECFG

#ifdef GP_COMP_KEYSCAN
#include "gpKeyScan.h"
#endif //GP_COMP_KEYSCAN



#ifdef GP_COMP_TXMONITOR
#include "gpTxMonitor.h"
#endif //GP_COMP_TXMONITOR

#ifdef GP_COMP_RANDOM
#include "gpRandom.h"
#endif /* GP_COMP_RANDOM */

#ifdef GP_COMP_VERSION
#include "gpVersion.h"
#endif //GP_COMP_VERSION


#include "gpAssert.h"
#ifdef GP_COMP_RXARBITER
#include "gpRxArbiter.h"
#endif //GP_COMP_RXARBITER

#ifdef GP_COMP_RFCHANNEL
#include "gpRFChannel.h"
#endif //GP_COMP_RFCHANNEL



#ifdef GP_COMP_SILEXCRYPTOSOC
#include "silexCryptoSoc.h"
#endif

#ifdef GP_COMP_TLS
#include "gpTls.h"
#endif
/*****************************************************************************
 *                    Type Definitions
 *****************************************************************************/

/*****************************************************************************
 *                    Static Data Definitions
 *****************************************************************************/

/*****************************************************************************
 *                    Static Function Definitions
 *****************************************************************************/

/*****************************************************************************
 *                    Public Function Definitions
 *****************************************************************************/

void gpBaseComps_StackInit(void)
{
//--------------------------
//Initialize logging components
//--------------------------



#if defined(GP_COMP_COM) && !defined(TBC_GPCOM)
#ifndef GP_BASECOMPS_DIVERSITY_NO_GPCOM_INIT
    gpCom_Init();
#endif //GP_BASECOMPS_DIVERSITY_NO_GPCOM_INIT
#endif

#ifdef GP_DIVERSITY_LOG
#ifndef GP_BASECOMPS_DIVERSITY_NO_GPLOG_INIT
    gpLog_Init();
#endif //GP_BASECOMPS_DIVERSITY_NO_GPLOG_INIT
#endif //GP_DIVERSITY_LOG

//--------------------------
//Init low level components
//--------------------------
#if !defined(GP_DIVERSITY_LINUXKERNEL) 
#endif

#ifdef GP_COMP_GPHAL
    gpHal_Init(false);
    gpHal_EnableInterrupts(true);
#endif //GP_COMP_GPHAL

#ifdef GP_COMP_RADIO
    gpRadio_Init();
#endif //GP_COMP_RADIO


#if defined(GP_DIVERSITY_JUMPTABLES) && !defined(GP_DIVERSITY_KEEP_NRT_IN_FLASH) 
    GP_LOG_SYSTEM_PRINTF("NRT ROM v%d",0,gpJumpTables_GetRomVersion());
    /* Make sure the application has been built with the right minimal ROM version */
    GP_ASSERT_SYSTEM(gpJumpTables_GetRomVersion() >= GPJUMPTABLES_MIN_ROMVERSION);
#endif // defined(GP_DIVERSITY_JUMPTABLES) && !defined(GP_DIVERSITY_KEEP_NRT_IN_FLASH) && !defined(GP_DIVERSITY_ROM_BUILD)

#ifdef GP_DIVERSITY_ROMUSAGE_FOR_MATTER
    gpJumpTablesMatter_Init();
#endif //GP_DIVERSITY_ROMUSAGE_FOR_MATTER

#ifdef GP_COMP_SCHED
#if !defined(GP_BASECOMPS_DIVERSITY_NO_GPSCHED_INIT) && !defined(GP_DIVERSITY_FREERTOS)
    gpSched_Init();
#endif
#if defined(GP_DIVERSITY_GPHAL_INTERN) &&  defined(GP_DIVERSITY_GPHAL_K8E)
#endif //defined(GP_DIVERSITY_GPHAL_INTERN) && (defined(GP_DIVERSITY_GPHAL_K8C) || defined(GP_DIVERSITY_GPHAL_K8D)) || defined(GP_DIVERSITY_GPHAL_K8E))
#endif //GP_COMP_SCHED



#ifdef GP_COMP_POOLMEM
    gpPoolMem_Init();
#endif //GP_COMP_POOLMEM

#ifdef GP_COMP_VERSION
    gpVersion_Init();
#endif //GP_COMP_VERSION

#if defined(GP_DIVERSITY_LINUXKERNEL) 
#endif




#ifdef GP_COMP_SCHED
    gpSched_StartTimeBase();
#endif //GP_COMP_SCHED

#if defined(GP_COMP_SCHED) && !defined(GP_SCHED_FREE_CPU_TIME)
    hal_SleepSetGotoSleepEnable(false);
#endif

#ifdef GP_COMP_UNIT_TEST
    gpUnitTest_Init();
#endif //GP_COMP_UNIT_TEST


#ifdef GP_COMP_TXMONITOR
    gpTxMonitor_Init(GP_BSP_TXMONITOR_SETTLING_TIME_IN_SYMBOLS());
#endif //GP_COMP_TXMONITOR

//--------------------------
// Init low level components with possible external communication
//--------------------------

#ifdef GP_COMP_NVM
    gpNvm_Init();
#endif //GP_COMP_NVM

#ifdef GP_COMP_RESET
    gpReset_Init();
#endif //GP_COMP_RESET

#ifdef GP_COMP_RXARBITER
    gpRxArbiter_Init();
#endif //GP_COMP_RXARBITER

#ifdef GP_COMP_PD
    gpPd_Init();
#endif //GP_COMP_PD
#ifdef GP_COMP_PAD
    gpPad_Init();
#endif //GP_COMP_PAD

#ifdef GP_COMP_GPHAL
    gpHal_AdvancedInit();
#endif //GP_COMP_GPHAL

#ifdef GP_COMP_SILEXCRYPTOSOC
    silexCryptoSoc_InitFuncptr();
#endif

#ifdef GP_COMP_TLS
    gpTls_Init();
#endif
//--------------------------
//Init higher level components
//--------------------------
#ifdef GP_COMP_RANDOM
    gpRandom_Init();
#endif //GP_COMP_RANDOM
#ifdef GP_COMP_ENCRYPTION
    gpEncryption_Init();
#endif //GP_COMP_ENCRYPTION



    gpBaseComps_MacInit();

#ifdef GP_COMP_BSPRECFG
    gpBspReCfg_Init();
#endif //GP_COMP_STAT

#ifdef GP_COMP_KEYSCAN
    gpKeyScan_Init();
#endif //GP_COMP_KEYSCAN



#ifdef GP_COMP_RFCHANNEL
    gpRFChannel_Init();
#endif //GP_COMP_RFCHANNEL

}

void gpBaseComps_MacInit(void)
{
#ifdef GP_COMP_MACCORE
    gpMacCore_Init();
#endif //GP_COMP_MACCORE
#ifdef GP_COMP_MACDISPATCHER
    gpMacDispatcher_Init();
#endif //GP_COMP_MACDISPATCHER
}

#ifdef GP_DIVERSITY_LINUXKERNEL
int gpBaseComps_StackInitKernel(void)
{
    int returnValue = 0;
#if defined(GP_COMP_COM)
    returnValue = gpCom_InitKernel();
    if (returnValue)
    {
        return returnValue;
    }
#endif

#if defined(GP_COMP_SCHED)
    returnValue = gpSched_InitKernel();
    if (returnValue)
    {
        goto BaseComps_StackInitKernel_Abort_Sched;
    }
#endif

    if (returnValue)
    {
#if defined(GP_COMP_SCHED)
BaseComps_StackInitKernel_Abort_Sched:
#endif
#if defined(GP_COMP_COM)
        gpCom_DeInitKernel();
#endif
        /* solitary ; to avoid empty if() body */ ;
    }
    return returnValue;
}
#endif // ifdef GP_DIVERSITY_LINUXKERNEL
