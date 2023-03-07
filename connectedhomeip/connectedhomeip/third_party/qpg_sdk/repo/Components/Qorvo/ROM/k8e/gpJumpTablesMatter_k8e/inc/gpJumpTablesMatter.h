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

#ifndef _GPJUMPTABLESMATTER_H_
#define _GPJUMPTABLESMATTER_H_

/*****************************************************************************
 *                    Includes Definitions
 *****************************************************************************/

#include "global.h"

/*****************************************************************************
 *                    Macro Definitions
 *****************************************************************************/

#if !defined(GPJUMPTABLESMATTER_MIN_ROMVERSION)
#define GPJUMPTABLESMATTER_MIN_ROMVERSION (1)
#endif //!defined(GPJUMPTABLESMATTER_MIN_ROMVERSION)

/*****************************************************************************
 *                    Public Function Definitions
 *****************************************************************************/

/** @brief Init function to ensure linking of jump table symbols
 *
 * Function to be called to ensure linking of full jumptable content.
 * Without reference to jumptable, full table is optimized out in case of a library build.
 */
void gpJumpTablesMatter_Init(void);

/** @brief Function to fetch ROM version
 *
 * @return ROM version number. Will return 0xFF if no valid ROM was found.
 */
UInt8 gpJumpTablesMatter_GetVersion(void);

#endif // _GPJUMPTABLESMATTER_H_
