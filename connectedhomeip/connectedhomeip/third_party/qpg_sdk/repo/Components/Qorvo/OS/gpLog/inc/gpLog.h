/*
 * Copyright (c) 2008-2016, GreenPeak Technologies
 * Copyright (c) 2017-2021, Qorvo Inc
 *
 * gpLog.h
 *
 * This file contains the API definition of the logging module (printfs and asserts).
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

#ifndef _GPLOG_H_
#define _GPLOG_H_

#if defined(GP_DIVERSITY_ROM_CODE)
#include "gpLog_RomCode.h"
#else //defined(GP_DIVERSITY_ROM_CODE)

/*****************************************************************************
 *                    Includes Definitions
 *****************************************************************************/

#include "global.h"

/*****************************************************************************
 *                    Macro Definitions
 *****************************************************************************/

#define GP_LOG_PSTR(s) (FLASH_STRING) GEN_STR2FLASH(s)

#define GP_LOG_SPLIT_UINT32(i) (UInt16)(((i)&0xFFFF0000) >> 16), (UInt16)((i)&0xFFFF)

#if defined(GP_DIVERSITY_LOG) && defined(GP_LOCAL_LOG)

#if defined(__IAR_SYSTEMS_ICC__) && defined(GP_DIVERSITY_ROM_CODE) && defined(GP_DIVERSITY_JUMPTABLES)
#define GP_LOG_PRINTF(s, ...)                 \
    do {                                               \
        static const char ROM rom_str[] @ ".rom" = s; \
        gpLog_Printf(GP_COMPONENT_ID, true, rom_str, __VA_ARGS__);                      \
    } while(false)
#else
#define GP_LOG_PRINTF(s, ...) gpLog_Printf(GP_COMPONENT_ID, true, GP_LOG_PSTR(s), __VA_ARGS__)
#endif


#else //defined(GP_DIVERSITY_LOG) && defined(GP_LOCAL_LOG)
#ifndef HAVE_NO_VARIABLE_LENGTH_MACRO
#define GP_LOG_PRINTF(s, ...)
#else
#define GP_LOG_PRINTF
#endif //HAVE_NO_VARIABLE_LENGTH_MACRO
#endif //defined(GP_DIVERSITY_LOG) && defined(GP_LOCAL_LOG)

#ifdef GP_DIVERSITY_LOG
#if defined(__IAR_SYSTEMS_ICC__) && defined(GP_DIVERSITY_ROM_CODE) && defined(GP_DIVERSITY_JUMPTABLES)
#define GP_LOG_SYSTEM_PRINTF(s, ...)                 \
    do {                                               \
        static const char ROM rom_str[] @ ".rom" = s; \
        gpLog_Printf(GP_COMPONENT_ID, true, rom_str, __VA_ARGS__);                      \
    } while(false)
#else
#define GP_LOG_SYSTEM_PRINTF(s, ...) gpLog_Printf(GP_COMPONENT_ID, true, GP_LOG_PSTR(s), __VA_ARGS__)
#endif
#else //GP_COMP_LOG
#ifndef HAVE_NO_VARIABLE_LENGTH_MACRO
#define GP_LOG_SYSTEM_PRINTF(s, ...)
#else
#define GP_LOG_SYSTEM_PRINTF
#endif //HAVE_NO_VARIABLE_LENGTH_MACRO
#define gpLog_PrintBuffer(length, pData)
#define gpLog_Flush()
#endif //GP_DIVERSITY_LOG

#define gpLog_PrintfEnable(enable)

/* Print bits easily */
#define BYTE_TO_BIN(byte)          \
    (byte & 0x80 ? '1' : '0'),     \
        (byte & 0x40 ? '1' : '0'), \
        (byte & 0x20 ? '1' : '0'), \
        (byte & 0x10 ? '1' : '0'), \
        (byte & 0x08 ? '1' : '0'), \
        (byte & 0x04 ? '1' : '0'), \
        (byte & 0x02 ? '1' : '0'), \
        (byte & 0x01 ? '1' : '0')

#define BYTE_TO_BIN_PATTERN "%c%c%c%c%c%c%c%c"

/*****************************************************************************
 *                    Type Definitions
 *****************************************************************************/

/*****************************************************************************
 *                    Static Data Definitions
 *****************************************************************************/

/*****************************************************************************
 *                    Static Function Prototypes
 *****************************************************************************/

/*****************************************************************************
 *                    Static Function Definitions
 *****************************************************************************/

/*****************************************************************************
 *                    Public Function Definitions
 *****************************************************************************/

#ifdef __cplusplus
extern "C" {
#endif

#if defined(GP_DIVERSITY_JUMPTABLES) && defined(GP_DIVERSITY_ROM_CODE)
#include "gpLog_CodeJumpTableFlash_Defs.h"
#endif // defined(GP_DIVERSITY_JUMPTABLES) && defined(GP_DIVERSITY_ROM_CODE)

/* JUMPTABLE_FLASH_FUNCTION_DEFINITIONS_START */

/**
*   @brief Initializes the logging module
*/
GP_API void gpLog_Init(void);

/**
*   @brief Prints a string with formats through the debug interface.
*
*   @param    componentID Component ID of component calling the print function.
*   @param    progmem     Indicate if format string buffer originates from FLASH or RAM memory.
*   @param    format_str  NULL terminated string with formatting parameters.
*   @param    length      Deprecated variable, previously indication size of following parameters.
*/
#ifdef __GNUC__
GP_API void gpLog_Printf(UInt8 componentID, Bool progmem, FLASH_STRING format_str, UInt8 length, ...) __attribute__((format(printf, 3, 5)));
#else
GP_API void gpLog_Printf(UInt8 componentID, Bool progmem, FLASH_STRING format_str, UInt8 length, ...);
#endif


#ifdef GP_DIVERSITY_LOG
/**
*   @brief Prints a data buffer in hex format.
*
*   @param    pData    pointer to the buffer
*   @param    length   length to be printed
*/
GP_API void gpLog_PrintBuffer(UInt16 length, UInt8* pData);

/**
*   @brief Flush any pending logging.
*/
GP_API void gpLog_Flush(void);




#endif //GP_DIVERSITY_LOG

/* JUMPTABLE_FLASH_FUNCTION_DEFINITIONS_END */

#ifdef __cplusplus
}
#endif

#endif //defined(GP_DIVERSITY_ROM_CODE)

#endif // _GPLOG_H_
