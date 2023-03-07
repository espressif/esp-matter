/*
 * Copyright (c) 2018, Qorvo Inc
 *
 * gpJumpTables_RomSupport.c
 *
 * Support routines used when linking with ROM code.
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

#define GP_COMPONENT_ID GP_COMPONENT_ID_JUMPTABLES_K8E

#include "global.h"
#include "gpAssert.h"
#include "gpLog.h"

/*****************************************************************************
 *                    Type Definitions
 *****************************************************************************/

/*****************************************************************************
 *                    Static Data Definitions
 *****************************************************************************/

/*****************************************************************************
 *                    Static Function Definitions
 *****************************************************************************/

/*****************************************************************************
 *                    Public Function Definitions
 *****************************************************************************/

/**
 * This routine is used in the flashjumptable for entries which will not be
 * used in the current compile-time configuration of the application.
 *
 * The layout of the flashjumptable is fixed - the ROM code depends on it.
 * Table entries which are not used in the current application, are linked
 * to this function.
 *
 * Given the configuration of the appliation and control flow through the
 * ROM code, it should be technically impossible that any unused slot in
 * the flashjumptable is invoked at runtime. If it somehow happens anyway,
 * this dummy function either asserts (in development mode) or returns zero
 * (in release mode).
 */
UInt32 JumpTables_FlashJumpTableDummyHandler(UInt32 lr)
{
    GP_LOG_SYSTEM_PRINTF("ERROR: undefined flashjump LR=0x%lx", 0, (unsigned long)lr);
    GP_ASSERT_DEV_INT(false);
    return 0;
}
