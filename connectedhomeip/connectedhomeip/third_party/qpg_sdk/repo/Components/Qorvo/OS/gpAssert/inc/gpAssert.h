/*
 * Copyright (c) 2010-2016, GreenPeak Technologies
 * Copyright (c) 2017-2019, Qorvo Inc
 *
 *   gpAssert.h
 *   This file contains the definitions of the public functions and enumerations of the gpAssert.
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

#ifndef _GP_ASSERT_H_
#define _GP_ASSERT_H_

#if defined(GP_DIVERSITY_ROM_CODE)
#include "gpAssert_RomCode.h"
#else //defined(GP_DIVERSITY_ROM_CODE)

/*****************************************************************************
 *                    Includes Definitions
 *****************************************************************************/
#include "global.h"

/*****************************************************************************
 *                    Type Definitions
 *****************************************************************************/

#ifdef GP_COMP_UNIT_TEST
#if defined(GP_DIVERSITY_WINDOWS)  
//Make sure unit and stacktest are able to stop after unexpected assert
#define GP_DIVERSITY_ASSERT_ACTION_EXIT
#endif //GP_DIVERSITY_WINDOWS
#endif //GP_COMP_UNIT_TEST

// level of assert that should be kept (lower means more critical)
#define GP_DIVERSITY_ASSERT_LEVEL_SYSTEM        0
#define GP_DIVERSITY_ASSERT_LEVEL_DEV_EXT       1
#define GP_DIVERSITY_ASSERT_LEVEL_DEV_INT       2
typedef UInt8 gpAssert_AssertLevel_t;

// Id of the assert -- Fixme: add more ids here
#define GP_ASSERT_ID_UNDEFINED                  0x00
typedef UInt8 gpAssert_AssertId_t;

// typedef for first parameter of assert macro (currently contains 2 bits level and optional 4 bits id)
typedef UInt8 gpAssert_AssertInfo_t;

// structure that is passed to assert callback function
typedef struct {
#ifdef GP_DIVERSITY_ASSERT_ENABLE_IDENTIFIER
    gpAssert_AssertLevel_t level;
    gpAssert_AssertId_t id;
#endif //GP_DIVERSITY_ASSERT_ENABLE_IDENTIFIER
    UInt8 componentId;
    UInt16 lineNumber;
    FLASH_STRING fileName;
} gpAssert_NotifyInfo_t;

// define type of assert callback
typedef void (*gpAssert_cbAssertIndication_t)(gpAssert_NotifyInfo_t* notifyInfo);

/*****************************************************************************
 *                    Macro Definitions
 *****************************************************************************/

#ifdef GP_FILENAME
#ifdef GP_DIVERSITY_LOG
//Define a FileName only once for use in Assertions - name comes from make environment
#if defined(__IAR_SYSTEMS_ICC__) && defined(GP_DIVERSITY_ROM_CODE) && defined(GP_DIVERSITY_JUMPTABLES)
static const char ROM gpFileName[] FLASH_PROGMEM  @ ".rom" = { XSTRINGIFY(GP_FILENAME) };
#else
static const char ROM gpFileName[] FLASH_PROGMEM = { XSTRINGIFY(GP_FILENAME) };
#endif // defined(__IAR_SYSTEMS_ICC__) && defined(GP_DIVERSITY_ROM_CODE) && defined(GP_DIVERSITY_JUMPTABLES)
#define GP_ASSERT_FILENAME gpFileName
#endif //GP_DIVERSITY_LOG
#endif //GP_FILENAME

#ifndef GP_ASSERT_FILENAME
#define GP_ASSERT_PSTR(s) (FLASH_STRING)GEN_STR2FLASH(s)
#ifdef GP_DIVERSITY_LOG
#if defined(__IAR_SYSTEMS_ICC__) && defined(GP_DIVERSITY_ROM_CODE) && defined(GP_DIVERSITY_JUMPTABLES)
static const char ROM gpFileName[] FLASH_PROGMEM  @ ".rom" = { XSTRINGIFY(__FILE__) };
#define GP_ASSERT_FILENAME gpFileName
#else
#define GP_ASSERT_FILENAME GP_ASSERT_PSTR(__FILE__)
#endif
#else
#define GP_ASSERT_FILENAME 0
#endif
#endif //GP_ASSERT_FILENAME

