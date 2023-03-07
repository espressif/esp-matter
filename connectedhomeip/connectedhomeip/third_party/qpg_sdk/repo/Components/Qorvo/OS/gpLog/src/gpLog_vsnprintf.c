/*
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

#define GP_COMPONENT_ID GP_COMPONENT_ID_LOG

#include "global.h"
#include "gpLog.h"
#include "hal.h"
#include "gpCom.h"

#include <stdarg.h>             // va_arg()
#include <string.h>             // memmove()
#include <stdio.h>              // memmove()

/*****************************************************************************
 *                    Macro Definitions
 *****************************************************************************/

#ifdef GP_BIG_ENDIAN
#define LOG_ENDIANESS gpLog_ParameterEndianessBig
#else
#define LOG_ENDIANESS gpLog_ParameterEndianessLittle
#endif

#ifdef GP_LOG_DIVERSITY_NO_TIME_NO_COMPONENT_ID
#define GP_LOG_HEADER_LENGTH            (0)
#else
#define GP_LOG_HEADER_LENGTH            (5+4)
#endif

#ifndef GP_LOG_MAX_LEN
#define GP_LOG_MAX_LEN                  100
#endif //GP_LOG_MAX_LEN

#ifndef GP_LOG_COMMUNICATION_ID
#define GP_LOG_COMMUNICATION_ID         GP_COM_DEFAULT_COMMUNICATION_ID
#endif //GP_LOG_COMMUNICATION_ID

/*****************************************************************************
 *                    Type Definitions
 *****************************************************************************/

#define gpLog_FormatStringModeString        0
#define gpLog_FormatStringModePointer       1
typedef UInt8 gpLog_FormatStringMode_t;

#define gpLog_ParameterEndianessLittle  0
#define gpLog_ParameterEndianessBig     1
typedef UInt8 gpLog_ParameterEndianess_t;

#define gpLog_CommandIDPrintfIndication         0x02
typedef UInt8 gpLog_CommandID_t;

/*****************************************************************************
 *                    Static Data Definitions
 *****************************************************************************/

#ifndef GP_LOG_DIVERSITY_NO_TIME_NO_COMPONENT_ID
static UInt8 gpLog_PrintfTotalCnt;
#endif // GP_LOG_DIVERSITY_NO_TIME_NO_COMPONENT_ID

#ifdef GP_LOG_BUFFERED_COMPID
static char Log_BufferCompId[GP_LOG_MAX_LEN];
#endif //GP_LOG_BUFFERED_COMPID

/*****************************************************************************
 *                    Static Function Prototypes
 *****************************************************************************/

static void Log_Formatted(UInt8 componentID, char* buf);

/*****************************************************************************
 *                    Static Function Definitions
 *****************************************************************************/

#ifdef GP_LOG_BUFFERED_COMPID
Bool Log_Buffer(char* buf)
{
    UIntLoop i;
    Bool buffered;

    UInt8 newBufLength;
    UInt8 logBufLength;

    newBufLength = strlen(buf);
    logBufLength = strlen(Log_BufferCompId);

    //Max buffering exceeded - dump what is stored
    if((newBufLength+logBufLength) >= GP_LOG_MAX_LEN)
    {
        Log_Formatted(GP_LOG_BUFFERED_COMPID, Log_BufferCompId);
        //Reset buffer
        MEMSET(Log_BufferCompId, 0, sizeof(Log_BufferCompId));
        logBufLength = 0;
    }

    //buffer to static buffer - limited to max length
    STRNCPY(&Log_BufferCompId[logBufLength], buf, GP_LOG_MAX_LEN-logBufLength);
    buffered = true;

    // check for \n present:
    for(i = 0; i < newBufLength; i++)
    {
        if(buf[i] == '\n') //End of line found
        {
            //Add buffered logging to end-of-line logging and let it pass
            STRNCPY(buf, Log_BufferCompId, GP_LOG_MAX_LEN);

            buf[strlen(buf)-1] = 0x0; //remove /n

            //Reset buffer
            MEMSET(Log_BufferCompId, 0, sizeof(Log_BufferCompId));
            buffered = false;
            break;
        }
    }

    return buffered;
}
#endif //GP_LOG_BUFFERED_COMPID

