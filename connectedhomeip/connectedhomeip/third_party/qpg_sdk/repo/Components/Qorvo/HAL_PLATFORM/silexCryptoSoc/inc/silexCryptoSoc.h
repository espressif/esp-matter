/*
 * Copyright (c) 2017-2019, 2021, Qorvo Inc
 *
 *   This software is owned by Qorvo Inc
 *   and protected under applicable copyright laws.
 *   It is delivered under the terms of the license
 *   and is intended and supplied for use solely and
 *   exclusively with products manufactured by
 *   Qorvo Inc.
 *
 *
 *   THIS SOFTWARE IS PROVIDED IN AN "AS IS"
 *   CONDITION. NO WARRANTIES, WHETHER EXPRESS,
 *   IMPLIED OR STATUTORY, INCLUDING, BUT NOT
 *   LIMITED TO, IMPLIED WARRANTIES OF
 *   MERCHANTABILITY AND FITNESS FOR A
 *   PARTICULAR PURPOSE APPLY TO THIS SOFTWARE.
 *   QORVO INC. SHALL NOT, IN ANY
 *   CIRCUMSTANCES, BE LIABLE FOR SPECIAL,
 *   INCIDENTAL OR CONSEQUENTIAL DAMAGES,
 *   FOR ANY REASON WHATSOEVER.
 *
 *   $Header$
 *   $Change$
 *   $DateTime$
 */

#ifndef _SILEX_CRYPTOSOC_H_
#define _SILEX_CRYPTOSOC_H_

/*****************************************************************************
 *                    Includes Definitions
 *****************************************************************************/

#include "global.h"

/*****************************************************************************
 *                    Macro Definitions
 *****************************************************************************/


/*****************************************************************************
 *                    Type Definitions
 *****************************************************************************/

/*****************************************************************************
 *                    Public Function Prototypes
 *****************************************************************************/

#ifdef __cplusplus
extern "C" {
#endif

#if defined(GP_DIVERSITY_JUMPTABLES) && defined(GP_DIVERSITY_ROM_CODE)
#include "gpJumpTables_DataTable.h"
#include "silexCryptoSoc_CodeJumpTableFlash_Defs.h"
#endif // defined(GP_DIVERSITY_JUMPTABLES) && defined(GP_DIVERSITY_ROM_CODE)

/* JUMPTABLE_FLASH_FUNCTION_DEFINITIONS_START */
/** @brief Initialize the Silex CryptoSoc library.
 *
 *  This function must be called before using any function from the Silex CryptoSoc library.
 *  It is typically called via @a gpBaseComps_StackInit().
 */

void silexCryptoSoc_Init(void);
/* JUMPTABLE_FLASH_FUNCTION_DEFINITIONS_END */

/** @brief
 * Few functions in Silex library are defined in ROM but not callable in initial versions of ROM (i.e they
 * are not part of the ROM jumptables). As a solution, function pointers are initialized to these functions
 * and other modules that use these functions from Flash use these function pointers to access these non-callable
 * functions. Based on ROM version, either the function pointer points to location of these functions in ROM
 * or to the ROM jumbtable if the functions are made callable.
 *
 *  This function must be called before using Silex CryptoSoc library.
 *  It is typically called via @a gpBaseComps_StackInit().
 */
void silexCryptoSoc_InitFuncptr(void);

#ifdef __cplusplus
}
#endif

#endif //_SILEX_CRYPTOSOC_H_
