/*
 * Copyright (c) 2017, 2020, Qorvo Inc
 *
 * random_qorvo.c
 *   This file contains the implementation of the qorvo random api for openthread.
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

/*****************************************************************************
 *                    Includes Definitions
 *****************************************************************************/

#include "gpRandom.h"
#include "random_qorvo.h"

/*****************************************************************************
 *                    Public Function Definitions
 *****************************************************************************/

void qorvoRandomInit(void)
{
}

void qorvoRandomGet(uint8_t* aOutput, uint8_t aOutputLength)
{
    gpRandom_GetNewSequence((UInt8)aOutputLength, (UInt8*)aOutput);
}
