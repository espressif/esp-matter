/*
 * Copyright (c) 2010-2016, GreenPeak Technologies
 * Copyright (c) 2017-2019, Qorvo Inc
 *
 * gpAssert.c
 *   This file contains the implementation of the Assert functions.
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

#define GP_COMPONENT_ID GP_COMPONENT_ID_ASSERT

#include "gpAssert.h"
#include "hal.h"
#ifdef GP_DIVERSITY_LOG
#include "gpLog.h"
#endif //GP_DIVERSITY_LOG
#ifdef GP_DIVERSITY_ASSERT_ACTION_RESET
#include "gpReset.h"
#endif //GP_DIVERSITY_ASSERT_ACTION_RESET

/*****************************************************************************
 *                    Macro Definitions
 *****************************************************************************/
#ifdef GP_DIVERSITY_ASSERT_REPORTING_LED_DEBUG
#define GP_ASSERT_DIVERSITY_LED_DEBUG_MAGIC_WORD 0x72
#endif

/*****************************************************************************
 *                    Functional Macro Definitions
 *****************************************************************************/

/*****************************************************************************
 *                    Type Definitions
 *****************************************************************************/

/*****************************************************************************
 *                    Static Function Prototypes
 *****************************************************************************/
#ifdef GP_DIVERSITY_ASSERT_REPORTING_LED_DEBUG
static void Assert_blinkLedAssert(UInt16 number);
#endif //GP_DIVERSITY_ASSERT_REPORTING_LED_DEBUG

/*****************************************************************************
 *                    Static Data Definitions
 *****************************************************************************/
#ifdef GP_DIVERSITY_ASSERT_REPORTING_CALLBACK
gpAssert_cbAssertIndication_t gpAssert_cbAssertIndication = NULL;
#endif //GP_DIVERSITY_ASSERT_REPORTING_CALLBACK

#ifdef GP_DIVERSITY_ASSERT_REPORTING_LED_DEBUG
static Bool gpAssert_DebugEnabled = 0;
#endif //GP_DIVERSITY_ASSERT_REPORTING_LED_DEBUG

#ifdef GP_DIVERSITY_LOG

#if  !defined(GP_ASSERT_DIVERSITY_LOG_FILENAME_LENGTH)
#define GP_ASSERT_DIVERSITY_LOG_FILENAME_LENGTH 30
#endif

FLASH_STRING gpAssert_filename;
UInt16       gpAssert_line;
#endif //GP_DIVERSITY_LOG

/*****************************************************************************
 *                    External Data Definition
 *****************************************************************************/

/*****************************************************************************
 *                    Static Function Definitions
 *****************************************************************************/
#if defined(GP_DIVERSITY_ASSERT_REPORTING_LED_DEBUG)
static void Assert_blinkLedAssert(UInt16 number)
{
    UIntLoop i = 5;

    HAL_LED_SET(RED);
    while(i)
    {
        UInt16 subtractor= 0;
        UIntLoop j;

        switch(i)
        {
            case 1:
                subtractor = 1;
                break;
            case 2:
                subtractor = 10;
                break;
            case 3:
                subtractor = 100;
                break;
            case 4:
                subtractor = 1000;
                break;
            case 5:
                subtractor = 10000;
                break;
            default:
                break;
        }

        for(j=0; j< 10; j++)
        {
            if(number >= subtractor)
            {
                HAL_LED_SET(GRN);
                number-=subtractor;
            }
            else
            {
                HAL_LED_CLR(GRN);
            }
            HAL_WAIT_MS(1);
            HAL_LED_CLR(GRN);
            HAL_WAIT_MS(1);
        }

        i--;
    }
    HAL_LED_CLR(RED);
}
#endif //GP_DIVERSITY_ASSERT_REPORTING_LED_DEBUG

/*****************************************************************************
 *                    Public Function Definitions
 *****************************************************************************/

