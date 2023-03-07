/*
 * Copyright (c) 2017-2021, Qorvo Inc
 *
 * platform_qorvo.c
 *   This file contains the implementation of the qorvo platform api for openthread.
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

#define GP_COMPONENT_ID GP_COMPONENT_ID_APP

/*****************************************************************************
 *                    Includes Definitions
 *****************************************************************************/

#include "platform_qorvo.h"
#include "gpLog.h"
#include "hal.h"
#include "gpSched.h"
#include "gpUtils.h"
#include "gpBaseComps.h"
#if defined(GP_DIVERSITY_JUMPTABLES)
#include "gpJumpTables.h"
#endif // defined(GP_DIVERSITY_JUMPTABLES)

/*****************************************************************************
 *                    Static Function Definitions
 *****************************************************************************/

/*****************************************************************************
 *                    Public Function Definitions
 *****************************************************************************/

void qorvoPlatMainLoop(bool canGoToSleep)
{
#ifndef GP_DIVERSITY_FREERTOS
    if(canGoToSleep)
    {
        gpSched_GoToSleep();
    }
    gpSched_Main_Body();
#endif //GP_DIVERSITY_FREERTOS
}

void qorvoPlatInit(qorvoPlatGotoSleepCheckCallback_t gotoSleepCheckCallback)
{
#ifndef GP_DIVERSITY_FREERTOS
    HAL_INITIALIZE_GLOBAL_INT();

    //Hardware initialisation
    HAL_INIT();

    //Radio interrupts that occur will only be handled later on in the main loop
    //Other interrupt source do not trigger any calls to blocks that are not initialized yet
    HAL_ENABLE_GLOBAL_INT();

    //Application_Init();
    gpBaseComps_StackInit();
    GP_UTILS_DUMP_STACK_POINTER();
    GP_UTILS_CHECK_STACK_PATTERN();
    GP_UTILS_CHECK_STACK_POINTER();


    gpSched_SetGotoSleepCheckCallback((gpSched_GotoSleepCheckCallback_t)gotoSleepCheckCallback);
#endif //GP_DIVERSITY_FREERTOS
#if defined(GP_DIVERSITY_JUMPTABLES)
    gpJumpTables_GetRomVersionFromRom();
#endif // defined(GP_DIVERSITY_JUMPTABLES)
}

