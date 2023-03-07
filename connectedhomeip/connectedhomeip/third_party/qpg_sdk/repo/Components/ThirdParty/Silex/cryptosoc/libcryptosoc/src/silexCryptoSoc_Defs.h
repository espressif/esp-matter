/*
 * Copyright (c) 2019, 2021, Qorvo Inc
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

#ifndef _SILEX_CRYPTOSOC_DEFS_H_
#define _SILEX_CRYPTOSOC_DEFS_H_

/*****************************************************************************
 *                    Includes Definitions
 *****************************************************************************/

#include "global.h"

#if defined(GP_DIVERSITY_ROM_CODE)
#include "silexCryptoSoc_RomCode_Defs.h"
#else //defined(GP_DIVERSITY_ROM_CODE)

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
#include "gpSched_CodeJumpTableFlash_Defs_defs.h"
#endif // defined(GP_DIVERSITY_JUMPTABLES) && defined(GP_DIVERSITY_ROM_CODE)

#if defined(GP_DIVERSITY_JUMPTABLES) && defined(GP_DIVERSITY_ROM_CODE)
// in case we are in ROM, we need to retrieve the pointer via a data jump table entry
#define SILEX_CRYPTOSOC_GET_GLOBALS() ((silexCryptoSoc_GlobalVars_t*)JumpTables_DataTable.silexCryptoSoc_GlobalVars_ptr)
#else // defined(GP_DIVERSITY_JUMPTABLES) && defined(GP_DIVERSITY_ROM_CODE)
// in case we're not in ROM, we can directly fetch the address of the global vars.
#define SILEX_CRYPTOSOC_GET_GLOBALS() (&silexCryptoSoc_GlobalVars)

#endif // defined(GP_DIVERSITY_JUMPTABLES) && defined(GP_DIVERSITY_ROM_CODE)

typedef struct silexCryptoSoc_GlobalVars_s {
    uint8_t zeroes[16];     // used in sx_aes.c
    uint8_t ii_preamble[8]; // used in sx_jpake_alg.c
    uint8_t G_preamble[8];  // used in sx_jpake_alg.c
    uint8_t X_preamble[8];  // used in sx_jpake_alg.c
    uint8_t V_preamble[8];  // used in sx_jpake_alg.c
} silexCryptoSoc_GlobalVars_t;

extern silexCryptoSoc_GlobalVars_t silexCryptoSoc_GlobalVars;

#ifdef __cplusplus
}
#endif

#endif //defined(GP_DIVERSITY_ROM_CODE)

#endif //_SILEX_CRYPTOSOC_DEFS_H_