static void Log_Formatted(UInt8 componentID, char* buf)
{
    UInt8 nbr_chars=0;
#ifdef GP_LOG_DIVERSITY_NO_TIME_NO_COMPONENT_ID
    nbr_chars=strlen(&buf[GP_LOG_HEADER_LENGTH]);
    buf[nbr_chars] = '\r';
    nbr_chars++;
    buf[nbr_chars] = '\n';
#else//
    UInt32 time = 0xFFFFFFFF;

    //Construction of header - first bytes are left empty
    buf[nbr_chars++] = gpLog_CommandIDPrintfIndication;
    buf[nbr_chars++] = componentID;
    buf[nbr_chars++] = gpLog_FormatStringModeString | (LOG_ENDIANESS << 1);
    buf[nbr_chars++] = 0; //Former interval counter
    HAL_DISABLE_GLOBAL_INT();
    gpLog_PrintfTotalCnt++;
    buf[nbr_chars++] = gpLog_PrintfTotalCnt;
    HAL_ENABLE_GLOBAL_INT();

    HAL_TIMER_GET_CURRENT_TIME(time);
    HOST_TO_LITTLE_UINT32(&time);

    MEMCPY(&(buf[nbr_chars]),&time,4);
    nbr_chars += 4;

    //Count given chars
    nbr_chars+=strlen(&buf[GP_LOG_HEADER_LENGTH]);
    buf[nbr_chars] = 0;  // Add terminator
#endif // GP_LOG_DIVERSITY_NO_TIME_NO_COMPONENT_ID

    gpCom_DataRequest(GP_COMPONENT_ID_LOG, nbr_chars+1, (UInt8*)buf, GP_LOG_COMMUNICATION_ID);
}

/*****************************************************************************
 *                    Public Function Definitions
 *****************************************************************************/

void gpLog_Init(void)
{
#ifndef GP_LOG_DIVERSITY_NO_TIME_NO_COMPONENT_ID
    gpLog_PrintfTotalCnt = 0;
#endif // GP_LOG_DIVERSITY_NO_TIME_NO_COMPONENT_ID
#ifdef GP_LOG_BUFFERED_COMPID
    MEMSET(Log_BufferCompId, 0, sizeof(Log_BufferCompId));
#endif //GP_LOG_BUFFERED_COMPID
}

void gpLog_Printf(UInt8 componentID, Bool progmem , FLASH_STRING format_str, UInt8 length , ...)
{
    char buf[GP_LOG_MAX_LEN+GP_LOG_HEADER_LENGTH+1];

    NOT_USED(progmem);

    // When gpCom module is disabled, then return directly (avoid build of the printf)
    if (!gpCom_GetTXEnable())
    {
        return;
    }

    va_list ap;
    va_start(ap,length);
    vsnprintf(&buf[GP_LOG_HEADER_LENGTH], GP_LOG_MAX_LEN-GP_LOG_HEADER_LENGTH, format_str, ap);
    va_end(ap);

#ifdef GP_LOG_BUFFERED_COMPID
    if((componentID == GP_LOG_BUFFERED_COMPID) && Log_Buffer(&buf[GP_LOG_HEADER_LENGTH]))
    {
        return;
    }
#endif
    Log_Formatted(componentID, buf);
}

#ifdef GP_DIVERSITY_LOG
void gpLog_Flush(void)
{
    gpCom_Flush();
}

void gpLog_PrintBuffer(UInt16 length, UInt8* pData)
{
    UInt16 k;
    for (k = 0; k < length; k+=8)
    {
        GP_LOG_SYSTEM_PRINTF("%u/%u: %04x %04x %04x %04x",10,(UInt16)k,(UInt16)length,
                                                            (UInt16)((UInt16)pData[0+k] << 8)+(UInt16)pData[1+k],
                                                            (UInt16)((UInt16)pData[2+k] << 8)+(UInt16)pData[3+k],
                                                            (UInt16)((UInt16)pData[4+k] << 8)+(UInt16)pData[5+k],
                                                            (UInt16)((UInt16)pData[6+k] << 8)+(UInt16)pData[7+k]);
        gpLog_Flush();
    }
}
#endif //GP_DIVERSITY_LOG
