/*
 * Copyright (c) 2010-2014, 2016, GreenPeak Technologies
 * Copyright (c) 2017-2018, Qorvo Inc
 *
 * gpSched_defs.h
 *   This file holds the internal defines, typedefs,... of the gpSched component
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

#ifndef _GP_SCHED_DEFS_H_
#define _GP_SCHED_DEFS_H_

/*****************************************************************************
 *                    Includes Definitions
 *****************************************************************************/

#include "gpUtils.h"
#include "gpSched.h"

/*****************************************************************************
 *                    Precompiler checks
 *****************************************************************************/

/*****************************************************************************
 *                    Macro Definitions
 *****************************************************************************/

#define GP_SCHED_TIME_COMPARE_BIGGER(t1,t2)         (!((UInt32)(((t2)  - (t1))) <= 0x80000000LU))
#define GP_SCHED_TIME_COMPARE_BIGGER_EQUAL(t1,t2)   ( ((UInt32)(((t1)  - (t2))) <  0x80000000LU))
#define GP_SCHED_TIME_COMPARE_LOWER_EQUAL(t1,t2)    ( ((UInt32)(((t2)  - (t1))) <= 0x80000000LU))
#define GP_SCHED_TIME_COMPARE_LOWER(t1,t2)          (!((UInt32)(((t1)  - (t2))) <  0x80000000LU))

/*****************************************************************************
 *                    Type Definitions
 *****************************************************************************/


typedef union {
    void_func                callback;      // Event to be executed without arguments
    gpSched_EventCallback_t  callbackarg;   // Event to be executed with arguments
} gpSched_func_t;

typedef struct gpSched_Event {
    UInt32                      time;       // sorting key : Absolute in 1us
    gpSched_func_t              func;
#if defined(GP_SCHED_DIVERSITY_USE_ARGS)  ||  defined(GP_DIVERSITY_JUMPTABLES)
    void*                       arg;
#endif // defined(GP_SCHED_DIVERSITY_USE_ARGS)  ||  defined(GP_DIVERSITY_JUMPTABLES)
#if defined(GP_SCHED_DIVERSITY_SCHEDULE_INSECONDSAPI)  ||  defined(GP_DIVERSITY_JUMPTABLES)
    UInt32                      RemainderInSeconds;
#endif //defined(GP_SCHED_DIVERSITY_SCHEDULE_INSECONDSAPI)  ||  defined(GP_DIVERSITY_JUMPTABLES)
} gpSched_Event_t;

typedef struct gpSched_globals_s {
    #if  defined(GP_DIVERSITY_JUMPTABLES)
    UInt8 gpSched_GoToSleepDisableCounter;
    gpSched_GotoSleepCheckCallback_t gpSched_cbGotoSleepCheck;
    UInt32 gpSched_GoToSleepTreshold;
    #endif //defined(GP_SCHED_DIVERSITY_SLEEP) ||  defined(GP_DIVERSITY_JUMPTABLES)
#if(!defined(GP_DIVERSITY_JUMPTABLES) || defined(GP_DIVERSITY_ROM_GPSCHED_V2))
    gpUtils_LinkFree_t *gpSched_EventFree_p;
    gpUtils_LinkList_t *gpSched_EventList_p;
#else
    gpUtils_LinkFree_t gpSched_EventFree_p[1];
    gpUtils_LinkList_t gpSched_EventList_p[1];
#endif
    #if defined(GP_DIVERSITY_JUMPTABLES)
    Bool  AppDiversitySleep;
    Bool  AppDiversityCom;
    Bool  AppDiversityComNoRx;
    #endif //defined(GP_DIVERSITY_JUMPTABLES)
} gpSched_globals_t;

/*****************************************************************************
 *                    External Data Definitions
 *****************************************************************************/

//extern gpUtils_LinkFree_t gpSched_EventFree[1];
//extern gpUtils_LinkList_t gpSched_EventList[1];

/*****************************************************************************
 *                    Static Function Prototypes
 *****************************************************************************/

#ifdef __cplusplus
extern "C" {
#endif

#if defined(GP_DIVERSITY_JUMPTABLES) && defined(GP_DIVERSITY_ROM_CODE)
#include "gpJumpTables_DataTable.h"
#include "gpSched_CodeJumpTableFlash_Defs_defs.h"
#endif// defined(GP_DIVERSITY_JUMPTABLES) && defined(GP_DIVERSITY_ROM_CODE)

/* JUMPTABLE_FLASH_FUNCTION_DEFINITIONS_START */
/* JUMPTABLE_ROM_FUNCTION_DEFINITIONS_START */
void gpSched_InitExtramData(void);
void Sched_Integration_Init(void);
void Sched_Integration_DeInit(void);

/* JUMPTABLE_FLASH_FUNCTION_DEFINITIONS_END */
/* JUMPTABLE_ROM_FUNCTION_DEFINITIONS_END */

#if defined(GP_DIVERSITY_JUMPTABLES) && defined(GP_DIVERSITY_ROM_CODE)
    // in case we are in ROM, we need to retrieve the pointer via a shim table entry
#define GP_SCHED_GET_GLOBALS()    ((gpSched_globals_t*) JumpTables_DataTable.gpSched_globals_ptr)
#else // defined(GP_DIVERSITY_JUMPTABLES) && defined(GP_DIVERSITY_ROM_CODE)
    // in case we're not in ROM, we can directly fetch the address of the global vars.
extern gpSched_globals_t gpSched_globals;
#define GP_SCHED_GET_GLOBALS()    (&gpSched_globals)
#endif // defined(GP_DIVERSITY_JUMPTABLES) && defined(GP_DIVERSITY_ROM_CODE)

#if defined(GP_DIVERSITY_JUMPTABLES) && defined(GP_DIVERSITY_ROM_CODE)
// When running from ROM, check run-time application diversities.

#define SCHED_APP_DIVERSITY_SLEEP()         (GP_SCHED_GET_GLOBALS()->AppDiversitySleep)
#define SCHED_APP_DIVERSITY_COM()           (GP_SCHED_GET_GLOBALS()->AppDiversityCom)
#define SCHED_APP_DIVERSITY_COM_NO_RX()     (GP_SCHED_GET_GLOBALS()->AppDiversityComNoRx)

#else // defined(GP_DIVERSITY_JUMPTABLES) && defined(GP_DIVERSITY_ROM_CODE)
// When running from flash, check compile-time application diversities.

#define SCHED_APP_DIVERSITY_SLEEP()         (false)

#ifdef GP_COMP_COM
#define SCHED_APP_DIVERSITY_COM()           (true)
#else
#define SCHED_APP_DIVERSITY_COM()           (false)
#endif

#define SCHED_APP_DIVERSITY_COM_NO_RX()     (false)

#endif // defined(GP_DIVERSITY_JUMPTABLES) && defined(GP_DIVERSITY_ROM_CODE)

#ifdef __cplusplus
}
#endif

#endif //_GP_SCHED_DEFS_H_
