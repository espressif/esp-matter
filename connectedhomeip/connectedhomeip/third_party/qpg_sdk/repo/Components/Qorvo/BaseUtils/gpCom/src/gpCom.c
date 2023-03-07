/*
 * Copyright (c) 2008-2016, GreenPeak Technologies
 * Copyright (c) 2017-2019, Qorvo Inc
 *
 * gpCom.c
 *
 * This file contains the implementation of the serial communication module.
 * It implements the GreenPeak serial protocol.
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

/*****************************************************************************
 *                    Includes Definitions
 *****************************************************************************/

#define GP_COMPONENT_ID GP_COMPONENT_ID_COM

#include "gpCom.h"
#include "gpCom_defs.h"

#include "gpLog.h"
#include "gpAssert.h"

#ifdef GP_COMP_SCHED
#include "gpSched.h"
#endif //def GP_COMP_SCHED


#ifdef GP_COMP_SCHED
#endif /*GP_COMP_SCHED*/

/*****************************************************************************
 *                    Macro Definitions
 *****************************************************************************/

/*****************************************************************************
 *                    Type Definitions
 *****************************************************************************/

/*****************************************************************************
 *                    Static Data Definitions
 *****************************************************************************/

Bool         gpCom_Initialized = false;

/*****************************************************************************
 *                    External Data Definition
 *****************************************************************************/
#ifdef GP_COM_DIVERSITY_ACTIVATE_TX_CALLBACK
static gpCom_cbActivateTx_t Com_ActivateTxCb[GP_COM_NR_OF_ACTIVATETXCB] = {NULL};
UInt8 nbrOfgpCom_ActivateTxCbs = 0;
#endif

#if defined(GP_COM_DIVERSITY_UNLOCK_TX_AFTER_RX)
Bool gpCom_TxLocked;
#endif

/*****************************************************************************
 *                    Static Function Prototypes
 *****************************************************************************/

/*****************************************************************************
 *                    Static Function Definitions
 *****************************************************************************/

/*****************************************************************************
 *                    Public Function Definitions
 *****************************************************************************/

void gpCom_Init(void)
{

    Com_InitRx();

#if defined(GP_DIVERSITY_COM_UART) || defined(GP_COM_DIVERSITY_SERIAL_SPI)
    gpComSerial_Init();
#endif //GP_DIVERSITY_COM_UART

#ifdef GP_COM_DIVERSITY_ACTIVATE_TX_CALLBACK
    MEMSET(Com_ActivateTxCb ,0,sizeof(Com_ActivateTxCb ));
    nbrOfgpCom_ActivateTxCbs  = 0;
#endif


}

Bool gpCom_DataRequest(UInt8 moduleID, UInt16 length, UInt8* pData, gpCom_CommunicationId_t commId)
{
    Bool ret = false;
#if defined(GP_DIVERSITY_LINUXKERNEL)
    //GP_LOG_SYSTEM_PRINTF("TX m=%02x l=%d d=%02x...",0,moduleID, length, pData[0]);
#endif
#if defined(GP_DIVERSITY_COM_UART) || defined(GP_COM_DIVERSITY_SERIAL_SPI)
    if(GP_COMM_ID_CARRIED_BY(commId,GP_COM_COMM_ID_UART1)
    || GP_COMM_ID_CARRIED_BY(commId,GP_COM_COMM_ID_UART2)
    || GP_COMM_ID_CARRIED_BY(commId,GP_COM_COMM_ID_STMUSB)
    || GP_COMM_ID_CARRIED_BY(commId,GP_COM_COMM_ID_SPI)
    || GP_COMM_ID_CARRIED_BY(commId,GP_COM_COMM_ID_USB))
    {
        ret = gpComSerial_DataRequest(moduleID, length, pData, commId);
    }
    else
#endif
    {
        ret = false;
    }

#if defined(GP_DIVERSITY_LINUXKERNEL) || defined(GP_COMPONENT_ID_HCI) || defined(GP_COMPONENT_ID_TESTIF) || defined(GP_COMPONENT_ID_LOG)
    if (ret == false
#if defined(GP_COMPONENT_ID_HCI) 
        && moduleID != GP_COMPONENT_ID_HCI
#endif
#if defined(GP_COMPONENT_ID_TESTIF) 
        && moduleID != GP_COMPONENT_ID_TESTIF
#endif
#if defined(GP_COMPONENT_ID_LOG) 
        && moduleID != GP_COMPONENT_ID_LOG
#endif
#if defined(GP_COMPONENT_ID_QVOT) 
        && (moduleID != GP_COMPONENT_ID_QVOT)
#endif
        && !(moduleID == GP_COMPONENT_ID_COM && pData[0] == 0xfe && pData[2] == 0x08 /*gpComLinkEvent_Ping_CmdId*/ )
    )
    {
        GP_ASSERT_SYSTEM(ret);
    }
#endif
    return ret;

}

