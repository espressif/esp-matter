/*
 * Copyright (c) 2017, 2020-2021, Qorvo Inc
 *
 * misc_qorvo.c
 *   This file contains the implementation of the qorvo misc api for openthread.
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

#include "gpReset.h"
#include "gpSched.h"
#include "gpLog.h"
#include "gpAssert.h"

#ifdef GP_COMP_GPHAL
#include "hal_user_license.h"
#endif


#include "misc_qorvo.h"

/*****************************************************************************
 *                    Macro Definitions
 *****************************************************************************/

/*****************************************************************************
 *                    Static Data Definitions
 *****************************************************************************/


/*****************************************************************************
 *                    Static Function Definitions
 *****************************************************************************/
/** Flag to set when reset has fired. Bool will be used externally in the RCP case. */
bool gPlatformPseudoResetWasRequested;

#if !defined(HAL_DIVERSITY_USB)
static void delayedReset(void)
{
    gpReset_ResetSystem();
}
#endif

/*****************************************************************************
 *                    Public Function Definitions
 *****************************************************************************/

void qorvoPlatReset(void)
{
    // short delay for the reset to get the ACKs out.
#if defined(HAL_DIVERSITY_USB)
    gPlatformPseudoResetWasRequested = true;
#else
    gpSched_ScheduleEvent(0, delayedReset);
#endif
}

void qorvoPlatAssertFail(const char *aFilename, int aLineNumber)
{
    GP_LOG_SYSTEM_PRINTF("ASSERT %s:%d", 0, aFilename, aLineNumber);
    GP_ASSERT_DEV_EXT(false);
}

void qorvoGetUserLicense(void)
{
    /* prevent the linker from throwing away the user license */
#ifdef GP_COMP_GPHAL
    hal_get_user_license();
#endif
}