// In case no global assert level is provided, define it (maps on previous structure).
#ifndef GP_DIVERSITY_GLOBAL_ASSERT_LEVEL
#define GP_DIVERSITY_GLOBAL_ASSERT_LEVEL GP_DIVERSITY_ASSERT_LEVEL_DEV_EXT
#endif //GP_DIVERSITY_GLOBAL_ASSERT_LEVEL


// when no reporting is defined, take callback (has also logging) as default
#if !defined(GP_DIVERSITY_ASSERT_REPORTING_LOG) && !defined(GP_DIVERSITY_ASSERT_REPORTING_CALLBACK) && !defined(GP_DIVERSITY_ASSERT_REPORTING_LED_DEBUG) && !defined(GP_DIVERSITY_ASSERT_REPORTING_NOTHING)
#define GP_DIVERSITY_ASSERT_REPORTING_CALLBACK
#endif //!defined(GP_DIVERSITY_ASSERT_REPORTING_LOG) && !defined(GP_DIVERSITY_ASSERT_REPORTING_CALLBACK) && !defined(GP_DIVERSITY_ASSERT_REPORTING_LED_DEBUG) && !defined(GP_DIVERSITY_ASSERT_REPORTING_NOTHING)

// when no action is defined, take endless loop (blinking led) as default
#if !defined(GP_DIVERSITY_ASSERT_ACTION_BLINK_LED) && !defined(GP_DIVERSITY_ASSERT_ACTION_EXIT) && !defined(GP_DIVERSITY_ASSERT_ACTION_RESET) && !defined(GP_DIVERSITY_ASSERT_ACTION_NOTHING)
#define GP_DIVERSITY_ASSERT_ACTION_BLINK_LED
#endif //!defined(GP_DIVERSITY_ASSERT_ACTION_BLINK_LED) && !defined(GP_DIVERSITY_ASSERT_ACTION_EXIT) && !defined(GP_DIVERSITY_ASSERT_ACTION_RESET) && !defined(GP_DIVERSITY_ASSERT_ACTION_NOTHING)

#if defined(GP_DIVERSITY_ASSERT_REPORTING_LOG) && !defined(GP_DIVERSITY_LOG)
#error Log reporting only possible when log is defined!
#endif

// define reporting behavior
#if defined(GP_DIVERSITY_ASSERT_REPORTING_CALLBACK)
#define GP_ASSERT_REPORT(info, compId, file, line)          gpAssert_CallCbAssertIndication(info,compId,file,line)
#elif defined(GP_DIVERSITY_ASSERT_REPORTING_LOG)
#define GP_ASSERT_REPORT(info, compId, file, line)          gpAssert_PrintLogString(compId,file,line)
#elif defined(GP_DIVERSITY_ASSERT_REPORTING_LED_DEBUG)
#define GP_ASSERT_REPORT(info, compId, file, line)          gpAssert_LedDebug(line)
#elif defined(GP_DIVERSITY_ASSERT_REPORTING_NOTHING)
#define GP_ASSERT_REPORT(info, compId, file, line)          do {} while(false)
#endif //GP_DIVERSITY_ASSERT_REPORTING_CALLBACK

// define action behavior
#if defined(GP_DIVERSITY_ASSERT_ACTION_BLINK_LED)
#define GP_ASSERT_FOLLOW_UP_ACTION()                    gpAssert_BlinkLed()
#elif defined(GP_DIVERSITY_ASSERT_ACTION_EXIT)
#define GP_ASSERT_FOLLOW_UP_ACTION()                    gpAssert_Exit()
#elif defined(GP_DIVERSITY_ASSERT_ACTION_RESET)
#define GP_ASSERT_FOLLOW_UP_ACTION()                    gpAssert_ResetSystem()
#elif defined(GP_DIVERSITY_ASSERT_ACTION_NOTHING)
#define GP_ASSERT_FOLLOW_UP_ACTION()                    do {} while(false)
#endif

// basic or extend handling (assert identifier specified or not)
#ifdef GP_DIVERSITY_ASSERT_ENABLE_IDENTIFIER
#define GP_ASSERT_DO_ASSERT_HANDLING(info, compId, file, line)  gpAssert_DoAssertHandling_Extended(info, compId, file, line)
#else
#define GP_ASSERT_DO_ASSERT_HANDLING(info, compId, file, line)  gpAssert_DoAssertHandling_Basic(compId, file, line)
#endif //GP_DIVERSITY_ASSERT_ENABLE_IDENTIFIER