UInt16 gpCom_GetFreeBufferSpace(UInt8 moduleID, gpCom_CommunicationId_t commId)
{
    NOT_USED(moduleID);
#if ! defined(GP_DIVERSITY_COM_UART) 
    NOT_USED(commId);
#endif //not defined GP_DIVERSITY_COM_UART
#if defined(GP_DIVERSITY_COM_UART) || defined(GP_COM_DIVERSITY_SERIAL_SPI)
    if((commId == GP_COM_COMM_ID_UART1) || (commId == GP_COM_COMM_ID_UART2) || (commId == GP_COM_COMM_ID_STMUSB)||(commId==GP_COM_COMM_ID_SPI))
    {
        return gpComSerial_GetFreeSpace(commId);
    }
    else
#endif
    {
        return 0;
    }
}

//Redirection functions
Bool gpCom_GetTXEnable(void)
{
    Bool enabled = false;

#if defined(GP_DIVERSITY_COM_UART) || defined(GP_COM_DIVERSITY_SERIAL_SPI)
    enabled |= gpComSerial_GetTXEnable();
#endif

    return enabled;
}

Bool gpCom_TXDataPending(void)
{

    Bool pending = false;

#if defined(GP_DIVERSITY_COM_UART) || defined(GP_COM_DIVERSITY_SERIAL_SPI)
    pending |= gpComSerial_TXDataPending();
#endif

    return pending;
}


void gpCom_Flush(void)
{

#if defined(GP_DIVERSITY_COM_UART) || defined(GP_COM_DIVERSITY_SERIAL_SPI)
    gpComSerial_Flush();
#endif
}

void gpCom_DeInit(void)
{
    Com_DeInitRx();

#if defined(GP_DIVERSITY_COM_UART) || defined(GP_COM_DIVERSITY_SERIAL_SPI)
    gpComSerial_DeInit();
#endif
}

void gpCom_HandleTx(void)
{
#if defined(GP_DIVERSITY_COM_UART) || defined(GP_COM_DIVERSITY_SERIAL_SPI)
    gpComSerial_HandleTx();
#endif
}

#ifdef GP_COMP_SCHED
Bool gpCom_DataRequestAndWaitForAck(UInt8 moduleID, UInt16 length, UInt8* pData, gpCom_CommunicationId_t commId, Bool *reqAcked, UInt32 timeout, UInt8 ackId)
{
    Bool ret;
    Bool timedOut = false;

#ifdef GP_DIVERSITY_LINUXKERNEL
    gpSched_PauseMainThread();
#endif //def GP_DIVERSITY_LINUXKERNEL
    *reqAcked = false;
    ret = gpCom_DataRequest(moduleID, length, pData, commId);

    //Just wait for ACK if data request was a success
    if(ret)
    {
        UInt32 endPollTime = timeout + gpSched_GetCurrentTime();

        while(!timedOut)
        {
            gpCom_HandleTx();
            gpCom_HandleRxBlocking(true, ackId);
            if(!(*reqAcked))
            {
                if(!gpSched_TimeCompareLower(gpSched_GetCurrentTime(), endPollTime))
                {
                    timedOut = true;
                }
            }
            else
            {
                break;
            }
        }
    }
#ifdef GP_DIVERSITY_LINUXKERNEL
    gpSched_ResumeMainThread();
#endif //def GP_DIVERSITY_LINUXKERNEL

    if(timedOut)
    {
        GP_LOG_SYSTEM_PRINTF("WFack fail m:%x l:%u [%x Id:%lx",0, moduleID, length, pData[0], (long unsigned int)commId);
        GP_ASSERT_SYSTEM(false);
    }

    return ret;
}
#endif /*GP_COMP_SCHED*/

#ifdef GP_COM_DIVERSITY_ACTIVATE_TX_CALLBACK
Bool gpCom_RegisterActivateTxCb(UInt8 moduleID, gpCom_cbActivateTx_t cb)
{
    NOT_USED(moduleID);
    GP_ASSERT_SYSTEM(nbrOfgpCom_ActivateTxCbs < GP_COM_NR_OF_ACTIVATETXCB );
    Com_ActivateTxCb[nbrOfgpCom_ActivateTxCbs] = cb;
    nbrOfgpCom_ActivateTxCbs++;
    return true;
}
#endif

#ifdef GP_COM_DIVERSITY_ACTIVATE_TX_CALLBACK
UInt16 Com_Call_ActivateTxCb(UInt16 overFlowCounter, gpCom_CommunicationId_t commId)
{
    UInt16 retval=0;
    UInt8 i;
    for (i=0; i < nbrOfgpCom_ActivateTxCbs; i++)
    {
        GP_ASSERT_SYSTEM(Com_ActivateTxCb[i]);
        retval += Com_ActivateTxCb[i](overFlowCounter-retval, commId);
    }
    return retval;
}
#endif

