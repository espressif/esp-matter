/*
 * Copyright (c) 2020, Qorvo Inc
 *
 * uart_qorvo.c
 *   This file contains the implementation of the qorvo uart api for openthread.
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

#define GP_COMPONENT_ID GP_COMPONENT_ID_QVOT

/*****************************************************************************
 *                    Includes Definitions
 *****************************************************************************/


#include "uart_qorvo.h"

#include "gpLog.h"
#include "gpPoolMem.h"
#include "gpSched.h"
#include "gpAssert.h"
#include "gpCom.h"

#include <stdio.h> //vsnprintf

/*****************************************************************************
 *                    Static Function Definitions
 *****************************************************************************/

void qorvoUart_cbTxDone(void)
{
    cbQorvoUartTxDone();
}

void qorvoUartSendOutput(const uint8_t* aBuf, uint16_t aBufLength)
{
    Bool result = gpCom_DataRequest(GP_COMPONENT_ID, aBufLength, (uint8_t*)aBuf, GP_COM_DEFAULT_COMMUNICATION_ID);
    if(result)
    {
        qorvoUart_cbTxDone();
    }
}

void cbUartSendInput(void* buffer)
{
    uint16_t len;
    uint8_t* buf = (uint8_t*)buffer;

    len = strlen((const char*)buf);
    qorvoUartSendInput((uint8_t*)buf, len);
    gpPoolMem_Free(buf);
}

void qorvoUart_cbHandleRx(UInt16 length, UInt8* pData, gpCom_CommunicationId_t commId)
{
    UInt8* buf;

    buf = GP_POOLMEM_MALLOC(length + 1);
    if(buf != NULL)
    {
        MEMCPY(buf, pData, length);
        buf[length] = 0;
        gpSched_ScheduleEventArg(0, cbUartSendInput, (void*)buf);
    }
}

/*****************************************************************************
 *                    Public Function Definitions
 *****************************************************************************/

void qorvoUartInit(void)
{
    gpCom_RegisterModule(GP_COMPONENT_ID, qorvoUart_cbHandleRx);
}

void qorvoUartDeInit(void)
{
    gpCom_DeRegisterModule(GP_COMPONENT_ID);
}

void qorvoUartLog(otLogLevel aLogLevel, otLogRegion aLogRegion, const char* aFormat, va_list args)
{
    UInt16 charsWritten;
    char logString[512];

    NOT_USED(aLogLevel);
    NOT_USED(aLogRegion);

    //Check clipping
    charsWritten = vsnprintf(&logString[0], sizeof(logString), aFormat, args);
    GP_ASSERT_DEV_INT(charsWritten <= sizeof(logString));

    qorvoUartSendOutput((UInt8*)logString, strlen(logString));
}

otError qorvoUartFlush(void)
{
    /* When both the gpCom TX buffer and the CLI buffer are exhausted, we flush the gpCom.
     * buffer. We sacrifice logging/CLI output to keep the system running. The flushing is
     * independent of the content, no partial (e.g. debug logging only) flushing is possible.
     */
    gpCom_Flush();
    qorvoUart_cbTxDone();
    return OT_ERROR_NONE;
}