// default assert macro
#define GP_ASSERT(info, check)                                                                  \
do {                                                                                            \
    if(GP_DIVERSITY_GLOBAL_ASSERT_LEVEL >= GP_ASSERT_GET_LEVEL(info))                           \
    {                                                                                           \
        if (!(check))                                                                           \
        {                                                                                       \
            GP_ASSERT_DO_ASSERT_HANDLING(info, GP_COMPONENT_ID, GP_ASSERT_FILENAME, __LINE__);  \
        }                                                                                       \
    }                                                                                           \
} while (false)

// assert macro that only checks value, but leaves handling to the user
#define GP_ASSERT_IF(level, check)     ((GP_DIVERSITY_GLOBAL_ASSERT_LEVEL >= level) ? (!(check)) : (false))

//debug backdoor
#ifdef GP_DIVERSITY_ASSERT_REPORTING_LED_DEBUG
#define GP_ASSERT_ENABLE_DEBUG()                   gpAssert_EnableDebug()
#else
#define GP_ASSERT_ENABLE_DEBUG()                   do {} while(false)
#endif //GP_DIVERSITY_ASSERT_REPORTING_LED_DEBUG

// map old asserts to new structure
#define GP_ASSERT_DEV_INT(x)        GP_ASSERT(GP_DIVERSITY_ASSERT_LEVEL_DEV_INT,x)
#define GP_ASSERT_DEV_EXT(x)        GP_ASSERT(GP_DIVERSITY_ASSERT_LEVEL_DEV_EXT,x)
#define GP_ASSERT_SYSTEM(x)         GP_ASSERT(GP_DIVERSITY_ASSERT_LEVEL_SYSTEM,x)

// fixme: move this functionality to a global place?
#define ASSERT_BM_SET(bf,bm,idx,v)              bf &= ~bm; bf |= (((UInt16)v) << idx)
#define ASSERT_BM_GET(value, mask, idx)         ((value & mask) >> idx)

// structure of info field
//  | 7 | 6 | 5 | 4 | 3 | 2 | 1 | 0 |
//  |       id      | x | x | level |
#define GP_ASSERT_LEVEL_BM                      0x03
#define GP_ASSERT_ID_BM                         0xF0
#define GP_ASSERT_LEVEL_IDX                     0
#define GP_ASSERT_ID_IDX                        4

#define GP_ASSERT_GET_LEVEL(info)               ASSERT_BM_GET(info, GP_ASSERT_LEVEL_BM, GP_ASSERT_LEVEL_IDX)
#define GP_ASSERT_SET_LEVEL(info, level)        ASSERT_BM_SET(info, GP_ASSERT_LEVEL_BM, GP_ASSERT_LEVEL_IDX, level)

#ifdef GP_DIVERSITY_ASSERT_ENABLE_IDENTIFIER
#define GP_ASSERT_GET_ID(info)                  ASSERT_BM_GET(info, GP_ASSERT_ID_BM, GP_ASSERT_ID_IDX)
#define GP_ASSERT_SET_ID(info, id)              ASSERT_BM_SET(info, GP_ASSERT_ID_BM, GP_ASSERT_ID_IDX, id)
#else
#define GP_ASSERT_GET_ID(info)                  GP_ASSERT_ID_UNDEFINED
#define GP_ASSERT_SET_ID(info, id)              do {} while(false)
#endif //GP_DIVERSITY_ASSERT_ENABLE_IDENTIFIER

/** @brief Call condition until timeout expires.
 *
 *  @param cond         code snippet that will be called in a while loop. Has to evaluate to a usable value.
 *  @param timeout      timeout in us
 *  @param pTimedOut    Pointer to variable is set to true if timeout has occured
*/
#define GP_DO_WHILE_TIMEOUT(cond, timeout, pTimedOut) do {  \
    UInt32 __timeout = timeout;                             \
    while ( (cond) )                                        \
    {                                                       \
        __timeout = Assert_doWhileTimeOutBody(__timeout);   \
        if (__timeout == 0)                                 \
        {                                                   \
            break;                                          \
        }                                                   \
    };                                                      \
    *pTimedOut = (__timeout == 0);                          \
} while(false)

