/*
 * Copyright (c) 2017, 2021, Qorvo Inc
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
#ifndef _GPJUMPTABLESS_H_
#define _GPJUMPTABLESS_H_

#if defined(GP_DIVERSITY_ROM_CODE)
#include "gpJumpTables_RomCode.h"
#else //defined(GP_DIVERSITY_ROM_CODE)

/*****************************************************************************
 *                    Includes Definitions
 *****************************************************************************/

#include "global.h"

/*****************************************************************************
 *                    Macro Definitions
 *****************************************************************************/

#if !defined(GPJUMPTABLES_MIN_ROMVERSION)
#define GPJUMPTABLES_MIN_ROMVERSION (1)
#endif //#if !defined(GPHAL_MIN_CHIPVERSION)

/*****************************************************************************
 *                    Public Function Definitions
 *****************************************************************************/

#ifdef __cplusplus
extern "C" {
#endif

#if defined(GP_DIVERSITY_JUMPTABLES) && defined(GP_DIVERSITY_ROM_CODE)
#include "gpJumpTables_CodeJumpTableFlash_Defs.h"
#include "gpJumpTables_CodeJumpTableRom_Defs.h"
#endif // defined(GP_DIVERSITY_JUMPTABLES) && defined(GP_DIVERSITY_ROM_CODE)

/* JUMPTABLE_FLASH_FUNCTION_DEFINITIONS_START */
/* JUMPTABLE_ROM_FUNCTION_DEFINITIONS_START */

#if defined(GP_DIVERSITY_JUMPTABLES)
/* @brief This function retrieves the ROM version value
 *  as specified at compile time of the ROM image.
 */
UInt8 gpJumpTables_GetRomVersion(void);

/* @brief This function retrieves the ROM version from the ROM
 */
UInt8 gpJumpTables_GetRomVersionFromRom(void);
#endif // defined(GP_DIVERSITY_JUMPTABLES)

/* JUMPTABLE_ROM_FUNCTION_DEFINITIONS_END */
/* JUMPTABLE_FLASH_FUNCTION_DEFINITIONS_END */

void gpJumpTables_Init(void);

#ifdef __cplusplus
}
#endif

#endif //defined(GP_DIVERSITY_ROM_CODE)

#endif // _GPJUMPTABLESS_H_
