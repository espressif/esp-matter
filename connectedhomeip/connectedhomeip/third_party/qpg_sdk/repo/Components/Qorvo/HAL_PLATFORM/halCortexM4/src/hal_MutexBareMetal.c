/*
 * Copyright (c) 2021, Qorvo Inc
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

/** @file Implementation of non OS specific Mutex wrappers */

/*****************************************************************************
 *                    Includes Definitions
 *****************************************************************************/
#define GP_COMPONENT_ID GP_COMPONENT_ID_HALCORTEXM4

#include "hal.h"

/*****************************************************************************
 *                    Macro Definitions
 *****************************************************************************/

/*****************************************************************************
 *                    Functional Macro Definitions
 *****************************************************************************/

/*****************************************************************************
 *                    Local Variable
 *****************************************************************************/

/*****************************************************************************
 *                    Public Function Definitions
 *****************************************************************************/

void hal_MutexCreate(void **dummyVar)
{
    (void)dummyVar;
    // nothing to do
}

void hal_MutexDestroy(void **dummyVar)
{
    (void)dummyVar;
    // nothing to do
}

Bool hal_MutexIsValid(void *dummyVar)
{
    (void)dummyVar;
    return true;
}

Bool hal_MutexIsAcquired(void *dummyVar)
{
    (void)dummyVar;
    return (!HAL_GLOBAL_INT_ENABLED());
}

void hal_MutexAcquire(void *dummyVar)
{
    (void)dummyVar;
    HAL_DISABLE_GLOBAL_INT();
}

void hal_MutexRelease(void *dummyVar)
{
    (void)dummyVar;
    HAL_ENABLE_GLOBAL_INT();
}