UInt32 Assert_doWhileTimeOutBody(UInt32 timeout)
{
    const UInt8 TimeDecrement = 4;

    if (timeout < TimeDecrement)
    {
        return 0;
    }

    HAL_WDT_RESET();
    HAL_WAIT_US(TimeDecrement);

    return (timeout - TimeDecrement);
}

UInt32 Assert_doWhileTimeOutBody_Assert(UInt32 timeout, UInt8 compId, FLASH_STRING filename, UInt16 line)
{
    UInt32 __timeout = Assert_doWhileTimeOutBody(timeout);
    if (__timeout == 0)
        GP_ASSERT_DO_ASSERT_HANDLING (GP_DIVERSITY_ASSERT_LEVEL_SYSTEM, compId,filename,line);
    return __timeout;
}

#ifdef GP_DIVERSITY_ASSERT_ENABLE_IDENTIFIER
void gpAssert_DoAssertHandling_Extended(gpAssert_AssertInfo_t assertInfo, UInt8 componentId, FLASH_STRING filename, UInt16 line)
{
    NOT_USED(assertInfo);
    NOT_USED(componentId);
    NOT_USED(filename);
    NOT_USED(line);

    GP_ASSERT_REPORT(assertInfo, componentId, filename, line);
    GP_ASSERT_FOLLOW_UP_ACTION();
}
#else
void gpAssert_DoAssertHandling_Basic(UInt8 componentId, FLASH_STRING filename, UInt16 line)
{
    NOT_USED(componentId);
    NOT_USED(filename);
    NOT_USED(line);

    GP_ASSERT_REPORT(0, componentId, filename, line);
    GP_ASSERT_FOLLOW_UP_ACTION();
}
#endif //GP_DIVERSITY_ASSERT_ENABLE_IDENTIFIER

#ifdef GP_DIVERSITY_ASSERT_REPORTING_CALLBACK
gpAssert_cbAssertIndication_t gpAssert_RegisterCbAssertIndication(gpAssert_cbAssertIndication_t cbAssertIndication)
{
    gpAssert_cbAssertIndication_t prevCb;

    prevCb = gpAssert_cbAssertIndication;
    gpAssert_cbAssertIndication = cbAssertIndication;

    return prevCb;
}
#endif //GP_DIVERSITY_ASSERT_REPORTING_CALLBACK

#ifdef GP_DIVERSITY_ASSERT_REPORTING_LED_DEBUG
void gpAssert_EnableDebug(void)
{
    gpAssert_DebugEnabled = GP_ASSERT_DIVERSITY_LED_DEBUG_MAGIC_WORD;
}
#endif //GP_DIVERSITY_ASSERT_REPORTING_LED_DEBUG

// reporting
#ifdef GP_DIVERSITY_ASSERT_REPORTING_CALLBACK
void gpAssert_CallCbAssertIndication(gpAssert_AssertInfo_t assertInfo, UInt8 componentId, FLASH_STRING filename, UInt16 line)
{
#ifndef GP_DIVERSITY_ASSERT_ENABLE_IDENTIFIER
    NOT_USED(assertInfo);
#endif //not defined GP_DIVERSITY_ASSERT_ENABLE_IDENTIFIER
    if(gpAssert_cbAssertIndication != NULL)
    {
        gpAssert_NotifyInfo_t notifyInfo;
#ifdef GP_DIVERSITY_ASSERT_ENABLE_IDENTIFIER
        notifyInfo.level = GP_ASSERT_GET_LEVEL(assertInfo);
        notifyInfo.id = GP_ASSERT_GET_ID(assertInfo);
#endif //GP_DIVERSITY_ASSERT_ENABLE_IDENTIFIER
        notifyInfo.componentId = componentId;
        notifyInfo.lineNumber = line;
        notifyInfo.fileName = filename;
        gpAssert_cbAssertIndication(&notifyInfo);
    }
    else
    {
#if defined(GP_DIVERSITY_LOG)
        // print logging when enabled
        gpAssert_PrintLogString(componentId, filename, line);
#endif //GP_DIVERSITY_LOG
    }
}
#endif //GP_DIVERSITY_ASSERT_REPORTING_CALLBACK