/** @brief Call condition until timeout expires. Assert if timeout is reached.
 *
 *  @param cond code that will be called in a while loop
 *  @param timeout timeout in us
*/
#define GP_DO_WHILE_TIMEOUT_ASSERT(cond, timeout) do {      \
    UInt32 __timeout = timeout;                             \
    while ( (cond) )                                        \
    {                                                       \
        __timeout = Assert_doWhileTimeOutBody_Assert(__timeout ,GP_COMPONENT_ID,GP_ASSERT_FILENAME, __LINE__); \
        if (__timeout == 0)                                 \
        {                                                   \
            break;                                          \
        }                                                   \
    };                                                      \
} while(false)


/*****************************************************************************
 *                    'Private' Function Definitions
 *****************************************************************************/

// These methods used by the GP_DO_WHILE_TIMEOUT(_ASSERT) macro's for code optimisations and should not be called a
// such. The macro implemenations are required to have proper condition evaluation and line printing in case of an
// assert. Reinstanciating the code has however code size impact so these methods reduce the reinstanciated code.
UInt32 Assert_doWhileTimeOutBody(UInt32 timeout);
UInt32 Assert_doWhileTimeOutBody_Assert(UInt32 timeout,UInt8 compId,  FLASH_STRING filename, UInt16 line);

/*****************************************************************************
 *                    Public Function Definitions
 *****************************************************************************/

#ifdef __cplusplus
extern "C" {
#endif

#if defined(GP_DIVERSITY_JUMPTABLES) && defined(GP_DIVERSITY_ROM_CODE)
#include "gpAssert_CodeJumpTableFlash_Defs.h"
#endif //defined(GP_DIVERSITY_JUMPTABLES) && defined(GP_DIVERSITY_ROM_CODE)
/* JUMPTABLE_FLASH_FUNCTION_DEFINITIONS_START */

#ifdef GP_DIVERSITY_ASSERT_ENABLE_IDENTIFIER
void gpAssert_DoAssertHandling_Extended(gpAssert_AssertInfo_t assertInfo, UInt8 componentId, FLASH_STRING filename, UInt16 line);
#else
void gpAssert_DoAssertHandling_Basic(UInt8 componentId, FLASH_STRING filename, UInt16 line);
#endif // GP_DIVERSITY_ASSERT_ENABLE_IDENTIFIER
#ifdef GP_DIVERSITY_ASSERT_REPORTING_CALLBACK
gpAssert_cbAssertIndication_t gpAssert_RegisterCbAssertIndication(gpAssert_cbAssertIndication_t cbAssertIndication);
#endif //GP_DIVERSITY_ASSERT_REPORTING_CALLBACK
#ifdef GP_DIVERSITY_ASSERT_REPORTING_LED_DEBUG
GP_API void gpAssert_EnableDebug(void);
#endif //GP_DIVERSITY_ASSERT_REPORTING_LED_DEBUG

// reporting functions
#ifdef GP_DIVERSITY_ASSERT_REPORTING_CALLBACK
void gpAssert_CallCbAssertIndication(gpAssert_AssertInfo_t assertInfo, UInt8 componentId, FLASH_STRING filename, UInt16 line);
#endif //GP_DIVERSITY_ASSERT_REPORTING_CALLBACK
#ifdef GP_DIVERSITY_LOG
void gpAssert_PrintLogString(UInt8 componentId, FLASH_STRING filename, UInt16 line);
#endif //GP_DIVERSITY_LOG
#ifdef GP_DIVERSITY_ASSERT_REPORTING_LED_DEBUG
void gpAssert_LedDebug(UInt16 line);
#endif //GP_DIVERSITY_ASSERT_REPORTING_LED_DEBUG

// action functions
#ifdef GP_DIVERSITY_ASSERT_ACTION_RESET
void gpAssert_ResetSystem(void);
#endif //GP_DIVERSITY_ASSERT_ACTION_RESET
#ifdef GP_DIVERSITY_ASSERT_ACTION_EXIT
void gpAssert_Exit(void);
#endif //GP_DIVERSITY_ASSERT_ACTION_EXIT
#ifdef GP_DIVERSITY_ASSERT_ACTION_BLINK_LED
void gpAssert_BlinkLed(void);
#endif //GP_DIVERSITY_ASSERT_ACTION_BLINK_LED
/* JUMPTABLE_FLASH_FUNCTION_DEFINITIONS_END */
/** @brief Notify an assert happened
*
*   @param componentId
*   @param lineNumber
*/
void gpAssert_cbIndication(UInt16 componentId, UInt16 lineNumber);

#ifdef __cplusplus
}
#endif //__cplusplus

#endif //defined(GP_DIVERSITY_ROM_CODE)
#endif //_GP_ASSERT_H_