#ifdef GP_DIVERSITY_LOG
void gpAssert_PrintLogString(UInt8 componentID, FLASH_STRING filename, UInt16 line)
{
    Char gpAssert_File[11+GP_ASSERT_DIVERSITY_LOG_FILENAME_LENGTH] = "Assert %i!";

    HAL_DISABLE_GLOBAL_INT();

    //Store in static variables for debuggers
    gpAssert_filename = filename;
    gpAssert_line = line;

    //Copy filename after '!'
    STRNCPY_P(&(gpAssert_File[10]), filename,GP_ASSERT_DIVERSITY_LOG_FILENAME_LENGTH);
    gpLog_Flush();
    gpLog_Printf(componentID, false , (FLASH_STRING)gpAssert_File,2,line);
    gpLog_Flush();
#if defined(GP_DIVERSITY_WINDOWS) || defined(GP_DIVERSITY_CORTEXM4)
#endif // defined(GP_DIVERSITY_LINUXUSER) || defined(GP_DIVERSITY_WINDOWS)

    HAL_ENABLE_GLOBAL_INT();
    return;
}
#endif //#ifdef GP_DIVERSITY_LOG

#ifdef GP_DIVERSITY_ASSERT_REPORTING_LED_DEBUG
void gpAssert_LedDebug(UInt16 line)
{
    HAL_DISABLE_GLOBAL_INT();
    if(gpAssert_DebugEnabled  == GP_ASSERT_DIVERSITY_LED_DEBUG_MAGIC_WORD)
    {
#ifdef GP_DIVERSITY_LOG
        //Flush out any pending messages
        gpLog_Flush();
#endif //GP_DIVERSITY_LOG
        volatile Bool EndlessLoop = true;
        while(EndlessLoop)
        {
            HAL_WDT_RESET();
            HAL_WAIT_MS(10);
            Assert_blinkLedAssert(line);
        }
    }
    HAL_ENABLE_GLOBAL_INT();
}
#endif //GP_DIVERSITY_ASSERT_REPORTING_LED_DEBUG

// action functions
#ifdef GP_DIVERSITY_ASSERT_ACTION_RESET
void gpAssert_ResetSystem(void)
{
    gpReset_ResetSystem();
}
#endif //GP_DIVERSITY_ASSERT_ACTION_RESET

#ifdef GP_DIVERSITY_ASSERT_ACTION_EXIT
void gpAssert_Exit(void)
{
#if defined(GP_DIVERSITY_WINDOWS) 
#ifdef GP_DIVERSITY_LOG
    gpLog_Flush();
#endif
    exit(-1);
#endif //defined(GP_DIVERSITY_WINDOWS) || defined(GP_DIVERSITY_LINUXUSER)
}
#endif //GP_DIVERSITY_ASSERT_ACTION_EXIT

#if defined(GP_DIVERSITY_ASSERT_ACTION_BLINK_LED)
void gpAssert_BlinkLed(void)
{
    volatile Bool EndLessLoop = true;

    HAL_DISABLE_GLOBAL_INT();
    //Performs the end loop with LED blink

    // When the debugger reset the variable EndLessLoop, the CPU can return to the position, where the assert was called
    while (EndLessLoop)
    {
        //toggle green led
        HAL_WDT_RESET();
        HAL_LED_TGL(GRN);
        HAL_WAIT_MS(500);
#ifdef GP_DIVERSITY_LOG
        //Flush out any pending messages
        gpLog_Flush();
#endif //GP_DIVERSITY_LOG
    }
    HAL_ENABLE_GLOBAL_INT();
}
#endif //GP_DIVERSITY_ASSERT_ACTION_BLINK_LED

